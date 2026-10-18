from pathlib import Path
import json,struct,subprocess,hashlib,math
r=Path('outputs/crazy-taxi-native');binary=r/'bin/original-continuous-drive-probe.exe';results=[]
for side in ['left','right']:
 source=r/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json';records=json.loads(source.read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];control=[];original={};elapsed=0
 for a,b in pairs:
  raw=struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0];delta=struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0];control.append(f"{raw} {a['frame_delta']} {delta} {b['frame_delta']}");elapsed+=delta;actor=bytes.fromhex(b['actor']);original[elapsed]=[0]+list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,28)[0],struct.unpack_from('<i',actor,1348)[0]]
 def run(lines):
  cp=subprocess.run([str(binary)],input='\n'.join(lines)+'\n',capture_output=True,text=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(lines) and all(x[0]==0 for x in rows);return rows
 assert run(control)==list(original.values())
 # Reconstruct only the explicitly injected input schedule from the capture tool:
 # A held throughout, left/right additionally held from VBlank index180.
 # Physics cumulative ticks are four ahead of elapsed capture ticks in this fixture.
 fixed=[]
 for tick in range(5,elapsed+1,5):
  raw=1022 if tick<185 else (990 if side=='left' else 1006)
  fixed.append(f'{raw} 5 5 5')
 rows=run(fixed);native={5*(i+1):row for i,row in enumerate(rows)};common=sorted(original.keys()&native.keys());assert common
 mismatches=[dict(cumulative_tick=t,source=original[t],native=native[t],distance_xz=math.hypot(original[t][1]-native[t][1],original[t][3]-native[t][3])) for t in common if original[t]!=native[t]]
 results.append(dict(side=side,source_variable_updates=len(original),fixed_five_updates=len(native),equal_elapsed_checkpoints=len(common),mismatched_checkpoints=len(mismatches),first_mismatch=mismatches[0] if mismatches else None,last_shared_tick=common[-1],last_source=original[common[-1]],last_native=native[common[-1]],last_distance_xz=math.hypot(original[common[-1]][1]-native[common[-1]][1],original[common[-1]][3]-native[common[-1]][3]),source_sha256=hashlib.sha256(source.read_bytes()).hexdigest()))
report=dict(status='COMPLETED_FIXED_FIVE_SOURCE_TRAJECTORY_COMPARISON',results=results,scope='Control native original variable schedule matches all495 sourceXYZ/heading/speed endpoints. Fixed5 native steps compared ONLY at exactly equal cumulative physics elapsed ticks. Synthetic input schedule reconstructs explicitly held A and turn from capturedVBlank180, with mid-frame baseline4tick phase correction; different input-producer polling times may affect transition. Not the live minimum5 policy, which can overshoot; not original fixed5 CPU oracle, wholeactor/gameplay/audio/RNG or general scheduler proof. No physics coefficients changed.',binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest())
(r/'reports/fixed-five-source-trajectory-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(results,indent=2))
