"""Ordinary original AE72..AF80 entry with explicit fade/frame contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
# Reachable source excludes AF0C..AF5A literal island.
for start,end in [(0xae72,0xaf0c),(0xaf5c,0xaf80)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
def state(p):return [p.read(0x2007542,2),p.read(0x2007544,2),p.read(0x20034d4),p.read(0x20034d8),p.read(0x4000010,2),p.read(0x4000012,2),p.read(0x200177b,1)]
class EntryProbe(ExitProbe):
 def read(self,a,size=4):
  if a in [0x4000010,0x4000012]:
   assert size==2;return self.io[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in [0x4000010,0x4000012]:
   assert size==2;self.io[a]=v&65535;return
  return super().write(a,v,size)
 def external(self,target):
  used={0x8026440:0,0x800e5a8:4,0x800d764:0,0x802c9f0:4,0x8026318:0,0x801ee28:0,0x802277c:0}
  assert target in used,hex(target);n=used[target];args=[self.reg[j] if j<n else 0 for j in range(4)]
  self.calls.append([target,*args,*state(self)])
  if target==0x800e5a8:self.left=1 if args[0] else self.wait;self.write(0x200177b,int(self.left!=0),1)
  if target==0x800d764:self.left=max(0,self.left-1);self.write(0x200177b,int(self.left!=0),1)
  if target==0x8026318:
   for a in [0x2007542,0x2007544]:self.write(a,(self.read(a,2)+1)&65535,2)
  self.reg[0]=0
cases=[(wait,w,h,sl) for wait in [0,1,2,33] for w,h in [(240,160),(0,0),(65535,65535)] for sl in [0,1,0xffffffff]]
cp=subprocess.run([str(root/'bin/normal-restart-entry-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (wait,w,h,sl),want in zip(cases,rows):
 p=EntryProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.io={};p.wait=wait;p.left=0;p.reg[10]=sl;p.write(0x2001708,0,1)
 for a,v,size in [(0x20034d4,123,4),(0x20034d8,456,4),(0x2007542,w,2),(0x2007544,h,2),(0x4000010,55,2),(0x4000012,77,2),(0x200177b,0,1)]:p.write(a,v,size)
 p.run(entry=0x800ae72,stop=0x800af80)
 actual=[len(p.calls),*[v for call in p.calls for v in call],*state(p)]
 assert actual==want,((wait,w,h,sl),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete reachable ordinary mode0 AE72..AF80 caller, fadeOUT wait0/1/2/33 actualD764 calls, dimensions reread after opaque26318 mutation, paired frame-owner writes, intro/passenger helpers, scroll clear, ready while city fade active. Helper bodies/fade algorithm, graphics setup beforeAE72 and complete playable Retry excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_restart_entry.c','src/normal_restart_entry.h','tests/normal_restart_entry_rom_compare.c','tools/compare_normal_restart_entry_rom.py']})
(root/'reports/normal-restart-entry-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
