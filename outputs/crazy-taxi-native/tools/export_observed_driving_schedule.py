"""Export observed VBlank timestamps independently of original physics delta."""
from pathlib import Path
import json,struct,hashlib
root=Path('outputs/crazy-taxi-native');results=[]
for side in ['left','right']:
 source=root/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json'
 records=json.loads(source.read_text())['records'];baseline=records[0]['counter'];pairs=[];outside=0
 for a,b in zip(records,records[1:]):
  aa=bytes.fromhex(a['actor']);bb=bytes.fromhex(b['actor'])
  motion=any(aa[o:o+4]!=bb[o:o+4] for o in [0,4,8,28,1348])
  if a['previous']!=b['previous']:pairs.append((a,b))
  elif motion:outside+=1
 assert not outside
 lines=[f'SOURCE_DRIVING_REFERENCE_V2 {len(pairs)}'];previous=0;physics=0;events=[]
 for a,b in pairs:
  tick=(b['counter']-baseline)&0xffffffff
  assert tick==b['tick']+1 and 0<tick-previous<=60
  raw=struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0]
  delta=struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0]
  actor=bytes.fromhex(b['actor']);pose=list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,28)[0],struct.unpack_from('<i',actor,1348)[0]]
  fields=[raw,a['frame_delta'],delta,b['frame_delta']]+pose+[tick]
  lines.append(' '.join(map(str,fields)));physics+=delta
  events.append(dict(observed_tick=tick,physics_delta=delta,cumulative_physics=physics));previous=tick
 target=root/f'reports/longsource{side}.observed.schedule.txt';target.write_text('\n'.join(lines)+'\n')
 results.append(dict(side=side,updates=len(pairs),motion_changes_outside_commits=outside,first=events[0],last=events[-1],timeline_offset_ticks=physics-previous,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),schedule_sha256=hashlib.sha256(target.read_bytes()).hexdigest(),events=events))
report=dict(status='PASS_OBSERVED_VBLANK_SCHEDULE_EXPORT',results=results,scope='Timestamps are actual sampled source counter offsets from the saved baseline, accurate to VBlank capture resolution. Physics delta remains original observed value. Baseline is mid-frame; cumulative physics delta is not time since baseline. No sub-VBlank event time or general playable scheduler proof.')
(root/'reports/observed-driving-schedule-export.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS observed schedule export',[(x['side'],x['updates'],x['first']['observed_tick'],x['timeline_offset_ticks']) for x in results])
