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
  assert t==0x802c9f0,hex(t);self.calls.append(t)
for i in ns['md'].disasm(ns['r'][0xd764:0xd8a8],0x800d764):ns['allowed_thumb'][i.address]=bytes(i.bytes)
rows=[]
for settings,seed in itertools.product(range(128,160),[0,165,255]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={};p.calls=[]
 for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
 p.write(0x2001eb0,settings,1);before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800d764,stop=0x10020000)
 assert p.calls==[0x802c9f0] and before==bytes(p.regions[0x2000000][0x34dc:0x351e])
 rows.append({'settings':settings,'seed':seed,'instructions':p.steps})
report={'status':'PASS_ORIGINAL_CHALLENGE_LOADING_INPUT_GATE','cases':len(rows),'instructions':sum(x['instructions'] for x in rows),'scope':'WholeD764 returning challenge-settings128..159 branch; initial2C9F0clear explicit no-state contract. ActualD788->D89C skips26318 hence noinputsampler through227AC->D764 in challenge mode. Does not proveclearcallee/bulkcopies orcompletebootstrapinputownership.'}
(root/'reports/bound-restart-loading-input-gate-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows))
