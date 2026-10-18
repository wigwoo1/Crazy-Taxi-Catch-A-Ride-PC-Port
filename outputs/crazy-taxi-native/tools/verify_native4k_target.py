import sys,pathlib,csv,json,re,statistics,hashlib
sys.path.insert(0,str(pathlib.Path('work/python-packages').resolve()))
from PIL import Image
r=pathlib.Path('outputs/crazy-taxi-native');stem='native4k-target-600'; rows=list(csv.DictReader((r/f'reports/{stem}.csv').open()));gpu=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open()));log=(r/f'reports/{stem}.log').read_text();assert len(rows)==len(gpu)==600 and not (r/f'reports/{stem}.err').read_text();assert 'submitted600 completed600' in log and 'rendered600 window blits600' in log
previous=0
for i,(c,g) in enumerate(zip(rows,gpu)):
 assert int(c['frame'])==int(g['frame'])==i and float(c['gpu_timestamp_ms'])==-1
 a,b=int(g['start_ns']),int(g['end_ns']);assert 0<a<b and a>=previous;previous=b;assert abs(float(g['gpu_ms'])-(b-a)/1e6)<1e-6
intervals=[float(c['interval_ms']) for c in rows[1:]]
image=Image.open(r/'reports/native4k-target-capture.bmp').convert('RGB');assert image.size==(3840,2160);image.save(r/'reports/native4k-target-capture.png');assert image.tobytes()==Image.open(r/'reports/native4k-target-capture.png').convert('RGB').tobytes()
meta=json.loads((r/'reports/native4k-target-capture.bmp.json').read_text());assert meta['width']==3840 and meta['height']==2160 and meta['gameplay_updates']==240
capturelog=(r/'reports/native4k-target-capture.log').read_text();assert 'rendered360 window blits360' in capturelog and '360 frames at3840x2160' in capturelog and not (r/'reports/native4k-target-capture.err').read_text()
report={'status':'PASS_NATIVE_4K_TARGET_ALLOCATION_DRAW_COMPLETION_CAPTURE','async_frames':600,'completed_gpu_pairs':600,'mean_interval_ms':statistics.mean(intervals),'average_fps':1000/statistics.mean(intervals),'p95_interval_ms':sorted(intervals)[len(intervals)*95//100],'maximum_interval_ms':max(intervals),'skipped_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1]),'gpu_draw_mean_ms':statistics.mean(float(g['gpu_ms']) for g in gpu),'capture':meta,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-native4k-target.exe').read_bytes()).hexdigest(),'scope':'Independent color3840x2160 RGBA8/depth24 FBO complete; 600 distinct GPU draw queries completed, 600 blit calls with GL error checks; 360 synchronous capture-run draw completions and blits, actual4K BMP readback. GPU timestamps bracket gameplay draw, not later display blit. No monitor scanout, window blit pixel/letterbox equivalence, sustained60, fullgame/sourcepixels proof. Startup/loading uses default framebuffer while target unbound; startup/menu tests pending. No playable launcher promotion.'}
(r/'reports/native4k-target-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ['capture','scope']},indent=2))
