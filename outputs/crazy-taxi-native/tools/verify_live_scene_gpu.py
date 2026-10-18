"""Bounded GPU scene comparison; mismatches are evidence, not a passing fidelity claim."""
from pathlib import Path
import subprocess,json,sys,hashlib
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');r=root/'reports';label='live-source-scene-gpu-120';exe=root/'bin/scene-viewer-source-scene-gpu-v2.exe'
args=['--drive','--recovered-camera','--taxi-owned-graphics','--source-scene-gpu-check','--ticks','120','--snapshot',str(r/(label+'.bmp')),'--taxi-source-capture',str(r/(label+'.json')),'--source-column-work-report',str(r/(label+'-scenery.json'))]
p=subprocess.run([str(exe),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(r/(label+'.log')).write_text(p.stdout);(r/(label+'.err')).write_text(p.stderr);assert p.returncode==0 and not p.stderr,(p.returncode,p.stderr)
j=json.loads((r/(label+'-scenery.json.gpu-scene.json')).read_text())
a=Image.open(r/(label+'.bmp')).convert('RGBA');b=Image.open(r/'live-cpu-frame-120.bmp').convert('RGBA');assert a.size==b.size==(3840,2160) and a.tobytes()==b.tobytes()
raw=(r/(label+'-scenery.json.gpu-scene.rgba')).read_bytes();assert len(raw)==153600
Image.frombytes('RGBA',(240,160),raw).transpose(Image.Transpose.FLIP_TOP_BOTTOM).save(r/(label+'-gpu.png'))
j.update(normal_4k_snapshot_exact=True,executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest())
(r/(label+'-verification.json')).write_text(json.dumps(j,indent=2));print(json.dumps(j))
