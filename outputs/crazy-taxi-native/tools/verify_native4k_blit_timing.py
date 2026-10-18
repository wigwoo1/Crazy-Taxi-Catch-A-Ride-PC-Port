from pathlib import Path
import csv,json,re,statistics,hashlib,sys
count=int(sys.argv[1]) if len(sys.argv)>1 else 600
assert count in (600,1200)
r=Path('outputs/crazy-taxi-native'); stem=f'native4k-blit-timing-{count}';cpu=list(csv.DictReader((r/f'reports/{stem}.csv').open()));gpu=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open()));log=(r/f'reports/{stem}.log').read_text();assert len(cpu)==len(gpu)==count and not (r/f'reports/{stem}.err').read_text();assert f'submitted{count} completed{count}' in log and f'rendered{count} window blits{count}' in log
previous=0
for i,(c,g) in enumerate(zip(cpu,gpu)):
 assert int(c['frame'])==int(g['frame'])==i and float(c['gpu_timestamp_ms'])==-1
 a,b,d,e=map(int,[g['start_ns'],g['end_ns'],g['blit_start_ns'],g['blit_end_ns']]);assert 0<a<b<=d<e and a>=previous;previous=e
 assert abs(float(g['gpu_ms'])-(b-a)/1e6)<1e-6 and abs(float(g['blit_gpu_ms'])-(e-d)/1e6)<1e-6
intervals=[float(c['interval_ms']) for c in cpu[1:]]
def stats(values):return {'mean':statistics.mean(values),'p95':sorted(values)[len(values)*95//100],'max':max(values)}
report={'status':'PASS_SEPARATE_DRAW_AND_BLIT_GPU_COMPLETION','frames':count,'interval_ms':stats(intervals),'average_fps':1000/statistics.mean(intervals),'draw_gpu_ms':stats([float(g['gpu_ms']) for g in gpu]),'blit_gpu_ms':stats([float(g['blit_gpu_ms']) for g in gpu]),'skipped_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1]),'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-native4k-blit-timing.exe').read_bytes()).hexdigest(),'scope':'All recorded frames have distinct completed ordered draw and blit timestamp pairs, final-only drain; normal game-state assertions. One bounded hidden-window run, not sustained scanout/device/stability/fullgame proof. Earlier slower runs preserved, timing differences not assigned a cause.'}
(r/f'reports/native4k-blit-timing-{count}-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
