exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for p in range(0,len(r)-4,4):
 if struct.unpack_from('<I',r,p)[0] in (0x080166b9,0x08016251,0x0801624d):
  lines.append(f'POINTER candidate {p:x}:'+r[max(0,p-24):p+28].hex())
for start,end in [(0x16250,0x162ac),(0x28038,0x280b8)]:
 lines.append(f'RANGE {start:x}..{end:x} candidate')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/state-specific-copy-ownership.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
