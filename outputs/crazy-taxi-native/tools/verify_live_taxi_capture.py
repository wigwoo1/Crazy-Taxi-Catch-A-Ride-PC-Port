from pathlib import Path
import sys,json,hashlib
sys.path.insert(0,'work/python-packages')
from PIL import Image
r=Path('outputs/crazy-taxi-native');p=r/'reports/live-taxi-framing.bmp';meta=json.loads(Path(str(p)+'.json').read_text());log=(r/'reports/live-taxi-framing.log').read_text()
assert not (r/'reports/live-taxi-framing.err').read_text();assert 'PASS paced native input-only boarding and original passenger voice playback;' in log
assert meta['gameplay_updates']==350 and 'LIVE SNAPSHOT: update350 644x461' in log
im=Image.open(p).convert('RGB');assert list(im.size)==[meta['width'],meta['height']];out=p.with_suffix('.png');im.save(out);assert Image.open(out).convert('RGB').tobytes()==im.tobytes()
report={'status':'PASS_LIVE_NATIVE_FRAMEBUFFER_CAPTURE','dimensions':list(im.size),'gameplay_updates':meta['gameplay_updates'],'pose_metadata':str(Path(str(p)+'.json')),'png_lossless_pixel_match':True,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-live-capture.exe').read_bytes()).hexdigest(),'image_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),'scope':'Actual live backbuffer read before swap at historical pose during native input-only pickup. Native perdraw state/RNG/cache assertions and pickup completion passed. Visual inspection shows full rear taxi body above viewport bottom in this capture. Original boot4200 reference also inspected; poses/timers/traffic differ, so no matching-pose pixel fidelity claim. 644x461 client, not4K or sustained60 proof.'}
(r/'reports/live-taxi-framing-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',meta['gameplay_updates'],im.size)
