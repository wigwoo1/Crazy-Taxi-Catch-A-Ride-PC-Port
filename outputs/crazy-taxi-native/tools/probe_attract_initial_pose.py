"""ROM-hash-verified initialization context; literal pools are data."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Initialization context only; distinguish branches and literal pools before semantic claims.']
for start,end in [(0xacaa,0xad02),(0xad38,0xae66),(0x14a58,0x14bec),(0x14c6a,0x14db6),(0xaef4,0xaf0c),(0xaf5c,0xaf76),(0xfec8,0xff48)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-initial-pose-source.txt').write_text('\n'.join(lines))
asset=pathlib.Path('outputs/crazy-taxi-native/assets/driving.ctr').read_bytes()
assert asset[:4]==b'CTR1' and len(asset)==34504
for index,address in enumerate((0x084a554c,0x084a5794)):
 assert asset[33288+584*index:33288+584*(index+1)]==r[address-0x08000000:address-0x08000000+584]
print('PASS both native vehicle templates match original normal and attract records byte-for-byte.')
print('\n'.join(lines))
