import csv,json,hashlib
from pathlib import Path
root=Path('outputs/crazy-taxi-native/reports');p=root/'challenge-contact-right.csv'
rows=[{k:int(v) for k,v in row.items()} for row in csv.DictReader(p.open())]
assert len(rows)==320
assert not any(r['collided'] for r in rows[:319])
r=rows[319]
assert r['collided']==1 and r['queries']==1
assert (r['normal_x'],r['normal_y'],r['normal_z'])==(0,0,256)
assert (r['x'],r['z'])==(r['corrected_x'],r['corrected_z'])
assert r['entry_speed']==rows[318]['speed']==27651 and r['speed']==3491
assert r['view_dz']<0 and r['away_dot']<0
report=dict(trace_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),first_contact_tick=319,boundary=r,scope='Measured production native movement reports obstacle contact and corrected position at tick319. Not independent ROM collision geometry/response fidelity proof; physical Xbox untested.')
(root/'challenge-contact-right-qualified.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS tick319 first reported obstacle contact: normal0,0,256 correctedposition retained and speed3491.')
