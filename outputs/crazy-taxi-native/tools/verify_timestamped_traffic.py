import pathlib,json,re,math,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');log=(r/'reports/presentation-traffic-active.log').read_text();count=int(re.search(r'TIMESTAMPED TRAFFIC VIEWS: (\d+)',log)[1]);assert count>0 and 'PASS FOCUS SUSPENSION: 60 render frames' in log
rows=[x.split() for x in (r/'reports/presentation-traffic-active-poses.txt').read_text().splitlines()];assert len(rows)==360;blends=0
for row in rows:
 assert len(row)==14;now,target,before,after=map(int,row[:4]);alpha=float(row[4]);held=int(row[5]);assert target==max(0,now-33500000)
 if held:assert before==after and alpha==0
 else:
  assert before<after and before<=target<=after and math.isclose(alpha,(target-before)/(after-before),abs_tol=1e-14);blends+=int(0<alpha<1)
assert blends>20
report={'status':'PASS_TIMESTAMPED_TRAFFIC_BRANCH_AND_IMMUTABILITY','draws':360,'prepared_traffic_views':count,'interior_pose_blends':blends,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-presentation-traffic.exe').read_bytes()).hexdigest(),'scope':'Historical traffic/submission branch exercised using same taxi/camera bracket; perdrawnativechecks preserve car/context/trafficactorbytes and60controlledfocusframes. Viewcount is preparation, not visiblepixel count. No source traffic trajectory/pixel or physicaldevice proof.'};(r/'reports/timestamped-traffic-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
