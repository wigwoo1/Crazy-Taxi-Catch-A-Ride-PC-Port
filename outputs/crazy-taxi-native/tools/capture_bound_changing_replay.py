from pathlib import Path
import json, subprocess, os
root=Path('outputs/crazy-taxi-native'); reports=root/'reports'
keys=[int(v) for v in (reports/'bound-key-replay-701.txt').read_text().split()]
# Change only the final124 drive callbacks, preserving the approach fixture.
for i in range(577,701):
 j=i-577; keys[i]=1 | ((j%4)<<4) | (256 if j%3==0 else 0) | (512 if j%5==0 else 0) | (2 if j%7==0 else 0)
keyfile=reports/'bound-changing-key-replay-701.txt';keyfile.write_text('\n'.join(map(str,keys))+'\n')
label='bound-changing-replay'; env=os.environ.copy(); env['TAXI_BOUND_SAMPLER_TRACE']='1'; env['TAXI_BOUND_REPLAY_INPUT_TRACE']='1'; env['TAXI_DRIVER_TRACE']=str(reports/(label+'-driver.txt'));env['TAXI_WORLD_TRACE']=str(reports/(label+'-world.txt'))
args=[str(root/'bin/scene-viewer-bound-sampler-replay.exe'),'--challenge-bound-failure-check','--ticks','1800','--snapshot',str(reports/(label+'.bmp')),'--bound-replay-records','701','--gameplay-key-replay',str(keyfile),'--gameplay-step-replay',str(reports/'bound-time-replay-701.txt')]
p=subprocess.run(args,env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(label+'.log')).write_text(p.stdout);(reports/(label+'.err')).write_text(p.stderr)
print('exit',p.returncode,'stderr',p.stderr)
fields=json.loads((reports/'ordinary-drag-corrected-full-driver-schema.json').read_text())['fields']
rows=[list(map(int,l.split())) for l in (reports/(label+'-driver.txt')).read_text().splitlines()]
ended=[row for row in rows if dict(zip(fields,row[1:]))['control_flag_1697']==1]
(reports/(label+'-driver-subset.txt')).write_text('\n'.join(' '.join(map(str,row)) for row in ended)+'\n')
print('driver updates',len(rows),'ended',len(ended),'distinct held',len({tuple(dict(zip(fields,row[1:]))['control_held_'+str(i)] for i in range(10)) for row in ended}))
assert p.returncode==0 and not p.stderr and len(ended)==124
world=[list(map(int,l.split())) for l in (reports/(label+'-world.txt')).read_text().splitlines()]
indices={row[0]+202 for row in ended}; selected=[row for row in world if row[0] in indices];assert len(selected)==124
(reports/(label+'-world-subset.txt')).write_text('\n'.join(' '.join(map(str,row)) for row in selected)+'\n')
for kind in ['driver','world']:
 source=(root/('tools/verify_changing_ended_'+kind+'.py')).read_text().replace('reports/changing-ended-', 'reports/bound-changing-replay-')
 (root/('tools/verify_bound_changing_replay_'+kind+'.py')).write_text(source)
print('Captured changing replay; original routine verification pending.')

buttons=[line.split(': ',1)[1] for line in p.stdout.splitlines() if line.startswith('BOUND REPLAY INPUT: ')]
assert len(buttons)==124
for line in buttons:
 row=list(map(int,line.split()));counter=row[0];assert row[1]==(keys[counter-1]^1023)
(reports/(label+'-buttons.txt')).write_text('\n'.join(buttons)+'\n')
for kind in ['commands','actions']:
 source=(root/('tools/verify_changing_ended_'+kind+'.py')).read_text().replace('reports/changing-ended-', 'reports/bound-changing-replay-').replace('bound-changing-replay-capture.log','bound-changing-replay.log')
 (root/('tools/verify_bound_changing_replay_'+kind+'.py')).write_text(source)
print('PASS124 replay source ownership checks.')

samplers=[line.split(': ',1)[1] for line in p.stdout.splitlines() if line.startswith('BOUND SAMPLER: ')]
assert samplers
(reports/(label+'-samplers.txt')).write_text('\n'.join(samplers)+'\n')
print('Captured',len(samplers),'complete sampler transitions.')
