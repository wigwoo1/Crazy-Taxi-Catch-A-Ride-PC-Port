from pathlib import Path
import json,struct,hashlib
root=Path('outputs/crazy-taxi-native');d=root/'reports/original-core-reference';trace=d/'serviceOwnersLeft.cpu-timing-trace.json';new=json.loads(trace.read_text())['records'];prior=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];pf=d/'serviceOwnersLeft.packed-driving-trace.json';packed=json.loads(pf.read_text())['records'];assert len(new)==len(prior)==len(packed)==1201
for a,b in zip(new,prior):assert {k:v for k,v in a.items() if k!='frame_service_owners'}==b
assert json.loads(pf.read_text())==json.loads((d/'cpuWindowsLeft.packed-driving-trace.json').read_text())
def decode(r):
 o=r['frame_service_owners'];v=bytes.fromhex(o['owners_7540']);return dict(parity=v[0],mode=v[1],bank=v[8],first=struct.unpack_from('<I',v,12)[0],second=struct.unpack_from('<I',v,16)[0],elapsed=struct.unpack_from('<I',v,28)[0],cursor=int.from_bytes(bytes.fromhex(o['cursor_1e10']),'little'))
commits=0;renderonly=0;ticks=[]
for i in range(1,len(new)):
 a,b=decode(new[i-1]),decode(new[i]);assert a['mode']==b['mode']==4
 if packed[i]['previous']!=packed[i-1]['previous']:
  assert b['first']==a['second'] and b['second']==a['first'];assert b['bank']==(a['bank']+1)&255;assert b['parity']==(a['parity']+1)&1;assert b['cursor']==(a['cursor']+4)&0xffffffff
  assert b['elapsed']==(a['elapsed']+packed[i]['frame_delta'])&0xffffffff;commits+=1;ticks.append(new[i]['tick'])
 else:assert a==b;(renderonly:=renderonly+1)
report=dict(status='PASS_ORIGINAL_FRAME_SERVICE_OWNER_TRANSITIONS',snapshots=len(new),commits=commits,noncommit_samples=renderonly,mode=4,commit_ticks=ticks,findings=['Actual mode4 swaps bothVRAM page pointers and increments8bit bank exactly once perprevious-countercommit.','Parity/cursor/elapsed transitions match original tail; intermediateVBlank samples retain all inspected owners.'],scope='Observed1200-VBlank sourceleft fixture with readonly owner instrumentation and exact priorCPU/packedgameplay noninterference. No instructionentry or hardware display-control trace; does not recover original variable work cadence or full live service integration.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),trace,pf]});(root/'reports/frame-service-owner-transition-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',commits,'source service commits;',renderonly,'intermediate samples retain page/bank/clock owners')
