from pathlib import Path
import os,subprocess,json
r=Path('outputs/crazy-taxi-native');rep=r/'reports';cases=[]
for name in ['restart','quit']:
 ticks=450;keys=[0]*ticks
 for n,key in [(300,8),(305,128),(310,1),(315,64),(320,1)]:keys[n-1]=key
 if name=='quit':keys[307-1]=128
 key=rep/f'bound-pause-exit-{name}-keys.txt';step=rep/f'bound-pause-exit-{name}-ticks.txt';key.write_text('\n'.join(map(str,keys))+'\n');step.write_text('initial_delta 1\n'+'1\n'*ticks)
 env=os.environ.copy();env['TAXI_BOUND_PAUSE_TRACE']='1'
 p=subprocess.run([str(r/'bin/scene-viewer-bound-pause-exit-probe-v5.exe'),'--challenge-bound-drive-preview','--gameplay-key-replay',str(key),'--gameplay-step-replay',str(step),'--ticks',str(ticks),'--snapshot',str(rep/f'bound-pause-exit-{name}.bmp')],env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (rep/f'bound-pause-exit-{name}.log').write_text(p.stdout);(rep/f'bound-pause-exit-{name}.err').write_text(p.stderr);print(name,p.returncode,p.stderr,flush=True)
 assert p.returncode==0 and not p.stderr
 exits=[l for l in p.stdout.splitlines() if l.startswith('BOUND PAUSE EXIT:')];assert len(exits)==1 and f'route{1 if name=="restart" else 2}' in exits[0],exits
 if name=='quit':assert 'BOUND PAUSE GRID: stage15 selection7 unfrozen fade-settled normal-menu-ticks33' in p.stdout
 requests=[int(l.split(':',1)[1],16) for l in p.stdout.splitlines() if l.startswith('BOUND PAUSE EXIT REQUEST:')]
 expected=[0x8010aac,0x8027754,0x802771c,0x8027730]+([] if name=='restart' else [0x8027730,0x8026ef8,0x8027110])
 assert requests==expected,(requests,expected)
 cases.append({'case':name,'exit':0,'transition':exits[0]})
(rep/'bound-pause-exit-verification.json').write_text(json.dumps({'status':'PASS_ISOLATED_NATIVE_BOUND_PAUSE_EXITS','cases':cases,'scope':'Actual timed Start/menu navigation/Yes inputs; native fade/reset/grid paths, car/effects/timer freeze assertions. Audio-enabled callbacks, original full exit state and current physical controller remain unverified.'},indent=2)+'\n')
