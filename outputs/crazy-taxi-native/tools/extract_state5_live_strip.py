exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
from png_writer import write_png
import json
row=struct.unpack_from('<7I',r,0x49d560+5*28);print('state5',[hex(v) for v in row])
root=row[4]-0x8000000;g=root+struct.unpack_from('<I',r,root+44)[0]
header=struct.unpack_from('<5I',r,g);print('group10',header)
palette=struct.unpack_from('<16H',r,0x290ad4)
out=pathlib.Path('outputs/crazy-taxi-native/assets/state5-live-strip');out.mkdir(exist_ok=True)
metrics=[]
for frame in range(4):
 a=root+struct.unpack_from('<I',r,g+20+frame*4)[0];metrics.append(struct.unpack_from('<9H',r,a))
print('metrics',metrics)
assert all(m[:2]==(4,2) for m in metrics)
for language in range(4):
 base=struct.unpack_from('<I',r,0x7ebf48+4*language)[0]-0x8000000
 rgba=bytearray(128*16*4)
 for half,m in enumerate(metrics):
  for y in range(16):
   for x in range(32):
    pos=base+half*256+(y//8*4+x//8)*32+(y%8)*4+x%8//2
    idx=r[pos]>>(4*(x&1))&15;c=palette[idx]
    pixel=bytes([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if idx else 0])
    dst=4*(y*128+half*32+x);rgba[dst:dst+4]=pixel
 write_png(out/f'language-{language}.png',128,16,rgba)
 (out/f'language-{language}.rgba').write_bytes(rgba)
(out/'provenance.json').write_text(json.dumps(dict(state5=[hex(v) for v in row],metrics=metrics,palette='08290ad4',qualification='Dynamic group10 copy source16324..6376/27fb8 stride256; callback166b8. State semantic identity pending.'),indent=2))
