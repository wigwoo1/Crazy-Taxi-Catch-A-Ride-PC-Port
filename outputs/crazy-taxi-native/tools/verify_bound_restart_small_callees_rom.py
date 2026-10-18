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
 def external(self,t):raise AssertionError(('unexpected callee',hex(t)))
rows=[]
for entry in [0x800f5e8,0x8007378,0x80073c0,0x8026440]:
 for seed,count,mode in itertools.product([0,165,255],[0,1,3],[0,3,4,5]):
  p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={};p.hardware={};p.writes=[]
  for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
  p.write(0x2001780,count,2);p.write(0x2001537,mode,1);p.write(0x2007530,mode,2);p.write(0x2007532,mode,2)
  p.writes=[];before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert before==bytes(p.regions[0x2000000][0x34dc:0x351e])
  assert not any(a<0x200351e and a+n>0x20034dc for a,n in p.writes)
  rows.append({'entry':hex(entry),'input_seed':seed,'pool_count':count,'submode':mode,'instructions':p.steps,'writes':[[hex(a),n] for a,n in p.writes if a<0x10000000]})
report={'status':'PASS_COMPLETE_SMALL_RESTART_CALLEES_INPUT_RETAINED','cases':len(rows),'thumb_instructions':sum(x['instructions'] for x in rows),'observations':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'Whole originalF5E8/7378/73C0/26440 complete returning bodies, noexternalcontracts;66inputbytes retained andnooverlappingstores. Poolcounts0/1/3 andsubmodes0/3/4/5. Hardware writes recorded memory contracts, notdevices. Remaining bootstrapcallee/ARM/bulk-fillinputownership andoriginalrestart service scheduling unqualified.'}
(root/'reports/bound-restart-small-callees-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['cases'],report['thumb_instructions'])
