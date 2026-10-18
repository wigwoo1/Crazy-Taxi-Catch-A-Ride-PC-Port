"""Current actual effectsrestore2771C ownership."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_music_playback_rom.py').read_text().split('rows=')[0],ns)
for i in ns['md'].disasm(ns['r'][0x2771c:0x27726],0x802771c):ns['allowed_thumb'][i.address]=bytes(i.bytes)
cases=steps=0
for action in ['restart','quit']:
 for driver in [0,3]:
  label=f'shared-bound-audio-{action}-driver{driver}'
  rows=[list(map(int,l.split(':',1)[1].split())) for l in (root/'reports'/f'{label}.log').read_text().splitlines() if l.startswith('BOUND EXIT EFFECTS:')]
  assert len(rows)==1
  before,saved,after,retained=rows[0]
  p=ns['DispatchProbe']((0,0,0,0,0,1,0,0,0,0));p.write(0x2007602,before,1);p.write(0x2007604,saved,1);p.reg[14]=0x10020001
  p.run(entry=0x802771c,stop=0x10020000)
  assert [p.read(0x2007602,1),p.read(0x2007604,1)]==[after,retained]
  cases+=1;steps+=p.steps
report=dict(status='PASS_CURRENT_EFFECTS_RESTORE_ROM',cases=cases,thumb_instructions=steps,scope='Actual initialized muted native exit effects-enabled and saved byte match complete original2771C; full audio trajectory/mixer/device excluded.')
(root/'reports/shared-bound-current-effects-restore-rom.json').write_text(json.dumps(report,indent=2));print('PASS',report)
