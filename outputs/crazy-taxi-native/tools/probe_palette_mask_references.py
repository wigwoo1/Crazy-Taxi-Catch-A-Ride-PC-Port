"""ROM palette-mask reference candidates; not runtime proof."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM hash verified. Literal reference candidates; surrounding DATA is not executed code proof.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 value=struct.unpack_from('<I',r,p)[0]
 if value not in (0x02001786,0x02001788):continue
 lines.append(f'CANDIDATE {off:x} pointer={value:08x}')
 for i in md.disasm(r[max(0,off-16):off+28],0x08000000+max(0,off-16)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/palette-mask-reference-candidates.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
