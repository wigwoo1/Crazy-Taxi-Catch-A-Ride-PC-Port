from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for i in ns['md'].disasm(ns['r'][0x8d70:0x8d8e],0x8008d70):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['InitProbe']):
 def external(self,target):
  assert target in [0x80206bc,0x8010b54,0x800dec8,0x8021c5c,0x800ba04];self.calls.append(hex(target))
rows=[]
for outer in [0,1,2,255]:
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.write(0x2001ea8,outer,1);p.run(entry=0x8008d70,stop=0x8008d8e if not outer else 0x8008f1c)
 expected=['0x80206bc','0x8010b54']+(['0x800dec8','0x8021c5c','0x800ba04'] if not outer else []);assert p.calls==expected,(outer,p.calls);rows.append({'outer':outer,'calls':p.calls,'instructions':p.steps})
(root/'reports/bound-pause-service-order-rom.json').write_text(json.dumps({'status':'PASS_ORIGINAL_BOUND_SERVICE_ORDER','cases':rows,'scope':'Actual8D70..8D8A including outer1EA8 branch; no-state external callees. Does not prove sampler, pause audio/hardware or countdown bodies.'},indent=2)+'\n');print('PASS Bound service order4outergates')
