"""Qualify actual digital steering traces; no physical Xbox/ROM oracle claim."""
import csv,hashlib,json
from pathlib import Path
root=Path('outputs/crazy-taxi-native/reports')
result={'scope':'Scripted digital commands through production drive_tick, physics and camera. No physical Xbox, original execution trajectory, or collision-cause proof.','directions':[]}
for side,sign in [('left',-1),('right',1)]:
 p=root/f'challenge-steering-{side}.csv'
 rows=[{k:int(v) for k,v in r.items()} for r in csv.DictReader(p.open())]
 assert len(rows)==320
 moving=[r for r in rows if r['tick']<319 and (r['dx'] or r['dz'])]
 visible=[r for r in moving if r['view_dx']]
 assert visible and all(r['view_dx']*sign>0 for r in visible)
 assert all(r['view_dz']>=0 and r['away_dot']>0 for r in moving)
 for i in range(1,len(rows)):
  assert rows[i]['dx']==rows[i]['x']-rows[i-1]['x']
  assert rows[i]['dz']==rows[i]['z']-rows[i-1]['z']
 result['directions'].append(dict(direction=side,sha256=hashlib.sha256(p.read_bytes()).hexdigest(),qualified_last_tick=318,moving_updates=len(moving),nonzero_lateral_updates=len(visible),final_qualified=rows[318],excluded_boundary=rows[319],boundary_note='Right tick319 backward/lateral reversal and speed collapse needs contact/state trace; excluded from steering inference, not hidden.' if side=='right' else 'Tick319 retained separately to use identical qualification window.'))
(root/'challenge-steering-qualified.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS actual digital left/right camera-space steering through tick318; right boundary319 explicitly unresolved.')
