import pathlib,json,subprocess,math
r=pathlib.Path('outputs/crazy-taxi-native');commands=[];expected=[];counts={}
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];events=[b for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];baseline=records[0]['counter'];stamps=[(0,0)]+[((e['counter']-baseline)*4389000000000//262144,n+1) for n,e in enumerate(events)]
 commands.extend(['R','P 0 0']);pending=1;history=[stamps[0]]
 for frame in range(380):
  now=frame*1000000000//60
  while pending<len(stamps) and stamps[pending][0]<=now:
   t,tag=stamps[pending];commands.append(f'P {t} {tag}');history.append((t,tag));history=history[-16:];pending+=1
  target=max(0,now-100000000);commands.append(f'Q {target}')
  if target<=history[0][0]:want=(history[0][1],history[0][1],0.,1)
  elif target>history[-1][0]:want=(history[-1][1],history[-1][1],0.,1)
  else:
   a,b=next((a,b) for a,b in zip(history,history[1:]) if a[0]<target<=b[0]);want=(a[1],b[1],(target-a[0])/(b[0]-a[0]),0)
  expected.append(want)
 counts[side]={'source_updates':len(events),'presentation_queries':380}
cp=subprocess.run([str(r/'bin/render-history-probe.exe')],input='\n'.join(commands)+'\n',capture_output=True,text=True,check=True);rows=cp.stdout.splitlines();assert len(rows)==len(expected)==760
for row,want in zip(rows,expected):
 a,b,alpha,held=row.split();assert (int(a),int(b),int(held))==(want[0],want[1],want[3]);assert math.isclose(float(alpha),want[2],abs_tol=1e-14)
report={'status':'PASS_NATIVE_TIMESTAMP_HISTORY','queries':760,'sources':counts,'scope':'Original observed variable update counters converted by exact source tick ratio; online60Hz queries use diagnostic100ms delay. Ring wrap, startup and late hold, reset betweenjourneys, and rejected equal timestamps preserve state. Not viewer integration, chosen product latency, pose/pixel fidelity or sustained60fps proof.'};(r/'reports/render-history-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
