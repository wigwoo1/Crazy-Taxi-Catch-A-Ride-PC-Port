import pathlib,json,struct,subprocess
r=pathlib.Path('outputs/crazy-taxi-native');commands=[];expected=[]
def pose(record):
 a=bytes.fromhex(record['actor']);return list(struct.unpack_from('<iii',a))+list(struct.unpack_from('<iii',a,0x498))+[struct.unpack_from('<h',a,0x18)[0]*4096,struct.unpack_from('<i',a,0x1c)[0],struct.unpack_from('<h',a,0x1a)[0]*4096]+[x*4096 for x in struct.unpack_from('<hhh',a,0x4a4)]
def interpolate(a,b,alpha):
 if alpha<=0:return a[:]
 if alpha>=1:return b[:]
 out=[int(x+(y-x)*alpha) for x,y in zip(a[:6],b[:6])]
 for x,y in zip(a[6:],b[6:]):
  delta=(y-x)&0xffffff
  if delta>0x800000:delta-=0x1000000
  value=(x+int(delta*alpha))&0xffffffff;out.append(value if value<0x80000000 else value-0x100000000)
 return out
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];events=[b for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];base=records[0]['counter'];stamps=[(0,pose(records[0]))]+[((e['counter']-base)*4389000000000//262144,pose(e)) for e in events];commands.append('R');history=[];pending=0
 for frame in range(380):
  now=frame*1000000000//60
  while pending<len(stamps) and stamps[pending][0]<=now:
   t,p=stamps[pending];commands.append('P '+str(t)+' '+' '.join(map(str,p)));history.append((t,p));history=history[-16:];pending+=1
  target=max(0,now-100000000);commands.append(f'Q {target}')
  if target<=history[0][0]:want=history[0][1]
  elif target>history[-1][0]:want=history[-1][1]
  else:
   a,b=next((a,b) for a,b in zip(history,history[1:]) if a[0]<target<=b[0]);want=interpolate(a[1],b[1],(target-a[0])/(b[0]-a[0]))
  expected.append(want)
cp=subprocess.run([str(r/'bin/render-pose-history-probe.exe')],input='\n'.join(commands)+'\n',capture_output=True,text=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert rows==expected
report={'status':'PASS_NATIVE_IMMUTABLE_POSE_HISTORY','samples':len(rows),'fields_per_sample':12,'scope':'Original moving taxi/camera snapshots, variableupdate timestamps, online60Hz queries with100ms diagnostic delay, ringwrap/reset/holds and shortest-angle interpolation. C asserts every sample and rejected duplicate push preserve history bytes. Presentation extension, not original60fps physics or viewer integration proof.'};(r/'reports/render-pose-history-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
