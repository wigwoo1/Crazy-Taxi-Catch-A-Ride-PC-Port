import pathlib,csv,json,statistics,re,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');runs=[]
for stem,count in [('live4k-async',600),('live4k-async-1200',1200)]:
 rows=list(csv.DictReader((r/f'reports/{stem}.csv').open()));gpu=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open()));log=(r/f'reports/{stem}.log').read_text();assert len(rows)==len(gpu)==count and not (r/f'reports/{stem}.err').read_text()
 assert f'ASYNC4K GPU: submitted{count} completed{count}' in log
 previous=0
 for n,(cpu,g) in enumerate(zip(rows,gpu)):
  assert int(cpu['frame'])==int(g['frame'])==n and float(cpu['gpu_timestamp_ms'])==-1
  a,b=int(g['start_ns']),int(g['end_ns']);assert 0< a<=b and a>=previous;previous=b;assert abs(float(g['gpu_ms'])-(b-a)/1e6)<1e-6
 intervals=[float(x['interval_ms']) for x in rows[1:]];times=[float(x['gpu_ms']) for x in gpu];assert min(intervals)>0 and min(times)>0
 runs.append({'frames':count,'mean_interval_ms':statistics.mean(intervals),'average_fps':1000/statistics.mean(intervals),'p95_interval_ms':sorted(intervals)[len(intervals)*95//100],'maximum_interval_ms':max(intervals),'gpu_timestamp_mean_ms':statistics.mean(times),'gpu_timestamp_max_ms':max(times),'skipped_60hz_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1])})
report={'status':'PASS_DISTINCT_ASYNC_NATIVE4K_COMPLETION_NEAR_60FPS_NOT_FULL_RELEASE_PROOF','runs':runs,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-live4k-async.exe').read_bytes()).hexdigest(),'scope':'Each frame uses distinct GPU timestamp objects; CPU CSV marks GPU duration unavailable during gameplay, final glFinish and availability checks produce ordered completed query pairs for all600/1200 frames. No perframe finish/result waits, no screenshots. Native game/RNG/cache immutability assertions pass. Longer20second run retains18 deadline skips and p95~21.9ms, so steady smooth60 not proven. Hidden oversized WGL framebuffer, not actual monitor scanout/internal render-target product architecture. Final drain measured after existing GL texture cleanup, so not an isolated presentation latency measurement. Original synchronous stress check preserved; earlier expiry failure unresolved.'}
(r/'reports/live4k-async-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',[(x['frames'],round(x['average_fps'],3),x['skipped_60hz_deadlines']) for x in runs])
