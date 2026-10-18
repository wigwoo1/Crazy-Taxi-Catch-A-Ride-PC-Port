"""Original city BG/OBJ palette and footer sources for native composition."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/city-selection')
meta=json.loads((out/'background.json').read_text());bgpal=int(meta['palette'],16)-0x8000000
b=0x520f8;g=b+struct.unpack_from('<I',r,b+16)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g);assert bits==16 and pals==2
pal=g+20+count*4;pix=pal+32*pals;indices=bytearray();frames=[]
for frame in (1,2):
 a=b+struct.unpack_from('<I',r,g+20+frame*4)[0];m=struct.unpack_from('<9H',r,a)
 tx,ty,ox,oy,cx,cy,w,h,tile=m
 assert (tx,ty)==((4,2) if frame==1 else (1,2)) and ox==oy==0 and tile+tx*ty<=tiles
 for y in range(ty*8):
  for x in range(tx*8):
   p=pix+(tile+(y//8)*tx+x//8)*32+(y%8)*4+x%8//2
   indices.append((r[p]>>(4*(x&1)))&15)
 frames.append(dict(frame=frame,metrics=list(m),descriptor=hex(a+0x8000000)))
assert len(indices)==640
(out/'background.rgb555').write_bytes(r[bgpal:bgpal+512]);(out/'footer.rgb555').write_bytes(r[pal:pal+32]);(out/'footer.indices').write_bytes(indices)
(out/'footer.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container='080520f8',palette=hex(pal+0x8000000),frames=frames,qualification='Original source footer only. Native rendering/blend control integration pending.'),indent=2)+'\n')
print('City source palette512/32 and footer640 extracted; metrics/provenance retained.')
