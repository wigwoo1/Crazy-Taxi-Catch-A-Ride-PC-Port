from pathlib import Path
import json,subprocess,hashlib
from PIL import Image
root=Path('outputs/crazy-taxi-native');reports=root/'reports'
for name,status in [('combined-native-comparison','COMBINED_NATIVE_ORIGINAL_FRAME_PASS'),('native-palette-comparison','NATIVE_ORIGINAL_PALETTE_PASS')]:
 assert json.loads((reports/('bound-third-frame-'+name+'.json')).read_text())['status']==status
paths=[root/'bin/bound-second-frame-visible-bitmap-native-compare.exe',reports/'bound-third-frame-combined-native-indexed.bin',root/'assets/challenge-flag/flag-indicator.indices',reports/'bound-third-frame-flag-bitmap-original-indexed.bin',reports/'bound-third-frame-native-palette-state.bin',reports/'bound-third-frame-palette-after.bin']
cp=subprocess.run(list(map(str,paths)),capture_output=True)
assert cp.returncode==0 and len(cp.stdout)==153600,(cp.returncode,len(cp.stdout))
indexed=paths[3].read_bytes();palette=paths[5].read_bytes()[4152:4664]
expected=bytearray()
for index in indexed:
 color=int.from_bytes(palette[index*2:index*2+2],'little')
 expected.extend([*((color>>(c*5)&31)*255//31 for c in range(3)),255])
assert cp.stdout==expected
(reports/'bound-third-frame-native-visible-bitmap.rgba').write_bytes(cp.stdout)
Image.frombytes('RGBA',(240,160),cp.stdout).save(reports/'bound-third-frame-native-visible-bitmap.png')
nonblack=sum(any(cp.stdout[i:i+3]) for i in range(0,len(cp.stdout),4))
assert nonblack>0
report={'status':'NATIVE_ORIGINAL_VISIBLE_BITMAP_PASS','compared_rgba_bytes':153600,'different_bytes':0,'nonblack_pixels':nonblack,'sha256':hashlib.sha256(cp.stdout).hexdigest(),'scope':'Combined direct native background, queue, clipping and polygon output (raw prepared packet and scene owners) plus existing native flag bitmap and actual independently-verified native palette equals original indexed bitmap and original third-frame palette. Bitmap layer only; published OAM/OBJ sprites, blending, windows, display capture, producer/boot/input/timing and whole playable port remain unverified.'}
(reports/'bound-third-frame-native-visible-bitmap-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report))
