"""Verify original demo actor template and retained-driver palette owner."""
import pathlib,hashlib,struct,json
root=pathlib.Path(__file__).resolve().parents[1]
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
# Attract-enabled direct branch skips normal selection call1cb70.
assert struct.unpack_from('<H',r,0xac04)[0]==0xd118
assert struct.unpack_from('<I',r,0xac30)[0]==0x02001708
# Attract template literal, excluding14ca8..14cab pool from code.
assert struct.unpack_from('<I',r,0x14ca8)[0]==0x084a5794
# Existing extracted native CTR1 templates exactly match both ROM choices.
asset=(root/'assets/driving.ctr').read_bytes();assert len(asset)==34504 and asset[:4]==b'CTR1'
for i,address in enumerate([0x084a554c,0x084a5794]):
 assert asset[33288+i*584:33288+(i+1)*584]==r[address-0x08000000:address-0x08000000+584]
# SourceAEf4 signed driver -> driver*12 + palette-member table +8.
assert struct.unpack_from('<I',r,0xaf50)[0]==0x02001690
assert struct.unpack_from('<I',r,0xaf54)[0]==0x084a551c
assert struct.unpack_from('<I',r,0xaf58)[0]==0x02001840
meta=json.loads((root/'assets/taxis/taxis.json').read_text())
records=[]
for driver in range(4):
 pointer=struct.unpack_from('<I',r,0x4a551c+12*driver)[0]
 assert pointer==int(meta['characters'][driver]['palette'],16)
 records.append({'retained_driver':driver,'original_palette':hex(pointer)})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'native_templates_rom_identical':True,'template_bytes_each':584,'palette_records':records,'scope':'exact template and palette-member owners; normal selection is bypassed; boot retained-driver initialization and complete actor/loading host sequence remain unverified'}
(root/'reports/attract-actor-startup-audit.json').write_text(json.dumps(report,indent=2));print(json.dumps(report))
