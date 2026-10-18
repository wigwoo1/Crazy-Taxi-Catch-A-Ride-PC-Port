"""Finite5404 prefix stopping before its persistent1E1F0 transfer."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
todo=[0x8005404];seen=set()
while todo:
 a=todo.pop()
 if a==0x800543c or a in seen:continue
 assert 0x8005404<=a<=0x800543a,hex(a);seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class BootProbe(InitProbe):
 def external(self,target):
  assert target in [0x801e2e4,0x8010a54,0x8026488,0x8016208],hex(target)
  self.calls.extend([target,self.reg[0] if target in [0x8026488,0x8016208] else 0])
  if target==0x801e2e4:self.write(0x2001ea5,self.system_gate,1)
  if target==0x8026488:self.write(0x2007560,1,1)
  if target==0x8016208:self.write(0x2001ea5,self.wait_gate,1)
cases=[(system,wait,music,effects) for wait in range(256) for system,music,effects in [(0,0,0),(1,1,255),(255,255,1),(0,128,64)]]
rows=subprocess.run([str(root/'bin/startup-boot-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 system,wait,music,effects=case;p=BootProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};p.scalars={0x2001ea4:165,0x2001ea5:165,0x2007560:255,0x2007600:music,0x2007602:effects,0x2001ea6:255};p.calls=[];p.system_gate=system;p.wait_gate=wait;p.reg[13]=0x10000400;p.run(entry=0x8005404,stop=0x800543c)
 got=[p.read(a) for a in [0x2001ea4,0x2001ea5,0x2007560,0x2007600,0x2007602,0x2001ea6]]+[len(p.calls)//2]+p.calls
 assert got==list(map(int,line.split())),(case,got,line);steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual5404 entry..543A before1E1F0 transfer: initialized0/processing0, ordered systeminit/codebase/audioselector2/wait10 requests, late retainedgate0 enables bothdomains otherwisepreservesbytes, mainmode1. All256 post-wait gatebytes and4retained domain/system fixtures. Helper bodies contracts; not completebootstrap/system/save/dispatcher composition or liveviewer proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_boot.c',root/'src/startup_boot.h',root/'tests/startup-boot-rom-compare.c',Path(__file__),root/'bin/startup-boot-rom-compare.exe']}}
(root/'reports/startup-boot-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
