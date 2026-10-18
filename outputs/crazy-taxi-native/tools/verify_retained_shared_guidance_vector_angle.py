"""Compare captured shared source vector-angle returns with production native math."""
import hashlib,json,subprocess
from pathlib import Path
root=Path('outputs/crazy-taxi-native')
capture=root/'reports/active-held-input/route-guidance-owner-calls.json'
if not capture.exists():raise SystemExit('PENDING: source guidance calls not yet captured')
data=json.loads(capture.read_text())
if not isinstance(data,list):raise SystemExit('Unexpected capture shape; inspect before comparison')
calls=[c for c in data if int(str(c['entry']),0)==0x0802e00c]
if not calls:raise SystemExit('PENDING: original vector-angle owner has not returned')
def signed(v):return v-0x100000000 if v&0x80000000 else v
exe=root/'bin/vector-angle-rom-compare.exe'
inputs=[tuple(signed(v) for v in c['arguments'][:2]) for c in calls]
p=subprocess.run([str(exe)],input=''.join(f'{x} {z}\n' for x,z in inputs),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in p.stdout.splitlines()]
assert len(rows)==len(calls)
for c,row in zip(calls,rows):assert row==[0,signed(c['result'])],(c,row)
report=dict(match=True,cases=len(calls),inputs=inputs,scope='Actual captured shared 2E00C vector-angle returns versus production native math on identical arguments. Does not verify complete route guidance, rendering or driving.',sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [capture,exe,root/'assets/driving.ctr']})
(root/'reports/retained-shared-guidance-vector-angle-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS shared guidance vector angles',len(calls))
