"""Compare full captured original traction RAM; reward playback is outside scope."""
import hashlib,json,pathlib,subprocess,sys,tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
# Exact complete user ROM identity, independent of probe metadata.
ROM_SHA='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
REGIONS={'0x2002634':0xe88,'0x30053d0':16,'0x30053c0':4,'0x2001534':0x980,'0x20025d4':4,'0x200262c':8,'0x202c4f0':16384,'0x20304f0':16384}
ORDER=['0x2002634','0x2001534','0x20025d4','0x200262c','0x202c4f0','0x20304f0']
def validate_capture(a):
 if a.get('status')!='ORIGINAL_TRACTION_RETURN_ABI_PASS':raise ValueError('Completed original traction return required')
 b,z=a['before'],a['after']
 if len(b)!=16 or len(z)!=16 or b[0]!=0x2002634 or b[13]!=z[13] or b[4:12]!=z[4:12]:raise ValueError('Invalid original calling convention')
 if type(a.get('instructions')) is not int or a['instructions']<=0:raise ValueError('Missing original instruction count')
 states=[]
 for field in ['state_before','state_after']:
  if set(a[field])!=set(REGIONS):raise ValueError('Incomplete captured regions')
  state={k:bytes.fromhex(a[field][k]) for k in REGIONS}
  if any(len(state[k])!=n for k,n in REGIONS.items()):raise ValueError('Incorrect captured region size')
  states.append(state)
 for k in ['0x30053d0','0x30053c0','0x202c4f0','0x20304f0']:
  if states[0][k]!=states[1][k]:raise ValueError('Unexpected original input write: '+k)
 return states

def verify(path,rom_path):
 raw=path.read_bytes();a=json.loads(raw);before,after=validate_capture(a)
 rom=rom_path.read_bytes()
 if len(rom)!=8388608 or hashlib.sha256(rom).hexdigest()!=ROM_SHA:raise ValueError('Original ROM identity mismatch')
 table=rom[0x7f09ee:0x7f0bf0];assert len(table)==514
 payload=b''.join(before[k] for k in ORDER)+table
 binary=ROOT/'bin/retained-traction-compare.exe'
 with tempfile.TemporaryDirectory(prefix='taxi-traction-') as tmp:
  p=pathlib.Path(tmp);(p/'in').write_bytes(payload)
  run=subprocess.run([str(binary),str(p/'in'),str(p/'out')],check=True,capture_output=True)
  native=(p/'out').read_bytes()
 expected=b''.join(after[k] for k in ORDER)+table
 diff=[i for i in range(min(len(native),len(expected))) if native[i]!=expected[i]]
 report={'audit_sha256':hashlib.sha256(raw).hexdigest(),'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'rom_sha256':ROM_SHA,'instructions':a['instructions'],'bytes_compared':len(expected),'native_bytes':len(native),'native_reward_request':run.stdout.decode().strip(),'scope':'One captured traction call RAM only. Reward side effects, complete movement, driving and gameplay remain unverified.'}
 if diff or len(native)!=len(expected):
  report.update(status='NATIVE_RETAINED_TRACTION_STATE_MISMATCH',different_byte_count=len(diff),first_differences=[{'payload_offset':i,'original':expected[i],'native':native[i]} for i in diff[:32]])
  (ROOT/'reports/native-retained-traction-mismatch.json').write_text(json.dumps(report,indent=2));raise ValueError('Native traction RAM mismatch')
 report['status']='NATIVE_RETAINED_TRACTION_STATE_PASS'
 (ROOT/'reports/native-retained-traction-comparison.json').write_text(json.dumps(report,indent=2));print(report['status'])
if __name__=='__main__':verify(pathlib.Path(sys.argv[1]),pathlib.Path(sys.argv[2]))
