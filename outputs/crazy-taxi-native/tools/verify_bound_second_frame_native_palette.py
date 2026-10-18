from pathlib import Path
import json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');audit=json.loads((root/'reports/bound-second-frame-palette-audit.json').read_text());assert audit['status']=='SOURCE_PALETTE_RETURN_ABI_PASS'
rom=Path('Crazy Taxi - Catch a Ride (USA).gba');cp=subprocess.run([str(root/'bin/bound-first-frame-palette-native-compare.exe'),str(root/'reports/bound-second-frame-palette-before.bin'),str(rom),str(audit['delta'])],capture_output=True)
expected=(root/'reports/bound-second-frame-palette-after.bin').read_bytes();indices=list(range(2616))+list(range(4152,5178));diff=[i for i in indices if i>=len(cp.stdout) or cp.stdout[i]!=expected[i]]
passed=cp.returncode==0 and len(cp.stdout)==len(expected)==5178 and not diff
report=dict(status='NATIVE_ORIGINAL_PALETTE_PASS' if passed else 'NATIVE_PALETTE_DEPENDENCY',native_exit=cp.returncode,compared_bytes=len(indices),different_bytes=len(diff),first_differences=diff[:32],palette_sha256=hashlib.sha256(cp.stdout[4152:5176]).hexdigest(),scope='Native city_palette_frame with actualregisteredROM/global/record/motion/bank owners, retainedsecondframe delta; compares3642retainedstate/palette/mosaic bytes, excludes1536originaltemporaryscratch bytes. Not wholepresentation/OAM/livepalette orcadence proof.')
(root/'reports/bound-second-frame-native-palette-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
(root/'reports/bound-second-frame-native-palette-state.bin').write_bytes(cp.stdout)
