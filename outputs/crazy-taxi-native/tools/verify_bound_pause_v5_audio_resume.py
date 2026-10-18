from pathlib import Path
import json,hashlib
r=Path('outputs/crazy-taxi-native')
s=(r/'tools/verify_bound_live_pause.py').read_text()
for driver in [0,3]:
 test=s.replace('scene-viewer-bound-live-pause-probe.exe','scene-viewer-bound-pause-exit-probe-v5.exe').replace("'--challenge-bound-drive-preview','--taxi-owned-graphics'",f"'--challenge-bound-drive-preview','--driver','{driver}','--recovered-game-audio','--audio-output-disabled','--taxi-owned-graphics'")
 test=test.replace('bound-live-pause-',f'bound-pause-v5-audio-driver{driver}-')
 exec(compile(test,'v5-pause-audio-qualification','exec'),{})
 p=r/f'reports/bound-pause-v5-audio-driver{driver}-verification.json';d=json.loads(p.read_text());d['scope']='Isolated v5 candidate: timed held/resume inputs, recovered audio enabled with one explicit host mixer callback per step and output muted; full car/effects/timer holds and music/effects disabled while frozen asserted. Original callback cadence, audible devices and full trajectory not verified.';d['binary_sha256']=hashlib.sha256((r/'bin/scene-viewer-bound-pause-exit-probe-v5.exe').read_bytes()).hexdigest();p.write_text(json.dumps(d,indent=2)+'\n')
