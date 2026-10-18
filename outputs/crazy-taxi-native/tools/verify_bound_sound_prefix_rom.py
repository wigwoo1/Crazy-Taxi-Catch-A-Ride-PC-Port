"""Original Bound sound service prefix; offline ordered-call boundaries only."""
from pathlib import Path
import json,hashlib,struct
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
r=ns['r'];md=ns['md']
for i in md.disasm(r[0x8d60:0x8d8e],0x8008d60):ns['allowed_thumb'][i.address]=bytes(i.bytes)
phase=struct.unpack_from('<I',r,0x8e54)[0];outer=struct.unpack_from('<I',r,0x8e58)[0]
assert phase==0x200153a and outer==0x2001ea8,(hex(phase),hex(outer))
class PrefixProbe(ns['InitProbe']):
 def external(self,target):
  assert target in [0x80206bc,0x8010b54,0x800dec8,0x8021c5c,0x800ba04],hex(target)
  self.calls.append(hex(target));self.reg[0]=0
cases=[]
for phase_value in [0,1,2]:
 for outer_value in [0,1]:
  for ended in [0,1]:
   p=PrefixProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
   p.write(phase,phase_value,1);p.write(outer,outer_value,1);p.write(0x2001697,ended,1)
   p.reg[13]=0x10000400;p.reg[14]=0x10020001
   stop=0x8008f10 if phase_value else 0x8008f1c if outer_value else 0x8008d8e
   p.run(entry=0x8008d60,stop=stop)
   expected=[] if phase_value else ['0x80206bc','0x8010b54']+([] if outer_value else ['0x800dec8','0x8021c5c','0x800ba04'])
   assert p.calls==expected,(phase_value,outer_value,ended,p.calls)
   cases.append(dict(phase=phase_value,outer=outer_value,ended=ended,calls=p.calls,instructions=p.steps))
report=dict(status='PASS_BOUND_SOUND_PREFIX_ORDER',cases=cases,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original8D60 through phase/outer gate before scalar controller. Exact ordered calls with explicit no-state callee contracts; ended1697 does not suppress10B54, outer is checked after sound service, phase nonzero bypasses prefix. Callee numerical effects/channel behavior/full caller loop and host audio timing not proved.')
(root/'reports/bound-sound-prefix-order-rom.json').write_text(json.dumps(report,indent=2));print('PASS12 Bound phase/outer/ended sound-prefix cases.')
