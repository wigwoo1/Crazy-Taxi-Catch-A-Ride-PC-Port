"""Native production GPU taxi against live published-OAM CPU pixels across driving poses."""
from pathlib import Path
import subprocess,json,hashlib,re,sys
sys.path.insert(0,'work/python-packages')
from PIL import Image
root=Path('outputs/crazy-taxi-native');exe=root/'bin/scene-viewer-taxi-live-gpu.exe';results=[]
for ticks in [1,30,60,90,120]:
 label='live-taxi-gpu-'+str(ticks);reports=root/'reports'
 args=['--drive','--recovered-camera','--taxi-owned-graphics','--taxi-live-pixels-check','--ticks',str(ticks),'--snapshot',str(reports/(label+'.bmp')),'--taxi-source-capture',str(reports/(label+'.json')),'--source-column-work-report',str(reports/(label+'-scenery.json'))]
 cp=subprocess.run([str(exe),*args],capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (reports/(label+'.log')).write_text(cp.stdout);(reports/(label+'.err')).write_text(cp.stderr)
 assert cp.returncode==0 and not cp.stderr,(ticks,cp.returncode,cp.stderr)
 match=re.search(r'PASS live taxi GPU/OAM CPU: all38400 RGB pixels exact, (\d+) opaque',cp.stdout);assert match,ticks
 pose=json.loads((reports/(label+'.json')).read_text());meta=json.loads((reports/(label+'.json.oam-layer.json')).read_text())
 layer=(reports/(label+'.json.oam-layer.rgba')).read_bytes();assert len(layer)==153600
 opaque=sum(bool(layer[i]) for i in range(3,len(layer),4));assert opaque==int(match[1])
 snapshot=Image.open(reports/(label+'.bmp')).convert('RGBA');assert snapshot.size==(3840,2160)
 results.append(dict(ticks=ticks,camera=pose['camera'],driver=meta['driver'],frame=meta['frame'],scale=meta['scale'],roll=meta['roll'],centers=meta['centers'],source_pixels=38400,opaque_pixels=opaque,layer_sha256=hashlib.sha256(layer).hexdigest(),snapshot_sha256=hashlib.sha256(snapshot.tobytes()).hexdigest()))
 print('PASS live GPU pose',ticks,'opaque',opaque,flush=True)
old=Image.open(root/'reports/live-taxi-oam-120.bmp').convert('RGBA');new=Image.open(root/'reports/live-taxi-gpu-120.bmp').convert('RGBA')
assert old.size==new.size and old.tobytes()==new.tobytes()
report=dict(status='PASS',cases=results,exact_source_rgb_channels=len(results)*38400*3,restored_120tick_snapshot_prior_exact=True,executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Production GPU taxi shader usesactual live fields/matrices/texture vs published OAM CPU oracle atsource240. All RGB including sentinel coverage exact across5native driving poses. 120tick prior4K snapshot restoredexact. CPU previouslyoriginal-qualified; does not claim independentlyoriginal-qualified entire pose chains/fullframe/shadows/otheractors/HUD/priority/continuous4Kpixels/cadence.')
(root/'reports/live-taxi-gpu-trajectory-verification.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report),flush=True)
