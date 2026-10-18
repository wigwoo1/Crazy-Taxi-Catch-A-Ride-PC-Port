"""Audit optional helper dispatch without hiding state or ownership differences."""
from pathlib import Path
import json,hashlib,collections
P=Path('./');B=P/'outputs/crazy-taxi-native';W=B/'work/llvm-build-20261014';R=B/'reports/original-core-reference'
old=(W/'audio-clock-235-native.log').read_text().splitlines()
off=(W/'driver-sound-0-235-native.log').read_text().splitlines();on=(W/'driver-sound-1-235-native.log').read_text().splitlines()
assert off==old,'NULL-observer wrapper changes verified output'
def gameplay(lines):return [l for l in lines if not l.startswith(('L ','V ','H '))]
a=gameplay(old);b=gameplay(on);differences=[{'line':i,'baseline':x,'observed':y} for i,(x,y) in enumerate(zip(a,b)) if x!=y]
requests=[list(map(int,l.split()[1:])) for l in on if l.startswith('L ')]
counts=collections.Counter((r[2],r[3]) for r in requests)
assert all(p<=2 and 0<=s<203 and 0<=h<=4 for _,_,p,s,h in requests)
report={'status':'PASS_NATIVE_DRIVER_SOUND_BOUNDARIES_AND_GAMEPLAY' if len(a)==len(b) and not differences and requests else 'PARTIAL_NATIVE_DRIVER_SOUND_BOUNDARIES','disabled_observer_output_exact':True,'gameplay_lines':len(b),'gameplay_mismatches':len(differences),'first_differences':differences[:12],'direct_sound_requests':len(requests),'request_counts':[{'phase':p,'sound':s,'count':n} for (p,s),n in sorted(counts.items())],'requests':requests,'physical_channels':{'baseline':[l for l in old if l.startswith('H ')],'with_direct_requests':[l for l in on if l.startswith('H ')]},'sha256':{p:hashlib.sha256((B/p).read_bytes()).hexdigest() for p in ['src/driver.c','src/driver.h','src/drive_world.c','src/drive_world.h','src/reference_driving.c','src/reference_driving.h','tests/continuous_driver_traffic_driver_sound_probe.c','work/llvm-build-20261014/continuous-driver-traffic-driver-sound.exe','work/llvm-build-20261014/driver-sound-1-235-native.log']},'limits':['Observer requests run after native gear/power/brake helper returns and before later driver/world services. Exact IRQ instruction placement inside helper is unproven.','External audio side effects do not roll back after a later math/query failure; default old APIs remain transactional.','235 diagnostic updates, not autonomous production scheduling.','Intermediate physical source snapshots and236 endpoint comparison remain required; passing game state does not prove audio channel parity.']}
(R/'driver-sound-boundaries-20261014.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:report[k] for k in ['status','disabled_observer_output_exact','gameplay_lines','gameplay_mismatches','first_differences','direct_sound_requests','request_counts','requests']}))