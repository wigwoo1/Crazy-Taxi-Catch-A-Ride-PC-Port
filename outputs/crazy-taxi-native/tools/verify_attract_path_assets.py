exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
result=[]
for city,scene in [('city1',0x7556cc),('city2',0x755734)]:
 count=struct.unpack_from('<H',r,scene+16)[0];table=struct.unpack_from('<I',r,scene+52)[0]-0x8000000
 for i in range(count):
  q=table+i*20
  if r[q+4:q+16].split(b'\0')[0]!=b'PATH_AI':continue
  n=struct.unpack_from('<I',r,q)[0];pointer=struct.unpack_from('<I',r,q+16)[0]
  raw=r[pointer-0x8000000:pointer-0x8000000+n*16]
  asset=pathlib.Path(f'outputs/crazy-taxi-native/assets/ai/{city}-path.cta').read_bytes()
  assert asset==b'CTA1'+struct.pack('<I',n)+raw
  result.append({'city':city,'source_descriptor':hex(q+0x8000000),'points':n,'source_points':hex(pointer),'native_asset_sha256':hashlib.sha256(asset).hexdigest(),'byte_exact':True})
import json
pathlib.Path('outputs/crazy-taxi-native/reports/attract-path-asset-verification.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
