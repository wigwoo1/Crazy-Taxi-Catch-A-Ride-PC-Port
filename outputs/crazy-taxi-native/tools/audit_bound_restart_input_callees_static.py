from pathlib import Path
import sys,json,hashlib
sys.path.insert(0,'work/python-packages');from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
roots=[0xf5e8,0x25c18,0x7378,0x73c0,0x27994,0xfec8,0xe154,0xe5a8,0x2ac6c,0x2c834,0x2c688,0x203c4,0x27730,0x26ef8,0x1e72c,0x27110,0x26440,0xaad2]
functions={};pending=[0x8000000+a for a in roots];indirect=[];inputrefs=[]
while pending:
 entry=pending.pop()
 if entry in functions:continue
 todo=[entry];seen=set();calls=set();unknown=[]
 while todo:
  a=todo.pop()
  if a in seen:continue
  if not 0x8000000<=a<0x8800000:unknown.append(['outside',hex(a)]);continue
  seen.add(a)
  if len(seen)>10000:raise RuntimeError(('too large',hex(entry)))
  i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1),None)
  if i is None:unknown.append(['decode',hex(a)]);continue
  if i.mnemonic=='ldr' and 'pc,' in i.op_str:
   disp=i.operands[1].mem.disp;lit=((a+4)&~3)+disp
   value=int.from_bytes(r[lit-0x8000000:lit-0x8000000+4],'little')
   if 0x20034dc<=value<0x200351e:inputrefs.append({'function':hex(entry),'instruction':hex(a),'literal':hex(value)})
  if i.mnemonic=='bl':
   target=i.operands[0].imm;calls.add(target);todo.append(a+i.size);continue
  if i.mnemonic=='bx':
   if i.op_str!='lr':unknown.append(['indirect_bx',hex(a),i.op_str])
   continue
  if i.mnemonic=='pop' and 'pc' in i.op_str:continue
  if 'pc' in i.op_str and i.mnemonic in ['mov','add','subs','ldr'] and i.op_str.startswith('pc'):
   unknown.append(['computed_pc',hex(a),i.op_str]);continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ['bic','bics','bkpt']:
   if not i.operands or i.operands[0].type!=2:unknown.append(['branch',hex(a),i.op_str]);continue
   todo.append(i.operands[0].imm)
   if i.mnemonic!='b':todo.append(a+i.size)
  else:todo.append(a+i.size)
 functions[entry]={'instructions':len(seen),'calls':[hex(t) for t in sorted(calls)],'unresolved':unknown}
 pending.extend(t for t in calls if 0x8000000<=t<0x8800000)
 if len(functions)>2000:raise RuntimeError('closure limit')
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'functions':{hex(k):v for k,v in functions.items()},'input_literal_references':inputrefs,'input_reset16100_reachable':0x8016100 in functions,'scope':'Static reachable Thumb direct-call closure from challenge restart bootstrap helpers and knownAAD2 descriptor target. Indirectcalls/computedPC unresolved; PC literal references do not prove memory write ranges or ARM/swi callees. Not full restart input ownership qualification.'}
p=Path('outputs/crazy-taxi-native/reports/bound-restart-input-callee-static-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');print('functions',len(functions),'input reset reachable',report['input_reset16100_reachable'],'inputrefs',inputrefs,'unresolved count',sum(len(v['unresolved']) for v in functions.values()))
