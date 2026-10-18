import pathlib,hashlib,sys,struct
sys.path.insert(0,'work/python-packages')
from capstone import *
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
lines=[]
for start,end in [(0x166d0,0x1671c),(0x16734,0x1673a),(0x28288,0x282e0),(0x1673a,0x167b6),(0x2b820,0x2b838),(0x2b840,0x2b852),(0x2b85c,0x2b880)]:
 lines.append(f'CODE {start:x}..{end:x}; bounded before known literal pools')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' ; resolved literal {struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/world-billboard-hud-order-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))



