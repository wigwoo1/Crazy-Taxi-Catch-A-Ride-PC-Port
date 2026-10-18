import json,struct,subprocess,hashlib
from pathlib import Path
r=Path('outputs/crazy-taxi-native');binary=r/'bin/source-cadence-presentation-probe.exe';results=[]
for side in ['left','right']:
 records=json.loads((r/f'reports/original-core-reference/longsource{side}.packed-driving-trace.json').read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];lines=[];expected=[]
 for a,b in pairs:
  raw=struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0];delta=struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0];lines.append(f"{raw} {a['frame_delta']} {delta} {b['frame_delta']}");actor=bytes.fromhex(b['actor']);expected.append([0]+list(struct.unpack_from('<iii',actor))+[struct.unpack_from('<i',actor,0x1c)[0],struct.unpack_from('<i',actor,0x544)[0]])
 for delay in [33500000,125000000]:
  cp=subprocess.run([str(binary),str(delay)],input='\n'.join(lines)+'\n',capture_output=True,text=True,check=True);rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert rows==expected,'Presentation changed source-cadence physics';metrics=json.loads(cp.stderr);assert metrics['render_frames']==metrics['held_latest']+metrics['held_startup']+metrics['blended']
  if delay==125000000:assert metrics['held_latest']==0
  else:assert metrics['held_latest']>0
  results.append(dict(side=side,original_matched_updates=len(rows),**metrics))
report=dict(status='PASS_SOURCE_CADENCE_NATIVE_PRESENTATION_60HZ_FIXTURE',results=results,scope='Headless virtual60Hz sampling of production posehistory/interpolation alongside connected native original-recorded variablephysics/inputupdates. PhysicsXYZ/heading/speed match all495sourceupdates for bothdelaysettings. Immutable car/context/command/input/history bytechecks eachsample. Physics/frame cumulative ticks converted to timestamps; captures startpartway throughoriginal update, so virtualtimeline is reconstructed, notactualhosttimestamps. Cameras omitted, noGPU drawing/OSscanout/audio/traffic/passengers.125msdelaychosenforobserved4..7tickgaps, notvalidatedgeneralplayablelatency/cadence; latestholds suppressed atcostaddedpresentationlatency. Doesnotprove stable60fps realhost orsource-faithfulgeneralscheduler.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'tests/source_cadence_presentation_probe.c',binary,r/'src/render_history.c',r/'src/render_pose_history.c',r/'src/render_interpolation.c']})
(r/'reports/source-cadence-presentation-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='hashes'},indent=2))
