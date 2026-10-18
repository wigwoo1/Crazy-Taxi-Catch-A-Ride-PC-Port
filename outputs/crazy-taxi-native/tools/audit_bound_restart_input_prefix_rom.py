from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for i in ns['md'].disasm(ns['r'][0x743c:0x7538],0x800743c):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['InitProbe']):
 def write(self,a,v,size=4):
  if 0x4000000<=a<0x4000400:self.hardware[a]=v;return
  super().write(a,v,size)
 def external(self,t):
  assert t in [0x800f5e8,0x8025c18,0x8007378,0x80073c0,0x8027994,0x800fec8,0x800e154,0x800e5a8,0x802ac6c,0x802c834,0x802c688,0x80203c4,0x803244c,0x8027730,0x8026ef8,0x801e72c,0x8027110,0x8026440],hex(t)
  self.calls.append(hex(t))
  if t==0x801e72c:self.reg[0]=0
rows=[]
for seed in [0,165,255]:
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([seed])*262144};p.scalars={0x3004c9c:0x84a4ff4,0x3004ca0:0x2002634};p.calls=[];p.hardware={};p.reg[13]=0x10000400;p.write(0x10000444,0x10000428)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.run(entry=0x800743c,stop=0x8007538)
 after=bytes(p.regions[0x2000000][0x34dc:0x351e]);assert before==after
 assert [p.read(a,1) for a in [0x2001691,0x200153c,0x200153d,0x200153a]]==[0]*4
 assert p.read(0x20034d4)==p.read(0x20034d8)==1
 rows.append({'seed':seed,'instructions':p.steps,'calls':p.calls,'input66_retained':True,'cleared_bytes':['1691','153c','153d','153a'],'time_owners34d4_34d8':1})
report={'status':'PASS_DIRECT_RESTART_PREFIX_INPUT_RETAINED','cases':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'Actual743C..7536 original prefix direct stores:66inputbytes retained, success/status cleared, time globals1. ALL external helpers includinggraphics/time/music/descriptor initialization use explicit no-state contracts, RNG returns0; therefore NOT full bootstrap/callee input ownership proof and does not authorize changing reset based on this alone.'}
(root/'reports/bound-restart-input-prefix-rom-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS direct prefix; external callees remain unqualified')
