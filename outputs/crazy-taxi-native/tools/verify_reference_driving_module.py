from pathlib import Path
import json,struct,subprocess,hashlib
r=Path('outputs/crazy-taxi-native');results=[]
for side in ['left','right']:
 path=r/f'reports/longsource{side}.schedule.txt';cp=subprocess.run([str(r/'bin/reference-driving-probe.exe'),str(path)],text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];records=json.loads((r/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json').read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];assert len(rows)==len(pairs)
 for i,((a,b),row) in enumerate(zip(pairs,rows)):
  actor=bytes.fromhex(b['actor']);expected=[i+1]+list(struct.unpack_from('<iii',actor,0x498))+list(struct.unpack_from('<hhh',actor,0x4a4));assert row==expected,dict(index=i,side=side,native=row,source=expected)
 results.append(dict(side=side,original_driving_checked_updates=len(rows),camera_pose_exact_updates=len(rows)))
report=dict(status='PASS_REFERENCE_DRIVING_MODULE_SOURCE_CAMERA',results=results,scope='Native connected driving fixture checks originalXYZheading/speed everyupdate internally; native cameraXYZpitchyawroll independently compared with originalactor498/4A4 observedoutputs atall495updates. Baseline and preset/freeze/targetowners importedonce, no originalCPU execution inmodule, no renderer/pixel/generalhosttiming/wholemission proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'src/reference_driving.c',r/'src/reference_driving.h',r/'bin/reference-driving-probe.exe']})
(r/'reports/reference-driving-module-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='hashes'},indent=2))
