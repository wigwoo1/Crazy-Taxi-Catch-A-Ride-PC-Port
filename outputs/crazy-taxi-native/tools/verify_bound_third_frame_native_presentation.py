from pathlib import Path
import struct,json,subprocess
root=Path('outputs/crazy-taxi-native');a=json.loads((root/'reports/bound-third-frame-presentation-return-audit.json').read_text());assert a['status']=='SOURCE_PRESENTATION_RETURN_ABI_PASS'
for call in a['helper_boundaries']:assert call['before']==call['after'],('helper mutated presentation owner',call['target'])
fixture=root/'reports/bound-third-frame-presentation.fixture';fixture.write_bytes(struct.pack('<15I',*a['fields_before']))
cp=subprocess.run([str(root/'bin/bound-first-frame-presentation-native-compare.exe'),str(fixture)],capture_output=True,text=True);assert cp.returncode==0;native=list(map(int,cp.stdout.split()));expected=a['fields_after']+[len(a['helper_boundaries'])]+[v for call in a['helper_boundaries'] for v in (int(call['target'],16),call['argument'])];assert native==expected,(native,expected)
report=dict(status='NATIVE_ORIGINAL_PRESENTATION_OWNER_PASS',fields=15,helper_requests=len(a['helper_boundaries']),before=a['fields_before'],after=a['fields_after'],scope='Actualshared26318 caller vs nativepresentation_service scalarowners andhelperorder/arguments; actualhelpers confirmedretainthese15owners and independentlyexecuted/comparedelsewhere. Native comparator delegateshelperbodies, not combinednativeframe/boot/activeinput/60Hz proof.')
(root/'reports/bound-third-frame-native-presentation-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
