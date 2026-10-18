"""Finite actual1E1F0 setup; stop at first persistent dispatcher call."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for entry,limit,stop in [(0x801e1f0,0x801e24a,0x801e24c),(0x8028f64,0x8028f82,None),(0x8028f84,0x8028f9a,None),(0x8028fa0,0x8028fb0,None),(0x8028fb8,0x8028fe0,None)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a==stop or a in seen:continue
  assert entry<=a<=limit,hex(a);seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
font_owner=struct.unpack_from('<I',r,0x28f9c)[0];h=struct.unpack_from('<H',r,0x28faa)[0];selected=struct.unpack_from('<I',r,((0x28faa+4)&~3)+(h&255)*4)[0]
assert font_owner==0x2009a8c
class StartupProbe(InitProbe):
 def external(self,target):
  assert target in [0x800ffe0,0x8028f64,0x8028f84,0x8028fb8,0x8028fa0,0x8016250,0x8016100,0x8010298],hex(target)
  if target==0x8028f84:self.run(entry=target,stop=self.reg[14]&~1);return
  a=self.reg[0] if target in [0x8028fb8,0x8028fa0,0x8016250,0x8010298] else 0;b=self.reg[1] if target in [0x8028fb8,0x8010298] else 0
  self.calls.extend([target,a,b])
  if target in [0x8028f64,0x8028fb8,0x8028fa0]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8016250:self.write(0x2001ea5,self.screen_gate,1)
  elif target==0x8010298:self.write(0x2001ea5,self.save_gate,1)
cases=[(gate,screen,save,seed) for gate in range(256) for screen,save,seed in [(0,0,0),(1,255,165),(255,1,255),(0,255,12345)]]
rows=subprocess.run([str(root/'bin/startup-init-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 gate,screen,save,seed=case;p=StartupProbe((0,0,0,0,0,1,0,0,0,0));p.regions={font_owner:bytearray(24)};p.scalars={0x2001ea5:gate,0x2001ea4:seed&255,0x2003892:seed&255,0x2003878:seed,selected:seed&255};p.calls=[];p.screen_gate=screen;p.save_gate=save
 for i in range(6):p.write(font_owner+i*4,seed+i*12345)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x801e1f0,stop=0x801e24c)
 got=[p.read(a) for a in [0x2001ea5,0x2001ea4,0x2003892,selected,0x2003878]]+[p.read(font_owner+i*4) for i in range(6)]+[len(p.calls)//3]+p.calls
 assert got==list(map(int,line.split())),(case,got,line);steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual1E1F0 entry..1E24A finite setup plus actual28F64/28F84 font reset,28FB8 register,28FA0 select. All256 retained gatebytes, screen0 gate mutation/save gate mutation and retained font slots; ordered defaults/resource/screen/input-reset/save(1,1) requests, initialized1/callback1E275/childsignal0. Stop BEFOREpersistentdispatcher; defaultsFFE0/screen16250/input16100/save10298 bodies contracts, no fullstartup/livebinding claim.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_init.c',root/'src/startup_init.h',root/'tests/startup-init-rom-compare.c',Path(__file__),root/'bin/startup-init-rom-compare.exe']}}
(root/'reports/startup-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
