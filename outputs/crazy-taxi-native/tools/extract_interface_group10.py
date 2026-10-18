import pathlib,struct,hashlib,json
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
root=0x70e50;g=root+struct.unpack_from('<I',r,root+4+10*4)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
assert count>=4 and bits==16 and 0<pals<=16
pal=g+20+count*4;pix=pal+32*pals
out=pathlib.Path('outputs/crazy-taxi-native/assets/interface-group10');out.mkdir(exist_ok=True)
rows=[];blob=bytearray()
for frame in range(4):
 a=root+struct.unpack_from('<I',r,g+20+frame*4)[0]
 m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
 assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
 colors=struct.unpack_from('<16H',r,pal);rgba=bytearray()
 for y in range(ty*8):
  for x in range(tx*8):
   p=pix+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
   idx=(r[p]>>(4*(x&1)))&15;c=colors[idx]
   rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if idx else 0])
 write_png(out/f'frame-{frame}.png',tx*8,ty*8,rgba)
 rows.append(dict(frame=frame,metrics=list(m),descriptor=hex(a+0x8000000),offset=len(blob),size=len(rgba)));blob.extend(rgba)
(out/'sprites.rgba').write_bytes(blob)
(out/'sprites.json').write_text(json.dumps(dict(root=hex(root+0x8000000),group=hex(g+0x8000000),header=[count,bits,tiles,start,pals],palette=hex(pal+0x8000000),frames=rows,qualification='Bank0 per source1675c/16760; caller ownership/registration still requires audit.'),indent=2))
print(json.dumps(rows,indent=2))
