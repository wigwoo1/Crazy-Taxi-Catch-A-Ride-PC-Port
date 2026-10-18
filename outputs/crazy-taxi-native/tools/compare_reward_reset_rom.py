"""Complete original2954C with explicit byte-fill1E784 contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x2954c:0x29574],0x802954c):allowed_thumb[i.address]=bytes(i.bytes)
class ResetProbe(ExitProbe):
 def external(self,target):
  assert target==0x801e784 and self.reg[0]==0x2009abc and self.reg[1]==0 and self.reg[2]==560,(target,self.reg[:3])
  self.calls.append(target)
  for j in range(560):self.write(0x2009abc+j,0,1)
cases=list(range(256))
cp=subprocess.run([str(root/'bin/reward-reset-rom-compare.exe')],input=''.join(str(c)+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for seed,want in zip(cases,rows):
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[14]=0x10020001
 for a,v in [(0x2009aa5,seed+1),(0x2009aa4,seed+2),(0x2009aa6,seed+3),(0x2009aa7,seed+4),(0x20025d6,seed+5)]:p.write(a,v&255,1)
 for a,v in [(0x2009aa8,seed*12345),(0x2009aac,seed*6789),(0x20016a0,seed*987)]:p.write(a,v)
 for j in range(560):p.write(0x2009abc+j,seed,1)
 for j in range(12):p.write(0x2009ab0+j,153,1)
 p.run(entry=0x802954c,stop=0x10020000)
 actual=[p.read(a,1) for a in [0x2009aa5,0x2009aa4,0x2009aa6]]+[p.read(0x2009aa8),p.read(0x2009aac),p.read(0x2009aa7,1),p.read(0x20025d6,1),p.read(0x20016a0)]+[p.read(0x2009abc+j,1) for j in range(560)]
 assert actual==want,(seed,actual[:8],want[:8]);assert len(p.calls)==1
 assert all(p.read(0x2009ab0+j,1)==153 for j in range(12));steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete2954C original caller: five scalar owners cleared plus exactly560 tip bytes; explicit1E784 byte-fill contract. Active count9AA7, voice25D6, pending cash16A0 and gap12byte sentinels retained. Full session initialization, helper-body byte-fill execution and playable Retry not covered.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/reward_session.c','src/reward_session.h','tests/reward_reset_rom_compare.c','tools/compare_reward_reset_rom.py']})
(root/'reports/reward-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
