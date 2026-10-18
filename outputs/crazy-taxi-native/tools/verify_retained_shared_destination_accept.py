"""Native comparison against captured original destination acceptance."""
import hashlib,json,struct,subprocess
from pathlib import Path
root=Path('outputs/crazy-taxi-native');path=root/'reports/active-held-input/destination-accept-owner-call.json'
if not path.exists():raise SystemExit('PENDING: actual original destination acceptance has not returned')
c=json.loads(path.read_text());record=bytes.fromhex(c['record']) if c['record'] is not None else bytes(48)
assert len(record)==48
selected=c['selected'];selected=selected-4294967296 if selected&2147483648 else selected
values=[selected,int(c['record_pointer']!=0),c['arguments'][0],*struct.unpack_from('<4H',record,40)]
exe=root/'bin/destination-accept-rom-compare.exe'
p=subprocess.run([str(exe)],input=' '.join(map(str,values))+'\n',capture_output=True,text=True,check=True)
native=int(p.stdout);assert native==c['result'],(native,c['result'])
report=dict(match=True,native=native,original=c['result'],inputs=values,instructions=c['instructions'],scope='Actual captured E0F4 return vs existing production destination acceptance on identical selected/record/material slots. Not delivery/fare/full-frame verification.',sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [path,exe,root/'reports/destination-accept-cfg.json']})
(root/'reports/retained-shared-destination-accept-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original shared destination acceptance',native)
