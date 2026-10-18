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
  assert t==0x8032488,hex(t)
  a,b=ns['signed'](self.reg[0]),ns['signed'](self.reg[1]);assert b
  self.reg[0]=ns['trunc'](a,b)&0xffffffff
for lo,hi in [(0x2c688,0x2c780),(0x2c834,0x2c83a),(0xe5a8,0xedf2)]:
 todo=[0x8000000+lo];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert 0x8000000+lo<=a<0x8000000+hi,hex(a);seen.add(a)
  i=next(ns['md'].disasm(ns['r'][a-0x8000000:a-0x8000000+4],a,count=1));ns['allowed_thumb'][a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic not in ['bl','bic','bics']:todo.append(i.operands[0].imm)

rows=[]
for entry in [0x802c688,0x802c834,0x800e5a8]:
 for seed in [0,165,255]:
  p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536),0x5000000:bytearray(1024)};p.scalars={}
  for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
  p.regions[0x3000000][0x69c0:0x6a28]=ns['r'][0x7ebd98:0x7ebe00]
  p.write(0x3006810,0x30069c0)
  p.write(0x2007542,240,2);p.write(0x2007544,160,2)
  p.reg[:4]=[31,0,0,0] if entry==0x802c688 else [0x200344c,0,0,0] if entry==0x802c834 else [1,32,1,1]
  before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]) and not any(a<0x200351e and a+n>0x20034dc for a,n in p.writes)
  rows.append({'entry':hex(entry),'seed':seed,'instructions':p.steps,'writes':[[hex(a),n] for a,n in p.writes if a<0x10000000]})
report={'status':'PASS_ORIGINAL_BOUND_WINDOW_FADE_INPUT_RETAINED','cases':len(rows),'instructions':sum(x['instructions'] for x in rows),'observations':rows,'scope':'Whole2C688(31,0,0),2C834(actor+E18),E5A8(1,32,1,1) returns with66inputbytes retained/nooverlappingstores. E5A8 fixture registeredpalettecount0; doesnotcoverloadedpalettecopy path orcompleteoriginalbootstrap. Divisioncallee ifcalled register-onlycontract,hardwarewrite memorycontracts.'}
(root/'reports/bound-restart-window-fade-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),report['instructions'])
