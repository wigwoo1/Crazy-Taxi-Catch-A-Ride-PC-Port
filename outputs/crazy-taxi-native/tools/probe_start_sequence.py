"""Reachable static start-sequence blocks; external calls/yield remain qualified."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
work=[0xba04];seen={};outside=set()
while work:
 off=work.pop()
 if off in seen:continue
 if not 0xba04<=off<0xbd40:outside.add(off);continue
 ins=list(md.disasm(r[off:off+4],0x8000000+off,count=1));assert ins
 i=ins[0];seen[off]=i
 if i.mnemonic in ['bx','pop'] and (i.mnemonic=='bx' or 'pc' in i.op_str):continue
 if i.mnemonic=='bl' and i.op_str=='#0x800c524':continue
 if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ['bl','blx','bic','bics'] and i.op_str.startswith('#'):
  work.append(int(i.op_str[1:],16)-0x8000000)
 work.append(off+i.size)
lines=['ROM SHA verified. Reachable static instruction blocks; BL external callees not traversed. C524 epilogue unwinds BA04 original stack; calls do not fall through.']
for off,i in sorted(seen.items()):
 row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800==0x4800:
  p=((off+4)&~3)+(h&255)*4;row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
 lines.append(row)
lines.append('OUTSIDE branch targets:'+','.join(hex(x) for x in sorted(outside)))
p=pathlib.Path('outputs/crazy-taxi-native/reports/start-sequence-reachable-source.txt');p.write_text('\n'.join(lines));print('\n'.join(lines))
