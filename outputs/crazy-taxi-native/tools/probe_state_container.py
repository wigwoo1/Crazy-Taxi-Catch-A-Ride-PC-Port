exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for start,end in [(0x16200,0x162ac),(0x27fb8,0x28030)]:
 lines.append(f'RANGE {start:x}..{end:x} candidate context; pools not executable evidence')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
for p in range(0,0x34000,4):
 if struct.unpack_from('<I',r,p)[0]==0x080166b9:lines.append(f'callback pointer candidate {p:x}')
pathlib.Path('outputs/crazy-taxi-native/reports/state-container-and-copy-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
