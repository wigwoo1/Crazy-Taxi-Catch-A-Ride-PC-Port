"""Preserve original scene RGB555 palettes for indexed-renderer checks."""
import hashlib, json, pathlib, struct
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
out=pathlib.Path('outputs/crazy-taxi-native/assets')
rows=[]
for name,scene in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
    address=struct.unpack_from('<I',rom,scene+24)[0]
    offset=address-0x08000000
    assert 0<=offset<=len(rom)-512
    palette=rom[offset:offset+512]
    (out/(name+'-palette.rgb555')).write_bytes(palette)
    rows.append(dict(scene=name,address=hex(address),sha256=hashlib.sha256(palette).hexdigest()))
(out/'scene-palettes.json').write_text(json.dumps(dict(rom_sha256=expected,palettes=rows),indent=2)+'\n')
print('Preserved three original scene palettes')
