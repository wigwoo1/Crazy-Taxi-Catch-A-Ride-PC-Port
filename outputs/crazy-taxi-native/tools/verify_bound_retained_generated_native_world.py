from pathlib import Path
import json,subprocess,hashlib
r=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba');cases=[]
for label in ('second','third'):
 args=[r/'bin/bound-retained-native-world-frame.exe',r/'assets/challenge-worlds/challenge5.ctw',r/'assets/challenge-worlds/challenge5-special.cts',r/'assets/challenge-worlds/challenge5.cti',r/f'reports/bound-{label}-frame-pre-raster-owner-02000000.bin',r/f'reports/bound-{label}-frame-pre-raster-owner-03000000.bin',rom,r/f'reports/bound-{label}-frame-native-background.fixture']
 cp=subprocess.run(list(map(str,args)),capture_output=True);expected=(r/f'reports/bound-{label}-frame-original-polygons-indexed.bin').read_bytes();diff=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b]
 cases.append(dict(frame=label,native_exit=cp.returncode,stderr=cp.stderr.decode(errors='replace'),bytes=len(cp.stdout),different_pixels=len(diff),first_differences=diff[:16],native_sha256=hashlib.sha256(cp.stdout).hexdigest(),original_sha256=hashlib.sha256(expected).hexdigest()))
 if cp.returncode==0:(r/f'reports/bound-{label}-frame-generated-native-world-indexed.bin').write_bytes(cp.stdout)
passed=len(cases)==2 and all(c['native_exit']==0 and c['bytes']==38400 and c['different_pixels']==0 for c in cases)
report=dict(status='NATIVE_GENERATED_ORIGINAL_WORLD_FRAME_PASS' if passed else 'NATIVE_GENERATED_WORLD_FRAME_DEPENDENCY',cases=cases,scope='Direct production native world_frame generation from recovered challenge5 assets, retained camera/trig/corners/prior generation/stamps/material cache/phase followed by native queue/material dispatch/depth clip/projection/raster/background. No captured packet vertices, clipped draw inputs, callback order or background pixels feed this pipeline. Background owner parameters remain retained fixtures; explicit mip0, observed normal textured families only. No live state progression/gameplay/hardware final display/4K/60Hz claim.')
(r/'reports/bound-retained-generated-native-world-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
