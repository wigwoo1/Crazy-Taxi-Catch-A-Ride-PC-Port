"""Static ROM control-flow evidence after the qualified Bound restart prefix."""
from pathlib import Path
import sys,struct,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
todo=[0x08007538];seen={};edges=[];calls=[];boundaries=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 if not 0x08007538<=a<0x080079d2:boundaries.add(a);continue
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));seen[a]=i
 if i.mnemonic=='bl':calls.append({'caller':hex(a),'target':i.op_str})
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':target=int(i.op_str[1:],16);todo.append(target);edges.append([hex(a),hex(target)]);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  target=int(i.op_str[1:],16);todo.append(target);edges.append([hex(a),hex(target)])
assert seen[0x0800756e].mnemonic=='bl' and seen[0x0800756e].op_str=='#0x803244c'
assert struct.unpack_from('<I',r,0x4a4ff8)[0]==0x08008d61
assert seen[0x080079ce].mnemonic=="bl" and seen[0x080079ce].op_str=="#0x8026318"
rows=[]
for a,i in sorted(seen.items()):
 row={'address':hex(a),'mnemonic':i.mnemonic,'operands':i.op_str}
 h=struct.unpack_from('<H',r,a-0x08000000)[0]
 if h&0xf800==0x4800:
  literal=((a+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,literal-0x08000000)[0])
 rows.append(row)
report={'status':'PASS_STATIC_BOUND_FRAME_FLOW','rom_sha256':hashlib.sha256(r).hexdigest(),'instructions':rows,'calls':sorted(calls,key=lambda x:x['caller']),'branch_edges':edges,'boundaries':[hex(a) for a in sorted(boundaries)],'frame_callback':'0x08008d60','scope':'Reachable static caller CFG from7538 bounded before79D2; external calls not executed, caller iteration/frame service/timing not verified. Descriptor+4 identity and756E callback dispatch checked. This does not authorize live frame integration.'}
(root/'reports/bound-restart-frame-flow.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],len(rows),'instructions',report['calls'],report['boundaries'])
