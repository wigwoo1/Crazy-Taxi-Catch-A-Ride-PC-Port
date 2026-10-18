from pathlib import Path
import json,struct,subprocess
root=Path('outputs/crazy-taxi-native')
audit=json.loads((root/'reports/bound-first-frame-flag-bitmap-prefix-next-dependency.json').read_text())
source=[c for c in audit['frame_object_submit_calls'] if c['arguments'][1]==3]
assert len(source)==5,('originalglyphcalls incomplete',len(source))
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
positions=root/'assets/challenge-flag/flag-hud-positions.bin';assert positions.read_bytes()==rom[0x7ec12c:0x7ec168]
cp=subprocess.run([str(root/'bin/bound-first-frame-hud-requests-native-compare.exe'),str(positions)],capture_output=True,text=True);assert cp.returncode==0
native=[list(map(int,row.split())) for row in cp.stdout.splitlines()];assert len(native)==5
expected=[]
for call in source:
 glyph,group,identity,flags=call['arguments'];assert group==3 and flags==0
 scaled=int(call['entry']=='0x8028658');assert call['entry'] in ('0x8028658','0x8028288')
 xyz=list(struct.unpack_from('<3i',rom,identity-0x08000000));expected.append([glyph,identity,scaled]+xyz)
assert native==expected,(native,expected)
report=dict(status='NATIVE_ORIGINAL_HUD_REQUESTS_PASS',requests=5,native=native,scope='Existing native2400tick HUD plan matches actual five original Bound caller glyph indices, sourcepositionidentities/scaledhelper selection and ROMpositionvalues; not actualqueue/OAM/fontpixels orwholeHUD.')
(root/'reports/bound-first-frame-native-hud-requests-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
