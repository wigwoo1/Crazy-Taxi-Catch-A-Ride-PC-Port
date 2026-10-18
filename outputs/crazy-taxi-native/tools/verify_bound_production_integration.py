from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';cases=[]
for name,ticks,mode,prior in [('normal',120,['--drive','--recovered-camera','--taxi-owned-graphics'],'scene-viewer-sound-service-refactor.exe'),('retry',1000,['--challenge-bound-retry-check'],'scene-viewer-ended-rear-probe.exe'),('failure',1800,['--challenge-bound-failure-check'],'scene-viewer-ended-rear-probe.exe')]:
 hashes=[]
 for version,exe in [('reference',prior),('integrated','scene-viewer-bound-integrated.exe')]:
  label='bound-production-'+name+'-'+version;image=reports/(label+'.bmp');p=subprocess.run([str(root/'bin'/exe),*mode,'--ticks',str(ticks),'--snapshot',str(image)],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
  (reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr);assert p.returncode==0 and not p.stderr,(name,version,p.returncode,p.stderr)
  hashes.append(hashlib.sha256(image.read_bytes()).hexdigest())
 assert hashes[0]==hashes[1],(name,hashes);cases.append(dict(case=name,ticks=ticks,exit=0,snapshot_byte_exact=True,sha256=hashes[1]));print('PASS',name,flush=True)
(reports/'bound-production-integration-verification.json').write_text(json.dumps(dict(status='PASS_MAIN_BOUND_NEUTRAL_INTEGRATION',cases=cases,scope='Main source normal120 unchanged versus priorproduction; Boundretry1000/failure1800 identical qualifiedneutralendedcandidate. Noaudio/hardware/fulltrajectory claim; mainaudioenabled integration still requires diagnosticjoinedverification.'),indent=2))
