"""Real original city sky pixels and complete ordered background+scenery composition."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
s=(root/'tools/compare_world_background_arm.py').read_text().split('cases=')[0]
s=s.replace('bytearray((seed+j*37+j//240*11)&255 for j in range(h*240))','bytearray(actual_pixels)').replace('(64,0x04030201)','(64,actual_pattern)')
ns={};exec(s,ns);ns['steps']=0
j=json.loads((root/'reports/material-phase-live-120.json.source240.json').read_text());yaw=j['yaw'];pitch=j['pitch'];wrapped=pitch&4095;signed=wrapped-4096 if wrapped>=2048 else wrapped;background_pitch=max(-142,min(142,int(signed/2)))
results=[]
for city,scene in [('city1',0x7556cc),('city2',0x755734)]:
 descriptor=struct.unpack_from('<H',rom,scene+20)[0];meta=json.loads((root/'assets/indexed-scenes'/f'{city}.json').read_text())['textures'][descriptor];address=int(meta['pixels'],16)-0x8000000;pixels=rom[address:address+meta['byte_length']];assert meta['width']==240 and meta['height']==160 and hashlib.sha256(pixels).hexdigest()==meta['sha256']
 ns['actual_pixels']=pixels;ns['actual_pattern']=pixels[-240]*0x01010101
 source=bytes(ns['original']([160,background_pitch,yaw,0,0,240,160,0]))
 cp=subprocess.run([str(root/'bin/source240-real-background-probe.exe'),str(root/'assets/indexed-scenes'/f'{city}.cti'),str(descriptor),str(pitch),str(yaw)],capture_output=True,check=True)
 assert len(cp.stdout)==38400 and cp.stdout==source,(city,'native original real background mismatch')
 out=root/'reports'/f'source240-real-background-{city}.bin';out.write_bytes(source)
 results.append({'city':city,'descriptor':descriptor,'background_pitch':background_pitch,'yaw':yaw,'framebuffer_bytes':38400,'framebuffer_sha256':hashlib.sha256(source).hexdigest()})
report={'status':'PASS','cities':results,'original_background_arm_instructions':ns['steps'],'scope':'Native world_background_scene using actual CTI texture vs complete original2FE0 actual immutable ROM pixels/replicated bottom-row pattern. Explicit pitch input derived from native helper, original pre2B6F8 pitch/clamp/dispatch prefix not executed. Source240 fullviewport, actualcapturedyaw, bothcities. No actors/palette/fullframe/cycles/cadence.'};(root/'reports/source240-real-background-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report,flush=True)
# Reuse existing complete original-raster vs native ordered-queue proof, now retaining actual verified background instead of synthetic173.
s=(root/'tools/verify_source240_ordered_scenery.py').read_text()
s=s.replace('source240-geometry-work.json.source240.json','material-phase-live-120.json.source240.json').replace('source240-ordered-scene-probe.exe','source240-background-scenery-probe.exe')
s=s.replace("str(root/'assets/indexed-scenes/city1.cti')]", "str(root/'assets/indexed-scenes/city1.cti'),str(root/'reports/source240-real-background-city1.bin')]")
s=s.replace('fb=bytes([173])*38400', "fb=(root/'reports/source240-real-background-city1.bin').read_bytes()")
s=s.replace('source240-ordered-scenery-indexed.bin','source240-background-scenery-indexed.bin').replace('source240-ordered-scenery-verification.json','source240-background-scenery-verification.json')
s=s.replace('sourcecontractsphase/cache0,initial173fb','sourcecontractsphase0/cache0 and original static citymaterials identicalphase120; actualverifiedROM background initialfb')
s=s.replace('Sceneryonly excludesoriginalbounds-cell traversal/background/actors/palette/cachelifecycle/trajectory/cycles/cadence','Background plus scenery, with producer and clipping separately qualified; excludes actors/palette/fullconsumerprefix/cycles/cadence')
exec(s,{})
