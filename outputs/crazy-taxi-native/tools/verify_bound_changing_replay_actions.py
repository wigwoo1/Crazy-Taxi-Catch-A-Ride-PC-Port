from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_action_rom.py').read_text().split('\ncases=')[0],ns)
fields=json.loads((root/'reports/ordinary-drag-corrected-full-driver-schema.json').read_text())['fields']
drivers=[dict(zip(fields,list(map(int,l.split()))[1:])) for l in (root/'reports/bound-changing-replay-driver-subset.txt').read_text().splitlines()]
buttons=[list(map(int,l.split())) for l in (root/'reports/bound-changing-replay-buttons.txt').read_text().splitlines()];assert len(drivers)==len(buttons)==124
assert 'selection7 profile0' in (root/'reports/bound-changing-replay.log').read_text()
steps=count=0
for row,d in zip(buttons,drivers):
 b=row[2:32];streams=[sum(b[i*3+j]<<i for i in range(10)) for j in range(3)]
 for action in [2,3,4,5]:
  p=ns['ActionProbe']((0,0,0,0,0,1),0);p.initialize(tuple(streams)+(0,d['input_power_gears_flags_4ac'],0,0,action));p.arm.mem[0x02001694]=1<<24;p.run(entry=0x08015d98,stop=0x10020000)
  assert p.reg[0]==d['control_action'+str(action)],(row[0],action,p.reg[0],d['control_action'+str(action)]);count+=1;steps+=p.steps
report=dict(status='PASS_CHANGING_ENDED_ACTIONS',cases=count,input_states=124,profile=0,thumb_instructions=steps,scope='Actual original15D98 action2/3/4/5 versus captured native actions using changing held/pressed/repeat fixtures and actual vehicleflags. Original sampler, hardware, fullengine trajectory andotherprofiles notqualified here.')
(root/'reports/bound-changing-replay-action-verification.json').write_text(json.dumps(report,indent=2));print('PASS',report)
