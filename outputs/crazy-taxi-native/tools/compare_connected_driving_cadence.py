import json,struct,subprocess,hashlib,math
from pathlib import Path
r=Path('outputs/crazy-taxi-native');binary=r/'bin/original-continuous-drive-probe.exe';results=[]
def run(lines):
 p=subprocess.run([str(binary)],input='\n'.join(lines)+'\n',capture_output=True,text=True,check=True)
 rows=[list(map(int,l.split())) for l in p.stdout.splitlines()];assert len(rows)==len(lines) and all(v[0]==0 for v in rows);return rows
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']]
 source_lines=[];fine_lines=[];end_indices=[];expected=[];physics_ticks=0
 for a,b in pairs:
  raw=struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0];delta=struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0];frame=b['frame_delta'];assert delta>0 and frame==delta
  source_lines.append(f"{raw} {a['frame_delta']} {delta} {frame}")
  for j in range(delta):fine_lines.append(f'{raw} 1 1 1')
  end_indices.append(len(fine_lines)-1);physics_ticks+=delta;actor=bytes.fromhex(b['actor']);expected.append([0]+list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,0x1c)[0],struct.unpack_from('<i',actor,0x544)[0]])
 coarse=run(source_lines);assert coarse==expected,'Recorded original cadence regression failed'
 fine=run(fine_lines);mismatch=[dict(source_tick=b['tick'],elapsed_physics_ticks=sum(struct.unpack_from('<i',bytes.fromhex(pair[1]['time_commands']))[0] for pair in pairs[:i+1]),source=coarse[i],unit_tick_native=fine[end_indices[i]]) for i,(a,b) in enumerate(pairs) if coarse[i]!=fine[end_indices[i]]]
 assert mismatch
 a,b=coarse[-1],fine[-1]
 results.append(dict(side=side,source_matched_updates=len(coarse),elapsed_physics_ticks=physics_ticks,unit_tick_updates=len(fine),mismatched_group_endpoints=len(mismatch),first_mismatch=mismatch[0],last_source=a,last_unit_tick_native=b,final_distance_xz=math.hypot(a[1]-b[1],a[3]-b[3]),final_speed_difference=b[5]-a[5]))
report=dict(status='SOURCE_CADENCE_EXACT_UNIT_TICK_NATIVE_DIVERGES',results=results,binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),scope='Same boot4200 native baseline eachrun. Recorded original input/physics ownership at155updates remains exactXYZ/heading/speed. Synthetic alternative splits each original delta into1tick updates, holds that recorded postinput raw sample throughout eachinterval, and gives inputproducerelapsed1. Totalphysics/sourceframe ticks matched perjourney; inputproducerprior-delta semantics and subintervalinput timing differ deliberately. Demonstrates combined native input/driver/world updatefrequency dependence, NOToriginal-ROM1tick oracle, causal coefficient isolation, actualhosttrajectory, correctedcadence or validated60Hzretiming.')
report['hashes']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'tests/original_continuous_drive_probe.c',r/'src/drive_world.c',r/'reports/original-core-reference/fullsharedleft.packed-driving-trace.json',r/'reports/original-core-reference/fullsharedright.packed-driving-trace.json']}
(r/'reports/connected-driving-cadence-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
