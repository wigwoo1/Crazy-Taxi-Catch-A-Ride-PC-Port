"""Current actual10AAC handle cleanup order and retention."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_game_audio_cleanup_rom.py').read_text().split('\ncases=')[0],ns)
cases=steps=stops=0
for action in ['restart','quit']:
 for driver in [0,3]:
  label=f'shared-bound-audio-stop-{action}-driver{driver}'
  lines=(root/'reports'/f'{label}.log').read_text().splitlines()
  starts=[i for i,l in enumerate(lines) if l.startswith('BOUND CLEANUP OWNER BEFORE:')]
  assert len(starts)==1
  i=starts[0];before=list(map(int,lines[i].split(':',1)[1].split()))
  j=next(k for k in range(i+1,len(lines)) if lines[k].startswith('BOUND CLEANUP OWNER AFTER:'))
  after=list(map(int,lines[j].split(':',1)[1].split()))
  ordered=[int(l.split(':',1)[1].split()[0]) for l in lines[i+1:j] if l.startswith('BOUND CHANNEL STOP:')]
  assert len(before)==10 and len(after)==11
  assert before==after[:10] and after[10]==len(ordered)
  p=ns['CleanupProbe']((0,0,0,0,0,1,0,0,0,0));p.calls=[]
  for k,h in enumerate(before):p.write(0x20025e8+k*4,0 if not h else 0x2009000+(h-1)*32)
  p.reg[14]=0x10020001;p.run(entry=0x8010aac,stop=0x10020000)
  assert p.calls==[0x2009000+(h-1)*32 for h in ordered]
  for k,h in enumerate(before):assert p.read(0x20025e8+k*4)==(0 if not h else 0x2009000+(h-1)*32)
  cases+=1;steps+=p.steps;stops+=len(ordered)
report=dict(status='PASS_CURRENT_CLEANUP_HANDLE_OWNER_ROM',cases=cases,stops=stops,thumb_instructions=steps,scope='Actual initialized native10AAC tenhandle order/count/retention matches whole original caller. Native handles mapped explicitly to original channel identities;27610 callback boundary separately qualified on prior actual captures. Original mixer/device/cadence and continuous restart bridge excluded.')
(root/'reports/shared-bound-current-cleanup-handle-owner-rom.json').write_text(json.dumps(report,indent=2));print('PASS',report)
