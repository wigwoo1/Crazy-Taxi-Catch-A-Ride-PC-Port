from pathlib import Path
import sys,json,struct,hashlib
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from extract_passengers import extract,write_asset
from extract_scene import SCENES
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
SCENES['challenge2']=struct.unpack_from('<I',rom,0x4a4f7c+48+16)[0]
result=extract(rom,'challenge2')
assert len(result['pedestrians'])==20
for actor in result['pedestrians']:
 address=int(actor['address'],16)-0x08000000
 assert rom[address:address+32].hex()==actor['raw_hex']
result['interpretation']='Generic actor format fields; customer_candidate is decoder metadata, not evidence of passenger behavior. Course callback owns balloons.'
out=Path('outputs/crazy-taxi-native/assets/challenge-worlds/challenge2-actors.ctp')
write_asset(result,out)
result['status']='Original scene+48 dynamic actor records, not scene+44 special billboards; native gameplay initialization pending.'
result['asset_sha256']=hashlib.sha256(out.read_bytes()).hexdigest()
Path('outputs/crazy-taxi-native/reports/challenge-ball-actors.json').write_text(json.dumps(result,indent=2)+'\n')
print('Original dynamic actors',len(result['pedestrians']),'routes',len(result['routes']),'destinations',len(result['destinations']))
print(json.dumps(result['pedestrians'][:3],indent=2))
