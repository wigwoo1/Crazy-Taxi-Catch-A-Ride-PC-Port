from pathlib import Path
import json,subprocess,hashlib
r=Path('outputs/crazy-taxi-native');capture_path=r/'reports/active-held-input/countdown-driver-owner-calls.json';capture_bytes=capture_path.read_bytes();d=json.loads(capture_bytes);cases=[]
def signed(x,b):return x-(1<<b) if x&(1<<(b-1)) else x
def state(row):return [signed(x,16) if i<4 else signed(x,32) if 5<=i<=7 else x for i,x in enumerate(row)]
for index,c in enumerate(d['countdown']):
 before=state(c['before']);expected=state(c['after']);cp=subprocess.run([str(r/'bin/retained-countdown-owner-compare.exe')],input=' '.join(map(str,before+c['inputs']))+'\n',capture_output=True,text=True);assert cp.returncode==0,(cp.returncode,cp.stderr)
 got=list(map(int,cp.stdout.split()));assert got[:9]==expected,(index,got,expected)
 cases.append(dict(frame=index+1,source_instructions=c['instructions'],inputs=c['inputs'],before=before,after=expected,native_events=got[9],different_state_fields=0))
assert 3<=len(cases)<=240
transitions=[dict(frame=c['frame'],before=c['before'],after=c['after'],native_events=c['native_events']) for c in cases if c['before'][0]!=c['after'][0] or bool(c['before'][1])!=bool(c['after'][1]) or c['before'][8]!=c['after'][8]]
release_frames=[c['frame'] for c in cases if c['before'][8] and not c['after'][8]]
report=dict(status='NATIVE_ORIGINAL_FADE_SEQUENCE_COUNTDOWN_STATE_PASS',capture_sha256=hashlib.sha256(capture_bytes).hexdigest(),native_executable_sha256=hashlib.sha256((r/'bin/retained-countdown-owner-compare.exe').read_bytes()).hexdigest(),native_source_sha256=hashlib.sha256((r/'src/game_start.c').read_bytes()).hexdigest(),cases=cases,driver_calls=len(d['driver']),transitions=transitions,control_release_observed=bool(release_frames),control_release_frames=release_frames,scope='Actual original BA04 challenge countdown nine retained fields vs native game_start_challenge_step under actual held A+Right retained source inputs. Native event requests shown but not yet compared with original art/audio calls. Driver before/after actor snapshots captured only, not compared here; no countdown skip/active driving trajectory proof.')
(r/'reports/retained-control-release-countdown-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
