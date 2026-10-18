"""Compare native command packing with captured original retained state only."""
import argparse, hashlib, json, pathlib, subprocess
parser=argparse.ArgumentParser()
parser.add_argument('audit',type=pathlib.Path)
args=parser.parse_args()
root=pathlib.Path('outputs/crazy-taxi-native')
audit=json.loads(args.audit.read_text())
assert audit['status']=='ORIGINAL_PLAYER_COMMAND_RETURN_ABI_PASS'
assert audit['command_address']=='0x030053d0'
sampler=bytes.fromhex(audit['sampler_hex'])
before=bytes.fromhex(audit['command_before_hex'])
expected=bytes.fromhex(audit['command_after_hex'])
assert len(sampler)==66 and len(before)==len(expected)==16
binary=root/'bin/retained-player-command-compare.exe'
values=list(sampler[6:16])+list(before)
result=subprocess.run([str(binary)],input=' '.join(map(str,values))+'\n',text=True,capture_output=True,check=True,timeout=10)
actual=bytes(map(int,result.stdout.split()))
assert actual==expected,{'native':actual.hex(),'original':expected.hex()}
report={'status':'NATIVE_RETAINED_COMMAND_BYTES_PASS','audit':str(args.audit),'audit_sha256':hashlib.sha256(args.audit.read_bytes()).hexdigest(),'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'command_before_hex':before.hex(),'command_after_hex':actual.hex(),'scope':'All16 native command bytes versus the captured original retained call, using its actual ten held bytes and initial command. Not full driving or physical Xbox verification.'}
(root/'reports/native-retained-player-command-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report))
