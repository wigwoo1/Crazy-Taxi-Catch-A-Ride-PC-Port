from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_sprite_replacement_rom.py').read_text().split('\ncases=')[0],ns)
for lo,hi in [(0xf5e8,0xf630),(0x7378,0x73bc),(0x73c0,0x7402),(0x26440,0x26468)]:
 for i in ns['md'].disasm(ns['r'][lo:hi],0x8000000+lo):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['ReplaceProbe']):
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
  if t==0x801e784:self.run(entry=t,stop=self.reg[14]&~1)
  else:
   assert t==0x8027fb8,hex(t);self.requests.append([self.reg[i] for i in range(4)]);self.run(entry=t,stop=self.reg[14]&~1)
cfg=json.loads((root/'reports/bound-result-init-full-cfg.json').read_text())
ns['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
rows=[]
for seed,driver,demo in itertools.product([0,165,255],range(4),[0,1]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={};p.requests=[]
 for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
 p.write(0x200388d,driver,1);p.write(0x2001539,driver,1);p.write(0x2001690,driver,1);p.write(0x2001eb0,128,1);p.write(0x2001708,demo,1)
 p.write(0x2007e38,0x8070e50)
 for group in range(32):p.write(0x2007e3c+group*4,0x6010000)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x801ee28,stop=0x10020000)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]) and not any(a<0x200351e and a+n>0x20034dc for a,n in p.writes)
 rows.append({'seed':seed,'driver':driver,'demo':demo,'instructions':p.steps,'resource_requests':p.requests,'write_ranges':[[hex(a),n] for a,n in p.writes if a<0x10000000]})
report={'status':'PASS_WHOLE_RESULT_INIT_WITH_RESOURCE_BODY_INPUT_RETAINED','cases':len(rows),'instructions':sum(x['instructions'] for x in rows),'observations':rows,'scope':'Whole1EE28 caller andactual1E784fills, challenge128 drivers0..3 anddemo0/1;66inputbytes retained/nooverlappingownedwrites. Wholeactual27FB8sprite resource bodies executed againstactualBound8070E50package andROMsource metadata. Groupdestination bases6010000 fixtures; hardwareDMAregister requests captured, copies notexecuted. Input retained/nooverlap inallcaller/resource/clearCPUstores. Complete27994allocation/VRAMcopy destinations andremainingbootstraphelpers unqualified.'}
(root/'reports/bound-result-init-resource-input-shadow-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),report['instructions'])
