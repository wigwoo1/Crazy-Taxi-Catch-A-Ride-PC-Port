from pathlib import Path
import os,subprocess,json
r=Path('outputs/crazy-taxi-native');rep=r/'reports'
for name,ticks in [('held',350),('resume',450)]:
 keys=[0]*ticks;keys[299]=8
 if name=='resume':keys[359]=8
 key=rep/f'bound-live-pause-{name}-keys.txt';step=rep/f'bound-live-pause-{name}-ticks.txt';key.write_text('\n'.join(map(str,keys))+'\n');step.write_text('initial_delta 1\n'+'1\n'*ticks)
 env=os.environ.copy();env['TAXI_BOUND_PAUSE_TRACE']='1'
 p=subprocess.run([str(r/'bin/scene-viewer-bound-live-pause-probe.exe'),'--challenge-bound-drive-preview','--taxi-owned-graphics','--gameplay-key-replay',str(key),'--gameplay-step-replay',str(step),'--ticks',str(ticks),'--snapshot',str(rep/f'bound-live-pause-{name}.bmp')],env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (rep/f'bound-live-pause-{name}.log').write_text(p.stdout);(rep/f'bound-live-pause-{name}.err').write_text(p.stderr);print(name,p.returncode,p.stderr,flush=True);assert p.returncode==0 and not p.stderr
 rows=[list(map(int,l.split(':',1)[1].split())) for l in p.stdout.splitlines() if l.startswith('BOUND PAUSE SAMPLE:')];assert len(rows)==ticks
 frozen=[a for a in rows if a[2]];assert len(frozen)==(51 if name=='held' else 60),len(frozen)
 assert len({tuple(a[4:8]) for a in frozen})==1
 if name=='resume':assert rows[-1][2]==0 and rows[-1][4]<frozen[0][4]
 print('PASS',name,'frozen',len(frozen),'holds',rows[-1][-1],flush=True)
(rep/'bound-live-pause-verification.json').write_text(json.dumps({'status':'PASS_ISOLATED_NATIVE_PAUSE_RESUME','cases':['held350','resume450'],'scope':'Timed physical-key replay native actual pause and resume; full car/effects/timer holds asserted, original shared input sampled once. Not original full trajectory, audio-enabled capture, physical controller/hardware, pause Retry/Quit or complete port.'},indent=2)+'\n')
