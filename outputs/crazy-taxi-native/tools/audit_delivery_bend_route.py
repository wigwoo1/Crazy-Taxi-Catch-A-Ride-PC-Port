import json,csv,math,subprocess
from pathlib import Path
r=Path('outputs/crazy-taxi-native');route=json.loads((r/'reports/live-delivery-route31.json').read_text())['points'];a,b=route[15:17];sx,sz=b[0]-a[0],b[1]-a[1];length=math.hypot(sx,sz)
rows=[{k:int(v) for k,v in x.items()} for x in csv.DictReader((r/'reports/delivery-full-world-movement-replay.csv').open())];collisions=[v for v in rows if v['collided'] and v['wall_queries']==0]
points=[];details=[]
for v in collisions:
 px,pz=v['start_x']-a[0],v['start_z']-a[1];fraction=(px*sx+pz*sz)/(length*length);lateral=(px*sz-pz*sx)/length;cx,cz=round(a[0]+fraction*sx),round(a[1]+fraction*sz)
 points.extend([(v['start_x']+v['dx'],v['start_y'],v['start_z']+v['dz']),(cx,v['start_y'],cz)])
 details.append(dict(index=v['index'],segment_fraction=fraction,lateral_distance=lateral,start_y=v['start_y'],requested_step=v['maximum_height_step']))
cp=subprocess.run([str(r/'bin/scene-obstacle-rom-compare.exe'),str(r/'assets/city1.ctw')],input=''.join(' '.join(map(str,p))+'\n' for p in points),text=True,capture_output=True,check=True)
query=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(query)==len(points)
for i,v in enumerate(details):v['attempted_endpoint_obstacle']=query[2*i][:3];v['route_projection_obstacle']=query[2*i+1][:3];v['route_projection_step']=query[2*i+1][1]-v['start_y']
report=dict(segment_indices=[15,16],segment_endpoints=[a,b],height_step_collision_count=len(collisions),first=details[0],maximum_absolute_lateral=max(abs(v['lateral_distance']) for v in details),projected_route_below_refusal_threshold=sum(v['route_projection_obstacle'][0]==1 and v['route_projection_step']<655 for v in details),details=details,scope='Geometric diagnostic projection onto original route31 segment15-to16. Queries use actual collided startY but projected routeXZ; those projections are synthetic diagnostics, not captured actor movement or proved playable original trajectory. Single full-displacement endpoint query does not replace subdivided actual movement queries. Does not prove controller fix or collision removal.')
(r/'reports/delivery-bend-route-deviation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='details'},indent=2))
