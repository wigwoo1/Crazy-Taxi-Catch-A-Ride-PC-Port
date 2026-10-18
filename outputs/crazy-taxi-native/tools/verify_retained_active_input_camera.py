import json,struct,subprocess
from pathlib import Path
r=Path('outputs/crazy-taxi-native');calls=json.loads((r/'reports/active-held-input/bound-retained-camera-owner-calls.json').read_text())['calls'];assert len(calls)==3
signed=lambda x:x-4294967296 if x>=2147483648 else x
cases=[];prior=None;carry=0
for index,c in enumerate(calls):
 a=c['owner_inputs']['actor'];g=c['owner_inputs']['globals'];assert a[10]==2
 owners=[a[0],a[2],g[0],g[2],a[12]]
 if prior is not None:assert owners==prior,(index,owners,prior);carry+=len(owners)
 values=[a[0],a[1],a[2],a[3],g[0],g[1],g[2],a[4],a[5],g[3],g[4]&255,g[5],g[6],a[6],*a[7:10],a[10],a[11],a[12],(g[7]>>16)&255]
 cp=subprocess.run([str(r/'bin/retained-camera-owner-compare.exe')],input=' '.join(map(str,map(signed,values)))+'\n',capture_output=True,text=True);assert cp.returncode==0
 native=list(map(int,cp.stdout.split()));o=c['owner_outputs'];expected=list(struct.unpack('<3i3h',bytes.fromhex(c['camera_after'])[:18]));got=native[6:12]
 prior=[o['actor'][0],o['actor'][2],o['globals'][0],o['globals'][2],o['actor'][12]]
 actual=native[:4]+[native[12]];assert actual==list(map(signed,prior)) and got==expected,(index,actual,prior,got,expected)
 cases.append(dict(frame=index+1,source_instructions=c['instructions'],view=got,defined_view_fields=6,retained_fields=5,differences=0))
report=dict(status='NATIVE_ORIGINAL_HELD_INPUT_CAMERA_SEQUENCE_PASS',cases=cases,between_frame_owner_checks=carry,scope='Three actual original shared caller14DD8 camera updates compared to native production helpers with retained owner continuity. Explicit held A+Right input startup fixture, external vehicle pose/preset inputs remain source captures. No independent native vehicle/warm trajectory or whole game/camera pixel fidelity claim; intermediate height/distance not captured.')
(r/'reports/retained-active-input-camera-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
