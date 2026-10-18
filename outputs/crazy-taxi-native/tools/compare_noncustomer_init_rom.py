"""Actual complete1E9D0 normal valid-index spawn; ground is an explicit contract."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x801e9d0;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x801eaa0,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class SpawnProbe(InitProbe):
 def external(self,t):
  assert t==0x8029b44,hex(t)
  self.ground_calls+=1;self.ground_input=[signed(self.read(self.reg[0]+j*4)) for j in range(3)];self.reg[0]=self.height&MASK
cases=[(seed,kind,int.from_bytes(r[0x7ec068+(kind-10)*4:0x7ec06c+(kind-10)*4],'little'),h) for seed in [0,165,255] for kind in [10,11,12] for h in [-2147483648,0,19264,2147483647]]
rows=subprocess.run([str(root/'bin/noncustomer-init-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases)
steps=0
for case,row in zip(cases,rows):
 seed,kind,bitmap,h=case
 for slot in [0,1,63]:
  p=SpawnProbe((0,0,0,0,0,1,0,0,0,0));p.regions[0x2004310]=bytearray(4096);actor=0x2004310+slot*64;p.regions[0x10011000]=bytearray(32)
  p.scalars[0x200430c]=slot;p.ground_calls=0;p.height=h
  p.regions[0x2004310][slot*64:(slot+1)*64]=bytes((seed+j*37)&255 for j in range(64))
  prior_y=signed(p.read(actor+12));p.write(0x10011003,kind,1);p.write(0x10011004,123456789);p.write(0x1001100c,0xfedcba98)
  p.reg[0]=0x10011000;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  actual=bytes(p.regions[0x2004310][slot*64:(slot+1)*64]);assert actual==bytes.fromhex(row),(case,slot,actual.hex(),row)
  assert p.ground_calls==1 and p.ground_input==[123456789,prior_y,signed(0xfedcba98)]
  assert p.scalars[0x200430c]==slot+1;steps+=p.steps
report={'status':'PASS','cases':len(cases)*3,'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole1E9D0 source valid-slot paths0/1/63 vs native resolved initializer, all64bytes incl retained ground hint, bitmap and dimensions. Original count increment checked. Ground29B44 explicit contract. Source kinds10/11/12 tested; other table kinds and full pool branch/lifecycle/render remain unqualified. No source RNG function calls on these paths.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/noncustomer_init.c',root/'src/noncustomer_init.h',Path(__file__)]}}
(root/'reports/noncustomer-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
