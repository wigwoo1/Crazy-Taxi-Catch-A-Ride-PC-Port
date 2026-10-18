import sys,hashlib,pathlib
sys.path.insert(0,'work/python-packages')
from capstone import *
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
s='\n'.join(f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in md.disasm(r[0x2dc78:0x2dd10],0x802dc78))
pathlib.Path('outputs/crazy-taxi-native/reports/obj-half-rotation-source.txt').write_text(s)
print(s)
