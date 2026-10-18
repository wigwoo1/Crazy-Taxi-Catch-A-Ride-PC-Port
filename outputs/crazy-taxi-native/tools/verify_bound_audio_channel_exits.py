from pathlib import Path
import subprocess,os,json
r=Path('outputs/crazy-taxi-native');rep=r/'reports';cases=[]
for name in ['restart','quit']:
 for driver in [0,3]:
  label=f'bound-audio-channel-exit-{name}-driver{driver}';env=os.environ.copy();env['TAXI_BOUND_PAUSE_TRACE']='1';env['TAXI_BOUND_AUDIO_STATE_TRACE']='1'
  args=[str(r/'bin/scene-viewer-bound-audio-channel-exit-capture.exe'),'--challenge-bound-drive-preview','--driver',str(driver),'--recovered-game-audio','--audio-output-disabled','--gameplay-key-replay',str(rep/f'bound-pause-exit-{name}-keys.txt'),'--gameplay-step-replay',str(rep/f'bound-pause-exit-{name}-ticks.txt'),'--ticks','450','--snapshot',str(rep/(label+'.bmp'))]
  p=subprocess.run(args,env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW);(rep/(label+'.log')).write_text(p.stdout);(rep/(label+'.err')).write_text(p.stderr);print(name,driver,p.returncode,p.stderr,flush=True)
  assert p.returncode==0 and not p.stderr
  requests=[int(l.split(':',1)[1],16) for l in p.stdout.splitlines() if l.startswith('BOUND PAUSE EXIT REQUEST:')]
  assert requests==[0x8010aac,0x8027754,0x802771c,0x8027730]+([] if name=='restart' else [0x8027730,0x8026ef8,0x8027110]),requests
  assert p.stdout.count('BOUND PAUSE EXIT:')==1
  if name=='quit':assert 'BOUND PAUSE GRID:' in p.stdout
  cases.append({'case':name,'driver':driver,'exit':p.returncode,'requests':requests})
(rep/'bound-audio-channel-exit-verification.json').write_text(json.dumps({'status':'PASS_NATIVE_AUDIO_ENABLED_PAUSE_EXIT_PATHS','cases':cases,'scope':'Isolated candidate actual timed keys, recovered audio enabled/output disabled; exact ordered parent requests and freeze assertions, native reset/grid route. Logical detailed restored channel/settings and original audio callback cadence, audible devices, physical Xbox, success registration remain unverified.'},indent=2)+'\n')
