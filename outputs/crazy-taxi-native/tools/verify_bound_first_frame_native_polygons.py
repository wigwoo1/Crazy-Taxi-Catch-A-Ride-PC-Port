from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native')
rom=Path('Crazy Taxi - Catch a Ride (USA).gba')
calls=json.loads((root/'reports/bound-first-frame-native-draw-inputs.json').read_text())
assert calls
initial=(root/'reports/bound-first-frame-original-background-indexed.bin').read_bytes();assert len(initial)==38400
blob=bytearray(initial)+struct.pack('<I',len(calls))
for c in calls:
 blob+=struct.pack('<7I',int(c['entry'],16),len(c['vertices']),c['width'],c['height'],c['log_width'],c['level'],c['texture_pointer'])
 for v in c['vertices']:blob+=struct.pack('<5I',*v)
fixture=root/'reports/bound-first-frame-native-polygons.fixture';fixture.write_bytes(blob)
cp=subprocess.run([str(root/'bin/bound-first-frame-polygon-native-compare.exe'),str(rom),str(fixture)],capture_output=True)
expected=(root/'reports/bound-first-frame-original-polygons-indexed.bin').read_bytes()
if cp.returncode==0:(root/'reports/bound-first-frame-native-polygons-indexed.bin').write_bytes(cp.stdout)
differences=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b]
passed=cp.returncode==0 and len(cp.stdout)==len(expected)==38400 and not differences
report=dict(status='NATIVE_ORIGINAL_POLYGON_FRAME_PASS' if passed else 'NATIVE_POLYGON_FRAME_DEPENDENCY',native_exit=cp.returncode,native_stderr=cp.stderr.decode(errors='replace'),draw_inputs=len(calls),compared_bytes=min(len(cp.stdout),len(expected)),different_pixels=len(differences),first_differences=differences[:32],native_sha256=hashlib.sha256(cp.stdout).hexdigest(),original_sha256=hashlib.sha256(expected).hexdigest(),scope='Native C recovered polygon routines against actual original caller inputs including source far-clipped projected vertices, ordered original background and all source drawn polygons; farclip input generation remains original diagnostic, not native full-frame integration, palette/presentation or gameplay proof.')
(root/'reports/bound-first-frame-native-polygons-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report))
