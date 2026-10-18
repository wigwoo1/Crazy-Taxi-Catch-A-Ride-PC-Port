"""Actual AC70..ACC6 prefix with explicit helper and RNG-return contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0xac70:0xacc6],0x800ac70):allowed_thumb[i.address]=bytes(i.bytes)
assert int.from_bytes(r[0x7ec048:0x7ec04c],'little')==0x8421fc0
assert list(r[0x7ec060:0x7ec064])==[63,88,115,34]
owners=[0x2001691,0x200153c,0x200153d]
class RestartProbe(ExitProbe):
 def external(self,target):
  args={0x8027730:0,0x8026ef8:1,0x801e72c:2,0x8027110:1,0x800f5e8:0,0x8025c18:2,0x8007378:0,0x80073c0:0}
  assert target in args,hex(target)
  n=args[target];self.calls.append([target,self.reg[0] if n else 0,self.reg[1] if n>1 else 0,*[self.read(a,1) for a in owners]])
  self.reg[0]=self.random if target==0x801e72c else 0
cases=[(random,*flags) for random in list(range(512))+[32767,32768,65534,65535] for flags in [(0,0,0),(1,255,128),(255,1,254)]]
cp=subprocess.run([str(root/'bin/normal-restart-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 p=RestartProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.random=case[0]
 p.reg[13]=0x10000400
 for a,v in zip(owners,case[1:]):p.write(a,v,1)
 # Retained selection/settings are outside this prefix's write ownership.
 for a,v in [(0x2001eb4,1),(0x2001eb1,3),(0x2001ea8,1),(0x2005f20,1)]:p.write(a,v,1)
 p.run(entry=0x800ac70,stop=0x800acc6)
 actual=[len(p.calls),*[v for call in p.calls for v in call],*[p.read(a,1) for a in owners]]
 assert actual==want,(case,actual,want)
 for a,v in [(0x2001eb4,1),(0x2001eb1,3),(0x2001ea8,1),(0x2005f20,1)]:assert p.read(a,1)==v
 steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual AC70..ACC6 original prefix; ordered helper targets/used arguments and owner bytes at each call plus final owners exact. RNG return injected under original 0..65535 range contract; RNG algorithm and all called helper side effects excluded. Retained byte fixtures unchanged by prefix. Later initialization/loading/fade/vehicle/passenger resets and playable Retry not covered.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_restart.c','src/normal_restart.h','tests/normal_restart_rom_compare.c','tools/compare_normal_restart_rom.py']})
(root/'reports/normal-restart-prefix-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
