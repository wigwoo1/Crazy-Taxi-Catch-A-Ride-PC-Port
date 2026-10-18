"""Verify extended offline driving captures without claiming native equivalence."""
import pathlib,json,hashlib,sys
root=pathlib.Path('outputs/crazy-taxi-native');ref=root/'reports/original-core-reference'
prefix=sys.argv[1] if len(sys.argv)>1 else 'shared'
ram=(ref/'boot4200.ewram').read_bytes();summary=[]
for side in ('left','right'):
 path=ref/f'{prefix}{side}.packed-driving-trace.json'
 records=json.loads(path.read_text())['records']
 prior=json.loads((ref/f'completeinput{side}.packed-driving-trace.json').read_text())['records']
 assert len(records)==len(prior)==361
 for record,old in zip(records,prior):
  assert {k:v for k,v in record.items() if k!='shared_owners'}==old
  assert set(record['shared_owners'])=={'02001534','0200164C','02001770','02001EA0','02001EB0','020025D0','02007600'}
 for address,raw in records[0]['shared_owners'].items():
  offset=int(address,16)-0x2000000;data=bytes.fromhex(raw)
  assert data==ram[offset:offset+len(data)]
 assert json.loads((ref/f'{prefix}{side}.actor-trace.json').read_text())==json.loads((ref/f'completeinput{side}.actor-trace.json').read_text())
 summary.append({'side':side,'snapshots':len(records),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'shared_bytes_per_snapshot':sum(len(bytes.fromhex(x)) for x in records[0]['shared_owners'].values())})
report={'status':'PASS_SHARED_DRIVING_REFERENCE_CAPTURE','captures':summary,'scope':'Seven explicit shared EWRAM ranges per baseline/VBlank snapshot; baseline bytes match independent boot4200 memory. All previous actor/input/command/timing snapshots and trajectories unchanged. Offline source reference only; no native continuous-trajectory equivalence or routine-entry ordering claim.'}
(root/'reports/shared-driving-capture-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
