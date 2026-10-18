"""Check measured native Jump camera geometry, not original-game fidelity."""
import csv, hashlib, json
from pathlib import Path
root = Path('outputs/crazy-taxi-native')
source = root / 'reports/challenge-camera-motion.csv'
rows = [{k: int(v) for k,v in row.items()} for row in csv.DictReader(source.open())]
assert len(rows) == 320
moving = []
for i,r in enumerate(rows):
    assert r['tick'] == i
    if i:
        assert r['dx'] == r['x']-rows[i-1]['x']
        assert r['dz'] == r['z']-rows[i-1]['z']
    assert r['away_dot'] == r['dx']*(r['x']-r['eye_x']) + r['dz']*(r['z']-r['eye_z'])
    heading = abs(r['heading'])//4096 * (-1 if r['heading'] < 0 else 1)
    difference = (r['camera_yaw']-heading+2048)%4096-2048
    assert r['frame'] == ((32-difference)&4095)>>6
    if r['dx'] or r['dz']:
        moving.append(r)
assert moving and all(r['away_dot']>0 and r['depth']>0 and r['frame']==0 for r in moving)
assert all(r['view_dz']>=0 for r in moving)
result = dict(trace_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),updates=len(rows),moving_updates=len(moving),first_moving_tick=moving[0]['tick'],last_moving_tick=moving[-1]['tick'],frames=sorted({r['frame'] for r in moving}),depth_range=[min(r['depth'] for r in moving),max(r['depth'] for r in moving)],finding='Measured baseline Jump movement is away from the native camera eye with positive taxi depth and frame0. Fixed-point camera rotation rounds some small movement to zero.',scope='Consistency of recorded native geometry and frame formula only. Does not prove original ROM trajectories, decoded artwork front/rear, steering sign, or rendered sprite placement.')
(root/'reports/challenge-camera-motion-qualified.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
