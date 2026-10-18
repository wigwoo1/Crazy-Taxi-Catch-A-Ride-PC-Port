"""AE48..AE72 game setup caller with explicit mutating helper contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0xae48:0xae72],0x800ae48):allowed_thumb[i.address]=bytes(i.bytes)
helpers=[0x8014a58,0x8006580,0x800b6ec,0x800df8c,0x80227ac,0x8011490,0x802954c,0x8021c10]
class SetupProbe(ExitProbe):
 def external(self,target):
  assert target==helpers[len(self.calls)],hex(target)
  if target==0x8014a58:assert self.reg[0]==0
  self.calls.append([target,self.read(0x20034d4),self.read(0x20034d8)])
  n=len(self.calls);self.write(0x20034d4,(self.read(0x20034d4)+n)&0xffffffff);self.write(0x20034d8,self.read(0x20034d8)^n)
  self.reg[0]=0
values=[0,1,2,128,65535,0x7fffffff,0x80000000,0xffffffff]
cases=[(sl,a,b) for sl in values for a,b in [(0,0),(1,255),(0xffffffff,0x80000000)]]
cp=subprocess.run([str(root/'bin/normal-restart-setup-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (sl,a,b),want in zip(cases,rows):
 p=SetupProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[10]=sl;p.write(0x20034d4,a);p.write(0x20034d8,b)
 p.run(entry=0x800ae48,stop=0x800ae72)
 actual=[len(p.calls),*[v for call in p.calls for v in call],p.read(0x20034d4),p.read(0x20034d8)]
 assert actual==want,((sl,a,b),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual AE48..AE72 writes both frame-owner words from retained sl before vehicle14A58(0),traffic6580,sessionB6EC,destinationDF8C,passengers227AC,audio11490,2954C,effect21C10. Explicit mutating opaque helper contracts test write timing and no later caller overwrite; helper bodies and full restart are not executed.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_restart.c','src/normal_restart.h','tests/normal_restart_setup_rom_compare.c','tools/compare_normal_restart_setup_rom.py']})
(root/'reports/normal-restart-setup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
