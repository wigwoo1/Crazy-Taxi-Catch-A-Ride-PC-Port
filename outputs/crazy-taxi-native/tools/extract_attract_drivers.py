"""Extract original demo score portraits from selected groups3..6."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
root=pathlib.Path(__file__).resolve().parents[1]
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();sha=hashlib.sha256(r).hexdigest()
assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
b=0x5a338;out=root/'assets/attract';rows=[]
for driver in range(4):
 group=driver+3;g=b+struct.unpack_from('<I',r,b+4+4*group)[0]
 count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
 assert (count,bits,tiles,pals)==(1,16,4,1)
 a=b+struct.unpack_from('<I',r,g+20)[0];m=struct.unpack_from('<9H',r,a)
 assert m==(2,2,0,0,0,0,16,16,0)
 pal=g+24;pixels=pal+32;values=struct.unpack_from('<16H',r,pal)
 rgba=bytearray()
 for y in range(16):
  for x in range(16):
   address=pixels+32*((y//8)*2+x//8)+(y%8)*4+(x%8)//2
   idx=(r[address]>>(4*(x&1)))&15;v=values[idx]
   rgba.extend([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if idx else 0])
 p=out/f'driver{driver}.rgba';p.write_bytes(rgba);assert p.read_bytes()==rgba
 write_png(out/f'driver{driver}.png',16,16,rgba)
 rows.append({'driver':driver,'group':group,'descriptor':hex(a+0x8000000),'metrics':list(m),'palette':hex(pal+0x8000000),'tile_start':start,'rgba_sha256':hashlib.sha256(rgba).hexdigest()})
report={'rom_sha256':sha,'source_selector':'DB06..DB0C: group3+packed bits5..6; sprite0','portraits':rows,'scope':'Original raw4bpp/RGB555/index0 transparency,16x16/zero offsets. Live palette1770 overrides and OAM priority/placement remain separate.'}
(out/'drivers.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
