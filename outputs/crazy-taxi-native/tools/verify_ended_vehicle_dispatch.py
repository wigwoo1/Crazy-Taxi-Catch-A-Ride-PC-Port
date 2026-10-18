"""Execute bounded original B29C ended/1534 vehicle-dispatch branches."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_vehicle_rom.py').read_text().split('\ncases=[')[0],ns)
ns['allowed_thumb'].update({i.address:bytes(i.bytes) for i in ns['md'].disasm(ns['r'][0xb29c:0xb312],0x0800b29c)})
class Dispatch(ns['MotionProbe']):
 def write(self,a,v,n=4):
  if a==0x030053c0 and n==4:self.arm.mem[a]=v&0xffffffff
  else:super().write(a,v,n)
 def external(self,target):
  assert target in [0x08011a3c,0x08015ac8,0x080145e4,0x08020d2c,0x08021078,0x08021220,0x080214f8,0x080208d4],hex(target)
  self.calls.append(target)
cases=[]
for ended in [0,1,255]:
 for flag in [0,1,255]:
  p=Dispatch((0,0,0,0,0,1),0);p.calls=[]
  p.arm.mem[0x02001694]=ended<<24;p.arm.mem[0x02001534]=flag;p.arm.mem[0x02003b00]=5
  p.reg[7]=p.base;p.reg[9]=0x02001534;p.reg[14]=0x10020001;p.run(entry=0x0800b29c,stop=0x0800b312)
  expected=[] if not ended and not flag else [0x08011a3c,0x08015ac8,0x080145e4,0x08020d2c,0x08021078,0x08021220,0x080214f8,0x080208d4]
  assert p.calls==expected,(ended,flag,p.calls)
  cases.append(dict(ended=ended,shared1534=flag,calls=[hex(t) for t in p.calls],thumb_instructions=p.steps))
report=dict(status='PASS_BOUNDED_DISPATCH',cases=cases,scope='Executed actualROMB29C..B310 branch instructions;1697/1534 combinations andordered vehicle/body/effect callbacks. Callbackbodies are explicit no-op contracts, no physicalstateeffects/fullenginepriorentry/cycles proof.')
(root/'reports/ended-vehicle-dispatch-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'dispatch cases')
