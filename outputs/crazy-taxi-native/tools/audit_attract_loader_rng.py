"""Qualify synchronous attract module initialization; excludes IRQ scheduling."""
import pathlib,hashlib,json,sys
root=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(root.parents[1]/'work/python-packages'))
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
reports=[]
for entry,stop in [(0x27730,0x27738),(0x26ef8,0x2702c)]:
 pending=[entry];seen={};returns=[]
 while pending:
  pc=pending.pop()
  if pc in seen:continue
  assert entry<=pc<stop
  i=next(md.disasm(r[pc:pc+4],0x8000000+pc,count=1));seen[pc]=f'{i.mnemonic} {i.op_str}'
  assert i.mnemonic not in ('bl','blx'),(hex(pc),i.mnemonic)
  if i.mnemonic=='bx':
   assert (entry==0x27730 and pc==0x27736 and i.op_str=='lr') or (entry==0x26ef8 and pc==0x2702a and i.op_str=='r0' and r[pc-2:pc].hex()=='01bc')
   returns.append(pc);continue
  assert not (i.mnemonic in ('mov','ldr','pop') and ('pc' in i.op_str.split(',')[0] or (i.mnemonic=='pop' and 'pc' in i.op_str)))
  if i.mnemonic=='b' or i.mnemonic in ('beq','bne','bhs','blo','bge','blt','bgt','ble','bhi','bls','bpl','bmi','bvs','bvc'):
   assert i.op_str.startswith('#0x');pending.append(int(i.op_str[1:],16)-0x8000000)
   if i.mnemonic=='b':continue
  pending.append(pc+i.size)
 assert returns
 reports.append({'entry':hex(0x8000000+entry),'reachable_instructions':len(seen),'returns':[hex(x+0x8000000) for x in returns],'direct_or_indirect_calls':0,'instructions':{hex(a+0x8000000):seen[a] for a in sorted(seen)}})
output={'scope':'reachable synchronous functions only; no subcalls or tailcalls, hence no synchronous RNG calls in these intervening owners; original IRQ/music scheduling is separate and unproven','functions':reports}
(root/'reports/attract-loader-rng-audit.json').write_text(json.dumps(output,indent=2))
print(json.dumps({'scope':output['scope'],'functions':[{k:v for k,v in x.items() if k!='instructions'} for x in reports]}))
