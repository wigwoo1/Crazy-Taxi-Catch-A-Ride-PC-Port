"""Original E5A8..E8EC all mask/channel normalization paths."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
import random
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
todo=[0x0800e5a8];seen=set()
while todo:
 a=todo.pop()
 if a==0x0800e8ee or a in seen:continue
 assert 0x0800e5a8<=a<0x0800e8ee,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.group(CS_GRP_JUMP):todo.append(i.operands[0].imm)
class SetupProbe(ExitProbe):
 def __init__(self,*args):self.globals=bytearray([0xa5]*32);super().__init__(*args)
 def read(self,a,size=4):
  if 0x02001770<=a<0x02001790:return int.from_bytes(self.globals[a-0x02001770:a-0x02001770+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x02001770<=a<0x02001790:self.globals[a-0x02001770:a-0x02001770+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
values=[0,1,2,3,4,8,16,32,64,128,256,512,1024,0x1000,0x2000,0xffff]+[3|v for v in [4,8,16,32,64,128,256]]
cases=[(mode,cap,bg,obj) for mode,cap in [(0,0),(1,32),(256,33),(257,255)] for bg,obj in zip(values,reversed(values))]
rng=random.Random(0xe5a8);cases += [tuple(rng.getrandbits(32) for _ in range(4)) for _ in range(256)]
run=subprocess.run([str(root/'bin/palette-fade-setup-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,line.split())) for line in run.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 probe=SetupProbe((0,0,0,0,0,1,0,0,0,0));probe.globals=bytearray([0xa5]*32);before=bytes(probe.globals)
 probe.reg[:4]=case;probe.reg[13]=0x10000400;probe.reg[14]=0x10020001
 probe.run(entry=0x0800e5a8,stop=0x0800e8ee)
 got=[probe.reg[7],probe.reg[8],probe.read(0x02001774,1),probe.read(probe.reg[13]+0x200),probe.reg[10]]+[probe.read(a,1) for a in range(0x02001775,0x0200177b)]
 assert got==want,(case,got,want)
 expected=bytearray(before);expected[4:11]=bytes([want[2]]+want[5:]);assert probe.globals==expected
 steps+=probe.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete E5A8..E8EC prefix all mask/channel paths, lowbyte mode/cap, ushort masks, priority bit rules, endpoint reset and unrelated32globals bytes retained including1770. Remaining mode-dependent setup/palette processing/hardware writes excluded; no inferred live flags.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-fade-setup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
