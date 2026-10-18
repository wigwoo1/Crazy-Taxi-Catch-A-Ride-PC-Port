from pathlib import Path
import json,subprocess
root=Path('outputs/crazy-taxi-native');audit=json.loads((root/'reports/bound-first-frame-oam-sort-audit.json').read_text());assert audit['status']=='SOURCE_OAM_SORT_RETURN_ABI_PASS'
cp=subprocess.run([str(root/'bin/bound-first-frame-oam-sort-native-compare.exe'),str(root/'reports/bound-first-frame-oam-sort-before.bin')],capture_output=True);expected=(root/'reports/bound-first-frame-oam-sort-after.bin').read_bytes();diff=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b];passed=cp.returncode==0 and len(cp.stdout)==len(expected)==4490 and not diff
report=dict(status='NATIVE_ORIGINAL_OAM_SORT_PASS' if passed else 'NATIVE_OAM_SORT_DEPENDENCY',native_exit=cp.returncode,entries=2,compared_bytes=4490,different_bytes=len(diff),first_differences=diff[:32],scope='Existing nativepairedsort versus actual280C4 on two shared firstframe taxi entries, fullqueue/matrix/counter snapshot compared; not fullmerge/OAMpublication orvisibleframe.')
(root/'reports/bound-first-frame-native-oam-sort-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
