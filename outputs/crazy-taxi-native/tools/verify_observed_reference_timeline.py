from pathlib import Path
import subprocess,json,struct,hashlib
root=Path('outputs/crazy-taxi-native');binary=root/'bin/reference-driving-probe.exe';results=[]
for side in ['left','right']:
 schedule=root/f'reports/longsource{side}.observed.schedule.txt';cp=subprocess.run([str(binary),str(schedule),'--timeline'],capture_output=True,text=True,check=True)
 rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];records=json.loads((root/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json').read_text())['records'];baseline=records[0]['counter'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];assert len(rows)==len(pairs)
 for i,((a,b),row) in enumerate(zip(pairs,rows)):
  actor=bytes.fromhex(b['actor']);expected=[(b['counter']-baseline)&0xffffffff,i+1]+list(struct.unpack_from('<iii',actor,0x498))+list(struct.unpack_from('<hhh',actor,0x4a4));assert row==expected
 results.append(dict(side=side,source_exact_driving_and_camera_updates=len(rows),observed_timestamps_exact=len(rows),first_tick=rows[0][0],last_tick=rows[-1][0]))
report=dict(status='PASS_NATIVE_OBSERVED_REFERENCE_TIMELINE',results=results,scope='New V2 loader retains exact original physics inputs and reproduces native XYZ/heading/speed internally plus independently compared camera at all495 updates. Every loaded schedule timestamp equals observed source counter offset. V1 cumulative fixtures still pass separately. Headless test only; live renderer not rebuilt or checked with V2 yet. Sampled VBlank resolution, no general physics scheduler.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),binary,root/'src/reference_driving.c',root/'tests/reference_driving_probe.c']})
(root/'reports/observed-reference-timeline-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(results,indent=2))
