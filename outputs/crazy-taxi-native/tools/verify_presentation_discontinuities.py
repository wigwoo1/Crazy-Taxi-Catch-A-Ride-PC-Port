import pathlib,json,math
r=pathlib.Path('outputs/crazy-taxi-native/reports');results=[]
for kind in ('focus','reset','lookback'):
 stem='presentation-'+kind;rows=[x.split() for x in (r/(stem+'-poses.txt')).read_text().splitlines()];assert len(rows)==360;blends=0;changes=[]
 for n,row in enumerate(rows):
  assert len(row)==14;now,target,before,after=map(int,row[:4]);alpha=float(row[4]);held=int(row[5]);assert target==max(0,now-33500000) and 0<=alpha<=1
  if held:assert before==after and alpha==0
  else:
   assert before<after and before<=target<=after
   assert math.isclose(alpha,(target-before)/(after-before),abs_tol=1e-14)
   blends+=int(0<alpha<1)
  if n and row[12]!=rows[n-1][12]:
   changes.append(n);assert held and before==after and before<=now
 assert blends>20
 log=(r/(stem+'.log')).read_text()
 if kind=='focus':assert 'PASS FOCUS SUSPENSION: 60 render frames' in log
 if kind=='reset':assert 'PASS RESPAWN CLOCK:' in log;assert int(rows[120][5])==1
 if kind=='lookback':assert len(changes)==2 and {int(x[12]) for x in rows}=={2,18}
 results.append({'kind':kind,'frames':len(rows),'interior_blends':blends,'mode_transition_rows':changes})
report={'status':'PASS_TIMESTAMPED_PRESENTATION_DISCONTINUITIES','results':results,'scope':'Controlled native focus/reset and scripted decodedXboxlookback with360draws each. Actual bracket fractions and modehistory clearing checked; perdrawcar/contextassertions passed. Not physicalOSfocus/controller/device or visualpixel verification.'};(r/'presentation-discontinuities-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
