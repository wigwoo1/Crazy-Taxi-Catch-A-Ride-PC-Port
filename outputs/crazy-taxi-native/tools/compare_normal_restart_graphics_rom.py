"""Actual ordinary ACC6..AE48 graphics caller; opaque helper contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for start,end in [(0xacc6,0xad02),(0xad38,0xae48)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
def state(p):return [p.read(0x200187c),p.read(0x2001804),p.read(0x200177b,1)]
class GraphicsProbe(ExitProbe):
 def read(self,a,size=4):
  if a==0x3004ca0:
   assert size==4;return self.context_pointer
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a==0x3004ca0:
   assert size==4;self.context_pointer=v;return
  return super().write(a,v,size)
 def external(self,target):
  used={0x8027994:2,0x800fec8:6,0x800e154:3,0x800e5a8:4,0x802ac6c:7,0x802c834:1,0x802ca94:1,0x802c688:3,0x80203c4:0,0x8027fb8:5,0x8026440:0,0x800d764:0}
  assert target in used,hex(target);n=used[target]
  args=[self.reg[j] if j<4 else self.read(self.reg[13]+4*(j-4)) for j in range(n)]+[0]*(7-n)
  self.calls.append([target,*args,*state(self)])
  if target==0x800e5a8:self.left=self.wait;self.write(0x200177b,int(self.left!=0),1)
  if target==0x800d764:self.left=max(0,self.left-1);self.write(0x200177b,int(self.left!=0),1)
  self.reg[0]=0
# Check source palette shared by both actual city descriptors.
base=int.from_bytes(r[0x7557a8:0x7557ac],'little');assert base==0x87556cc
for city in [0,1]:assert int.from_bytes(r[base-0x8000000+104*city+24:base-0x8000000+104*city+28],'little')==0x84a5a34
cases=[(city,viewport,wait,sl) for city in [0,1] for viewport in [0,1,127,128,129,255] for wait in [0,1,33] for sl in [1,0xffffffff]]
cp=subprocess.run([str(root/'bin/normal-restart-graphics-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (city,viewport,wait,sl),want in zip(cases,rows):
 p=GraphicsProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.wait=wait;p.left=0;p.reg[10]=sl;p.reg[13]=0x10000400
 for a,v,size in [(0x2001708,0,1),(0x2001eb4,city,1),(0x2001780,viewport,1),(0x3004ca0,0x2002634,4),(0x200187c,123,4),(0x2001804,456,4),(0x200177b,0,1),(0x1000041c,0,4),(0x10000424,0x2001790,4)]:p.write(a,v,size)
 p.run(entry=0x800acc6,stop=0x800ae48)
 actual=[len(p.calls),*[v for call in p.calls for v in call],*state(p)]
 assert actual==want,((city,viewport,wait,sl),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Reachable ordinary mode0 ACC6..AE48 graphics caller, both city descriptors/palette, signed viewport byte boundary, up to7 register/stack arguments, pointer writes at exact call boundaries, loading fade gate0/1/33D764 requests. Caller retainedsl/SP local contracts explicit. Decoder/palette/camera/render/fade helper bodies excluded; native adapter integration and full playable Retry not covered.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_restart_graphics.c','src/normal_restart_graphics.h','tests/normal_restart_graphics_rom_compare.c','tools/compare_normal_restart_graphics_rom.py']})
(root/'reports/normal-restart-graphics-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
