import csv,json,sys,hashlib
from pathlib import Path
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/compare_bound_movement_world_rom.py').read_text().split('schema=')[0])
address=0x087556cc
movement=[{k:int(v) for k,v in row.items()} for row in csv.DictReader((root/'reports/live-movement-native4k.movement.csv').open())]
queries=[{k:int(v) for k,v in row.items()} for row in csv.DictReader((root/'reports/live-movement-native4k.queries.csv').open())]
assert sum(v['queries'] for v in movement)==len(queries)
cursor=0;groups=[]
for row in movement:
 group=queries[cursor:cursor+row['queries']];cursor+=row['queries']
 assert all((q['source_counter'],q['waypoint'])==(row['source_counter'],row['waypoint']) for q in group)
 groups.append(group)
indices=set(round(i*(len(movement)-1)/63) for i in range(64))
late=[i for i,v in enumerate(movement) if 15<=v['waypoint']<=18]
indices.update(late[round(i*(len(late)-1)/31)] for i in range(32))
indices.update(sorted(range(len(movement)),key=lambda i:max((q['height']-q['query_y'] for q in groups[i]),default=-2147483648),reverse=True)[:16])
steps=0
for i in sorted(indices):
 v=movement[i];case=tuple(v[k] for k in ['start_x','start_y','start_z','dx','dy','dz'])+(0,v['preserve_y'])
 probe=CombinedMovementProbe(case);probe.arm.mem[0x3006810]=address;probe.run(entry=0x802a27c,stop=0x10020000)
 expected=[v[k] for k in ['result_x','result_y','result_z','normal_x','normal_y','normal_z','queries','collided']]
 assert probe.outputs()==expected,dict(index=i,original=probe.outputs(),captured=expected)
 assert probe.queries==[tuple(q[k] for k in ['query_x','query_y','query_z']) for q in groups[i]],dict(index=i,query_mismatch=True)
 steps+=probe.steps
traction=[list(map(int,l.split())) for l in (root/'reports/live-movement-native4k.traction.txt').read_text().splitlines()]
motion=[list(map(int,l.split())) for l in (root/'reports/live-movement-native4k.motion.txt').read_text().splitlines()]
assert all(len(v)==23 for v in traction) and all(len(v)==54 for v in motion)
from collections import Counter
report=dict(status='PASS_CAPTURED_POSITION_AND_QUERY_SEQUENCE',movement_records=len(movement),query_records=len(queries),original_compared_records=len(indices),thumb_instructions=steps,traction_records=len(traction),traction_flag8_records=sum(bool(v[7]&8) for v in traction),motion_records=len(motion),motion_delta_histogram=dict(Counter(v[5] for v in motion)),rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual native successful delivery movement inputs and captured position/normal/query count/collision and ordered XYZ query sequence compared with original2A27C nested original29E84 on ROM geometry. Uniform whole-run, waypoint15..18 and largest height-step samples. Offline qualified sqrt/division and containment contracts. Does not compare impact/postheight caller or independent original whole trajectory; trace overhead may affect elapsed cadence. No attribution of earlier failure or proof of stable60.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_live_captured_movement_rom.py',root/'src/movement.c',root/'reports/live-movement-native4k.movement.csv',root/'reports/live-movement-native4k.queries.csv']}
(root/'reports/live-captured-movement-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))

