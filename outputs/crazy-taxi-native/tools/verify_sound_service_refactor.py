from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';cases=[]
for name,ticks,mode in [('normal',120,['--drive','--recovered-camera','--taxi-owned-graphics']),('retry',1000,['--challenge-bound-retry-check']),('failure',1800,['--challenge-bound-failure-check'])]:
 results=[]
 for version,exe in [('before','scene-viewer-host-action-refactor.exe'),('after','scene-viewer-sound-service-refactor.exe')]:
  label='sound-service-refactor-'+name+'-'+version;image=reports/(label+'.bmp')
  p=subprocess.run([str(root/'bin'/exe),*mode,'--ticks',str(ticks),'--snapshot',str(image)],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
  (reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr)
  assert p.returncode==0 and not p.stderr,(name,version,p.returncode,p.stderr)
  results.append(hashlib.sha256(image.read_bytes()).hexdigest())
 assert results[0]==results[1],(name,results)
 cases.append(dict(case=name,ticks=ticks,exit_before=0,exit_after=0,snapshot_byte_exact=True,sha256=results[0]))
 print('PASS',name,flush=True)
report=dict(status='PASS_SOUND_SERVICE_EXTRACTION',cases=cases,scope='Production sound service extracted without changing its ordinary call site; scripted normal120/Boundretry1000/Boundfailure1800 exit0 emptyerr and snapshots byte exact versus prior production. No active audio/channel/controller-state or hardware coverage in these neutral snapshots; challenge sound integration remains pending.')
(reports/'sound-service-refactor-verification.json').write_text(json.dumps(report,indent=2))
