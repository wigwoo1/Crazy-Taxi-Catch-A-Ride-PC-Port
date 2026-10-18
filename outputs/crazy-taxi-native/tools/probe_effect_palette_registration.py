"""Hash-verified effect container palette registration source/data."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Palette registration contexts; literals explicitly annotated, pools DATA.']
for start,end in [(0x27994,0x27aba),(0x27ae0,0x27bb0),(0x27bb0,0x27cd0),(0xe388,0xe3a8),(0xe3ac,0xe3c0),(0xe3c4,0xe430),(0xe438,0xe484),(0xe49c,0xe4e0),(0xe4e0,0xe57c),(0xe588,0xe5a0),(0xad38,0xad80),(0xaf08,0xaf1c)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800!=0x4800:lines.append(row);continue
  p=((i.address-0x8000000+4)&~3)+(h&255)*4
  lines.append(row+f' literal={struct.unpack_from("<I",r,p)[0]:08x}')
for name,root in [('normal',0x70e50),('attract',0x5a338)]:
 count=struct.unpack_from('<I',r,root)[0];bank=0
 lines.append(f'CONTAINER {name} groups{count}')
 for group in range(count):
  header=root+struct.unpack_from('<I',r,root+4+4*group)[0]
  frames,bpp,tiles,start,pals=struct.unpack_from('<5I',r,header)
  palette=header+20+frames*4
  lines.append(f'group{group}: palettebank{bank} count{pals} ROM{palette+0x8000000:08x}')
  bank+=pals
pathlib.Path('outputs/crazy-taxi-native/reports/effect-palette-registration-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
