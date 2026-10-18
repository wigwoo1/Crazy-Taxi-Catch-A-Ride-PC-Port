import sys
sys.path.insert(0,"work/python-packages")
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
route=json.loads((root/'reports/live-delivery-route31.json').read_text())['points']
observation=json.loads((root/'reports/live-delivery-turns-native4k-verification.json').read_text())
points=[tuple(v) for v in route[12:20]]
points += [(v['x'],v['z']) for v in observation['progress'] if v['waypoint']>=16][::4]
points=list(dict.fromkeys(points));cases=[(x,0,z) for x,z in points]
selected=[{'tick':i} for i in range(len(cases))]
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
report={'equal':True,'cases':len(cases),'query_plane_y':0,'thumb_instructions':steps,'obstacle_kinds':kinds,'blocking_polygons':blockers,'queries':cases,'rom_sha256':hashlib.sha256(r).hexdigest(),'native_binary_sha256':hashlib.sha256((root/'bin/scene-obstacle-rom-compare.exe').read_bytes()).hexdigest(),'scope':'Original29E84 obstacle and29664 ground functions versus native city1 asset for route31 points12..19 and sampled stuck-area X/Z positions, all queryY0. Explicit division/containment helper contracts, null optional contact output. X/Z trajectory is actual; Y0 is a diagnostic query plane, not captured actor or attempted movement-query Y. Does not prove movement correction, actual collision cause, full original route trajectory or wholegame fidelity.'}
(root/'reports/live-delivery-bend-ground-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='queries'},indent=2))
