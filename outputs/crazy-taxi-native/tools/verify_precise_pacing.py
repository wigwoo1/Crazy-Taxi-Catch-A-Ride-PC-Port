import pathlib,csv,json,statistics,re,hashlib
r=pathlib.Path('outputs/crazy-taxi-native'); runs=[]
for stem in ['live4k-async-1200','live4k-precise-1200']:
 rows=list(csv.DictReader((r/f'reports/{stem}.csv').open())); gpu=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open())); log=(r/f'reports/{stem}.log').read_text()
 assert len(rows)==len(gpu)==1200 and not (r/f'reports/{stem}.err').read_text()
 assert 'ASYNC4K GPU: submitted1200 completed1200' in log
 previous=0
 for i,(cpu,g) in enumerate(zip(rows,gpu)):
  assert int(cpu['frame'])==int(g['frame'])==i and float(cpu['gpu_timestamp_ms'])==-1
  a,b=int(g['start_ns']),int(g['end_ns']); assert 0<a<b and a>=previous; previous=b
  assert abs(float(g['gpu_ms'])-(b-a)/1e6)<1e-6
 intervals=[float(x['interval_ms']) for x in rows[1:]]; durations=[float(x['gpu_ms']) for x in gpu]; assert min(intervals)>0
 item={'run':stem,'frames':len(rows),'mean_interval_ms':statistics.mean(intervals),'average_fps':1000/statistics.mean(intervals),'p95_interval_ms':sorted(intervals)[len(intervals)*95//100],'maximum_interval_ms':max(intervals),'skipped_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1]),'gpu_mean_ms':statistics.mean(durations),'gpu_max_ms':max(durations)}
 if 'precise' in stem:
  waits,spins=map(int,re.search(r'high-resolution timer waits(\d+) final-spin passes(\d+)',log).groups()); assert waits>0
  item.update(timer_waits=waits,spin_passes=spins)
 runs.append(item)
report={'status':'PASS_COMPLETION_AND_TIMER_EXERCISE','runs':runs,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-precise-pacing.exe').read_bytes()).hexdigest(),'scope':'Sequential 20-second hidden native 4K runs, distinct completed GPU queries for all frames, original simulation/RNG immutability assertions. Scheduling comparison is observational, not proof of sustained smooth 60 fps or monitor scanout. Source physics/audio use actual elapsed time; precision flag changes presentation waiting only. Product launcher unchanged.'}
(r/'reports/precise-pacing-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
