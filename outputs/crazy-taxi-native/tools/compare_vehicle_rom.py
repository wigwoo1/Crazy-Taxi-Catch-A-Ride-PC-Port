"""Complete original vehicle update through return; diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_traction_rom.py').read().split('\ncases=[')[0])
allowed_thumb={}
for name in ['vehicle','steering','orientation','traction','vector-angle','response']:
 cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-'+name+'-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
assert r[0x15c24:0x15c26]==bytes.fromhex('7047')
class VehicleProbe(CombinedProbe):
 run=MotionProbe.run
 def write(self,a,v,n=4):
  if a in [0x20025d4,0x200262c]:
   assert n==1;self.arm.mem[a&~3]=(self.arm.read(a&~3)&~255)|(v&255)
  else:MotionProbe.write(self,a,v,n)
 def __init__(self,case):
  super().__init__(case,0,0);self.reg[14]=0x10020001;self.reward=[0,0,0,0]
  for off,v,n in [(0x53c,10000,4),(0x570,8000,4),(0x574,10000,4),(0x744,123,4),(0x4ac,16,1)]:self.write(self.base+off,v,n)
  for a in [0x20025d4,0x200262c,0x2002630]:self.arm.mem[a]=0
  ctr=pathlib.Path('outputs/crazy-taxi-native/assets/driving.ctr').read_bytes()
  for i in range(4096):self.arm.mem[0x20304f0+i*4]=(int.from_bytes(ctr[8196+2*i:8198+2*i],'little',signed=True)>>4)&MASK
 def external(self,target):
  if target in [0x8014394,0x802e00c,0x8015bb8]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8015c24:pass
  elif target==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  elif target==0x802901c:self.reward=[1]+self.reg[:3]
  else:super().external(target)
 def outputs(self):return super().outputs()+[signed(self.read(self.base+0x564)),signed(self.read(self.base+0x744)),self.read(0x20025d4,1),self.read(0x200262c,1)]+self.reward
cases=[(speed,steer,pitch,54,-3,delta,drift,timer,state,flags) for speed in [71424,122309] for steer in [0,64,-64] for pitch in [-178,60] for delta in [1,2] for drift in [-500000,500000] for timer in [4,128] for state in [0,1] for flags in [0,8]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/vehicle-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=armsteps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=VehicleProbe(case);probe.run(entry=0x80126dc,stop=0x10020000);got=probe.outputs();assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(cases),thumb_instructions=steps,matrix_arm_instructions=armsteps,angle_arm_instructions=angle_probe.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Full original126DC throughreturn129C8, originalsteering/orientation/traction/response/vectorangle, matrix/angleARM. BIOSsqrt/divisioncontracts,rewardrequestcapture,trigglobalfixture. Not acceleration/collision/fullGBAframe.')
pathlib.Path('outputs/crazy-taxi-native/reports/vehicle-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
