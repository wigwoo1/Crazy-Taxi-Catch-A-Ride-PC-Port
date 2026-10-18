from pathlib import Path
import sys,json,struct,time
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
steps=0;j=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());count=struct.unpack_from('<I',(root/'assets/city1.ctw').read_bytes(),8)[0];expected={p['source_index'] for p in j['packets'] if p['kind']==0};accepted=[]
for ref in range(count):
 o=original([0,[ref],*j['camera'],j['yaw'],j['pitch']])
 if o['cursor']:accepted.append(ref)
 if (ref+1)%1024==0:print('Checked',ref+1,'of',count,'references; accepted',len(accepted),flush=True)
actual=set(accepted)
report={'status':'MATCH' if actual==expected else 'DIFFERENT','references':count,'original_accepted':accepted,'isolated_native_accepted':sorted(expected),'original_only':sorted(actual-expected),'native_only':sorted(expected-actual),'original_arm_instructions':steps,'scope':'Exhaustive original31EC per-normal-reference submission at actualsource240camera vs isolated native scene capture. Original cache0/phase0/generation1/trig/division contracts. Allcity scan bypasses originalcelltraversal; any additional accepted reference requires bounds/cell ownership investigation; not proof of fullscene/bounds/cadence.'}
(root/'reports/source240-all-normal-reference-observation.json').write_text(json.dumps(report,indent=2)+'\n');print(report,flush=True)

sys.exit(0 if actual==expected else 1)
