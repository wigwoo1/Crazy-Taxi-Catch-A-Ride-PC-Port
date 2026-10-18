from pathlib import Path
import subprocess,os,json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';cases=[]
for driver in [0,3]:
 label='bound-audio-retry-reset-driver'+str(driver);env=os.environ.copy();env['TAXI_BOUND_AUDIO_RETRY_CHECK']='1'
 p=subprocess.run([str(root/'bin/scene-viewer-bound-audio-retry-reset-state.exe'),'--challenge-bound-retry-check','--driver',str(driver),'--recovered-game-audio','--audio-output-disabled','--ticks','1000','--snapshot',str(reports/(label+'.bmp'))],env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr)
 boundaries=[line for line in p.stdout.splitlines() if line.startswith('BOUND AUDIO RETRY BOUNDARY PASS:')]
 cases.append(dict(driver=driver,exit=p.returncode,stderr=p.stderr,boundary_passes=len(boundaries),boundary=boundaries))
 print('driver',driver,'exit',p.returncode,'boundaries',len(boundaries),'stderr',p.stderr,flush=True)
report=dict(status='CAPTURED_NATIVE_AUDIO_RETRY_BOUNDARY',cases=cases,candidate_sha256=hashlib.sha256((root/'bin/scene-viewer-bound-audio-retry-reset-state.exe').read_bytes()).hexdigest(),scope='Isolated actual audio-enabled Bound retry boundary: shared failure/scalar/actor reset plus sound cleared/retained fields/shared commit assertions. Logical mixer callback cadence explicit harness, not original audible/hardware/full trajectory verification.')
(reports/'bound-audio-retry-reset-boundary-verification.json').write_text(json.dumps(report,indent=2))
assert all(case['exit']==0 and not case['stderr'] and case['boundary_passes']==1 for case in cases)
report['status']='PASS_NATIVE_BOUND_AUDIO_RETRY_BOUNDARY';(reports/'bound-audio-retry-reset-boundary-verification.json').write_text(json.dumps(report,indent=2));print('PASS both audio-enabled actual retry boundaries and1000tickfixtures.')

reset_rows=[]
for driver in [0,3]:
 log=(reports/('bound-audio-retry-reset-driver'+str(driver)+'.log')).read_text();rows=[line.split(':',1)[1].strip() for line in log.splitlines() if line.startswith('BOUND AUDIO RESET:')];assert len(rows)==1;reset_rows.extend(rows)
(reports/'bound-audio-retry-reset-states.txt').write_text('\n'.join(reset_rows)+'\n')
