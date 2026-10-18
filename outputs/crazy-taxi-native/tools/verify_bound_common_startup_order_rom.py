from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for i in ns['md'].disasm(ns['r'][0xabc4:0xabec],0x800abc4):ns['allowed_thumb'][i.address]=bytes(i.bytes)
expected=[0x8014a58,0x800b6ec,0x800df8c,0x80227ac,0x8011490,0x801ee28,0x802277c,0x8021c10]
class Probe(ns['InitProbe']):
 def external(self,target):
  assert target==expected[len(self.calls)],(target,self.calls)
  if not self.calls:assert self.reg[0]==0
  self.calls.append(target)
p=Probe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800abc4,stop=0x10020000);assert p.calls==expected
(root/'reports/bound-common-startup-order-rom.json').write_text(json.dumps({'status':'PASS_ORIGINAL_CALL_ORDER','calls':[hex(a) for a in p.calls],'instructions':p.steps,'scope':'Actual wholeABC4 dispatch order with explicit no-state external callee contracts; individual vehicle/session routines separately verified, not complete joined caller state or all global ownership.'},indent=2)+'\n');print('PASS original common startup order',p.steps)
