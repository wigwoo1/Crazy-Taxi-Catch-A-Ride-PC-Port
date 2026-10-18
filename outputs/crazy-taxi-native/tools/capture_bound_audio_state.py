from pathlib import Path
import subprocess,os,json,sys,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';env=os.environ.copy();env['TAXI_BOUND_SOUND_TRACE']='1';env['TAXI_BOUND_AUDIO_STATE_TRACE']='1'
driver=int(sys.argv[1]) if len(sys.argv)>1 else 0
assert 0<=driver<=3
label='bound-audio-service-state-capture'+('-driver'+str(driver) if driver else '');args=[str(root/'bin/scene-viewer-bound-audio-state-probe.exe'),'--challenge-bound-failure-check','--driver',str(driver),'--recovered-game-audio','--audio-output-disabled','--ticks','1800','--snapshot',str(reports/(label+'.bmp'))]
p=subprocess.run(args,env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr)
rows=[list(map(int,l.split(': ',1)[1].split())) for l in p.stdout.splitlines() if l.startswith('BOUND SOUND SERVICE: ')]
print('exit',p.returncode,'stderr',p.stderr,'services',len(rows),'ended',sum(r[0]!=0 for r in rows),'outer',sum(r[1]!=0 for r in rows),'muted',sum(r[2]!=0 for r in rows))
report=dict(status='NATIVE_CAPTURE_ONLY',candidate_sha256=hashlib.sha256((root/'bin/scene-viewer-bound-audio-state-probe.exe').read_bytes()).hexdigest(),exit=p.returncode,stderr=p.stderr,service_calls=len(rows),ended_calls=sum(r[0]!=0 for r in rows),outer_calls=sum(r[1]!=0 for r in rows),muted_calls=sum(r[2]!=0 for r in rows),drivers=sorted({r[3] for r in rows}),scope='Isolated native full audio controller calls at original Bound prefix before scalar/failure controller. Logical audio only, no audible output or original numerical controller/cadence proof on these frames.')
(reports/(label+'-verification.json')).write_text(json.dumps(report,indent=2))
assert p.returncode==0 and not p.stderr and report['ended_calls']>0 and report['outer_calls']>0 and report['drivers']==[driver]
print('PASS native Bound sound-service candidate handoff.')

report['status']='PASS_NATIVE_BOUND_AUDIO_HANDOFF';(reports/(label+'-verification.json')).write_text(json.dumps(report,indent=2))

states=[line.split(':',1)[1].strip() for line in p.stdout.splitlines() if line.startswith('BOUND AUDIO STATE:')]
assert len(states)==report['service_calls']
(reports/(label+'-states.txt')).write_text('\n'.join(states)+'\n')
print('Captured',len(states),'actual audio controller states and ordered requests.')
