"""Recover original group7 ended-state sprites; no timing assumptions."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
b=0x70e50;g=b+struct.unpack_from('<I',r,b+32)[0]
assert struct.unpack_from('<5I',r,g)==(3,16,48,391,1)
pal=g+32;pix=pal+32;values=struct.unpack_from('<16H',r,pal)
colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(values)]
frames=bytearray();atlas=bytearray(96*32*4);rows=[]
for i in range(3):
 a=b+struct.unpack_from('<I',r,g+20+i*4)[0];m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
 assert (tx,ty)==(4,4) and tile+16<=48
 frame=bytearray(32*32*4)
 for y in range(32):
  for x in range(32):
   p=pix+(tile+(y//8)*4+x//8)*32+y%8*4+x%8//2
   idx=(r[p]>>(4*(x&1)))&15;col=colors[idx]
   frame[(y*32+x)*4:(y*32+x+1)*4]=col
   dst=(y*96+i*32+x)*4;atlas[dst:dst+4]=col if idx else bytes((40,40,40,255))
 frames.extend(frame);rows.append({'index':i,'rom_descriptor':hex(a+0x8000000),'metrics':list(m)})
o=pathlib.Path('outputs/crazy-taxi-native/assets/hud');o.mkdir(exist_ok=True)
(o/'ended.rgba').write_bytes(frames);write_png(o/'ended-atlas.png',96,32,atlas)
(o/'ended.json').write_text(json.dumps({'rom_sha256':EXPECTED,'group':hex(g+0x8000000),'palette':hex(pal+0x8000000),'pixels':hex(pix+0x8000000),'format':'3 RGBA8 32x32 frames; source index0 transparency','source_draw':'0801fd40..0801fdd2 group7 at y80, runtime020016c0 scale','frames_sha256':hashlib.sha256(frames).hexdigest(),'descriptors':rows},indent=2))
print('Recovered three original ended sprites:',o)
