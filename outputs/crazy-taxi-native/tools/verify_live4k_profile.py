import csv,json,statistics,pathlib,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');results={}
for stem,binary in [('live4k-profile','scene-viewer-live4k-profile.exe'),('live4k-gpu-timers','scene-viewer-live4k-gpu-timers.exe')]:
 rows=list(csv.DictReader((r/f'reports/{stem}.csv').open()));assert len(rows)==600 and not (r/f'reports/{stem}.err').read_text();assert 'LIVE4K GPU COMPLETED: 600 frames at3840x2160' in (r/f'reports/{stem}.log').read_text()
 stats={}
 for key in rows[0]:
  if key in ['frame','updates']:continue
  v=[float(x[key]) for x in rows[1:]];assert min(v)>=0;stats[key]={'mean':statistics.mean(v),'p95':sorted(v)[len(v)*95//100],'maximum':max(v)}
 results[stem]={'frames':len(rows),'stats_ms':stats,'binary_sha256':hashlib.sha256((r/'bin'/binary).read_bytes()).hexdigest()}
gpu=results['live4k-gpu-timers']['stats_ms'];assert 0<gpu['gpu_timestamp_ms']['mean']<1 and gpu['gpu_finish_ms']['maximum']>100
report={'status':'PASS_NATIVE_4K_PROFILING_PERFORMANCE_NOT_VERIFIED','runs':results,'scope':'600-frame native4K gameplay runs without screenshots. Perframe QPC splits update/preparation/GL submission/glFinish/state-check/swap costs; second run brackets draw commands with GPU timestamps. CPU completion waits greatly exceed GPU draw timestamp intervals; this supports investigating synchronization/dispatch/hidden drawable scheduling rather than asserting a slow shader. Does not identify exact driver/OS cause. Everyframe glFinish is diagnostic; next control must collect completed GPU timestamps asynchronously while preserving natural production submission. No60fps or monitor scanout claim; earlier expiry failure remains unresolved.'}
(r/'reports/live4k-profile-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS profile; GPU timestamp mean',gpu['gpu_timestamp_ms']['mean'],'completion wait max',gpu['gpu_finish_ms']['maximum'])
