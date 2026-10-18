from pathlib import Path
import json,math,subprocess
root=Path('outputs/crazy-taxi-native')
rows=json.loads((root/'reports/bound-second-frame-sqrt-calls.json').read_text())
assert rows
values={0,1,2,3,4,65535,65536,2147483647,2147483648,4294967295}
values.update(row['input'] for row in rows)
for k in (255,256,32767,32768,65535):
 values.update((k*k-1,k*k,k*k+1))
for value in sorted(values):
 result=subprocess.run([str(root/'bin/retained-frame-integer-root-compare.exe'),str(value)],check=True,capture_output=True,text=True)
 assert int(result.stdout)==math.isqrt(value),(value,result.stdout)
for row in rows:assert row['result']==math.isqrt(row['input'])
report={'status':'NATIVE_RETAINED_FRAME_INTEGER_ROOT_CONTRACT_PASS','observed_calls':len(rows),'values_checked':len(values),'scope':'Native unsigned floor square-root mathematical contract, observed source arguments plus boundaries; supplied ROM veneer identity separately qualified. BIOS absent: no BIOS register clobber or timing equivalence claim.'}
(root/'reports/retained-frame-integer-root-native-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(report)
