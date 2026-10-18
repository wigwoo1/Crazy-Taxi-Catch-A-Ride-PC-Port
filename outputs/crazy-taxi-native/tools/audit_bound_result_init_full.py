exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
work=[0x1ee28];seen={};literals={};outside=set();calls=set()
while work:
 a=work.pop()
 if a in seen:continue
 if not 0x1ee28<=a<0x1f0e8:outside.add(a);continue
 i=next(md.disasm(r[a:a+4],0x8000000+a,count=1));seen[a]=i;h=struct.unpack_from('<H',r,a)[0]
 if h&0xf800==0x4800:literals[hex(i.address)]=hex(struct.unpack_from('<I',r,((a+4)&~3)+(h&255)*4)[0])
 if i.mnemonic=='bx' or i.mnemonic=='pop' and 'pc' in i.op_str:continue
 if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
 if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ['bl','blx','bic','bics'] and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
 work.append(a+i.size)
assert not outside, outside
lines=[f'{i.address:08X}: {i.mnemonic} {i.op_str}'+(' literal='+literals[hex(i.address)] if hex(i.address) in literals else '') for a,i in sorted(seen.items())]
p=pathlib.Path('outputs/crazy-taxi-native/reports');(p/'bound-result-init-full-source.txt').write_text('\n'.join(lines)+'\n');(p/'bound-result-init-full-cfg.json').write_text(json.dumps({'instructions':[{'address':hex(i.address),'bytes':bytes(i.bytes).hex()} for a,i in sorted(seen.items())],'literal_references':literals,'calls':[hex(a) for a in sorted(calls)],'scope':'Full bounded reachable1EE28 CFG, external calls not traversed/executed, static audit only.'},indent=2));print('\n'.join(lines))
