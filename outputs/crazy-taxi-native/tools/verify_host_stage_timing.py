from pathlib import Path
import csv,json,re,statistics,hashlib
r=Path('outputs/crazy-taxi-native');stem='host-stage-timing-1200';cpu=list(csv.DictReader((r/f'reports/{stem}.csv').open()));gpu=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open()));log=(r/f'reports/{stem}.log').read_text();assert len(cpu)==len(gpu)==1200 and not (r/f'reports/{stem}.err').read_text();assert 'submitted1200 completed1200' in log
stages=['update_ms','prepare_ms','submit_ms','gpu_finish_ms','checks_ms','swap_ms'];previous=0
for i,(c,g) in enumerate(zip(cpu,gpu)):
 assert int(c['frame'])==int(g['frame'])==i
 a,b,d,e=map(int,[g['start_ns'],g['end_ns'],g['blit_start_ns'],g['blit_end_ns']]);assert 0<a<b<=d<e and a>=previous;previous=e
 if i:assert abs(float(c['interval_ms'])-float(c['pre_due_ms'])-sum(float(c[k]) for k in stages))<0.00001
 assert abs(float(c['swap_ms'])-float(c['blit_cpu_ms'])-float(c['swap_only_ms']))<0.00001
 assert all(float(c[k])>=0 for k in stages+['pump_ms','pacer_call_ms','deadline_late_ms'])
intervals=[float(c['interval_ms']) for c in cpu[1:]];slow=sorted(range(1,len(cpu)),key=lambda i:float(cpu[i]['interval_ms']),reverse=True)[:10]
report={'status':'PASS_HOST_STAGE_ACCOUNTING_AND_GPU_COMPLETION','frames':1200,'mean_interval_ms':statistics.mean(intervals),'average_fps':1000/statistics.mean(intervals),'p95_interval_ms':sorted(intervals)[len(intervals)*95//100],'maximum_interval_ms':max(intervals),'skipped_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1]),'longest_frames':[{'cpu':cpu[i],'gpu':gpu[i]} for i in slow],'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-host-stage-timing.exe').read_bytes()).hexdigest(),'scope':'CPU stages account for complete swap-to-swap interval within CSV rounding. Pump includes window events and audio output; pacer-call time includes OS scheduling while inside waits. pre_due includes prior-frame post-swap audio/time bookkeeping, diagnostics, waits, message pump and unmeasured scheduling. No causal OS/driver attribution, actual scanout or sustained60 proof.'}
(r/'reports/host-stage-timing-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ['longest_frames','scope']},indent=2));print(json.dumps(report['longest_frames'][0],indent=2))
