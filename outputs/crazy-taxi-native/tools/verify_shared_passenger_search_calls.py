from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');p=root/'reports/active-held-input/passenger-search-calls.json';d=json.loads(p.read_text());assert d['status']=='SHARED_ORIGINAL_NATIVE_PASSENGER_SEARCH_PASS';cases=[]
for c in d['calls']:
 count=c['count'];xy=[v-0x100000000 if v&0x80000000 else v for v in c['arguments'][:2]];fixture=struct.pack('<I2i',count,*xy)+bytes.fromhex(c['before'])+b''.join(struct.pack('<2i',*s) for s in c['spawns']);cp=subprocess.run([str(root/'bin/passenger-search-owner-compare.exe')],input=fixture,capture_output=True);expected=struct.pack('<2i',c['index'],c['distance'])+bytes.fromhex(c['after']);assert cp.returncode==0 and cp.stdout==expected
 cases.append(dict(count=count,taxi=xy,index=c['index'],distance=c['distance'],source_instructions=c['instructions'],compared_bytes=len(expected)))
assert cases
report=dict(status='NATIVE_ACTUAL_RETAINED_PASSENGER_SEARCH_PASS',cases=cases,source_capture_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),scope='Native passenger_search candidate/distance/customer44byte owners vs original050AC at actual shared retained post-fade callers. Explicit held input startup fixture; no independently native vehicle trajectory, boarding/delivery/fare or whole gameplay claim.')
(root/'reports/shared-retained-passenger-search-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
