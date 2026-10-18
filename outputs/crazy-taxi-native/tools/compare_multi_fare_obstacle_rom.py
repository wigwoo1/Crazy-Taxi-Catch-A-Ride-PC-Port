"""Original29E84 city1 queries on actual failed third-trip native positions.
Reuses explicit signed division/containment helper contracts; no gameplay
emulation or original input trajectory oracle is claimed.
"""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_ground_contact_rom.py').read().split('contactcfg=')[0])
import csv
cfg=json.loads((root/'reports/challenge-bound-obstacle-cfg.json').read_text())
allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
address=0x087556cc
polygons=int.from_bytes(r[address-0x8000000+0x28:address-0x8000000+0x2c],'little')
rows=list(csv.DictReader((root/'reports/multi-fare-third-route-failure.csv').open()))
selected=[row for i,row in enumerate(rows) if int(row['waypoint'])>=14 and (i%50==0 or i==len(rows)-1)]
cases=[tuple(int(row[k]) for k in ['x','y','z']) for row in selected]
contact_rows=[row for row in csv.DictReader((root/'reports/multi-fare-third-obstacles.csv').open()) if int(row['waypoint'])>=14]
stride=max(1,len(contact_rows)//40)
for row in contact_rows[::stride]:
 cases.append(tuple(int(row[k]) for k in ['query_x','query_y','query_z']))
 selected.append(row)
contactcfg=json.loads((root/'reports/challenge-bound-contact-interpolation-cfg.json').read_text())
ground_allowed={int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/challenge-bound-ground-cfg.json').read_text())['instructions']}
ground_allowed.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in contactcfg['instructions']})
class NoContactGroundProbe(GroundProbe):
 def external(self,target):
  if target==0x802a978:
   assert self.read(self.reg[13]+4)==0;self.reg[0]=0
  else:super().external(target)

cp=subprocess.run([str(root/'bin/scene-obstacle-rom-compare.exe'),str(root/'assets/city1.ctw')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
native=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(native)==len(cases)
steps=0;kinds={0:0,1:0,2:0};blockers={}
for case,row,original_row in zip(cases,native,selected):
 p=GroundProbe((0,0,0,0,0,1),0);point=p.base;out=point+0x100
 for off,v in zip([0,4,8],case):p.write(point+off,v)
 p.arm.mem[0x3006810]=address;p.arm.mem[0x3006940]=0x80029fc
 p.reg[:2]=[point,out];p.reg[14]=0x10020001;p.run(entry=0x8029e84,stop=0x10020000)
 height=signed(p.reg[0]);ptr=p.read(out);kind=2 if height==2147483647 else 0 if height==-2147483648 else 1
 got=[kind,height,(ptr-polygons)//24 if kind==2 else 65535]
 assert got==row[:3],dict(point=case,tick=original_row['tick'],original=got,native=row)
 steps+=p.steps;kinds[kind]+=1
 saved_allowed=allowed_thumb;allowed_thumb=ground_allowed
 g=NoContactGroundProbe((0,0,0,0,0,1),0);point=g.base;normal=point+0x100;material=point+0x200
 for off,v in zip([0,4,8],case):g.write(point+off,v);g.write(normal+off,0)
 g.write(material,0);g.arm.mem[0x3006810]=address;g.arm.mem[0x3006940]=0x80029fc
 g.reg[:4]=[point,normal,material,0];g.reg[14]=0x10020001;g.run(entry=0x8029664,stop=0x10020000)
 gh=signed(g.reg[0]);ground=[int(gh!=-2147483648),gh]+[signed(g.read(normal+i)) for i in [0,4,8]]+[g.read(material)]
 assert ground==row[3:],dict(point=case,original=ground,native=row[3:])
 steps+=g.steps;allowed_thumb=saved_allowed
 if kind==2:blockers[str(got[2])]=blockers.get(str(got[2]),0)+1
names=['tools/compare_multi_fare_obstacle_rom.py','tests/scene_obstacle_rom_compare.c','src/ground.c','src/world_asset.c','assets/city1.ctw','reports/multi-fare-third-route-failure.csv','reports/multi-fare-third-obstacles.csv']
report={'equal':True,'cases':len(cases),'sampled_post_frame_positions':88,'attempted_wall_queries':len(cases)-88,'thumb_instructions':steps,'kinds':kinds,'blocking_polygons':blockers,'rom_sha256':hashlib.sha256(r).hexdigest(),'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in names},'scope':'Actual original29E84 obstacle and29664 ground city1 ROM descriptor/grid/materials/geometry for sampled third-trip post-frame positions and attempted wall queries. Exact obstacle kind/height/blocker and ground height/normal/material. Explicit source division/containment helper contracts and null optional contact output. Does not prove movement correction, body height integration, driver, original full trajectory or whole frame equivalence.'}
(root/'reports/multi-fare-obstacle-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
