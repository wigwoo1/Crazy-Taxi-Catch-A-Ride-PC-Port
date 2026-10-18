"""Verify original demo packed score consumers; no emulation or ROM writes."""
import pathlib, hashlib, json
root=pathlib.Path(__file__).resolve().parents[1]
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
sha=hashlib.sha256(r).hexdigest()
assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert r[0xdb06:0xdb0e]==bytes.fromhex('09784906890f0331')
assert r[0xda84:0xda92]==bytes.fromhex('0168c90b0f4bd01802785206520e')
# DB08/0A left shift25 then logical right30 in 32-bit registers.
for byte in range(256):
 source=(((byte<<25)&0xffffffff)>>30)+3
 assert source==3+((byte>>5)&3)
# DA8E/90 left shift25 then right25 keeps seven fractional bits.
for byte in range(256):
 assert (((byte<<25)&0xffffffff)>>25)==(byte&127)
for driver in range(4):
 for license in range(16):
  for profile in range(2):
   for fraction in range(128):
    word=(12345<<15)|(fraction<<8)|(profile<<7)|(driver<<5)|(license<<1)|1
    assert word>>15==12345
    assert (word>>8)&127==fraction
    assert (word>>5)&3==driver
report={'rom_sha256':sha,'driver_instruction_bytes':'09784906890f0331','money_instruction_bytes':'0168c90b0f4bd01802785206520e','driver_byte_cases':256,'fraction_byte_cases':256,'packed_roundtrip_cases':16384,'driver_group':'3+((record[4]>>5)&3)','whole':'little_endian_word(record+4)>>15','fraction':'record[5]&127','scope':'Exact source shifts and native score packing compatibility; live score overlay, icon assets and font/OAM placement remain unfinished.'}
(root/'reports/attract-score-packing-audit.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report,indent=2))
