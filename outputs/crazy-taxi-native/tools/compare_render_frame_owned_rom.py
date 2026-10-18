"""Complete original 2B330 caller with actual ROM ARM callback result execution."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
import random
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
arm={};exec(open('outputs/crazy-taxi-native/tools/compare_projection_divide_rom.py').read().split('\nvalues=')[0],arm)
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
todo=[0x0802b330];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x0802b330<=a<=0x0802b440,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class FrameProbe(ExitProbe):
 def __init__(self,*args):self.graphics=bytearray([0xa5]*256);self.divisions=[];super().__init__(*args)
 def read(self,a,size=4):
  if a==0x03006940:return 0x080029fc
  if 0x03006810<=a<0x03006910:return int.from_bytes(self.graphics[a-0x03006810:a-0x03006810+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x03006810<=a<0x03006910:self.graphics[a-0x03006810:a-0x03006810+size]=(value&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,value,size)
 def external(self,target):
  assert target==0x08032454 and self.reg[2]==0x080029fc
  self.divisions.extend(self.reg[:3]);self.reg[0]=arm['original'](self.reg[0],self.reg[1])
  # Callback clobbers r1-r3; deliberately poison to expose caller dependencies.
  self.reg[1:4]=[0xbad00001,0xbad00002,0xbad00003]
cases=[(0,0,240,160,120,80,192,256,38400,0x12345678,0x87654321,0x5a5a5a5a)]
for focal in [0,1,192,0xffffffff,0x80000000]:
 for depth in [0,1,256,38400,0x80000000,0xffffffff]:cases.append((0xfffffffd,7,239,159,120,80,focal,depth,depth,11,22,0xfffffffc))
rng=random.Random(0x2b330)
cases += [tuple(rng.getrandbits(32) for _ in range(12)) for _ in range(128)]
run=subprocess.run([str(root/'bin/render-frame-owned-rom-compare.exe')],input=''.join(' '.join(f'{v:08x}' for v in c)+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[[int(v,16) for v in line.split()] for line in run.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 p=FrameProbe((0,0,0,0,0,1,0,0,0,0));p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.write(0x03006810,0x02009000)
 for offset,value in zip([8,12,16,20,24,28,40,44,48],case[:9]):p.write(0x03006810+offset,value)
 p.write(0x02009040,case[9]);p.write(0x02009044,case[10])
 for i in range(8):p.write(0x03006810+0x5c+16*i,(case[11]+i)&MASK)
 before=bytes(p.graphics);p.run(entry=0x0802b330,stop=0x10020000)
 got=[p.read(0x03006810+offset) for offset in range(0x50,0xf0,4)]+p.divisions
 assert got==want,(case,got,want)
 assert p.graphics[:0x50]==before[:0x50] and p.graphics[0xf0:]==before[0xf0:]
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'actual_arm_instructions':arm['steps'],'scope':'Native render_frame_owned global03006940 routed division matches complete original2B330 corner padding/stream words; ordered callback inputs additionally checked through reference harness; nested actual ARM029FC executes quotient result. r1-r3 deliberately poisoned instead of modeling exact scratch register side effects. Engine pointer/callback address fixtures, relocation and GPU/runtime bootstrap excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/render-frame-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',report)
