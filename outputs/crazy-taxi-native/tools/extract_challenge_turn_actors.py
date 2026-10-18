from pathlib import Path
import sys,json,struct,hashlib
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from extract_passengers import extract,write_asset
from extract_scene import SCENES,EXPECTED
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
address=struct.unpack_from('<I',rom,0x4a4fdc+16)[0];assert address==0x087ebd30
SCENES['challenge4']=address
result=extract(rom,'challenge4')
for row in result['pedestrians']+result['destinations']:
 off=int(row['address'],16)-0x08000000;raw=bytes.fromhex(row['raw_hex']);assert rom[off:off+len(raw)]==raw
out=Path('outputs/crazy-taxi-native/assets/challenge-worlds/challenge4-actors.ctp');write_asset(result,out)
result['status']='ROM source records/routes/destinations only; native eligibility/ground ownership requires separate checks.'
result['asset_sha256']=hashlib.sha256(out.read_bytes()).hexdigest()
Path('outputs/crazy-taxi-native/reports/challenge-turn-actors.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS Turn original source actors',len(result['pedestrians']),'routes',len(result['routes']),'destinations',len(result['destinations']))
print([(p['kind'],p['destination_index'],p['position_xyz'],p['matching_route_indices']) for p in result['pedestrians']])
