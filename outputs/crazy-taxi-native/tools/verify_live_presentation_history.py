import pathlib,json,math,sys
r=pathlib.Path('outputs/crazy-taxi-native/reports');stem=sys.argv[1] if len(sys.argv)>1 else 'presentation-startup'
rows=[x.split() for x in (r/(stem+'-poses.txt')).read_text().splitlines()];assert rows and all(len(x)==14 for x in rows);blends=holds=0;minimum=1.;maximum=0.
for row in rows:
 now,target,before,after=map(int,row[:4]);alpha=float(row[4]);held=int(row[5]);assert target==max(0,now-33500000) and 0<=alpha<=1
 if held:assert before==after and alpha==0;holds+=1
 else:
  assert before<after and before<=target<=after
  assert math.isclose(alpha,(target-before)/(after-before),abs_tol=1e-14)
  if 0<alpha<1:blends+=1;minimum=min(minimum,alpha);maximum=max(maximum,alpha)
assert blends>20
log=(r/(stem+'.log')).read_text();assert 'PASS normal startup gameplay music:' in log or 'PASS paced native input-only boarding and original passenger voice playback;' in log
report={'status':'PASS_LIVE_TIMESTAMP_PRESENTATION_FRACTIONS','samples':len(rows),'interior_blends':blends,'held_samples':holds,'interior_alpha_range':[minimum,maximum],'scope':'Actual startup candidate history fractions follow recorded brackets and33.5ms provisionaldelay. Native per-draw assertions preserve car and driver/worldcontext. Not pixels/HUD/otheractors synchronization, sustained4K60 or physical-input latency verification.'};(r/(stem+'-presentation-verification.json')).write_text(json.dumps(report,indent=2)+'\n');print(report)
