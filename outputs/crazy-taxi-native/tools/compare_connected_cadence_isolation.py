import json,struct,subprocess,hashlib,math
from pathlib import Path
r=Path('outputs/crazy-taxi-native');binary=r/'bin/connected-cadence-isolation-probe.exe';results=[]
for side in ('left','right'):
 trace=r/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json';records=json.loads(trace.read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];lines=[];expected=[]
 for a,b in pairs:
  raw=struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0];delta=struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0];lines.append(f"{raw} {a['frame_delta']} {delta} {b['frame_delta']}");actor=bytes.fromhex(b['actor']);expected.append([0]+list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,0x1c)[0],struct.unpack_from('<i',actor,0x544)[0]])
 payload='\n'.join(lines)+'\n'
 for mode in range(3):
  cp=subprocess.run([str(binary),str(mode)],input=payload,capture_output=True,text=True,check=True);rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(expected) and all(v[0]==0 for v in rows)
  mismatch=[dict(update=i,source_tick=pairs[i][1]['tick'],source=e,native=n) for i,(e,n) in enumerate(zip(expected,rows)) if e!=n]
  if mode==0:assert not mismatch,'Control baseline diverged'
  results.append(dict(side=side,mode=mode,meaning=['control_source_sampling_and_physics','source_input_sampling_split_physics','split_input_sampling_source_physics'][mode],updates=len(rows),mismatched_endpoints=len(mismatch),first_mismatch=mismatch[0] if mismatch else None,last_original=expected[-1],last_native=rows[-1],final_distance_xz=math.hypot(expected[-1][1]-rows[-1][1],expected[-1][3]-rows[-1][3]),final_speed_difference=rows[-1][5]-expected[-1][5]))
report=dict(status='COMPLETED_CONNECTED_CADENCE_ISOLATION',results=results,scope='Fresh originalboot4200 native baseline permode, same495observed original endpoint rawkeys. Control reproduces originalXYZ/heading/speed. Mode1 samples original inputproducer once with originalprior delta, retains fixed action/held values and restores that sampled command at each physics substep; originaldelta subdivided into1 with equal total physics/frame ticks. Mode2 splits inputproducer prior delta into1tick calls at same constant rawkeys, physics retains originaldelta and onecall. Synthetic experiments isolate these invocation policies, not every per-frame event ownership, original1tick CPU oracle or proven livehostbug. No production physics changed; score/audio/traffic/passenger/fullactor not compared.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'tests/connected_cadence_isolation_probe.c',binary,r/'src/drive_world.c',r/'src/input.c']})
(r/'reports/connected-cadence-isolation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='hashes'},indent=2))
