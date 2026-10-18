"""Original movement with explicit constant-height obstacle query fixture."""
exec(open('outputs/crazy-taxi-native/tools/verify_world_fill_rom.py').read().split('root=pathlib.Path')[0])
import math
root=pathlib.Path('outputs/crazy-taxi-native')
cfg=json.loads((root/'reports/challenge-bound-movement-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
class MovementProbe(MotionProbe):
 def write(self,a,v,n=4):
  if 0x3006760<=a<0x3006810:
   old=self.arm.read(a&~3);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  elif a==0x30053f0:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  elif target==0x8032454:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(trunc(a,b) if b else a)&MASK
  elif target==0x8029e84:
   self.queries.append(tuple(signed(self.read(self.reg[0]+i)) for i in [0,4,8]));self.reg[0]=self.height&MASK
  else:raise AssertionError(('external',hex(target)))
 def __init__(self,case):
  super().__init__((0,0,0,0,0,1),0);x,y,z,dx,dy,dz,self.height,preserve=case;self.queries=[]
  for a in range(0x3006760,0x3006810,4):self.arm.mem[a]=0
  self.arm.mem[0x3006940]=0x80029fc;self.arm.mem[0x2001eb0]=128 if preserve else 0;self.arm.mem[0x2001534]=5<<24
  for off,v in zip([0x100,0x104,0x108,0x110,0x114,0x118],[x,y,z,dx,dy,dz]):self.write(self.base+off,v)
  self.reg[:3]=[0,self.base+0x100,self.base+0x110];self.reg[14]=0x10020001
 def outputs(self):return [signed(self.read(self.base+0x100+i)) for i in [0,4,8]]+[signed(self.read(0x30067a0+i)) for i in [0,4,8]]+[len(self.queries),self.read(0x30067b8)]
cases=[(1000,3000,-500,dx,dy,dz,h,p) for dx,dy,dz in [(0,0,0),(-18,24,163),(500,0,-900),(-1000,100,700),(255,0,0),(256,0,0)] for h in [-2147483648,0,3000,3654,3655,4000] for p in [0,1]]
res=subprocess.run([str(root/'bin/movement-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=MovementProbe(case);probe.run(entry=0x802a27c,stop=0x10020000);got=probe.outputs();assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original movement position path; explicit constant-height obstacle query, sqrt and division contracts. No material blockers, impact, original obstacle selection or full trajectory claim.')
(root/'reports/movement-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
