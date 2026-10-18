"""Actual shared channel stop represented fields vs original27610."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_audio_stop_one_rom.py').read_text().split('\ncases=')[0],ns)
steps=count=active_count=0
for action in ['restart','quit']:
 for driver in [0,3]:
  label=f'shared-bound-audio-stop-{action}-driver{driver}'
  rows=[list(map(int,l.split(':',1)[1].split())) for l in (root/'reports'/f'{label}.log').read_text().splitlines() if l.startswith('BOUND CHANNEL STOP:')]
  assert rows,label
  for row in rows:
   assert len(row)==19
   active,stamp,phase,step,rate,f4,f5,volume,last=row[1:10]
   p=ns['ExitProbe']((0,0,0,0,0,1,0,0,0,0));base=0x2009000
   for off,value,n in [(0,stamp,4),(4,f4,1),(5,f5,1),(6,volume,1),(7,last,1),(8,rate,2),(0x18,0x8100000 if active else 0,4),(0x1c,phase,4),(0x14,step,4)]:p.write(base+off,value,n)
   p.write(0x2007564,21024);p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=0x8027610,stop=0x10020000)
   actual=[int(bool(p.read(base+0x18))),p.read(base),p.read(base+0x1c),p.read(base+0x14),p.read(base+8,2),p.read(base+4,1),p.read(base+5,1),p.read(base+6,1),p.read(base+7,1)]
   assert actual==row[10:],(label,actual,row[10:])
   count+=1;active_count+=bool(active);steps+=p.steps
report=dict(status='PASS_CURRENT_CHANNEL_STOP_ROM',calls=count,active_calls=active_count,thumb_instructions=steps,scope='Actual initialized muted shared channel stops: all9 represented fields exact against original27610. Original decoder pointers, full10AAC owner order, mixer cadence and audible playback excluded.')
(root/'reports/shared-bound-current-channel-stop-rom.json').write_text(json.dumps(report,indent=2));print('PASS',report)
