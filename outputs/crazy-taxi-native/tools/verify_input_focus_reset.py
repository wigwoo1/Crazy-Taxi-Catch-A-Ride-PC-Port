import pathlib,json
r=pathlib.Path('outputs/crazy-taxi-native/reports');results=[]
for kind in ('focus-suspend','respawn-clock'):
 stem='input-owner-'+kind;rows=[list(map(int,x.split())) for x in (r/(stem+'.txt')).read_text().splitlines()];samples=[list(map(int,x.split())) for x in (r/(stem+'-input.txt')).read_text().splitlines()];owner=1;last=0;checks=0
 for n,row in enumerate(rows):
  if n==120:owner=1
  if row[7]>last:
   assert row[7]==last+1
   assert samples[row[7]-1][:2]==[row[2],owner]
   checks+=1
  last=row[7]
  if row[6]:owner=row[2]
 assert len(samples)==checks
 assert samples[rows[120][7]-1][:2]==[1,1]
 log=(r/(stem+'.log')).read_text();assert ('PASS FOCUS SUSPENSION:' if kind=='focus-suspend' else 'PASS RESPAWN CLOCK:') in log
 results.append({'kind':kind,'trace_rows':len(rows),'input_updates_verified':checks})
report={'status':'PASS_INPUT_TIMING_FOCUS_RESET','results':results,'scope':'Controlled native host events, actual sampled deltas; not OS focus, physical reset key or audio listening verification.'};(r/'input-owner-focus-reset-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
