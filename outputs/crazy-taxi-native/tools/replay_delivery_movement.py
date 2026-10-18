import json,subprocess,hashlib,csv
from pathlib import Path
from collections import Counter
r=Path('outputs/crazy-taxi-native');schema=json.loads((r/'reports/ordinary-full-world-state-schema.json').read_text())['fields']
rows=[dict(zip(schema,map(int,line.split()[1:]))) for line in (r/'reports/delivery-full-world-native4k.txt').read_text().splitlines()]
def signed(v):return (v+2**31)%2**32-2**31
cases=[tuple(v[k] for k in ['input_x','input_y','input_z'])+(signed(v['dx']+(v['input_impulse_74c']>>12)),v['dy'],signed(v['dz']+(v['input_impulse_750']>>12)),int((v['settings']&224)==128 and v['submode']==5)) for v in rows]
cp=subprocess.run([str(r/'bin/delivery-movement-replay.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True)
out=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(out)==len(rows)
records=[]
for i,(v,c,o) in enumerate(zip(rows,cases,out)):
 assert o[0]==0 and (o[1],o[3])==(v['output_x'],v['output_z']),dict(index=i,replay=o,fullworld=v)
 records.append(dict(index=i,start_x=c[0],start_y=c[1],start_z=c[2],dx=c[3],dy=c[4],dz=c[5],preserve_y=c[6],result_x=o[1],result_y=o[2],result_z=o[3],normal_x=o[4],normal_y=o[5],normal_z=o[6],queries=o[7],collided=o[8],wall_queries=o[9],maximum_height_step=o[10],delta=v['delta'],speed_before=v['speed544'],speed_after=v['output_speed544']))
with (r/'reports/delivery-full-world-movement-replay.csv').open('w',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(records[0]));w.writeheader();w.writerows(records)
bend=[v for v in records if -360000<=v['start_x']<=-350000 and 80000<=v['start_z']<=90000]
def counts(v):return dict(records=len(v),collisions=sum(x['collided'] for x in v),wall_contacts=sum(x['wall_queries']>0 for x in v),height_step_refusals=sum(x['collided'] and x['wall_queries']==0 and x['maximum_height_step']>=655 for x in v),nonzero_request_zero_xz=sum((x['dx'] or x['dz']) and (x['result_x'],x['result_z'])==(x['start_x'],x['start_z']) for x in v))
report=dict(status='PASS_ALL_REPLAY_XZ_EQUALS_CAPTURE',whole_run=counts(records),bend=counts(bend),bend_collided_delta_histogram=dict(Counter(v['delta'] for v in bend if v['collided'])),maximum_height_step=max(v['maximum_height_step'] for v in bend),first_bend_collision=next((v for v in bend if v['collided']),None),scope='Native position-only replay using actual captured complete world inputs including wrapped impulse additions and preserveY predicate. All7973 resultXZ independently equal recorded full-world resultXZ. Collision classification from unchanged native movement and recovered city1 geometry. Does not independently prove original collision/query sequence on allcases, impact output equivalence or why input-only navigation attempted impassable terrain; prior128fullworldROM samples remain separate evidence.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'tests/delivery_movement_replay.c',r/'bin/delivery-movement-replay.exe',r/'reports/delivery-full-world-native4k.txt',r/'assets/city1.ctw']})
(r/'reports/delivery-full-world-movement-replay.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
