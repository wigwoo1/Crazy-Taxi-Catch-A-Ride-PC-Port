"""Whole26440 source blend/clock reset, logical hardware owners."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x26440:0x26468],0x8026440):allowed_thumb[i.address]=bytes(i.bytes)
class ResetProbe(ExitProbe):
 def read(self,a,size=4):
  if a in [0x4000050,0x4000052]:return self.hardware[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in [0x4000050,0x4000052]:self.hardware[a]=v&((1<<(size*8))-1)
  else:super().write(a,v,size)
rng=random.Random(0x26440)
cases=[(a,b,c,123,0xffffffff) for a in [0,1,16,255,256,65535] for b in [0,1,16,256,65535] for c in [0,0xffffffff]]
cases += [tuple(rng.getrandbits(32) for _ in range(5)) for _ in range(128)]
cp=subprocess.run([str(root/'bin/presentation-reset-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (a,b,current,previous,delta),want in zip(cases,rows):
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.hardware={0x4000050:0xabcd,0x4000052:0xdcba};p.reg[14]=0x10020001
 for addr,value,size in [(0x20000e0,a&65535,2),(0x20000e2,b&65535,2),(0x2003520,current,4),(0x2003524,previous,4),(0x2003b00,delta,4)]:p.write(addr,value,size)
 p.run(entry=0x8026440,stop=0x10020000)
 actual=[p.hardware[0x4000050],p.hardware[0x4000052],p.read(0x2003520),p.read(0x2003524),p.read(0x2003b00)]
 assert actual==want,((a,b,current,previous,delta),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Whole26440 blend control1f4f, alpha halfword shift/OR/truncation, delta1 and previous=current. Logical retained hardware values only; does not clear or advance a fade. No monitor/frame/audio timing proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/presentation_service.c','src/presentation_service.h','tests/presentation_reset_rom_compare.c','tools/compare_presentation_reset_rom.py']})
(root/'reports/presentation-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
