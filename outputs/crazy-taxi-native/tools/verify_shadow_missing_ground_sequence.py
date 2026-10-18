"""Original taxi retained terrain point across missing-ground and recovery queries."""
from pathlib import Path
import json,subprocess
root=Path('outputs/crazy-taxi-native');gn={};exec((root/'tools/compare_bound_ground_contact_rom.py').read_text().split('\nschema=')[0],gn)
gn['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/effect-height-cfg.json').read_text())['instructions']})
sn={};exec((root/'tools/verify_taxi_shadow_joined_renderer.py').read_text().split('\nexpected=')[0],sn)
results=[];current_scene=0
class QueryProbe(sn['JoinedShadowProbe']):
 def external(self,target):
  if target!=0x08029b44:return super().external(target)
  self.query_point=[gn['signed'](self.read(self.reg[0]+i*4)) for i in range(3)]
  g=gn['GroundProbe']((0,0,0,0,0,1),0)
  for off,v in zip([0,4,8],self.query_point):g.write(g.base+off,v)
  g.arm.mem[0x03006810]=current_scene;g.arm.mem[0x03006940]=0x080029fc;g.reg[0]=g.base;g.reg[14]=0x10020001;g.run(entry=0x08029b44,stop=0x10020000)
  self.query_height=gn['signed'](g.reg[0]);self.reg[0]=g.reg[0]
points=[(22079,-33983),(-1000000,-1000000),(22105,-33956),(1000000,1000000),(0,0),(22261,-33791),(22571,-33470),(23047,-32975)]
for city,scene in [('city1',0x087556cc),('city2',0x08755734)]:
 current_scene=scene;hint=0;base=0x02002634
 for index,(x,z) in enumerate(points):
  p=QueryProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.depth_offsets=[]
  for a in range(base,base+0xea0,4):p.arm.mem[a]=0
  for a in range(0x03006810,0x03006b00,4):p.arm.mem[a]=0
  p.arm.mem[0x02001708]=0;p.arm.mem[0x030053e0]=0
  for off,val,n in [(0,x,4),(4,19200,4),(8,z,4),(0x1c,14737408,4),(0xe8c,hint,4)]:p.write(base+off,val,n)
  p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x0801556c,stop=0x080155e8)
  assert p.query_point==[x,hint,z]
  cp=subprocess.run([str(root/'bin/city-height-probe.exe'),str(root/'assets'/f'{city}.ctw')],input=f'{x} {hint} {z}\n',capture_output=True,text=True,check=True,timeout=45);rc,height=map(int,cp.stdout.split());assert height==p.query_height and rc==int(height!=-2147483648)
  native_retained=(height-192)&0xffffffff;actual=p.read(base+0xe8c);assert actual==native_retained
  assert p.read(base+0xe88)==x&0xffffffff and p.read(base+0xe90)==z&0xffffffff
  results.append(dict(city=city,index=index,query=[x,hint,z],height=height,found=rc,retained_height=gn['signed'](actual)))
  hint=gn['signed'](actual)
assert len(results)==16 and any(not c['found'] for c in results)
report=dict(status='PASS',cases=results,scope='Actual1556C through155E6 query/retainedpoint instructions withactual original29B44 cityterrain in separate context; nativeworldheight query and wrapped groundminus192 retainedvalue matchmissingground/recovery sequences. Division/containment contracts inherited. Stopsbefore shadowprojection; nativefull-live offroad/reset/failure cadence unverified.')
(root/'reports/shadow-missing-ground-sequence-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(results),'sequence queries')
