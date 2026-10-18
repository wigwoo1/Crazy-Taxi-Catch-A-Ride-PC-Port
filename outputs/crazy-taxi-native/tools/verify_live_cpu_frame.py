"""Actual live native CPU scene/shadow/taxi composition vs qualified source240 frame."""
from pathlib import Path
import subprocess,json,hashlib,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');reports=root/'reports';label='live-cpu-frame-120';exe=root/'bin/scene-viewer-cpu-frame-live.exe'
args=['--drive','--recovered-camera','--taxi-owned-graphics','--source-frame-cpu','--ticks','120','--snapshot',str(reports/(label+'.bmp')),'--taxi-source-capture',str(reports/(label+'.json')),'--source-column-work-report',str(reports/(label+'-scenery.json'))]
cp=subprocess.run([str(exe),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(label+'.log')).write_text(cp.stdout);(reports/(label+'.err')).write_text(cp.stderr);assert cp.returncode==0 and not cp.stderr,(cp.returncode,cp.stderr)
metadata=json.loads((reports/(label+'-scenery.json.cpu-frame.json')).read_text());pose=json.loads((reports/(label+'.json')).read_text())
assert metadata['camera']==pose['camera'][:3] and metadata['yaw']==pose['camera'][4] and metadata['pitch']==pose['camera'][3]
assert metadata['camera']==[20625,19666,-35593] and metadata['shadow_ground']==19200 and metadata['shadow_frame']==0
assert metadata['packets']==38 and metadata['work']==[9444,8204,73,2121]
indexed=(reports/(label+'-scenery.json.cpu-frame-indexed.bin')).read_bytes();rgba=(reports/(label+'.json.scene-shadow-taxi.rgba')).read_bytes()
assert indexed==(reports/'source240-scenery-shadow-indexed.bin').read_bytes();assert rgba==(reports/'native-scene-shadow-taxi-source240.rgba').read_bytes()
assert hashlib.sha256(rgba).hexdigest()=='1f2939856b1967c9c6eaa0709541119f147c25c43143faea122b5b234044c94c'
prior=Image.open(reports/'live-taxi-gpu-120.bmp').convert('RGBA');current=Image.open(reports/(label+'.bmp')).convert('RGBA')
assert prior.size==current.size==(3840,2160) and prior.tobytes()==current.tobytes()
Image.frombytes('RGBA',(240,160),rgba).save(reports/(label+'-reference.png'))
report=dict(status='PASS',camera=pose['camera'],packets=38,work=metadata['work'],indexed_bytes=38400,rgba_bytes=153600,qualified_reference_exact=True,normal_4k_snapshot_prior_exact=True,rgba_sha256=hashlib.sha256(rgba).hexdigest(),native_executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Live native game builds source240 CPUreference from current world_frame packets andsourcequeue seededactualnativeground shadow, productionworld_indexed/background routines, thenlivepublishedtaxi CPUOAMlayer/currentnativeBGpalette. Exact previouslyoriginal-qualified combinedframe atthispose, normal4Krender unchanged. Otheractors/HUD/windows/hardwarepriorities/livepalette lifecycle/originalgroundquery/fullframeoracle/continuous4K/cadence unverified. Offline originalprobes never linked.')
(reports/'live-cpu-frame-integration-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
