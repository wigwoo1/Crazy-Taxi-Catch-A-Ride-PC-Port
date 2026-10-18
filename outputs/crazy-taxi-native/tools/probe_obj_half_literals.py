import pathlib,struct
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
for p in (0x2dcc4,0x2dcc8,0x2dccc):print(hex(p),hex(struct.unpack_from('<I',r,p)[0]))
