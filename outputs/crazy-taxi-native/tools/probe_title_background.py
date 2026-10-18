exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for start,end in [(0x1e21c,0x1e274),(0x162ca,0x162e6),(0x16390,0x163d4),(0x16648,0x166b8)]:
 lines.append(f'CONTEXT {start:x}..{end:x}; pools require validation')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
for p in range(0,len(r)-4,4):
 if struct.unpack_from('<I',r,p)[0]==0x0801e275:lines.append(f'POINTER1e274 candidate {p:x}')
pathlib.Path('outputs/crazy-taxi-native/reports/title-background-entry-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
