from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for lo,hi in [(0xf5e8,0xf630),(0x7378,0x73bc),(0x73c0,0x7402),(0x26440,0x26468)]:
 for i in ns['md'].disasm(ns['r'][lo:hi],0x8000000+lo):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['InitProbe']):
 def __init__(self,*args):
  self.hardware={};self.writes=[];super().__init__(*args)
 def read(self,a,size=4):
  if 0x4000000<=a<0x4000400:return self.hardware.get(a,0)
  return super().read(a,size)
 def write(self,a,v,size=4):
  self.writes.append((a,size))
  if 0x4000000<=a<0x4000400:self.hardware[a]=v;return
  super().write(a,v,size)
 def external(self,t):
  assert t in [0x801e784,0x801e7f0],hex(t);self.run(entry=t,stop=self.reg[14]&~1)
for lo,hi in [(0x2277c,0x22796),(0x11490,0x114f0),(0x21c10,0x21c28),(0xdf8c,0xdf9c)]:
 for i in ns['md'].disasm(ns['r'][lo:hi],0x8000000+lo):ns['allowed_thumb'][i.address]=bytes(i.bytes)
rows=[]
for entry in [0x800b6ec,0x8011490,0x802277c,0x8021c10]:
 for seed,settings in itertools.product([0,165,255],[128,129,159,255]):
  p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={}
  for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
  p.write(0x2001eb0,settings,1);p.write(0x2001537,5,1);p.write(0x2007542,240,2)
  before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]) and not any(a<0x200351e and a+n>0x20034dc for a,n in p.writes)
  rows.append({'entry':hex(entry),'seed':seed,'settings':settings,'instructions':p.steps,'write_ranges':[[hex(a),n] for a,n in p.writes if a<0x10000000]})
report={'status':'PASS_WHOLE_RESTART_SESSION_RESET_INPUT_RETAINED','cases':len(rows),'instructions':sum(x['instructions'] for x in rows),'observations':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'WholeB6EC withactual1E7F0copy,11490,2277C and21C10 withactual1E784fill, input66retained/nooverlappingstores. Challenge mode/settings fixtures, noexternalno-state callees. Hardware memorywrites explicit ifany. Otherbootstrapresource/graphics/ARMhelpers andfullscheduling unqualified.'}
(root/'reports/bound-restart-session-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),report['instructions'])
