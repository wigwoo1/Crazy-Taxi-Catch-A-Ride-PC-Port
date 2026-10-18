"""Hash-verified loading-service mode gates and attract container metadata."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Branch contexts; embedded pool words are data.']
for start,end in [(0xd764,0xd7b0),(0xd898,0xd8b8),(0x227ac,0x22878),(0x229c6,0x22a38)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
base=0x5a338
lines.append('DATA attract container first16words: '+str(struct.unpack_from('<16I',r,base)))
for index in range(11):
 g=base+struct.unpack_from('<I',r,base+4+index*4)[0]
 header=struct.unpack_from('<5I',r,g)
 lines.append(f'attract group{index} header={header} address={g+0x08000000:08x}')
normal=0x70e50
for index in (1,7,8):
 a=base+struct.unpack_from('<I',r,base+4+index*4)[0]
 normal_index=1 if index==7 else index
 b=normal+struct.unpack_from('<I',r,normal+4+normal_index*4)[0]
 ha=struct.unpack_from('<5I',r,a);hb=struct.unpack_from('<5I',r,b)
 da=[struct.unpack_from('<9H',r,base+struct.unpack_from('<I',r,a+20+j*4)[0]) for j in range(ha[0])]
 db=[struct.unpack_from('<9H',r,normal+struct.unpack_from('<I',r,b+20+j*4)[0]) for j in range(hb[0])]
 lines.append(f'attractgroup{index}/normalgroup{normal_index} header_match={ha==hb} descriptor_match={da==db}, headers={ha}/{hb}')
pathlib.Path('outputs/crazy-taxi-native/reports/attract-loading-service-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
