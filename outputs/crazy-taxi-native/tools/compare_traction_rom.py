"""Bounded original traction/vector helper vs native, BIOS sqrt explicit contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_motion_steering.py').read().split('\ncases=[')[0])
import math
source=source.replace("'bgt':not z and n==v}","'bgt':not z and n==v,'blt':n!=v}").replace("'ldrsh','str'","'ldrsh','ldrsb','str'").replace("else (1 if m in ['ldrb','strb'] else 4)","else (1 if m in ['ldrb','ldrsb','strb'] else 4)").replace("if m=='ldrsh' and value&32768:value-=65536","if m=='ldrsh' and value&32768:value-=65536\n     if m=='ldrsb' and value&128:value-=256")
exec(source);allowed_thumb={}
for filename in ['challenge-bound-traction-cfg.json','challenge-bound-vector-angle-cfg.json']:
 cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/'+filename).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TractionProbe(MotionProbe):
 def write(self,a,v,n=4):
  if a in [0x20025d4,0x200262c]:
   assert n==1;self.arm.mem[a&~3]=(self.arm.read(a&~3)&~255)|(v&255)
  else:super().write(a,v,n)
 def __init__(self,case):
  super().__init__((0,0,0,0,0,1),0);self.reg[14]=0x10020001;self.reward=[0,0,0,0]
  fx,fz,cx,cz,speed,phase,phaseb,flags,gate,latch=case
  for off,v,n in [(0x53c,fx,4),(0x540,fz,4),(0x570,cx,4),(0x574,cz,4),(0x544,speed,4),(0x564,99,4),(0xe6a,phase,1),(0xe6b,phaseb,1),(0x28,flags,1)]:self.write(self.base+off,v,n)
  for a in [0x2001694,0x2001eb0,0x20025d4,0x200262c,0x2002630]:self.arm.mem[a]=0
  self.arm.mem[0x2001694]=gate<<16;self.arm.mem[0x2001eb0]=128;self.arm.mem[0x200262c]=latch
  ctr=pathlib.Path('outputs/crazy-taxi-native/assets/driving.ctr').read_bytes()
  for i in range(4096):
   self.arm.mem[0x202c4f0+i*4]=(int.from_bytes(ctr[4+i*2:6+i*2],'little',signed=True)>>4)&MASK
   self.arm.mem[0x20304f0+i*4]=(int.from_bytes(ctr[8196+i*2:8198+i*2],'little',signed=True)>>4)&MASK
 def external(self,target):
  if target==0x802e00c:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  elif target==0x802901c:self.reward=[1]+self.reg[:3]
  else:super().external(target)
 def outputs(self):return [signed(self.read(self.base+0x564)),signed(self.read(self.base+0x544)),self.read(0x20025d4,1),self.read(0x200262c,1)]+self.reward
cases=[(fx,fz,8000,10000,speed,phase,0,flags,gate,latch) for fx,fz in [(0,0),(100,200),(10000,20000),(-10000,-20000)] for speed in [45520,45521,91040,91041,136560,136561] for phase in [0,2] for flags in [0,8] for gate in [0,1] for latch in [0,1]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/traction-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=TractionProbe(case);probe.run(entry=0x8014394,stop=0x10020000);got=probe.outputs();assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original traction and original vector-angle executed; explicit BIOS sqrt and signed division contracts; rewards captured as requests; decoded CTR trig globals fixture. Not full world/frame.')
pathlib.Path('outputs/crazy-taxi-native/reports/traction-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
