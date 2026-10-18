"""Actual joined original prefix/background against camera-pitch scene API."""
from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native')
s=(root/'tools/verify_source240_joined_background.py').read_text().split('\nrows=')[0]
ns={};exec(compile(s,'joined-background-definitions','exec'),ns)
Probe=ns['JoinedBackgroundProbe'];cases=[]
for city,scene in [('city1',0x087556cc),('city2',0x08755734)]:
 for pitch in [-1024,-285,-142,-64,-9,-4,0,1,8,64,142,285,1024,4095]:
  yaw=-485;p=Probe(pitch,yaw,scene);p.run(0x0802b6ac,stop=0x0802b6f8)
  fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600))
  args=[str(root/'bin/source240-real-background-probe.exe'),str(root/'assets/indexed-scenes'/f'{city}.cti'),'0',str(pitch),str(yaw)]
  native=subprocess.run(args,capture_output=True,check=True,timeout=45).stdout
  assert fb==native,(city,pitch)
  half=int(subprocess.run([str(root/'bin/background-pitch-probe.exe')],input=str(pitch)+'\n',capture_output=True,text=True,check=True,timeout=45).stdout)
  args[-2]=str(half);double=subprocess.run(args,capture_output=True,check=True,timeout=45).stdout
  cases.append(dict(city=city,pitch=pitch,source_prefix_pitch=p.background_input_pitch,exact_bytes=38400,double_conversion_mismatch_pixels=sum(a!=b for a,b in zip(native,double)),sha256=hashlib.sha256(native).hexdigest()))
  print('PASS original pitch',city,pitch,flush=True)
report=dict(status='PASS',cases=cases,scope='Actual original joined2B6AC angle/numeric prefix and ARM2FE0 city bitmap vs native scene API supplied raw camera pitch. Double-conversion mismatch retained. Explicit numeric helper contracts inherited. No scene queue/actors/palette/fullframe/cycles claim.')
(root/'reports/background-pitch-integration-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'cases')
