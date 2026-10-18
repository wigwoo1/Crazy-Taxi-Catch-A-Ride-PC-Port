exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Validated spans and static direct BL candidates; no call absence claim.']
for start,end in [(0x162f0,0x16340),(0x16370,0x163d4),(0x166b8,0x1671c),(0x16734,0x167b6)]:
 lines.append(f'\nSPAN {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' ; literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&2047)<<12)|((b&2047)<<1)
 if d&0x400000:d-=0x800000
 if off+4+d==0x166b8:lines.append(f'DIRECT BL166b8 candidate {off:x}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-producer-ownership-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
