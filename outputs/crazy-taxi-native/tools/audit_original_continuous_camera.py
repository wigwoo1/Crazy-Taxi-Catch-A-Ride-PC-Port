import pathlib,json,struct,subprocess
r=pathlib.Path('outputs/crazy-taxi-native');results=[]
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];steps=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']]
 payload=''.join(f"{struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0]} {a['frame_delta']} {struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0]} {b['frame_delta']}\n" for a,b in steps)
 cp=subprocess.run([str(r/'bin/original-continuous-camera-probe.exe')],input=payload,text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(steps);mismatches=[]
 for (_,record),row in zip(steps,rows):
  actor=bytes.fromhex(record['actor']);shared=bytes.fromhex(record['shared_owners']['020025D0']);w=lambda a:struct.unpack_from('<i',actor,a)[0];expected=[0,w(0x654),w(0x650),struct.unpack_from('<i',shared,0x48)[0],struct.unpack_from('<i',shared,0x58)[0],w(0x76c),w(0x770)]+list(struct.unpack_from('<iii',actor,0x498))+list(struct.unpack_from('<hhh',actor,0x4a4))
  if row!=expected:mismatches.append({'tick':record['tick'],'native':row,'source':expected})
 results.append({'side':side,'updates':len(steps),'mismatches':len(mismatches),'first_mismatch':mismatches[0] if mismatches else None,'last_native':rows[-1]})
assert all(not x['mismatches'] for x in results)
report={'status':'PASS_CONTINUOUS_NATIVE_MOVING_CAMERA_155_UPDATES','scope':'Native driving and camera history baseline-seeded once. Fixed preset/target/freeze owners currently retained from baseline; full camera output/state checked against original observed updates. Not pixels or host framing proof.','results':results};(r/'reports/original-continuous-camera-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
