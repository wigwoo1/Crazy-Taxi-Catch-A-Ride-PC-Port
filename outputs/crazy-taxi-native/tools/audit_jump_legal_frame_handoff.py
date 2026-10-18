"""Audit native shared-state handoff between separately observed driver and world updates."""
import json,hashlib
from pathlib import Path
root=Path('outputs/crazy-taxi-native')
def load(prefix):
 schema=root/('reports/'+prefix+'-state-schema.json');trace=root/('reports/'+('jump-legal-full-driver' if 'driver' in prefix else 'jump-legal-full-world')+'-states.txt');fields=json.loads(schema.read_text())['fields'];return {int(a[0]):dict(zip(fields,map(int,a[1:]))) for a in [l.split() for l in trace.read_text().splitlines()]},[schema,trace]
driver,files=load('bound-route-dash-full-driver');world,wfiles=load('bound-route-world-full');files+=wfiles
pairs=[('motion_x','input_x','output_x'),('motion_y','input_y','output_y'),('motion_z','input_z','output_z'),('motion_heading1c','input_heading_1c','output_heading_1c'),('motion_heading20','input_heading_20','output_heading_20'),('motion_drift69c','input_drift_69c','output_drift_69c'),('motion_pitch','pitch','output_pitch'),('motion_roll','roll','output_roll'),('motion_speed544','speed544','output_speed544'),('motion_flags28','flags28','output_flags28'),('power_scale_e84','input_height_scale_e84','output_height_scale_e84')]
checks=0
assert set(driver)==set(world)
for tick,d in driver.items():
 w=world[tick]
 for dk,winput,woutput in pairs:
  assert d['output_'+dk]==w[winput],(tick,'driver to world',dk,d['output_'+dk],w[winput]);checks+=1
  if tick+1 in driver:
   assert w[woutput]==driver[tick+1]['input_'+dk],(tick,'world to next driver',dk,w[woutput],driver[tick+1]['input_'+dk]);checks+=1
 for dk,wk in [('displacement_x','dx'),('displacement_y','dy'),('displacement_z','dz')]:assert d['output_'+dk]==w[wk],(tick,dk);checks+=1
report=dict(cases=len(driver),scalar_handoff_checks=checks,scope='Cross-observation native driver/world field handoffs and next-frame feedback; isolated ROM component comparisons separate. Not continuous original full-frame execution.',sha256={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in files+[root/'src/drive_world.c',Path(__file__)]})
(root/'reports/jump-legal-frame-handoff-audit.json').write_text(json.dumps(report,indent=2));print('PASS',report)
