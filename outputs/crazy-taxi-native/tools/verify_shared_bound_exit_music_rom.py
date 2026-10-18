"""Actual initialized native exit music owner versus original wrappers."""
from pathlib import Path
import json
rroot=Path('outputs/crazy-taxi-native');ns={}
exec((rroot/'tools/compare_music_playback_rom.py').read_text().split('rows=')[0],ns)
steps=cases=0
for action in ['restart','quit']:
 for driver in [0,3]:
  label=f'shared-bound-audio-{action}-driver{driver}'
  rows=[list(map(int,l.split(':',1)[1].split())) for l in (rroot/'reports'/f'{label}.log').read_text().splitlines() if l.startswith('BOUND EXIT MUSIC:')]
  assert len(rows)==(2 if action=='restart' else 3)
  for address,before,after in rows:
   p=ns['DispatchProbe']((0,0,0,0,0,1,0,0,0,0));p.write(0x20075f6,before,1);p.reg[14]=0x10020001
   p.run(entry=address,stop=0x10020000)
   assert p.read(0x20075f6,1)==after
   steps+=p.steps;cases+=1
report=dict(status='PASS_CURRENT_EXIT_MUSIC_OWNER_ROM',calls=cases,thumb_instructions=steps,scope='Actual initialized muted shared exit playing byte matches complete original27754/27730 wrappers. Channel stop internals, effects restoration, mixer cadence and audible playback excluded.')
(rroot/'reports/shared-bound-current-exit-music-owner-rom.json').write_text(json.dumps(report,indent=2));print('PASS',report)
