"""Actual2B0B0 state stores and complete zero-depth/null-engine return paths."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for start,end in [(0x802b0b0,0x802b0d0),(0x802b310,0x802b320)]:
 for i in md.disasm(r[start-0x8000000:end-0x8000000],start):allowed_thumb[i.address]=bytes(i.bytes)
class GateProbe(ExitProbe):
 def __init__(self,*args):self.globals=bytearray(256);super().__init__(*args)
 def read(self,a,size=4):
  if a==0x3006810:return 0x2009000 if self.present else 0
  if 0x3006810<=a<0x3006910:return int.from_bytes(self.globals[a-0x3006810:a-0x3006810+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x3006810<=a<0x3006910:self.globals[a-0x3006810:a-0x3006810+size]=(value&((1<<(8*size))-1)).to_bytes(size,'little')
  else:super().write(a,value,size)
 def external(self,target):raise AssertionError(hex(target))
cases=[(depth,color,present) for depth,present in [(0,0),(0,1),(1,0),(38400,0),(0xffffffff,0)] for color in [0,0x12345678,0xffffffff]]
cp=subprocess.run([str(root/'bin/restart-shade-gate-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=cp.stdout.splitlines();assert len(rows)==len(cases);steps=0
for c,row in zip(cases,rows):
 depth,color,present=c;p=GateProbe((0,0,0,0,0,1,0,0,0,0));p.present=present;p.reg[:2]=[depth,color];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(0x3006854,123);p.write(0x3006858,456)
 for j in range(2560):p.write(0x202baec+j,165,1)
 p.run(entry=0x802b0b0,stop=0x10020000)
 a,b,lookup=row.split();assert [p.read(0x3006854),p.read(0x3006858)]==[int(a),int(b)]
 assert bytes(p.read(0x202baec+j,1) for j in range(2560))==bytes.fromhex(lookup);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual2B0B0 globaldepth/color stores plus completezero-depth/null-engine epilogue. ALL2560 lookupbytes unchanged matched nativeadapter. Nonzero+present palette-generation body excluded; noGPU/restart-host integration claim.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/restart_render_context.c','src/restart_render_context.h','tests/restart_shade_gate_rom_compare.c','tools/compare_restart_shade_gate_rom.py']})
(root/'reports/restart-shade-gate-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
