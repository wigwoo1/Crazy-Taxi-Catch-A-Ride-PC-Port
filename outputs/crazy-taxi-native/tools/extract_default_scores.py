"""Reconstruct original0801004a..b6 default leaderboard from ROM."""
import pathlib,struct,hashlib,json
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5';assert hashlib.sha256(r).hexdigest()==expected
records=bytearray(r[0x4a54c4:0x4a54c4+80]);rows=[]
for i in range(10,100):records.extend(b'FGL\0'+struct.pack('<I',(5000-55*(i-10))<<15))
assert len(records)==800
for i in range(100):
 z=struct.unpack_from('<I',records,i*8+4)[0]
 rows.append(dict(index=i,name_bytes=records[i*8:i*8+3].hex(),raw_word=hex(z),whole=z>>15,fraction=(z>>8)&127))
assert all(rows[i]['whole']>=rows[i+1]['whole'] for i in range(99))
o=pathlib.Path('outputs/crazy-taxi-native/assets/results')
(o/'default-scores.cts').write_bytes(b'CTS1'+struct.pack('<I',100)+records)
(o/'default-scores.json').write_text(json.dumps(dict(rom_sha256=expected,first_ten_rom='084a54c4',initialization='0801004a..080100b6',runtime='020022b0',format='CTS1 count100 plus original8byte records; following90 are source initializer FGL/5000-55*(index-10)',records_sha256=hashlib.sha256(records).hexdigest(),rows=rows),indent=2)+'\n')
print([(z['index']+1,z['whole'],z['fraction']) for z in rows[:11]]);print('last',rows[-1])
