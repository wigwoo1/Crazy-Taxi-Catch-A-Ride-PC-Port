from pathlib import Path
import json,subprocess
root=Path('outputs/crazy-taxi-native')
audit=json.loads((root/'reports/bound-first-frame-overlay-timer-audit.json').read_text());assert audit['status']=='SOURCE_TIMER_RETURN_ABI_TEXT_PASS'
expected=bytes.fromhex(audit['output_hex']);cp=subprocess.run([str(root/'bin/bound-first-frame-timer-native-compare.exe')],capture_output=True)
passed=cp.returncode==0 and len(expected)==9 and cp.stdout==expected
report=dict(status='NATIVE_ORIGINAL_TIMER_TEXT_PASS' if passed else 'NATIVE_TIMER_DEPENDENCY',native_exit=cp.returncode,original_hex=expected.hex(),native_hex=cp.stdout.hex(),scope='Existing sharednativechallengeHUD plan versus actual2083C/10988 formatter ninebytes for retained2400ticks. Doesnotverifyactivecountdown/glyphsubmission orwholeHUD.')
(root/'reports/bound-first-frame-native-timer-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
