from pathlib import Path
import csv,json,re,statistics,hashlib
r=Path('outputs/crazy-taxi-native');runs=[]
for visibility in ['hidden','visible']:
 stem=f'window-control-{visibility}-1200';c=list(csv.DictReader((r/f'reports/{stem}.csv').open()));g=list(csv.DictReader((r/f'reports/{stem}.csv.gpu.csv').open()));log=(r/f'reports/{stem}.log').read_text();assert len(c)==len(g)==1200 and not (r/f'reports/{stem}.err').read_text();assert 'submitted1200 completed1200' in log
 m=re.search(r'4K WINDOW CONTROL: requested(\w+) actual_visible(\d+) client(\d+)x(\d+)',log);assert m and m[1]==visibility and int(m[2])==(visibility=='visible');assert int(m[3])>0 and int(m[4])>0
 prev=0
 for i,(cpu,gpu) in enumerate(zip(c,g)):
  assert int(cpu['frame'])==int(gpu['frame'])==i
  a,b,d,e=map(int,[gpu['start_ns'],gpu['end_ns'],gpu['blit_start_ns'],gpu['blit_end_ns']]);assert 0<a<b<=d<e and a>=prev;prev=e
  if i:assert abs(float(cpu['interval_ms'])-float(cpu['pre_due_ms'])-sum(float(cpu[k]) for k in ['update_ms','prepare_ms','submit_ms','gpu_finish_ms','checks_ms','swap_ms']))<0.00001
  assert abs(float(cpu['swap_ms'])-float(cpu['blit_cpu_ms'])-float(cpu['swap_only_ms']))<0.00001
 intervals=[float(x['interval_ms']) for x in c[1:]];worst=max(range(1,len(c)),key=lambda i:float(c[i]['interval_ms']))
 runs.append({'visibility':visibility,'actual_client':[int(m[3]),int(m[4])],'frames':len(c),'mean_interval_ms':statistics.mean(intervals),'average_fps':1000/statistics.mean(intervals),'p95_interval_ms':sorted(intervals)[len(intervals)*95//100],'maximum_interval_ms':max(intervals),'skipped_deadlines':int(re.search(r'(\d+) skipped60Hz deadlines',log)[1]),'swap_only_max_ms':max(float(x['swap_only_ms']) for x in c),'gpu_draw_max_ms':max(float(x['gpu_ms']) for x in g),'gpu_blit_max_ms':max(float(x['blit_gpu_ms']) for x in g),'worst_frame':{'cpu':c[worst],'gpu':g[worst]}})
report={'status':'PASS_ACTUAL_VISIBLE_HIDDEN_WINDOW_CONTROLS','runs':runs,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-window-control.exe').read_bytes()).hexdigest(),'scope':'Same binary and internal4K target; sequential bounded20second runs with measured actual window visibility/client size, all completed GPU draw/blit pairs and complete CPU interval accounting. SW_SHOWNOACTIVATE does not request foreground; actual occlusion, scanout, device controls and stable60 remain unverified. Sequential runs are observational, not causal attribution.'}
(r/'reports/window-control-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({'runs':[{k:v for k,v in x.items() if k!='worst_frame'} for x in runs]},indent=2))
