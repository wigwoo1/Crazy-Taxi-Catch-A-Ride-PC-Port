"""Recover original 202F0 segment distance reachable Thumb instructions, excluding literal pools."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
pending=[0x202f0];seen={};calls=set();literals=[]
while pending:
 off=pending.pop()
 if off in seen:continue
 assert 0x202f0<=off<0x203c4,hex(off)
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)
 if i.mnemonic=='ldr' and 'pc' in i.op_str:
  h=int.from_bytes(r[off:off+2],'little');lit=((off+4)&~3)+(h&255)*4;literals.append((hex(i.address),hex(int.from_bytes(r[lit:lit+4],'little'))))
 if i.mnemonic=='bl':calls.add(int(i.op_str.lstrip('#'),16));pending.append(off+i.size)
 elif i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 elif i.mnemonic.startswith('b') and i.mnemonic!='bics':
  pending.append(int(i.op_str.lstrip('#'),16)-0x8000000)
  if i.mnemonic!='b':pending.append(off+i.size)
 else:pending.append(off+i.size)
pathlib.Path('outputs/crazy-taxi-native/reports/route-segment-distance-cfg.json').write_text(json.dumps(dict(entry='0x80202f0',instructions=[seen[k] for k in sorted(seen)],calls=[hex(k) for k in sorted(calls)],literals=literals,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original 202F0 segment distance reachable instructions; static recovery only.'),indent=2))
print('Recovered',len(seen),'instructions')
