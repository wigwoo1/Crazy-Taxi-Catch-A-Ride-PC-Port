"""Reachable music startup source CFG; excludes literal islands and opaque data aliases."""
from pathlib import Path
import hashlib,json,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
functions=[]
for entry,limit in [(0x8026ef8,0x8027044),(0x8027110,0x80271e4),(0x8027730,0x8027738)]:
 todo=[entry];seen={};literals=[];returns=[];calls=[]
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<limit,hex(a)
  i=next(cs.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1))
  seen[a]={'address':hex(a),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}
  if i.mnemonic=='ldr' and '[pc,' in i.op_str:
   off=int(i.op_str.split('#')[1].split(']')[0],0)
   ptr=((a+4)&~3)+off
   value=int.from_bytes(r[ptr-0x8000000:ptr-0x8000000+4],'little')
   literals.append({'instruction':hex(a),'literal_address':hex(ptr),'value':hex(value)})
  if i.mnemonic=='bx':
   assert i.op_str=='lr' or (a in (0x802702a,0x80271c8) and i.op_str=='r0' and bytes(r[a-0x8000000-2:a-0x8000000])==bytes.fromhex('01bc')),(hex(a),i.op_str)
   returns.append(hex(a));continue
  if i.mnemonic=='pop' and 'pc' in i.op_str:returns.append(hex(a));continue
  if i.mnemonic in ('bl','blx'):calls.append(seen[a]);todo.append(a+i.size);continue
  if i.mnemonic.startswith('b'):
   assert i.mnemonic not in ('bkpt',),seen[a]
   todo.append(int(i.op_str.lstrip('#'),0))
   if i.mnemonic!='b':todo.append(a+i.size)
  else:todo.append(a+i.size)
 assert returns and not calls,(hex(entry),calls)
 assert all(x['value']!='0x2003b04' for x in literals)
 functions.append({'entry':hex(entry),'instructions':[seen[a] for a in sorted(seen)],'literal_references':literals,'returns':returns,'calls':calls})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'functions':functions,'conclusion':'Complete reachable music module loader26EF8, order start27110 and playback stop27730 contain no calls or indirect control transfers except returns. None directly loads known shared RNG owner02003B04 via literal. These routines introduce no nested RNG function calls in Retry prefix.','limits':'Static source CFG only, not execution or native equivalence. Indirect data-owner aliases and malformed module data excluded; native tracker scheduling may separately consume RNG. Full AC70 startup and passenger227AC ordering remain unverified.','source_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/restart-music-dependency-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',[(f['entry'],len(f['instructions']),len(f['literal_references'])) for f in functions])
