from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';exe=root/'bin/scene-viewer-bound-whole-startup.exe';cases=[]
for name,ticks,args in [('normal',120,['--drive','--recovered-camera','--taxi-owned-graphics']),('bound400',400,['--challenge-bound-drive-preview','--taxi-owned-graphics','--taxi-source-capture',str(reports/'bound-whole-startup-main-400.json')]),('retry',1000,['--challenge-bound-retry-check']),('failure',1800,['--challenge-bound-failure-check'])]:
 image=reports/f'bound-whole-startup-main-{name}.bmp';p=subprocess.run([str(exe),*args,'--ticks',str(ticks),'--snapshot',str(image)],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (reports/f'bound-whole-startup-main-{name}.log').write_text(p.stdout);(reports/f'bound-whole-startup-main-{name}.err').write_text(p.stderr);assert p.returncode==0 and not p.stderr,(name,p.returncode,p.stderr)
 if name=='normal':assert image.read_bytes()==(reports/'bound-production-normal-integrated.bmp').read_bytes(),'normal changed'
 if name=='bound400':
  capture=json.loads((reports/'bound-whole-startup-main-400.json').read_text());assert capture['base']==-512;assert capture['owned_graphics'] and capture['recovered_camera']
 cases.append({'case':name,'ticks':ticks,'exit':0,'image_sha256':hashlib.sha256(image.read_bytes()).hexdigest()});print('PASS',name,flush=True)
from PIL import Image
Image.open(reports/'bound-whole-startup-main-bound400.bmp').save(reports/'bound-whole-startup-main-400.png')
(reports/'bound-whole-startup-main-verification.json').write_text(json.dumps({'status':'PASS_NATIVE_BOUND_CAMERA_BASE','cases':cases,'scope':'Native snapshots, normal byte-exact regression and existing Bound retry/failure checks. Camera680 original startup separately verified; complete original Bound pose/projection/full-frame and hardware60fps not established.'},indent=2)+'\n')
