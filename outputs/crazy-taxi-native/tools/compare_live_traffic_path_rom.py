"""Original traffic path advance vs actual captured normal-follow contacts."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
for name in ['advance','initialize']:
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/f'reports/traffic-path-{name}-cfg.json').read_text())['instructions']})
class PathProbe(TaxiProbe):
 def read(self,a,n=4):
  if 0x10003000<=a<0x10010000:return (self.arm.mem.get(a&~3,0)>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x10003000<=a<0x10010000:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x802e190:
   self.initializations+=1;self.run(entry=target,stop=self.reg[14]&~1)
  elif target in [0x8032454,0x803247c]:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
f=root/'reports/current-full-traffic-actor-states.txt';rows=[list(map(int,l.split())) for l in f.read_text().splitlines()];steps=0;checked=0;fallbacks=0
for index,a in enumerate(rows):
 if a[15]!=0:continue
 p=PathProbe((0,0,0,0,0,1,0,0,0,0));p.initializations=0;state=0x10003000;desc=0x10003100;road=0x10004000
 p.arm.mem[0x3006940]=0x8032489;p.write(desc,a[61]);p.write(desc+16,road)
 for j,v in enumerate(a[62:]):p.write(road+j*4,v)
 for off,v in [(0,desc),(4,a[21]),(8,a[22]),(12,road+a[22]*16),(16,a[23]),(20,a[24]),(24,a[25])]:p.write(state+off,v)
 p.reg[:2]=[state,a[4]&MASK];p.reg[14]=0x10020001;p.run(entry=0x802e264,stop=0x10020000)
 got=[signed(p.read(state+4)),p.read(state+8),signed(p.read(state+16)),signed(p.read(state+20)),p.read(state+24,2)]
 assert got==a[46:51],dict(index=index,original=got,native=a[46:51]);steps+=p.steps;checked+=1;fallbacks+=p.initializations
report=dict(cases=checked,initialize_calls=fallbacks,pass_match=True,thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original complete2E264 path advance, nested2E190 on demand vs captured normal-follow path outputs; relocated descriptor/point memory and signed division contract. Not full caller/frame proof.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [f,root/'tools/compare_live_traffic_path_rom.py',root/'src/traffic_path.c']}
(root/'reports/live-traffic-path-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
