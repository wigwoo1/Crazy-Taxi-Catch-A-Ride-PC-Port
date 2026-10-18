from pathlib import Path
import subprocess,json,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');r=root/'reports';cases=[]
for t in [1,30,60,90,120]:
 label='shadow-ground-owner-'+str(t);scene=r/(label+'-scenery.json');capture=r/(label+'.json')
 args=['--drive','--recovered-camera','--taxi-owned-graphics','--source-scene-gpu-check','--ticks',str(t),'--snapshot',str(r/(label+'.bmp')),'--taxi-source-capture',str(capture),'--source-column-work-report',str(scene)]
 p=subprocess.run([str(root/'bin/scene-viewer-shadow-ground-owner.exe'),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (r/(label+'.log')).write_text(p.stdout);(r/(label+'.err')).write_text(p.stderr);assert p.returncode==0 and not p.stderr,(t,p.returncode,p.stderr)
 j=json.loads(Path(str(scene)+'.gpu-scene.json').read_text());j['ticks']=t
 combined=json.loads(Path(str(capture)+'.combined-gpu.json').read_text());assert combined['rgb555_mismatch_pixels']==0,(t,combined)
 j['combined_gpu']=combined
 current=Image.open(r/(label+'.bmp')).convert('RGBA');prior=Image.open(r/('cpu-pitch-fixed-'+str(t)+'.bmp')).convert('RGBA');assert current.size==prior.size==(3840,2160) and current.tobytes()==prior.tobytes(),t
 j['normal_4k_snapshot_exact']=True
 raw_combined=Path(str(capture)+'.combined-gpu.rgba').read_bytes();assert len(raw_combined)==153600
 Image.frombytes('RGBA',(240,160),raw_combined).transpose(Image.Transpose.FLIP_TOP_BOTTOM).save(r/(label+'-reference.png'))

 cpu=Path(str(capture)+'.scene-shadow-taxi.rgba').read_bytes();assert cpu==(r/('live-cpu-trajectory-'+str(t)+'.json.scene-shadow-taxi.rgba')).read_bytes()
 raw=Path(str(scene)+'.gpu-scene.rgba').read_bytes();Image.frombytes('RGBA',(240,160),raw).transpose(Image.Transpose.FLIP_TOP_BOTTOM).save(r/(label+'-gpu.png'));cases.append(j);print(t,j,flush=True)
(r/'shadow-ground-owner-trajectory-verification.json').write_text(json.dumps(dict(cases=cases,cpu_reference_unchanged=True,scope='Production source240 GPU background/scenery/shadow; comparison preserves any genuine mismatches. No taxi/whole-game/continuous4K/stable60 claim.'),indent=2))
