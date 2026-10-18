from pathlib import Path
import json,struct
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
r=ns['r'];freeze=struct.unpack_from('<I',r,0x7aa0)[0];assert freeze==0x2005f20,hex(freeze)
for i in ns['md'].disasm(r[0x79ec:0x7a06],0x80079ec):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['InitProbe']):
 def external(self,target):
  assert target in [0x8010aac,0x8027754,0x802771c,0x8027730],hex(target);self.calls.append(hex(target));self.reg[0]=0
cases=[]
for frozen in [0,1,255]:
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.write(freeze,frozen,1);p.run(entry=0x80079ec,stop=0x8007a06)
 expected=['0x8010aac']+(['0x8027754','0x802771c'] if frozen else [])+['0x8027730'];assert p.calls==expected
 cases.append(dict(frozen=frozen,calls=p.calls,instructions=p.steps))
(root/'reports/challenge-exit-audio-order-rom.json').write_text(json.dumps(dict(status='PASS',cases=cases,scope='Original79EC..7A02 exact exitcleanup before frozen restoration and musicstop; explicit no-state callees, not sound channel body or whole exit qualification.'),indent=2));print('PASS original challenge cleanup order3gates')
