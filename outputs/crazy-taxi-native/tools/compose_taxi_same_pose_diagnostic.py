"""Diagnostic same-pose composition; not complete original final-frame proof."""
from pathlib import Path
import struct,json,hashlib,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native')
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
pose=json.loads((root/'reports/taxi-pose-source240-geometry.json').read_text())
assert pose['camera']==[20626,19666,-35593] and pose['yaw']==-485 and pose['pitch']==1
pointer=struct.unpack_from('<I',rom,0x7556cc+24)[0]
colors=struct.unpack_from('<256H',rom,pointer-0x08000000)
comparison=[]
for label in ['boot3300','boot4200']:
 captured=struct.unpack('<256H',(root/'reports/original-core-reference'/(label+'.05000000.bin')).read_bytes()[:512])
 assert tuple(colors)==captured
 comparison.append(dict(label=label,matching_entries=256))
indexed=(root/'reports/taxi-pose-background-scenery-indexed.bin').read_bytes()
assert hashlib.sha256(indexed).hexdigest()==json.loads((root/'reports/taxi-pose-background-scenery-verification.json').read_text())['indexed_framebuffer_sha256']
layer=(root/'reports/taxi-recorded-pose-layer.rgba').read_bytes()
assert hashlib.sha256(layer).hexdigest()=='9ec19cb9a9d5e21ebe35b70faf4aff600a343f9f3219a899753842ff0e59444b'
rgba=bytearray()
for i,index in enumerate(indexed):
 color=colors[index]
 rgba.extend(((color&31)*255//31,((color>>5)&31)*255//31,((color>>10)&31)*255//31,255))
 if layer[i*4+3]:rgba[-4:]=layer[i*4:i*4+4]
Image.frombytes('RGBA',(240,160),bytes(rgba)).save(root/'reports/taxi-scenery-same-pose-diagnostic.png')
report=dict(status='DIAGNOSTIC_COMPOSITION',camera=pose['camera'],yaw=pose['yaw'],pitch=pose['pitch'],scene_palette_pointer=hex(pointer),captured_palette_comparison=comparison,pixel_sha256=hashlib.sha256(rgba).hexdigest(),scope='Matching-pose separately qualified scenery and taxi, opaque overlay. Initial city ROM palette exact captured BG palettes; live palette service, shadows, traffic, HUD, hardware priorities and complete original final-frame oracle remain unverified. Source240 diagnostic, not playable port or widescreen proof.')
(root/'reports/taxi-scenery-same-pose-diagnostic.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report),flush=True)
