from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_game_audio_controller_reset_rom.py').read_text().split('\nseeds=')[0],ns)
rows=[]
for driver in [0,3]:
 lines=(root/'reports'/f'bound-audio-reset-exit-restart-driver{driver}.log').read_text().splitlines()
 captures=[list(map(int,l.split(':',1)[1].split())) for l in lines if l.startswith('BOUND AUDIO RESET:')]
 assert len(captures)==2, len(captures)
 rows.extend(captures)
assert len(rows)==4 and all(len(row)==50 for row in rows);steps=0
for row in rows:
 p=ns['UpdateProbe']((0,0,0,0,0,1,0,0,0,0))
 for k,v in zip(ns['sf'],row[:24]):a,n,_=ns['smap'][k];p.write(a,v,n)
 p.write(0x20025e0,row[24]);p.reg[14]=0x10020001;p.run(entry=0x8011490,stop=0x10020000);actual=[]
 for k in ns['sf']:
  a,n,sgn=ns['smap'][k];v=p.read(a,n);actual.append(v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v)
 actual.append(ns['signed'](p.read(0x20025e0)));assert actual==row[25:],(actual,row[25:]);steps+=p.steps
report=dict(status='PASS_ACTUAL_BOUND_RETRY_AUDIO_RESET_ROM',cases=4,fields_per_case=25,thumb_instructions=steps,scope='Original11490 all24controller fields plus auxiliary25E0 exact on actual initialized native post-import initial and retry prestates for drivers0/3. Actual reset/actor/scalar/shared commit assertions separately pass at restart. Channel cleanup, full initializer/import/bridge ownership and original service/mixer cadence not proved.')
(root/'reports/bound-current-audio-retry-reset-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
