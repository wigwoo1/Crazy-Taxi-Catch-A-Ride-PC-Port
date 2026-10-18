"""Audit continuous native input evolution against offline original snapshots."""
import pathlib,json,struct,subprocess
r=pathlib.Path('outputs/crazy-taxi-native');ref=r/'reports/original-core-reference';results=[]
for side in ('left','right'):
 records=json.loads((ref/f'fullshared{side}.packed-driving-trace.json').read_text())['records']
 for mode in ('boundary_prior','boundary_post','history_prior','history_post'):
  steps=[]
  for prior,current in zip(records,records[1:]):
   changed=current['previous']!=prior['previous'] if mode.startswith('boundary') else current['input_history']!=prior['input_history']
   if changed:steps.append((prior,current))
  lines=[records[0]['input_history']]
  for prior,current in steps:
   raw=bytes.fromhex(current['input_history']);shared=current['shared_owners'];block=bytes.fromhex(shared['02001EA0']);flags=bytes.fromhex(shared['0200164C'])
   assert flags[0xbc]==0, 'Active override requires uncaptured170C mask'
   lines.append(f"{struct.unpack_from('<H',raw,2)[0]} {(prior if mode.endswith('prior') else current)['frame_delta']} {block[10]} {flags[0xbc]} {struct.unpack_from('<H',flags,0xc0)[0] if len(flags)>0xc1 else 0}")
  cp=subprocess.run([str(r/'bin/input-continuity-probe.exe')],input='\n'.join(lines)+'\n',text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(steps)
  mismatch=[]
  for (_,current),row in zip(steps,rows):
   expected=list(struct.unpack('<3H40B10H',bytes.fromhex(current['input_history'])[:66]))
   if row!=expected:mismatch.append({'tick':current['tick'],'fields':[n for n,(a,b) in enumerate(zip(row,expected)) if a!=b],'native':row,'source':expected})
  results.append({'side':side,'mode':mode,'steps':len(steps),'matched':len(steps)-len(mismatch),'first_mismatch':mismatch[0] if mismatch else None})
assert all(x['matched']==x['steps'] for x in results if x['mode'].endswith('prior'))
assert all(x['first_mismatch']['tick']==89 for x in results if x['mode'].endswith('post'))
report={'status':'PASS_CONTINUOUS_NATIVE_INPUT_PRIOR_FRAME_OWNER','scope':'Baseline-seeded continuous native input only; raw input values sampled from original history. Candidate invocation and prior/post frame owners audited, not full native trajectory. Original override inactive asserted in every step; sampled hardware inputs used, not a host controller test.','results':results}
(r/'reports/continuous-input-reference-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps([ {k:v for k,v in x.items() if k!='first_mismatch'}|{'first_mismatch_tick':x['first_mismatch']['tick'] if x['first_mismatch'] else None} for x in results],indent=2))
