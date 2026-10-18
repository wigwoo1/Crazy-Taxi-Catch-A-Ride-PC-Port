"""Original English registration block results labels and ordinals; ROM read-only."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
base=0x49dfe0;count=struct.unpack_from('<I',r,base)[0]
ids=[176,182,188,194]+list(range(331,437))+[439]
blob=bytearray(b'CRT1'+struct.pack('<I',len(ids)));rows=[]
for i in ids:
 assert i<count
 a=base+struct.unpack_from('<I',r,base+4+i*4)[0]
 z=r.index(0,a);s=r[a:z]
 assert 0<len(s)<32 and all(32<=c<256 for c in s)
 blob.extend(struct.pack('<HH',i,len(s)));blob.extend(s)
 rows.append(dict(id=i,address=hex(a+0x8000000),text=s.decode('latin1'),bytes_hex=s.hex()))
o=pathlib.Path('outputs/crazy-taxi-native/assets/results')
(o/'text.crt').write_bytes(blob)
(o/'text.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,block='0849dfe0',registration='0801e202..0801e208',entries=rows),indent=2)+'\n')
assert rows[-2]['text']=='99th'
print('Extracted',len(ids),'original results labels, driver names and rank ordinals.')

a=base+struct.unpack_from('<I',r,base+4+330*4)[0];text=r[a:r.index(0,a)]
pathlib.Path('outputs/crazy-taxi-native/assets/ranking/game-over.got').write_bytes(b'GOT1'+bytes([len(text)])+text)

ids=[176,182,188,194]+list(range(337,438))
blob=bytearray(b'CRT1'+struct.pack('<I',len(ids)));rows=[]
for i in ids:
 a=base+struct.unpack_from('<I',r,base+4+i*4)[0];text=r[a:r.index(0,a)]
 assert 0<len(text)<32 and all(c>=32 for c in text)
 blob.extend(struct.pack('<HH',i,len(text)));blob.extend(text)
 rows.append(dict(id=i,address=hex(a+0x8000000),text=text.decode('latin1')))
o=pathlib.Path('outputs/crazy-taxi-native/assets/name-entry')
(o/'text.crt').write_bytes(blob)
(o/'text.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,entries=rows),indent=2)+'\n')
print('Name entry prompt:',rows[-1]['text'])
