import pathlib,hashlib
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
s=r[0x49d624:0x49d624+43];assert r[0x49d624+43]==0
pathlib.Path('outputs/crazy-taxi-native/assets/results/name-characters.ctn').write_bytes(b'CTN1'+s)
print('Extracted43 original name-entry characters including sourceENDmarker.')

import struct
positions=struct.unpack_from('<16h',r,0x7ec614)
slots=[struct.unpack_from('<2i',r,0x7ec634+i*12) for i in range(3)]
pathlib.Path('outputs/crazy-taxi-native/assets/name-entry/layout.cnl').write_bytes(b'CNL1'+struct.pack('<16h',*positions)+b''.join(struct.pack('<2i',*slot) for slot in slots))
print('Source ring positions',positions,'entry slots',slots)
