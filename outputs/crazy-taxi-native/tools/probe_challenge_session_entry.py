"""SHA-guarded static dispatcher reachability; external calls are dependencies."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
work=[0x740c];seen={};calls=set();outside=set()
while work:
 off=work.pop()
 if off in seen:continue
 if not 0x740c<=off<0x7b20:outside.add(off);continue
 ins=list(md.disasm(r[off:off+4],0x8000000+off,count=1));assert ins
 i=ins[0];seen[off]=i
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
 if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
 work.append(off+i.size)
lines=['ROM SHA verified. Static dispatcher; external BL not executed/traversed.']
for off,i in sorted(seen.items()):
 row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800==0x4800:
  p=((off+4)&~3)+(h&255)*4;row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
 lines.append(row)
lines+=['CALLS:'+','.join(hex(x) for x in sorted(calls)),'OUTSIDE:'+','.join(hex(x) for x in sorted(outside))]
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-session-entry-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))

