"""Native CPU scenery, shadow, and taxi source240 diagnostic frame."""
from pathlib import Path
import struct,json,subprocess,hashlib,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native')
inputs={};exec((root/'tools/verify_source240_scenery_shadow.py').read_text().split('\ncp=subprocess.run')[0],inputs)
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
pointer=struct.unpack_from('<I',rom,0x7556cc+24)[0];palette=rom[pointer-0x08000000:pointer-0x08000000+512]
palette_path=root/'reports/source240-city1-palette.bin';palette_path.write_bytes(palette)
for label in ['boot3300','boot4200']:assert palette==(root/'reports/original-core-reference'/(label+'.05000000.bin')).read_bytes()[:512]
pose=json.loads((root/'reports/live-taxi-gpu-120.json').read_text());geometry=inputs['j']
assert geometry['camera']==pose['camera'][:3] and geometry['yaw']==pose['camera'][4] and geometry['pitch']==pose['camera'][3]
exe=root/'bin/source240-shadow-scene-probe.exe';args=[str(root/'assets/indexed-scenes/city1.cti'),str(root/'reports/source240-real-background-city1.bin'),str(root/'assets/taxi-software/taxi-software.ctx'),str(root/'reports/live-taxi-gpu-120.json.oam-input.bin'),str(palette_path)]
cp=subprocess.run([str(exe),*args],input=('\n'.join(inputs['lines'])+'\n').encode(),capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
assert len(cp.stdout)==38*38420+153600
indexed=cp.stdout[37*38420:37*38420+38400];assert indexed==(root/'reports/source240-scenery-shadow-indexed.bin').read_bytes()
assert hashlib.sha256(indexed).hexdigest()==json.loads((root/'reports/source240-scenery-shadow-verification.json').read_text())['indexed_framebuffer_sha256']
layer=(root/'reports/live-taxi-gpu-120.json.oam-layer.rgba').read_bytes();colors=struct.unpack('<256H',palette);expected=bytearray()
for i,index in enumerate(indexed):
 color=colors[index];expected.extend(((color&31)*255//31,((color>>5)&31)*255//31,((color>>10)&31)*255//31,255))
 if layer[i*4+3]:expected[-4:]=layer[i*4:i*4+4]
rgba=cp.stdout[-153600:];assert rgba==expected
(root/'reports/native-scene-shadow-taxi-source240.rgba').write_bytes(rgba)
Image.frombytes('RGBA',(240,160),rgba).save(root/'reports/native-scene-shadow-taxi-source240.png')
report=dict(status='PASS',camera=pose['camera'],vehicle=pose['vehicle'],packets=38,rgba_bytes=153600,exact_independent_composition_bytes=153600,rgba_sha256=hashlib.sha256(rgba).hexdigest(),indexed_sha256=hashlib.sha256(indexed).hexdigest(),native_executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Entire diagnostic source240 frame constructed natively from original-qualified scene andshadow rasters/nativequeue plus livepublishedtaxi CPU OAM layer andoriginalcitypalette. Originalqueue andeveryintermediatescenery/shadowpixel separatelyqualified, finalnativeRGBAexactindependentcomposition. Otheractors/HUD/window/priority/livepalette/cadence/full original finalframe oracle and4Kcontinuous adaptation remainunverified; not complete playable port.')
(root/'reports/native-scene-shadow-taxi-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
