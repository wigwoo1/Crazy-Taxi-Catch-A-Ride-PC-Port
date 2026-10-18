from pathlib import Path
import json,struct,subprocess,hashlib
from PIL import Image
root=Path('outputs/crazy-taxi-native');r=root/'reports';prefix='bound-second-frame-'
for name,status in [('combined-native-comparison','COMBINED_NATIVE_ORIGINAL_FRAME_PASS'),('native-palette-comparison','NATIVE_ORIGINAL_PALETTE_PASS'),('native-oam-publication-comparison','NATIVE_ORIGINAL_OAM_PUBLICATION_PASS'),('published-obj-layer-comparison','NATIVE_PUBLISHED_OBJ_LAYER_CONTRACT_PASS')]:
 assert json.loads((r/(prefix+name+'.json')).read_text())['status']==status
f=json.loads((r/(prefix+'display-owner-capture.json')).read_text())['fields']
state=struct.pack('<4h2i12H',f['bg2_pa'],f['bg2_pb'],f['bg2_pc'],f['bg2_pd'],f['bg2_ref_x_low']|(f['bg2_ref_x_high']<<16),f['bg2_ref_y_low']|(f['bg2_ref_y_high']<<16),*[f[key] for key in ('display','bg2_control','mosaic','blend_control','blend_alpha','blend_brightness','window0_horizontal','window1_horizontal','window0_vertical','window1_vertical','window_inside','window_outside')])
rom='Crazy Taxi - Catch a Ride (USA).gba'
flush=subprocess.run([str(root/'bin/bound-first-frame-sprite-flush-native-compare.exe'),str(r/(prefix+'sprite-flush-before.bin')),rom],capture_output=True)
assert flush.returncode==0 and flush.stdout==(r/(prefix+'sprite-flush-after.bin')).read_bytes()
bitmap=(r/(prefix+'combined-native-indexed.bin')).read_bytes();oam=(r/(prefix+'native-oam-publication.bin')).read_bytes()[5898:6922]
palette=(r/(prefix+'native-palette-state.bin')).read_bytes()[4152:5176]
fixture=state+bitmap+oam+flush.stdout[770:]+palette+(root/'assets/challenge-flag/flag-indicator.indices').read_bytes()
cp=subprocess.run([str(root/'bin/source-display-native-compare.exe')],input=fixture,capture_output=True)
assert cp.returncode==0 and len(cp.stdout)==153600,(cp.returncode,len(cp.stdout))
expected=bytearray((r/(prefix+'native-visible-bitmap.rgba')).read_bytes());layer=(r/(prefix+'native-published-obj-layer.rgba')).read_bytes()
for i in range(0,len(expected),4):
 if layer[i+3]:expected[i:i+4]=layer[i:i+4]
assert cp.stdout==expected
for offset in (0,16,18,20,22,24,26,28):
 invalid=bytearray(fixture);invalid[offset]^=1
 failed=subprocess.run([str(root/'bin/source-display-native-compare.exe')],input=invalid,capture_output=True)
 assert failed.returncode==3 and not failed.stdout
(r/(prefix+'native-source-display.rgba')).write_bytes(cp.stdout)
Image.frombytes('RGBA',(240,160),cp.stdout).save(r/(prefix+'native-source-display.png'))
report={'status':'NATIVE_CAPTURED_DISPLAY_COMPOSITION_CONTRACT_PASS','rgba_bytes':153600,'different_bytes':0,'unsupported_state_rejections':8,'sha256':hashlib.sha256(cp.stdout).hexdigest(),'scope':'Native combined world output, native HUD indicator, native palette, native sprite publication/copy buffers joined by typed native composer under captured display/transform settings. Exact previous independently-checked layer composition. Observed opaque priority0 identity mode4 contract only; independent hardware finaldisplay oracle, othereffects, live producer/boot/input/4K60Hz and completedgame unproven.'}
(r/(prefix+'native-source-display-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
