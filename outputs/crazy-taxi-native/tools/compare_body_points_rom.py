"""Original complete145E4 attachment transforms compared to native on real poses."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
source=source.replace('self.steps<1000','self.steps<50000');exec(source);VehicleProbe.run=MotionProbe.run
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/body-points-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class BodyProbe(VehicleProbe):
 def external(self,target):
  if target==0x8032460:
   entry=self.reg[5];assert entry==0x8000a40;self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  else:super().external(target)
schema=json.loads((root/'reports/bound-route-world-full-state-schema.json').read_text())['fields'];poses=[]
for l in (root/'reports/bound-route-world-full-states.txt').read_text().splitlines():
 a=list(map(int,l.split()));d=dict(zip(schema,a[1:]));poses.append(tuple(d[k] for k in ['output_x','output_y','output_z','output_pitch','output_roll','output_heading_1c','output_drift_69c']))
res=subprocess.run([str(root/'bin/body-points-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in poses),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(poses);steps=armsteps=0
for index,(pose,row) in enumerate(zip(poses,rows)):
 probe=BodyProbe((0,0,0,0,0,1,0,0,0,0));x,y,z,pitch,roll,heading,drift=pose
 for off,val,n in [(0,x,4),(4,y,4),(8,z,4),(0x18,pitch,2),(0x1a,roll,2),(0x1c,heading,4),(0x69c,drift,4)]:probe.write(probe.base+off,val,n)
 probe.reg[0]=probe.base;probe.run(entry=0x80145e4,stop=0x10020000)
 got=[signed(probe.read(probe.base+off+i*16+j*4)) for off in [0x3d8,0x438] for i in range(6) for j in range(3)]
 assert got==row,dict(index=index,pose=pose,original=got,native=row);steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(poses),thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal145E4 transforms with originalARMmatrix helpers vsnative36XYZ attachmentoutputs on161actualworldpostposes. Trig/globalinitializer fixtures; noteffects/render/fullframe.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/body_points.c',root/'src/impact.c',root/'tests/body_points_rom_compare.c',root/'tools/compare_body_points_rom.py',root/'bin/body-points-rom-compare.exe',root/'reports/body-points-cfg.json',root/'reports/bound-route-world-full-states.txt']}
(root/'reports/body-points-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
