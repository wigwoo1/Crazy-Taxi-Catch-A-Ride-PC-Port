"""Extract original results portraits/class sprites; no invented artwork."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
mode=sys.argv[1] if len(sys.argv)>1 else 'results'
assert mode in ('results','name-entry')
b=0x64f38 if mode=='results' else 0x76d10;o=pathlib.Path('outputs/crazy-taxi-native/assets')/mode;o.mkdir(exist_ok=True)
frames=bytearray();rows=[];atlas=bytearray(256*128*4)
for group in range(3,9):
 g=b+struct.unpack_from('<I',r,b+4+group*4)[0]
 count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
 assert bits==16 and pals==1
 pal=g+20+count*4;pix=pal+32;v=struct.unpack_from('<16H',r,pal)
 colors=[bytes([((z>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,z in enumerate(v)]
 for index in range(count):
  a=b+struct.unpack_from('<I',r,g+20+index*4)[0];m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
  assert tile+tx*ty<=tiles and tx<=8 and ty<=4
  frame=bytearray(tx*8*ty*8*4)
  slot=len(rows)
  for y in range(ty*8):
   for x in range(tx*8):
    p=pix+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
    idx=(r[p]>>(4*(x&1)))&15;col=colors[idx]
    dest=(y*tx*8+x)*4;frame[dest:dest+4]=col
    sx=(slot%4)*64+x;sy=(slot//4)*32+y;dest=(sy*256+sx)*4
    atlas[dest:dest+4]=col if idx else bytes((40,40,40,255))
  rows.append(dict(group=group,index=index,descriptor=hex(a+0x8000000),palette=hex(pal+0x8000000),pixels=hex(pix+0x8000000),metrics=list(m),width=tx*8,height=ty*8,byte_offset=len(frames),byte_length=len(frame)))
  frames.extend(frame)
(o/'sprites.rgba').write_bytes(frames);write_png(o/'sprites-atlas.png',256,128,atlas)
(o/'sprites.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container=hex(b+0x8000000),entry='08023952' if mode=='results' else '08025434',format='RGBA8 variable sized frames; source index0 transparent; complete tile bounds, crop/anchor metrics retained',frames_sha256=hashlib.sha256(frames).hexdigest(),frames=rows),indent=2)+'\n')
print(f'Extracted {len(rows)} original results frames, {len(frames)} bytes; all tile bounds checked.')

(o/'sprites.crs').write_bytes(b'CRS1'+struct.pack('<I',len(rows))+b''.join(struct.pack('<9H',*row['metrics']) for row in rows))
