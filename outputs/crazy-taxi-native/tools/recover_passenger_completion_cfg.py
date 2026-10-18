"""Recover boarding completion bounded reachable block; literal pools excluded."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
pending=[0xb098];seen={};calls=set();literals=[];exits=set()
while pending:
 off=pending.pop()
 if off in [0xb10c,0xb2d8]:exits.add(off);continue
 if off in seen:continue
 assert 0xb098<=off<0xb0f2,hex(off)
 ins=next(md.disasm(r[off:off+4],0x8000000+off,count=1));m=ins.mnemonic;op=ins.op_str
 seen[off]=dict(address=hex(ins.address),bytes=ins.bytes.hex(),mnemonic=m,operands=op)
 if m=='ldr' and 'pc' in op:
  h=int.from_bytes(r[off:off+2],'little');lit=((off+4)&~3)+(h&255)*4
  literals.append(dict(instruction=hex(ins.address),address=hex(lit+0x8000000),value=hex(int.from_bytes(r[lit:lit+4],'little'))))
 if m=='bl':calls.add(int(op.lstrip('#'),16));pending.append(off+ins.size)
 elif m.startswith('b') and m!='bics':
  assert m!='bx';pending.append(int(op.lstrip('#'),16)-0x8000000)
  if m!='b':pending.append(off+ins.size)
 else:pending.append(off+ins.size)
report=dict(entry='0x800b098',instructions=[seen[k] for k in sorted(seen)],calls=[hex(k) for k in sorted(calls)],exits=[hex(k+0x8000000) for k in sorted(exits)],literals=literals,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original boarding completion through start/search or later frame boundary; static recovery only.')
pathlib.Path('outputs/crazy-taxi-native/reports/passenger-completion-cfg.json').write_text(json.dumps(report,indent=2))
for i in report['instructions']:print(i['address'],i['mnemonic'],i['operands'])
print('LITERALS',literals)
