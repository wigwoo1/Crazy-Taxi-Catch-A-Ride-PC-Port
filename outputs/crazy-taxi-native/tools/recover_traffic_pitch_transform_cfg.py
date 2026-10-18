"""Recover reachable traffic-pitch-transform instructions, excluding literal pools. No execution claim."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
from collections import Counter
import json
pending=[0x2dbc8];seen={};calls=set();returns=[];literals=[]
while pending:
 off=pending.pop()
 if off in seen:continue
 assert 0x2dbc8<=off<0x2dc20,hex(off)
 ins=next(md.disasm(r[off:off+4],0x8000000+off,count=1));m=ins.mnemonic;op=ins.op_str
 seen[off]=dict(address=hex(ins.address),bytes=ins.bytes.hex(),mnemonic=m,operands=op)
 if m=='ldr' and 'pc' in op:
  h=int.from_bytes(r[off:off+2],'little');lit=((off+4)&~3)+(h&255)*4
  literals.append(dict(instruction=hex(ins.address),address=hex(lit+0x8000000),value=hex(int.from_bytes(r[lit:lit+4],'little'))))
 if m=='bl':calls.add(int(op.lstrip('#'),16));pending.append(off+ins.size)
 elif m=='bx' or (m=='pop' and 'pc' in op):returns.append(off)
 elif m in ['b','beq','bne','bgt','blt','bge','ble','bhi','bls','bhs','blo','bmi','bpl','bvs','bvc']:
  pending.append(int(op.lstrip('#'),16)-0x8000000)
  if m!='b':pending.append(off+ins.size)
 else:pending.append(off+ins.size)
print(len(seen),calls,returns)

report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),entry='0x802dbc8',instructions=[seen[k] for k in sorted(seen)],calls=[hex(k) for k in sorted(calls)],returns=[hex(k+0x8000000) for k in returns],literals=literals,mnemonics=dict(Counter(i['mnemonic'] for i in seen.values())),scope='Reachable Thumb control-flow inventory only; excludes unreachable literal pools. No routine execution or fidelity claim.')
pathlib.Path('outputs/crazy-taxi-native/reports/traffic-pitch-transform-cfg.json').write_text(json.dumps(report,indent=2))
print('CFG recovered')
