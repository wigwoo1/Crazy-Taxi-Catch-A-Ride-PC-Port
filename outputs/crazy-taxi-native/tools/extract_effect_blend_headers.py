"""Preserve original display header blend halfwords, not scene world headers."""
import pathlib,hashlib,struct,json
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
out=pathlib.Path('outputs/crazy-taxi-native/assets/effects')
rows=[]
for address in (0x080340d8,0x0803415c,0x080341e0,0x08034264,0x080342e8,0x0803436c,0x080344f8):
 data=rom[address-0x08000000+0x80:address-0x08000000+0x84]
 a,b=struct.unpack('<HH',data)
 rows.append(dict(header=hex(address),offsets=[128,130],coefficient_evb=a,coefficient_eva=b,source_sha256=hashlib.sha256(data).hexdigest()))
assert all(row['coefficient_evb']==8 and row['coefficient_eva']==8 for row in rows)
manifest=dict(rom_sha256=expected,records=rows,qualification='Display header5778 copies+80 to020000e0 (BLDALPHA high byte EVB), +82 to020000e2 (low byte EVA). Restore26440 uses BLDCNT1f4f and these retained globals. Header addresses from qualified direct loader callers;3415c is contiguous title index1 candidate. No complete gameplay inheritance or underlying second-target coverage proof; do not select runtime blend from this manifest alone.')
path=out/'blend-header-provenance.json';path.write_text(json.dumps(manifest,indent=2)+'\n')
assert json.loads(path.read_text())==manifest
print('Verified seven original display header coefficient pairs, each8/8. Runtime owner remains qualified.')
