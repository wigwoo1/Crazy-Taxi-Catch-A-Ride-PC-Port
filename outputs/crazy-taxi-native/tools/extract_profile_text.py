"""Source English profile labels440..443, hash checked and ROM read-only."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
base=0x49dfe0;blob=bytearray(b'CPR1');rows=[]
for i in range(440,444):
 offset=struct.unpack_from('<I',r,base+4+i*4)[0];assert offset
 a=base+offset;text=r[a:r.index(0,a)]
 assert 0<len(text)<32 and all(c>=32 for c in text)
 blob.extend(struct.pack('<HH',i,len(text))+text)
 rows.append(dict(id=i,address=hex(a+0x8000000),text=text.decode('latin1')))
p=pathlib.Path('outputs/crazy-taxi-native/assets/profile')
(p/'text.cpr').write_bytes(blob)
(p/'text.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,entries=rows),indent=2)+'\n')
print('Extracted four original profile labels.')
