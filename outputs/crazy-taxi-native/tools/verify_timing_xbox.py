import json,pathlib,hashlib
root=pathlib.Path('outputs/crazy-taxi-native'); captures={}; traces={}
for direction in ('left','right','center','deadzone'):
 p=root/f'reports/native-timing-xbox-{direction}.json'
 captures[direction]=json.loads(p.read_text())
 traces[direction]=[list(map(int,line.split())) for line in (root/f'reports/native-timing-xbox-{direction}-camera.txt').read_text().splitlines()]
 assert len(traces[direction])==174 and all(len(row)==32 for row in traces[direction])
 assert captures[direction]['vehicle'][:3]!=[22079,19200,-33983]
assert all(traces[d][:144]==traces['center'][:144] for d in traces)
assert captures['left']['vehicle'][3]>14737408>captures['right']['vehicle'][3]
assert captures['center']==captures['deadzone'] and traces['center']==traces['deadzone']
report={'status':'PASS','captures':captures,'common_prefix_camera_calls':144,'deadzone_exact_match':True,'scope':'Recorded Xbox sample decode through native driving: RT alone moves vehicle; full left increases heading, full right decreases, matching independently observed original directions. Stick value7849 matches centered input exactly. No physical Xbox device or full trajectory equivalence verified; diagnostic recorded source timing only. Installed playable unchanged.','binary_sha256':hashlib.sha256((root/'bin/scene-viewer-timing-xbox.exe').read_bytes()).hexdigest()}
(root/'reports/native-timing-xbox-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
