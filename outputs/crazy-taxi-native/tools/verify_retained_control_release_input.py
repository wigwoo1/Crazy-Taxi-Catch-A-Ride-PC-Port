from pathlib import Path
import json,subprocess
r=Path('outputs/crazy-taxi-native');d=json.loads((r/'reports/active-held-input/input-calls.json').read_text());calls=d['presentation'];cases=[]
for index,c in enumerate(calls):
 prior=bytes.fromhex(c['before']);expected=bytes.fromhex(c['after']);assert len(prior)==len(expected)==66
 cp=subprocess.run([str(r/'bin/retained-input-owner-compare.exe')],input=' '.join(map(str,c['sample']+list(prior)))+'\n',capture_output=True,text=True);assert cp.returncode==0
 native=bytes(map(int,cp.stdout.split()));diff=[i for i,(a,b) in enumerate(zip(native,expected)) if a!=b]
 cases.append(dict(frame=index+1,original_instructions=c['instructions'],sample=c['sample'],native_bytes=len(native),different_bytes=len(diff),first_differences=diff[:16],held_A=expected[6],held_Right=expected[10],pressed_A=expected[36],pressed_Right=expected[40]))
passed=3<=len(cases)<=200 and all(c['native_bytes']==66 and c['different_bytes']==0 and c['sample'][0]==1006 for c in cases)
report=dict(status='NATIVE_ORIGINAL_FADE_SEQUENCE_HELD_INPUT_PASS' if passed else 'RETAINED_HELD_INPUT_DEPENDENCY',cases=cases,scope='Original presentation15C3C sampler executed with explicit held A+Right KEYINPUT1006 across retained source frames vs native input_update from actual prior66byte owners. Prior states are captured including game intervening writes; not independent native vehicle trajectory, countdown bypass, Xbox device test or completed gameplay.')
(r/'reports/retained-control-release-input-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
