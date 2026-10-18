"""Complete original2C790 viewport stores; signed division explicit contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True;todo=[0x802c790];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x802c790<=a<0x802c826,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 assert not(i.mnemonic.startswith('b') and i.mnemonic!='bl')
 todo.append(a+i.size)
class ViewProbe(ExitProbe):
 def read(self,a,size=4):
  if a==0x300692c:return 0x2009100
  if a==0x3006938:return 0x2009200
  if 0x3006810<=a<0x3006910:return int.from_bytes(self.graphics[a-0x3006810:a-0x3006810+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x3006810<=a<0x3006910:self.graphics[a-0x3006810:a-0x3006810+size]=(value&((1<<(8*size))-1)).to_bytes(size,'little')
  else:super().write(a,value,size)
 def __init__(self,*args):self.graphics=bytearray(256);super().__init__(*args)
 def external(self,target):
  assert target==0x8032488 and self.reg[1]==192
  numerator=signed(self.reg[0]);self.reg[0]=(abs(numerator)//192*(-1 if numerator<0 else 1))&MASK
cases=[(0,0,240,160),(1,3,239,159),(-3,-7,241,163),(10,20,-30,-40),(2147483647,2147483647,1,1),(-2147483648,0,-2147483648,160)]+[(i,i+1,240-i,160-i) for i in range(-9,10)]
cp=subprocess.run([str(root/'bin/render-viewport-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 p=ViewProbe((0,0,0,0,0,1,0,0,0,0));p.reg[:4]=[v&MASK for v in c];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(0x300683c,123);p.write(0x3006840,456)
 p.run(entry=0x802c790,stop=0x10020000)
 offsets=[8,12,16,20,24,28,32,36,40,56,60,44,48]
 got=[signed(p.read(0x3006810+o)) for o in offsets]+[signed(p.read(base+j*4)) for base in [0x2009100,0x2009200] for j in range(4)]
 assert got==want,(c,got,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original2C790 all viewport/projection and two patched callback-bound arrays exact; wraps, negative/unaligned edges, priornearfar preserved. Original32488 signeddivision explicit contract; nativecallback arrays mapped fixtures, noARMcode patching/GPUintegration.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/render_viewport.c','src/render_viewport.h','tests/render_viewport_rom_compare.c','tools/compare_render_viewport_rom.py']})
(root/'reports/render-viewport-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
