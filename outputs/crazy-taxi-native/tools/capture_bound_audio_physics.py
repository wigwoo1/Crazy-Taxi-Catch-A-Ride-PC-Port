from pathlib import Path
import subprocess,os,json,sys,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';env=os.environ.copy();env['TAXI_BOUND_SOUND_TRACE']='1';env['TAXI_BOUND_AUDIO_STATE_TRACE']='1'
driver=int(sys.argv[1]) if len(sys.argv)>1 else 0
assert 0<=driver<=3
label='bound-audio-service-physics-capture'+('-driver'+str(driver) if driver else '');args=[str(root/'bin/scene-viewer-bound-audio-state-probe.exe'),'--challenge-bound-failure-check','--driver',str(driver),'--recovered-game-audio','--audio-output-disabled','--ticks','1800','--snapshot',str(reports/(label+'.bmp'))]
env['TAXI_DRIVER_TRACE']=str(reports/(label+'-driver.txt'));env['TAXI_WORLD_TRACE']=str(reports/(label+'-world.txt'))
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

fields=json.loads((reports/'ordinary-drag-corrected-full-driver-schema.json').read_text())['fields']
driver_rows=[list(map(int,l.split())) for l in (reports/(label+'-driver.txt')).read_text().splitlines()]
ended=[row for row in driver_rows if dict(zip(fields,row[1:]))['control_flag_1697']==1]
(reports/(label+'-driver-subset.txt')).write_text('\n'.join(' '.join(map(str,row)) for row in ended)+'\n')
world_rows=[list(map(int,l.split())) for l in (reports/(label+'-world.txt')).read_text().splitlines()]
print('Actualaudio driver/world/ended counts',len(driver_rows),len(world_rows),len(ended))
selected=[row for row in world_rows if row[0] in {r[0]+202 for r in ended}]
assert len(selected)==len(ended)
(reports/(label+'-world-subset.txt')).write_text('\n'.join(' '.join(map(str,row)) for row in selected)+'\n')
