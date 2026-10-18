exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
values=struct.unpack_from('<16H',r,0x290b88);blob=bytearray()
for frame in range(3):
 raw=r[0xfcff8+frame*512:0xfcff8+(frame+1)*512]
 for y in range(32):
  for x in range(32):
   v=raw[((y//8)*4+x//8)*32+(y%8)*4+x%8//2];i=(v>>(4*(x&1)))&15;c=values[i]
   blob.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0])
p=pathlib.Path('outputs/crazy-taxi-native/assets/start-phase/initial.rgba');p.write_bytes(blob);assert p.read_bytes()==blob
print('PASS source initial startup group7 three frames extracted.')
