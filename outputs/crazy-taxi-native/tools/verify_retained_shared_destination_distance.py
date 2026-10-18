"""Compare actual captured original E034 result with the native scalar routine."""
import hashlib,json,subprocess
from pathlib import Path
root=Path('outputs/crazy-taxi-native')
path=root/'reports/active-held-input/destination-distance-owner-call.json'
if not path.exists():raise SystemExit('PENDING: original shared E034 capture has not returned yet')
c=json.loads(path.read_text())
def signed(v):return v-4294967296 if v&2147483648 else v
record=bytes.fromhex(c['record']) if c['record'] is not None else bytes(48)
assert len(record)==48
values=[signed(c['selected']),signed(c['arguments'][0]),signed(c['arguments'][1]),int(c['record_pointer']!=0),*record]
exe=root/'bin/retained-destination-distance-owner-compare.exe'
p=subprocess.run([str(exe)],input=' '.join(map(str,values))+'\n',capture_output=True,text=True,check=True)
original=signed(c['result']);native=int(p.stdout);assert original==native,(original,native)
report=dict(match=True,original=original,native=native,original_instructions=c['instructions'],input=values[:4],scope='Actual captured original E034 scalar result vs production taxi_destination_distance on identical selected/record/XY. Source sqrt uses mathematical BIOS contract. No full-frame or destination render proof.',sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [path,exe,root/'src/destination_guidance.c',root/'reports/destination-distance-cfg.json']})
(root/'reports/retained-shared-destination-distance-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS shared destination distance',original)
