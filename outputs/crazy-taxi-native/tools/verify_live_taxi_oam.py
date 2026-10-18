"""Bounded live native taxi layer + complete original scenery diagnostic verification."""
from pathlib import Path
import json,hashlib,subprocess,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');label='live-taxi-oam-120'
exe=root/'bin/scene-viewer-taxi-oam-live.exe';reports=root/'reports'
args=['--drive','--recovered-camera','--taxi-owned-graphics','--ticks','120','--snapshot',str(reports/(label+'.bmp')),'--taxi-source-capture',str(reports/(label+'.json')),'--source-column-work-report',str(reports/(label+'-scenery.json'))]
cp=subprocess.run([str(exe),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(label+'.log')).write_text(cp.stdout);(reports/(label+'.err')).write_text(cp.stderr)
assert cp.returncode==0 and not cp.stderr,(cp.returncode,cp.stderr)
pose=json.loads((reports/(label+'.json')).read_text());metadata=json.loads((reports/(label+'.json.oam-layer.json')).read_text())
geometry_path=reports/(label+'-scenery.json.source240.json');geometry=json.loads(geometry_path.read_text())
assert geometry['camera']==pose['camera'][:3] and geometry['yaw']==pose['camera'][4] and geometry['pitch']==pose['camera'][3]
assert metadata['driver']==0 and metadata['frame']==pose['frame']==0 and metadata['centers']==pose['centers']==[[106,102],[133,102]]
assert metadata['scale']==pose['scale']==109 and metadata['depth']==pose['depth']==3537
assert metadata['roll']==pose['vehicle'][4]==4093 and [metadata['sine'],metadata['cosine']]==[1,255]
recorded=(reports/'taxi-recorded-pose-publish-native.bin').read_bytes()
assert json.loads((reports/'taxi-recorded-pose-publication-verification.json').read_text())['status']=='PASS'
expected_input=recorded[-1024:]+recorded[788:33556]+recorded[36116:36628]
actual_input=(reports/(label+'.json.oam-input.bin')).read_bytes();assert actual_input==expected_input
layer=(reports/(label+'.json.oam-layer.rgba')).read_bytes()
assert layer==(reports/'taxi-recorded-pose-layer.rgba').read_bytes()
assert hashlib.sha256(layer).hexdigest()=='9ec19cb9a9d5e21ebe35b70faf4aff600a343f9f3219a899753842ff0e59444b'
# Qualify complete scenery at THIS live camera, never reuse the x20626 image.
_live_read=Path.read_text
def live_read(path,*args,**kwargs):
 if path.name in ['source240-geometry-work.json.source240.json','material-phase-live-120.json.source240.json']:
  return _live_read(geometry_path,*args,**kwargs)
 return _live_read(path,*args,**kwargs)
Path.read_text=live_read
try:
 prefix=_live_read(root/'tools/verify_source240_clean_render_memory.py').split('\np=OneFrameblockScene();')[0]
 exec(prefix)
 p=OneFrameblockScene();p.run(0x0802b6a0)
finally:Path.read_text=_live_read
assert len(removed)==40 and len(p.requests)==37 and p.counts==[8660,8006,73,2121]
indexed=bytes(p.shared[a] for a in range(0x10080000,0x10089600))
assert indexed==(reports/'source240-background-scenery-indexed.bin').read_bytes()
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
rom=producer['rom'];palette_pointer=int.from_bytes(rom[0x7556cc+24:0x7556cc+28],'little')
colors=[int.from_bytes(rom[palette_pointer-0x08000000+i*2:palette_pointer-0x08000000+i*2+2],'little') for i in range(256)]
rgba=bytearray()
for i,index in enumerate(indexed):
 color=colors[index];rgba.extend(((color&31)*255//31,((color>>5)&31)*255//31,((color>>10)&31)*255//31,255))
 if layer[i*4+3]:rgba[-4:]=layer[i*4:i*4+4]
Image.frombytes('RGBA',(240,160),bytes(rgba)).save(reports/(label+'-source240-composition.png'))
report=dict(status='PASS',live_camera=pose['camera'],live_vehicle=pose['vehicle'],driver=metadata['driver'],native_layer_bytes=153600,exact_published_oam_bytes=1024,exact_object_tile_bytes=32768,exact_object_palette_bytes=512,scenery_packets=37,scenery_cleaned_contexts=40,scenery_counts=p.counts,layer_sha256=hashlib.sha256(layer).hexdigest(),composition_sha256=hashlib.sha256(rgba).hexdigest(),native_executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Live native production published taxi owners byte exact previously original-qualified identical render-tail parameters. Fresh complete original copied background/scenery shared-memory execution matches every native intermediate at this live camera x20625. Diagnostic opaque overlay with initial recovered city palette; original complete-frame oracle, shadows, other actors/HUD, hardware priority/live palette service/cadence, widescreen and60fps remain unverified. Original instruction probes offline only, absent from native game.')
(reports/'live-taxi-oam-integration-verification.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report),flush=True)
