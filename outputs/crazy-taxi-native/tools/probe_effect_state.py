import pathlib,hashlib,struct,sys
sys.path.insert(0,'work/python-packages')
from capstone import *
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);lines=['Static candidates; context may include literal pools, not execution proof.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 v=struct.unpack_from('<I',r,p)[0]
 if v not in (0x0200356a,0x02003570):continue
 lines.append(f'\nLITERAL candidate {off:x} value {v:x}')
 for i in md.disasm(r[max(0,off-8):off+24],0x8000000+max(0,off-8)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
for off in range(0x16500,0x166d0,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xff00==0xb500:lines.append(f'ENTRY candidate PUSH {off:x}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-state-reference-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
