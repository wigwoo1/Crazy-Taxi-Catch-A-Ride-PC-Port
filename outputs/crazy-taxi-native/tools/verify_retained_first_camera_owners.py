import json,struct,subprocess
from pathlib import Path
r=Path('outputs/crazy-taxi-native');p=r/'reports/bound-first-frame-flag-bitmap-prefix-next-dependency.json';c=json.loads(p.read_text())['frame_camera_calls'][0];a=c['owner_inputs']['actor'];g=c['owner_inputs']['globals'];signed=lambda x:x-4294967296 if x>=2147483648 else x
values=[a[0],a[1],a[2],a[3],g[0],g[1],g[2],a[4],a[5],g[3],g[4]&255,g[5],g[6],a[6],*a[7:10],a[10],a[11],a[12],(g[7]>>16)&255]
assert a[10]==2
cp=subprocess.run([str(r/'bin/retained-camera-owner-compare.exe')],input=' '.join(map(str,map(signed,values)))+'\n',capture_output=True,text=True);assert cp.returncode==0
native=list(map(int,cp.stdout.split()));output=c['owner_outputs'];expected=list(struct.unpack('<3i3h',bytes.fromhex(c['camera_after'])[:18]));got=native[6:12]
retained=[signed(output['actor'][0]),signed(output['actor'][2]),signed(output['globals'][0]),signed(output['globals'][2]),output['actor'][12]];actual=native[:4]+[native[12]]
passed=got==expected and retained==actual
report=dict(status='NATIVE_RETAINED_FIRST_CAMERA_OWNER_PASS' if passed else 'NATIVE_RETAINED_CAMERA_DEPENDENCY',source_instructions=c['instructions'],native_view=got,original_view=expected,retained_fields=5,retained_equal=retained==actual,scope='Actual original14DD8 first retained frame inputs from shared source caller vs production native yaw/pitch/offset/mode2 view. Six view fields and five retained owners exact. Height/distance intermediate fields not captured; subsequent camera calls not yet exported in completed run. Idle constructed prefix, no active gameplay trajectory/camera pixel fidelity claim.')
(r/'reports/retained-first-camera-owner-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
