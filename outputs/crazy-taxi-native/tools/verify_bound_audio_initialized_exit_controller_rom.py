"""Compare actual initialized pause/exit controller calls; external returns are contracts."""
from pathlib import Path
r=Path('outputs/crazy-taxi-native')
source=(r/'tools/verify_bound_hud_current_audio_rom.py').read_text()
for action in ['restart','quit']:
 for driver in [0,3]:
  label=f'bound-audio-initialized-exit-{action}-driver{driver}'
  rows=[line.split(':',1)[1].strip() for line in (r/'reports'/f'{label}.log').read_text().splitlines() if line.startswith('BOUND AUDIO STATE:')]
  assert rows,label
  (r/'reports'/f'{label}-states.txt').write_text('\n'.join(rows)+'\n')
  exec(compile(source.replace("label='bound-hud-current-audio'",f'label={label!r}'),label,'exec'),{})
