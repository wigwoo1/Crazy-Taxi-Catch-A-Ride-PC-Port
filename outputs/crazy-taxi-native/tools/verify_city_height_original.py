"""Original29B44 real city terrain vs productionnative height query."""
from pathlib import Path
import json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_bound_ground_contact_rom.py').read_text().split('\nschema=')[0],ns)
ns['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/effect-height-cfg.json').read_text())['instructions']});Ground=ns['GroundProbe'];cases=[]
points=[]
for t in [1,30,60,90,120]:
 d=json.loads((root/'reports'/('live-taxi-gpu-'+str(t)+'.json')).read_text());x,y,z=d['vehicle'][:3];points += [(x,0,z),(x,y,z),(x,-2147483648,z)]
points += [(-1000000,0,-1000000),(1000000,0,1000000),(0,0,0)]
for city,scene in [('city1',0x087556cc),('city2',0x08755734)]:
 cp=subprocess.run([str(root/'bin/city-height-probe.exe'),str(root/'assets'/f'{city}.ctw')],input=''.join(' '.join(map(str,p))+'\n' for p in points),capture_output=True,text=True,check=True,timeout=45)
 rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(points)
 for point,row in zip(points,rows):
  g=Ground((0,0,0,0,0,1),0)
  for off,v in zip([0,4,8],point):g.write(g.base+off,v)
  g.arm.mem[0x03006810]=scene;g.arm.mem[0x03006940]=0x080029fc;g.reg[0]=g.base;g.reg[14]=0x10020001;g.run(entry=0x08029b44,stop=0x10020000)
  original=ns['signed'](g.reg[0]);assert original==row[1] and row[0]==int(original!=-2147483648),(city,point,original,row)
  cases.append(dict(city=city,point=point,height=original,found=row[0],thumb_instructions=g.steps))
report=dict(status='PASS',cases=cases,native_executable_sha256=hashlib.sha256((root/'bin/city-height-probe.exe').read_bytes()).hexdigest(),scope='Actual original29B44 withimmutable realcity1/city2 ROM descriptors/geometry vs nativeworld_asset_height; liveXZ withY0/vehicleY/INTMIN plusoutside points. OriginalARMcontainment boundedcallee-save arithmetic anddivision contracts remain. No fullgame/terraininterpolationcontinuous trajectory claim.')
(root/'reports/city-height-original-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'queries',sum(c['found'] for c in cases),'hits')
