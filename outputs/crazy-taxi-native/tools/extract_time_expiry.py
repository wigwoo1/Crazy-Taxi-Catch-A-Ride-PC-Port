"""Extract five original language variants copied at0800c0ca..c10e."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
pal=0x290a20;v=struct.unpack_from('<16H',r,pal)
colors=[bytes([((z>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,z in enumerate(v)]
out=pathlib.Path('outputs/crazy-taxi-native/assets/hud');atlas=bytearray(96*160*4);frames=bytearray();rows=[]
for lang in range(5):
 addr=struct.unpack_from('<I',r,0x7ec288+lang*4)[0];p=addr-0x8000000
 assert 0<=p<=len(r)-1536
 for frame in range(3):
  for y in range(32):
   for x in range(32):
    source=p+frame*512+((y//8)*4+x//8)*32+(y%8)*4+(x%8)//2
    index=(r[source]>>(4*(x&1)))&15;col=colors[index];frames.extend(col)
    dst=((lang*32+y)*96+frame*32+x)*4;atlas[dst:dst+4]=col if index else bytes((40,40,40,255))
 rows.append({'language_index':lang,'rom_pixels':hex(addr),'bytes':1536})
(out/'time-expiry.rgba').write_bytes(frames);write_png(out/'time-expiry-atlas.png',96,160,atlas)
(out/'time-expiry.json').write_text(json.dumps({'rom_sha256':EXPECTED,'language_table':'087ec288','language_runtime':'0200388d','palette_source':'08290a20','copy_routine':'08027fb8','copy_caller':'0800c0ca..0800c10e','format':'RGBA8 language-major,3 frames32x32 per language; index0 transparent','pixels_sha256':hashlib.sha256(frames).hexdigest(),'variants':rows},indent=2))
print('Recovered expiry art: five variants, three frames each')
