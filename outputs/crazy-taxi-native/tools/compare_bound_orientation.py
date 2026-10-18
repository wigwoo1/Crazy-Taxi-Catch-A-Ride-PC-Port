"""Isolated original orientation comparison. Not linked into playable runtime."""
source=open('outputs/crazy-taxi-native/tools/compare_bound_motion.py').read().split('cases=[')[0]
source=source.replace("pc=0x80126dc","pc=0x8012c6c").replace("while pc!=0x80129a8:","while pc!=0x10020000:").replace("['ldr','ldrh','ldrsh','str','strh']","['ldr','ldrb','ldrh','ldrsh','str','strb','strh']").replace("else 4\n    if m.startswith('str')", "else (1 if m in ['ldrb','strb'] else 4)\n    if m.startswith('str')")
source=source.replace("elif m=='bl':", "elif m=='pop':\n    regs=sorted(rn(o) for o in ops)\n    for j,k in enumerate(regs):self.reg[k]=self.read(self.reg[13]+4*j)\n    self.reg[13]+=4*len(regs)\n   elif m=='bx':nextpc=val(ops[0])&~1\n   elif m=='bl':")
source=source.replace("'bge':n==v}","'bge':n==v,'beq':z,'bgt':not z and n==v}")
exec(source)
cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-orientation-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
exec(open('outputs/crazy-taxi-native/tools/angle_rom_probe.py').read())
angle_probe=AngleProbe(r)
class OrientationProbe(MotionProbe):
 def __init__(self,case):
  pitch,heading,roll,heading20,drift,force,timer,state,speed,flags,delta=case
  super().__init__((speed,0,pitch,0,roll,delta),0);self.reg[14]=0x10020001
  for off,v,n in [(0,100,4),(4,200,4),(8,300,4),(0x1c,heading,4),(0x20,heading20,4),(0x69c,drift,4),(0x540,force,4),(0x2a,timer,2),(0x564,state,4),(0x358,4,1),(0x28,flags,1),(0xe6a,2,1)]:self.write(self.base+off,v,n)
  for a in [0x2001eb0,0x2001534,0x2001694,0x2001ea8,0x2001778,0x2001658,0x2001654,0x2001650,0x200164c]:self.arm.mem[a]=0
  self.arm.mem[0x2001eb0]=128;self.arm.mem[0x2001534]=5<<24
 def external(self,target):
  if target==0x802dd5c:
   self.reg[0]=angle_probe.run(self.reg[0],self.reg[1])&MASK
  else:super().external(target)
 def outputs(self):
  timer=self.read(self.base+0x2a,2);timer=timer-65536 if timer&32768 else timer
  return [signed(self.read(self.base+x)) for x in [0x1c,0x20,0x69c]]+[timer,signed(self.read(self.base+0x544)),signed(self.read(self.base)),signed(self.read(self.base+8)),0]
cases=[(p,h,-3,h-196608,d,12345,t,s,122309,f,delta) for p in [-178,-5,60] for h in [-430080,221184] for d in [-500000,0,500000] for t in [0,4,64,128] for s in [0,1] for f in [0,8] for delta in [1,2]]
result=subprocess.run(['outputs/crazy-taxi-native/bin/orientation-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in result.stdout.splitlines()];assert len(rows)==len(cases);steps=0;armsteps=0
for case,row in zip(cases,rows):
 probe=OrientationProbe(case);probe.run();got=probe.outputs();assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(cases),thumb_instructions=steps,arm_instructions=armsteps,angle_arm_instructions=angle_probe.steps,scope='Phase2 Bound settings128/submode5. Original orientation control flow and ARM rotation executed; signed division explicit scalar contract; original ARM angle difference executed. Not whole frame/GBA trajectory.',rom_sha256=hashlib.sha256(r).hexdigest())
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-orientation-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
