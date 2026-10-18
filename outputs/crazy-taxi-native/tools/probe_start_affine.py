"""Reachable affine sprite CFG; branch tables require explicit target seeds."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
work=[0x28658];seen={};indirect=[]
while work:
 off=work.pop()
 if off in seen or not 0x28658<=off<0x28bd0:continue
 ins=list(md.disasm(r[off:off+4],0x8000000+off,count=1));assert ins
 i=ins[0];seen[off]=i
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='mov' and i.op_str.startswith('pc,'):
  indirect.append(off);continue
 if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
 work.append(off+i.size)
# Known mode jump table28790, same five modes as nonaffine.
for n in range(5):
 target=struct.unpack_from('<I',r,0x28790+4*n)[0]
 assert 0x08028658<=target<0x08028bd0
lines=['ROM SHA verified; external BL callees not traversed; mode table not traversed automatically.']
for off,i in sorted(seen.items()):
 line=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800==0x4800:
  p=((off+4)&~3)+(h&255)*4;line+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
 lines.append(line)
lines.append('INDIRECT_MODE_DISPATCH:'+str(indirect))
pathlib.Path('outputs/crazy-taxi-native/reports/start-affine-reachable-source.txt').write_text('\n'.join(lines))
print('Recovered',len(seen),'reachable affine instructions; indirect mode dispatch remains explicit dependency.')
