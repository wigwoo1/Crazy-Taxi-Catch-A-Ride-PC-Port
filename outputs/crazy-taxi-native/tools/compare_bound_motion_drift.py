"""Combined original motion/orientation execution; diagnostic, not runtime."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_orientation.py').read().split('\ncases=[')[0])
source=source.replace('def run(self):','def run(self,entry=0x80126dc,stop=0x80129a8):').replace('pc=0x8012c6c','pc=entry').replace('while pc!=0x10020000:','while pc!=stop:')
exec(source)
for filename in ['challenge-bound-motion-cfg.json','challenge-bound-orientation-cfg.json']:
 cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/'+filename).read_text())
 if filename.startswith('challenge-bound-motion'):allowed_thumb={}
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class CombinedProbe(MotionProbe):
 def __init__(self,case,pre,steering_flags):
  super().__init__(case[:6],pre);self.steering_flags=steering_flags
  drift,timer,state,flags=case[6:]
  for off,v,n in [(0,100,4),(4,200,4),(8,300,4),(0x69c,drift,4),(0x2a,timer,2),(0x564,state,4),(0x358,4,1),(0x28,flags,1),(0xe6a,2,1)]:self.write(self.base+off,v,n)
  for a in [0x2001eb0,0x2001534,0x2001694,0x2001ea8,0x2001778,0x2001658,0x2001654,0x2001650,0x200164c]:self.arm.mem[a]=0
  self.arm.mem[0x2001eb0]=128;self.arm.mem[0x2001534]=5<<24
 def external(self,target):
  if target==0x80129e0:self.reg[0]=self.pre&MASK;self.write(self.base+0x28,self.steering_flags,1)
  elif target==0x8012c6c:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x802dd5c:self.reg[0]=angle_probe.run(self.reg[0],self.reg[1])&MASK
  else:super().external(target)
 def outputs(self):
  def half(off):
   v=self.read(self.base+off,2);return v-65536 if v&32768 else v
  return [half(0x57c)]+[signed(self.read(self.base+off)) for off in [0x1c,0x20,0xc,0x10,0x14,0x540,0x69c]]+[half(0x2a)]+[signed(self.read(self.base+off)) for off in [0x544,0,8]]
cases=[(speed,steer,pitch,54,-3,delta,drift,timer,state,flags) for speed in [71424,122309] for steer in [0,64,-64] for pitch in [-178,60] for delta in [1,2] for drift in [-500000,500000] for timer in [4,128] for state in [0,1] for flags in [0,8]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/motion-drift-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=armsteps=0
for case,row in zip(cases,rows):
 assert row[0]==0,dict(case=case,row=row)
 probe=CombinedProbe(case,row[1],row[2]);probe.run();got=probe.outputs();assert got==row[3:],dict(case=case,original=got,native=row[3:]);steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(cases),thumb_instructions=steps,matrix_arm_instructions=armsteps,angle_arm_instructions=angle_probe.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Combined original motion through129A6 and full airborne orientation; original ARM matrix/angle helpers executed; steering result/flags and signed division explicit contracts. Not traction/world/fullGBAframe.')
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-motion-drift-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
