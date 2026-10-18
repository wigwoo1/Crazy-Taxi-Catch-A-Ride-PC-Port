"""Compare actual retained original angle returns against production native scalar."""
import json,subprocess,hashlib
from pathlib import Path
root=Path('outputs/crazy-taxi-native');capture=root/'reports/active-held-input/angle-approach-owner-calls.json'
if not capture.exists():raise SystemExit('PENDING: retained original angle approach has not returned')
rows=json.loads(capture.read_text());assert isinstance(rows,list) and rows
inputs=[]
for row in rows:
 a,b,s=row['arguments'][:3];inputs.append((a,b,s-0x100000000 if s&0x80000000 else s))
exe=root/'bin/retained-angle-approach-compare.exe'
p=subprocess.run([str(exe)],input=''.join(f'{a} {b} {s}\n' for a,b,s in inputs),text=True,capture_output=True,check=True)
native=list(map(int,p.stdout.split()));assert len(native)==len(rows)
for row,value in zip(rows,native):assert row['registers'][0]==value,(row,value)
report=dict(match=True,cases=len(rows),inputs=inputs,source_instructions=sum(row['instructions'] for row in rows),scope='Captured shared original 2DD68 angle approach scalar returns versus production traffic_heading on identical inputs. Does not prove entire frame, driving or native volatile ABI equivalence.',sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [capture,exe,root/'src/traffic_heading.c']})
(root/'reports/retained-shared-angle-approach-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS retained original/native angle approach',len(rows))
