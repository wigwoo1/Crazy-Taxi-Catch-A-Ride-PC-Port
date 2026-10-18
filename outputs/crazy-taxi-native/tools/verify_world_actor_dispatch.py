"""Execute original world actor dispatcher; callback bodies explicit contracts."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_vehicle_rom.py').read_text().split('\ncases=[')[0],ns)
ns['allowed_thumb'].update({i.address:bytes(i.bytes) for i in ns['md'].disasm(ns['r'][0x15ac8:0x15af8],0x08015ac8)})
class Dispatcher(ns['MotionProbe']):
 def external(self,target):
  assert target==0x08011f80,hex(target)
  self.calls.append((self.reg[0],self.reg[1]))
  if self.change is not None and len(self.calls)==1:self.arm.mem[0x020034d4]=self.change
rows=[]
for count,change in [(0,None),(1,None),(2,None),(8,None),(3,1),(1,3)]:
 p=Dispatcher((0,0,0,0,0,1),0);p.calls=[];p.change=change;p.arm.mem[0x020034d4]=count
 p.reg[14]=0x10020001;p.run(entry=0x08015ac8,stop=0x10020000)
 expected_count=count if change is None or count==0 else change
 expected=[(i,0x02002634+i*3744) for i in range(expected_count)]
 assert p.calls==expected,(count,change,p.calls,expected)
 rows.append(dict(initial_count=count,count_after_first_callback=change,calls=[dict(index=i,actor=hex(a)) for i,a in p.calls],thumb_instructions=p.steps))
assert len(rows)==6
report=dict(status='PASS_EXECUTED_DISPATCH',cases=rows,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),scope='Actual15AC8 dispatcher instructions through return; original02002634 actor base,3744stride,index argument and live count020034D4 reload. Original11F80 callback body is a no-op except explicit live-count mutation contract. No joined original physics, prior engine scheduling, camera/effects or full trajectory claim.')
(root/'reports/world-actor-dispatch-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(rows),'executed world dispatcher cases')
