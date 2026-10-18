from pathlib import Path
import subprocess,os,json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';cases=[]
source=json.loads((reports/'bound-main-audio-overlay-source.json').read_text());assert hashlib.sha256((root/'src/scene_viewer.c').read_bytes()).hexdigest()==source['main_source_sha256']
for mode,ticks in [('failure',1800),('retry',1000)]:
 for driver in [0,3]:
  label='bound-main-audio-'+mode+'-driver'+str(driver);env=os.environ.copy();env['TAXI_BOUND_AUDIO_STATE_TRACE']='1';env['TAXI_BOUND_AUDIO_RETRY_CHECK']='1';env['TAXI_DRIVER_TRACE']=str(reports/(label+'-driver.txt'));env['TAXI_WORLD_TRACE']=str(reports/(label+'-world.txt'))
  args=[str(root/'bin/scene-viewer-bound-main-audio-probe.exe'),'--challenge-bound-'+mode+'-check','--driver',str(driver),'--recovered-game-audio','--audio-output-disabled','--ticks',str(ticks),'--snapshot',str(reports/(label+'.bmp')),'--effect-state-trace',str(reports/(label+'-effects.txt'))]
  p=subprocess.run(args,env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
  (reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr);assert p.returncode==0 and not p.stderr,(mode,driver,p.stderr)
  state='\n'.join(line.split(':',1)[1].strip() for line in p.stdout.splitlines() if line.startswith('BOUND AUDIO STATE:'))+'\n';(reports/(label+'-states.txt')).write_text(state)
  if mode=='failure':
   prior='bound-audio-service-state-capture'+('-driver3' if driver else '');assert state==(reports/(prior+'-states.txt')).read_text()
   assert (reports/(label+'.bmp')).read_bytes()==(reports/(prior+'.bmp')).read_bytes()
   if driver==0:
    for suffix in ['driver.txt','world.txt','effects.txt']:
     assert (reports/(label+'-'+suffix)).read_bytes()==(reports/('bound-audio-presentation-capture-'+suffix)).read_bytes(),suffix
  else:
   reset=[line.split(':',1)[1].strip() for line in p.stdout.splitlines() if line.startswith('BOUND AUDIO RESET:')];assert len(reset)==1
   prior='bound-audio-cleanup-retry-driver'+str(driver);prior_log=(reports/(prior+'.log')).read_text();prior_reset=[line.split(':',1)[1].strip() for line in prior_log.splitlines() if line.startswith('BOUND AUDIO RESET:')];assert reset==prior_reset
   assert (reports/(label+'.bmp')).read_bytes()==(reports/(prior+'.bmp')).read_bytes()
  cases.append(dict(mode=mode,driver=driver,exit=0,snapshot_exact_qualified_candidate=True,audio_or_reset_state_exact=True));print('PASS main',mode,driver,flush=True)
report=dict(status='PASS_MAIN_BOUND_AUDIO_INTEGRATION',cases=cases,main_source_sha256=source['main_source_sha256'],probe_sha256=hashlib.sha256((root/'bin/scene-viewer-bound-main-audio-probe.exe').read_bytes()).hexdigest(),scope='Current main functional source with observers/strict logicalaudio snapshot service. Failure0/3: all916 audio controller pre/post/request rows and finalBMP exact originalqualifiedcandidate; driver0 full680driver/882world/882effect captures byteexact qualified same-run references. Retry0/3 actual50fieldreset andfinalBMP exact priororiginalqualifiedreset candidate; existingmainretrychecks pass. Originalbridge/globalinit/callbackbodies/mixercadence/frozenhardware/fullport still incomplete.')
(reports/'bound-main-audio-integration-verification.json').write_text(json.dumps(report,indent=2));print('PASS current main audio-enabled integration4cases.')
