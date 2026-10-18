import csv,json,hashlib
from pathlib import Path
root=Path('outputs/crazy-taxi-native/reports');traces={};report=[]
for stick in (-32768,-16384,0,7849,7850,16384,32767):
 p=root/f'challenge-analog-{stick}.csv';rows=[{k:int(v) for k,v in row.items()} for row in csv.DictReader(p.open())];traces[stick]=rows
 assert len(rows)==290 and all(not r['collided'] for r in rows)
 magnitude=abs(stick);command=0 if magnitude<=7849 else (8+(magnitude-7850)*184//((32768 if stick<0 else 32767)-7850))*(1 if stick<0 else -1)
 assert all(r['stick']==stick and r['analog_command']==command for r in rows)
 if abs(stick)>=16384:
  lateral=[r['view_dx'] for r in rows if r['view_dx']]
  assert lateral and all(x*stick>0 for x in lateral)
 assert all(r['view_dz']>=0 for r in rows)
 report.append(dict(stick=stick,command=command,trace_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),final_heading=rows[-1]['heading'],final_frame=rows[-1]['frame']))
for key in ('x','y','z','heading','camera_yaw','frame','speed'):
 assert [r[key] for r in traces[0]]==[r[key] for r in traces[7849]]
assert traces[7850][-1]['heading']!=traces[0][-1]['heading']
(root/'challenge-analog-qualified.json').write_text(json.dumps(dict(samples=report,scope='Injected Xbox decode/stick conversion through production drive_tick analog command branch and native physics/camera; scripted throttle and drive selection. Not physical hardware, live trigger/action mapping or original-ROM execution verification.'),indent=2)+'\n')
print('PASS7 injected stick samples: left/right motion signs, no contacts, neutral/dead-zone equal trajectories, first outside-dead-zone changes heading.')
