"""Compare eleven original shared CB84 scalar owners with production native code."""
import hashlib,json,subprocess
from pathlib import Path
root=Path('outputs/crazy-taxi-native')
path=root/'reports/active-held-input/trip-start-owner-call.json'
c=json.loads(path.read_text())
if 'input' not in c:raise SystemExit('PENDING: exact shared trip input capture not available')
rng=json.loads((root/'reports/retained-trip-start-rng-replay.json').read_text())
assert rng['seed']==c['rng_before'] and rng['after']==c['rng_after'], 'Replay original RNG for new seed before comparing'
def signed(v):return v-4294967296 if v&2147483648 else v
def state(a):
 out=list(a);pointer=out[0]
 if pointer:
  assert 0x02005f2c<=pointer<0x02005f2c+128*44 and (pointer-0x02005f2c)%44==0
  out[0]=(pointer-0x02005f2c)//44
 else:out[0]=-1
 for i in range(1,5):out[i]=signed(out[i])
 return out
inp=c['input'];values=state(c['before'])+[inp['customer_index'],signed(inp['route_length']),signed(inp['remaining']),rng['result'],inp['category'],inp['settings0'],inp['settings3']]
exe=root/'bin/retained-trip-start-owner-compare.exe'
p=subprocess.run([str(exe)],input=' '.join(map(str,values))+'\n',text=True,capture_output=True,check=True)
out=list(map(int,p.stdout.split()));expected=state(c['after']);assert len(out)==12 and out[:11]==expected,(out,expected)
assert out[11]==int(inp['settings0']&224!=128)
report=dict(match=True,native=out[:11],original=expected,palette_request=out[11],input=inp,random_result=rng['result'],original_instructions=c['instructions'],scope='All eleven actual captured shared original CB84 scalar owners vs production taxi_trip_start using exact captured inputs and separately replayed original RNG. Customer pointers normalized to native indices. No palette DMA/audio/full-frame gameplay proof.',sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [path,root/'reports/retained-trip-start-rng-replay.json',exe,root/'src/trip_start.c']})
(root/'reports/retained-shared-trip-start-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS eleven shared trip scalar owners')
