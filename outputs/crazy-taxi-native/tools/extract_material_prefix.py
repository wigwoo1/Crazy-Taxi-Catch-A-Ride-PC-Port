"""Preserve signed material-index -1 backing data without executable ROM."""
import pathlib,hashlib,struct,json
from extract_scene import EXPECTED,BASE,SCENES
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/material-prefix');out.mkdir(exist_ok=True)
records=[]
for name,header in SCENES.items():
 address=struct.unpack_from('<I',rom,header-BASE+28)[0]
 offset=address-BASE-28
 assert 0<=offset<=len(rom)-28
 data=rom[offset:offset+28]
 (out/(name+'.bin')).write_bytes(data)
 records.append(dict(scene=name,material_base=hex(address),signed_index=-1,
                     source=hex(address-28),size=28,flags=data[20],sha256=hashlib.sha256(data).hexdigest()))
 (out/(name+'.bin')).read_bytes()==data or (_ for _ in ()).throw(AssertionError('write mismatch'))
(out/'provenance.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,records=records,
 qualification='Original preceding ROM bytes. Not a normal material record; only source signed-index -1 reads qualify. Initializer2b44c copies scene header; palette helpers2c558/2b0b0 retain material pointer. Native viewer uses only flags+20 for signed index -1. Later mode-specific header mutations remain outside this ordinary setup audit.'),indent=2)+'\n')
print('Preserved and re-read three 28-byte material-prefix records; runtime consumer pending.')
