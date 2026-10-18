from pathlib import Path
import json,subprocess
r=Path('outputs/crazy-taxi-native');d=json.loads((r/'reports/active-held-input/countdown-driver-owner-calls.json').read_text());cases=[]
def signed(x,b):return x-(1<<b) if x&(1<<(b-1)) else x
def state(row):return [signed(x,16) if i<4 else signed(x,32) if 5<=i<=7 else x for i,x in enumerate(row)]
for index,c in enumerate(d['countdown']):
 before=state(c['before']);expected=state(c['after']);cp=subprocess.run([str(r/'bin/retained-countdown-owner-compare.exe')],input=' '.join(map(str,before+c['inputs']))+'\n',capture_output=True,text=True);assert cp.returncode==0,(cp.returncode,cp.stderr)
 got=list(map(int,cp.stdout.split()));assert got[:9]==expected,(index,got,expected)
 cases.append(dict(frame=index+1,source_instructions=c['instructions'],inputs=c['inputs'],before=before,after=expected,native_events=got[9],different_state_fields=0))
assert len(cases)==3
report=dict(status='NATIVE_ORIGINAL_RETAINED_COUNTDOWN_STATE_PASS',cases=cases,driver_calls=len(d['driver']),scope='Actual original BA04 challenge countdown nine retained fields vs native game_start_challenge_step under actual held A+Right retained source inputs. Native event requests shown but not yet compared with original art/audio calls. Driver before/after actor snapshots captured only, not compared here; no countdown skip/active driving trajectory proof.')
(r/'reports/retained-active-countdown-owner-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
