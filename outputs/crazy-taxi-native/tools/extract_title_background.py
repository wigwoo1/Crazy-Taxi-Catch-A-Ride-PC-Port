"""Source BG0 extraction for name entry and ranking; no invented art."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
for screen,b,layer in [('title',0x3415c,0)]:
 descriptor=b;b+=layer*4
 mp=struct.unpack_from('<I',r,b)[0]-0x8000000;pal=struct.unpack_from('<I',r,b+16)[0]-0x8000000;p=struct.unpack_from('<I',r,b+32)[0]-0x8000000;start=p
 palette_bytes=struct.unpack_from('<H',r,descriptor+56+layer*2)[0];size=struct.unpack_from('<H',r,descriptor+64+layer*2)[0];w=struct.unpack_from('<H',r,descriptor+72+layer*2)[0];h=struct.unpack_from('<H',r,descriptor+80+layer*2)[0]
 assert palette_bytes in (32,512) and r[descriptor+124+layer]==1
 tile_bytes=64 if palette_bytes==512 else 32;assert size%tile_bytes==0
 tiles=bytearray()
 for tile in range(size//tile_bytes):
  block=bytearray()
  while len(block)<tile_bytes:
   command=r[p];p+=1;n=command&127;assert n and len(block)+n<=tile_bytes
   if command&128:block.extend(r[p:p+n]);p+=n
   else:block.extend(bytes([r[p]])*n);p+=1
  tiles.extend(block)
 assert len(tiles)==size and p<=mp
 vals=struct.unpack_from('<'+str(palette_bytes//2)+'H',r,pal)
 colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255]) for v in vals]
 entries=struct.unpack_from('<'+str(w*h)+'H',r,mp);rgba=bytearray(w*8*h*8*4);indices=bytearray(w*8*h*8)
 for y in range(h*8):
  for x in range(w*8):
   e=entries[(y//8)*w+x//8];tile=e&1023;assert tile<size//tile_bytes
   xx=7-x%8 if e&1024 else x%8;yy=7-y%8 if e&2048 else y%8
   if tile_bytes==64:index=tiles[tile*64+yy*8+xx]
   else:
    index=((tiles[tile*32+yy*4+xx//2]>>(4*(xx&1)))&15)+16*(e>>12)
   assert index<len(colors);indices[y*w*8+x]=index;dst=(y*w*8+x)*4;rgba[dst:dst+4]=colors[index]
 o=pathlib.Path('outputs/crazy-taxi-native/assets')/screen;o.mkdir(exist_ok=True)
 (o/'background.indices').write_bytes(indices);(o/'background.rgba').write_bytes(rgba);(o/'background.tiles').write_bytes(tiles);write_png(o/'background.png',w*8,h*8,rgba)
 (o/'background.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,descriptor=hex(descriptor+0x8000000),layer=layer,map=hex(mp+0x8000000),palette=hex(pal+0x8000000),compressed_tiles=hex(start+0x8000000),compressed_end=hex(p+0x8000000),tile_bytes=tile_bytes,width=w*8,height=h*8,format='opaque BG0 over palette backdrop; source tile flips and 4bpp palette banks',rgba_sha256=hashlib.sha256(rgba).hexdigest()),indent=2)+'\n')
 print(screen,w*8,h*8,'tiles',size//tile_bytes,'compressed-end',hex(p))

