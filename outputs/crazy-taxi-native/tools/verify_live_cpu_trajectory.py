"""Bounded live CPU frame trajectory checks; original oracle only at qualified tick120."""
from pathlib import Path
import subprocess,json,hashlib,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');reports=root/'reports';exe=root/'bin/scene-viewer-cpu-frame-live.exe';cases=[]
for ticks in [1,30,60,90,120]:
 label='live-cpu-trajectory-'+str(ticks)
 capture=reports/(label+'.json');scene=reports/(label+'-scenery.json')
 args=['--drive','--recovered-camera','--taxi-owned-graphics','--source-frame-cpu','--ticks',str(ticks),'--snapshot',str(reports/(label+'.bmp')),'--taxi-source-capture',str(capture),'--source-column-work-report',str(scene)]
 cp=subprocess.run([str(exe),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (reports/(label+'.log')).write_text(cp.stdout);(reports/(label+'.err')).write_text(cp.stderr)
 assert cp.returncode==0 and not cp.stderr,(ticks,cp.returncode,cp.stderr)
 pose=json.loads(capture.read_text());meta=json.loads(Path(str(scene)+'.cpu-frame.json').read_text())
 assert meta['camera']==pose['camera'][:3] and meta['pitch']==pose['camera'][3] and meta['yaw']==pose['camera'][4]
 indexed=Path(str(scene)+'.cpu-frame-indexed.bin').read_bytes();rgba=Path(str(capture)+'.scene-shadow-taxi.rgba').read_bytes();layer=Path(str(capture)+'.oam-layer.rgba').read_bytes()
 assert len(indexed)==38400 and len(rgba)==len(layer)==153600
 opaque=0
 for i in range(0,len(layer),4):
  if layer[i+3]:
   assert rgba[i:i+4]==layer[i:i+4],(ticks,i)
   opaque+=1
 previous_layer=(reports/('live-taxi-gpu-'+str(ticks)+'.json.oam-layer.rgba')).read_bytes()
 assert layer==previous_layer
 snapshot=Image.open(reports/(label+'.bmp')).convert('RGBA');prior=Image.open(reports/('live-taxi-gpu-'+str(ticks)+'.bmp')).convert('RGBA')
 assert snapshot.size==prior.size==(3840,2160) and snapshot.tobytes()==prior.tobytes(),ticks
 qualified=ticks==120
 if qualified:
  assert indexed==(reports/'source240-scenery-shadow-indexed.bin').read_bytes()
  assert rgba==(reports/'native-scene-shadow-taxi-source240.rgba').read_bytes()
 Image.frombytes('RGBA',(240,160),rgba).save(reports/(label+'-reference.png'))
 cases.append(dict(ticks=ticks,camera=pose['camera'],shadow_ground=meta['shadow_ground'],shadow_frame=meta['shadow_frame'],packets=meta['packets'],work=meta['work'],opaque_taxi_pixels=opaque,rgba_sha256=hashlib.sha256(rgba).hexdigest(),normal_4k_snapshot_exact=True,original_qualified_combined_reference_exact=qualified))
 print('PASS native CPU trajectory',ticks,flush=True)
report=dict(status='PASS',cases=cases,executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Five live native CPU combined frames: coherent camera, exact live taxi overlay and prior GPU taxi layer, normal 4K render unchanged. Only tick120 combined scenery/shadow pixels compared to original-qualified reference; other four scenery/shadow frames have no independent original oracle. No whole-game fidelity, GPU scenery equivalence, stable60fps or physical Xbox claim.')
(reports/'live-cpu-frame-trajectory-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
