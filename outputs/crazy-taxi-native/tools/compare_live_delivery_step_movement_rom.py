import sys,csv
sys.path.insert(0,"work/python-packages")
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
rows_captured=list(csv.DictReader((root/'reports/live-delivery-queries-native4k.csv').open()))
late=[v for v in rows_captured if int(v['waypoint'])>=16]
stepped=[v for v in late if int(v['height'])-int(v['query_y'])>=655]
assert stepped
sampled=sorted(stepped,key=lambda v:int(v['height'])-int(v['query_y']),reverse=True)[:16]
cases=[(int(v['query_x'])-128,int(v['query_y']),int(v['query_z']),128,0,0,int(v['height']),0) for v in sampled]
res=subprocess.run([str(root/'bin/movement-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=MovementProbe(case);probe.run(entry=0x802a27c,stop=0x10020000);got=probe.outputs();assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps
report={'equal':True,'cases':len(cases),'thumb_instructions':steps,'late_queries':len(late),'late_queries_at_or_above_655':len(stepped),'maximum_step':max(int(v['height'])-int(v['query_y']) for v in stepped),'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Original2A27C position-only movement routine vs native with explicit constant-height query fixtures using captured late-route queryY/heights. Fixture startX=queryX-128 and128unitX displacement are diagnostic, not captured real motion/vehicle impact. Tests original refusal of>=655 height steps, not whole original trajectory or full collision response.'}
(root/'reports/live-delivery-height-step-movement-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
