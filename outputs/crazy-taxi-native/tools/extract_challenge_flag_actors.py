from pathlib import Path
import sys,json,struct,hashlib
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from extract_passengers import extract,write_asset
from extract_scene import SCENES
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
SCENES['challenge1']=struct.unpack_from('<I',rom,0x4a4f7c+24+16)[0]
result=extract(rom,'challenge1')
out=Path('outputs/crazy-taxi-native/assets/challenge-worlds/challenge1-actors.ctp')
write_asset(result,out)
result['status']='Original scene+48 dynamic actor records, not scene+44 special billboards; native gameplay initialization pending.'
result['asset_sha256']=hashlib.sha256(out.read_bytes()).hexdigest()
Path('outputs/crazy-taxi-native/reports/challenge-flag-actors.json').write_text(json.dumps(result,indent=2)+'\n')
print('Original dynamic actors',len(result['pedestrians']),'routes',len(result['routes']),'destinations',len(result['destinations']))
print(json.dumps(result['pedestrians'][:3],indent=2))
