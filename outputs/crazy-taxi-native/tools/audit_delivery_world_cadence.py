import json,hashlib
from pathlib import Path
from collections import Counter
r=Path('outputs/crazy-taxi-native');schema=json.loads((r/'reports/ordinary-full-world-state-schema.json').read_text())['fields']
rows=[dict(zip(schema,map(int,line.split()[1:]))) for line in (r/'reports/delivery-full-world-native4k.txt').read_text().splitlines()]
assert rows
bend=[v for v in rows if -360000<=v['input_x']<=-350000 and 80000<=v['input_z']<=90000]
report=dict(capture_records=len(rows),per_update_speed_decay_enabled=sum(not(v['flags28']&16) for v in rows),delta_histogram=dict(Counter(v['delta'] for v in rows)),bend_records=len(bend),bend_speed_decay_enabled=sum(not(v['flags28']&16) for v in bend),bend_delta_histogram=dict(Counter(v['delta'] for v in bend)),scope='Independent audit of this failed full-world native capture only. Bend selection is explicit XYZ rectangle, not waypoint association or correspondence to another run. flags28 is actual before-world input: original15B54 speed decay executes whenever bit16 absent. No causal attribution or proof of corrected60Hz gameplay.',sha256=hashlib.sha256((r/'reports/delivery-full-world-native4k.txt').read_bytes()).hexdigest())
(r/'reports/delivery-full-world-cadence-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
