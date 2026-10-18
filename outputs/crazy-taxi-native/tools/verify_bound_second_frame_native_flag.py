from pathlib import Path
import json,subprocess
root=Path('outputs/crazy-taxi-native')
cp=subprocess.run([str(root/'bin/bound-first-frame-flag-native-compare.exe'),str(root/'reports/bound-second-frame-original-polygons-indexed.bin'),str(root/'assets/challenge-flag/flag-indicator.indices'),str(root/'reports/bound-second-frame-flag-bitmap-original-indexed.bin')],capture_output=True)
passed=cp.returncode==0 and len(cp.stdout)==153600
report=dict(status='NATIVE_ORIGINAL_FLAG_RGBA_PASS' if passed else 'NATIVE_FLAG_DEPENDENCY',exit=cp.returncode,output_bytes=len(cp.stdout),scope='Existing native indicator helper versus original bitmap over240x160 convertedRGBA; deliberately unique testpalette0..255, not actualpaletteownership; retained secondframe x4 y137 variant1.')
(root/'reports/bound-second-frame-native-flag-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
if passed:(root/'reports/bound-second-frame-native-flag-test-palette.rgba').write_bytes(cp.stdout)
print(json.dumps(report));assert passed
