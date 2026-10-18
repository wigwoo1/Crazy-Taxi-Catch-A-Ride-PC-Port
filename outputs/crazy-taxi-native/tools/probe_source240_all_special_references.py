from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_special_producer.py').read_text().split('steps=0;cases=[]')[0])
steps=0;j=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());count=struct.unpack_from('<I',(root/'assets/city1-special.cts').read_bytes(),4)[0];expected={p['source_index'] for p in j['packets'] if p['kind']==2};accepted=[]
for ref in range(count):
 o=original([0,ref|0x8000,*j['camera'],j['yaw'],j['pitch']])
 if o['cursor']:accepted.append(ref)
actual=set(accepted)
report={'status':'MATCH' if actual==expected else 'DIFFERENT','references':count,'original_accepted':accepted,'isolated_native_accepted':sorted(expected),'original_only':sorted(actual-expected),'native_only':sorted(expected-actual),'original_arm_instructions':steps,'scope':'Exhaustive fulloriginal31EC per-special-reference at actualsource240camera vs isolated native scene capture. Allcity scan bypasses originalcelltraversal/cachelifecycle; zero phase/cache and patched generation/trig/division contracts; not fullframe/cyclesproof.'}
(root/'reports/source240-all-special-reference-observation.json').write_text(json.dumps(report,indent=2)+'\n');print(report)

sys.exit(0 if actual==expected else 1)
