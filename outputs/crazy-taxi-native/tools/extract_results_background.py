"""Recover results BG0 with original per-tile RLE and source tile map."""
import pathlib,hashlib,struct,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(r).hexdigest()==EXPECTED
b=0x3436c;mp=struct.unpack_from('<I',r,b)[0]-0x8000000;pal=struct.unpack_from('<I',r,b+16)[0]-0x8000000;p=struct.unpack_from('<I',r,b+32)[0]-0x8000000;start=p
assert struct.unpack_from('<H',r,b+56)[0]==512
size=struct.unpack_from('<H',r,b+64)[0];w=struct.unpack_from('<H',r,b+72)[0];h=struct.unpack_from('<H',r,b+80)[0]
assert (size,w,h,r[b+124])==(3392,30,20,1)
tiles=bytearray()
for tile in range(size//64):
 block=bytearray()
 while len(block)<64:
  command=r[p];p+=1;n=command&127;assert n and len(block)+n<=64
  if command&128:block.extend(r[p:p+n]);p+=n
  else:block.extend(bytes([r[p]])*n);p+=1
 tiles.extend(block)
assert len(tiles)==size and p<=mp
values=struct.unpack_from('<256H',r,pal);colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255]) for v in values]
rgba=bytearray(w*8*h*8*4);entries=struct.unpack_from('<'+str(w*h)+'H',r,mp);used=set()
for y in range(h*8):
 for x in range(w*8):
  e=entries[(y//8)*w+x//8];tile=e&1023;assert tile<size//64;used.add(tile)
  xx=7-x%8 if e&1024 else x%8;yy=7-y%8 if e&2048 else y%8
  index=tiles[tile*64+yy*8+xx];offset=(y*w*8+x)*4;rgba[offset:offset+4]=colors[index]
o=pathlib.Path('outputs/crazy-taxi-native/assets/results');o.mkdir(exist_ok=True)
(o/'background.rgba').write_bytes(rgba);(o/'background.tiles').write_bytes(tiles);write_png(o/'background.png',w*8,h*8,rgba)
(o/'background.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,descriptor='0803436c',map=hex(mp+0x8000000),palette=hex(pal+0x8000000),compressed_tiles=hex(start+0x8000000),compressed_end=hex(p+0x8000000),tile_count=size//64,used_tiles=sorted(used),width=w*8,height=h*8,decoder='0801e860 called per64byte tile via08007190',format='opaque RGBA8 BG0 over palette backdrop; source h/v tile flips',rgba_sha256=hashlib.sha256(rgba).hexdigest()),indent=2)+'\n')
print(f'Results BG0: {w*8}x{h*8}, {size//64} tiles, compressed end {p:08x}, all600 map entries in bounds.')
