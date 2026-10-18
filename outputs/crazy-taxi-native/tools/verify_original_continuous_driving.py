import pathlib,json,struct,subprocess,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');results=[]
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];steps=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']]
 for mode in ('prior','post'):
  lines=[]
  for a,b in steps:
   owner=a if mode=='prior' else b
   lines.append(f"{struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0]} {a['frame_delta']} {struct.unpack_from('<i',bytes.fromhex(owner['time_commands']))[0]} {owner['frame_delta']}")
  cp=subprocess.run([str(r/'bin/original-continuous-drive-probe.exe')],input='\n'.join(lines)+'\n',text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(steps)
  mismatches=[]
  for (_,b),row in zip(steps,rows):
   actor=bytes.fromhex(b['actor']);expected=[0]+list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,0x1c)[0],struct.unpack_from('<i',actor,0x544)[0]]
   if row!=expected:mismatches.append({'tick':b['tick'],'native':row,'source':expected})
  if mode=='post':assert not mismatches
  else:assert mismatches[0]['tick']==89
  results.append({'side':side,'physics_owner':mode,'steps':len(steps),'mismatches':len(mismatches),'first_mismatch':mismatches[0] if mismatches else None,'last_native':rows[-1]})
report={'status':'PASS_CONTINUOUS_NATIVE_DRIVING_POST_PHYSICS_PRIOR_INPUT','matched_updates':155,'binary_sha256':hashlib.sha256((r/'bin/original-continuous-drive-probe.exe').read_bytes()).hexdigest(),'scope':'Two360VBlank source journeys, one original boot4200 state import per run. Continuous native input/driver/world with recovered city1 queries/runtime data: xyz/heading/speed exact every observed update. Hardware inputs and cadence supplied from original observations. Not full actor-byte equivalence, host scheduler equivalence, other routes/collisions/gameplay, camera pixels or complete port verification.','results':results}
(r/'reports/original-continuous-driving-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['matched_updates'])
