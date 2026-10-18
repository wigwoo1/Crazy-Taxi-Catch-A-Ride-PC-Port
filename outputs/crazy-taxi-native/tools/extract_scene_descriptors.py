"""Preserve the three original104-byte scene descriptors; readonly ROM."""
from pathlib import Path
import hashlib,json
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
out=Path('outputs/crazy-taxi-native/assets/scene-descriptors');out.mkdir(exist_ok=True);records=[]
for name,off in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
 blob=rom[off:off+104];(out/(name+'.bin')).write_bytes(blob)
 records.append(dict(scene=name,rom_address=hex(0x08000000+off),sha256=hashlib.sha256(blob).hexdigest()))
(out/'manifest.json').write_text(json.dumps(dict(rom_sha256=hashlib.sha256(rom).hexdigest(),descriptors=records),indent=2)+'\n')
print('Preserved three exact originalscene descriptors')
