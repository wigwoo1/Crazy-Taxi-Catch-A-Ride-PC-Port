"""Keep captured driver elapsed stamps distinct from original observation clocks."""
from pathlib import Path
import json,hashlib
P=Path('./');B=P/'outputs/crazy-taxi-native';R=B/'reports/original-core-reference'
schedule=R/'dispatchclean-native-continuous-235.schedule.txt';dispatch=R/'callbackdispatchminimal1200.traffic-dispatch-entry-trace.json'
text=schedule.read_text().splitlines();assert text[0]=='SOURCE_DRIVING_REFERENCE_V4 235'
rows=[list(map(int,l.split())) for l in text[1:]];events=json.loads(dispatch.read_bytes())['call_events']
assert len(rows)==235 and len(events)==236
records=[]
for j,(row,event) in enumerate(zip(rows,events)):
    assert len(row)==15 and row[2]==event['frame_delta']
    source=event['observed_after_core_tick'];stamp=row[9]
    records.append({'update':j,'source_observation_tick':source,'fixture_stamp':stamp,'stamp_offset':stamp-source,'logical_delta':row[2],'frame_delta':row[3],'bank_delta':stamp-(rows[j-1][9] if j else 0),'source_observation_delta':source-events[j-1]['observed_after_core_tick'] if j else None})
different=[r for r in records[1:] if r['bank_delta']!=r['source_observation_delta']]
offsets=sorted(set(r['stamp_offset'] for r in records))
report={'status':'PASS_DISTINCT_FIXTURE_AND_SOURCE_CLOCKS_AUDITED','updates':235,'stamp_offsets':offsets,'mismatched_interupdate_bank_counts':len(different),'different_intervals':different,'preview_region':[r for r in records if 490<=r['source_observation_tick']<=525],'sha256':{'schedule':hashlib.sha256(schedule.read_bytes()).hexdigest(),'dispatcher_events':hashlib.sha256(dispatch.read_bytes()).hexdigest()},'limits':['Source observation is the core tick after dispatcher entry was observed, not the exact earlier10B54 call or mixer interrupt timestamp.','This audit does not change native playback clocks or inject expected channel phases.','A constant stamp offset may preserve interupdate bank counts but cannot establish initial bank phase or interrupt/call ordering.']}
(R/'audio-fixture-clocks-20261014.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:report[k] for k in ['status','updates','stamp_offsets','mismatched_interupdate_bank_counts','different_intervals','preview_region']}))