"""Hash verified palette flag references; candidates are not runtime traces."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['02001770 Thumb literal candidates; contexts may contain DATA.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 if not 0x02001760<=struct.unpack_from('<I',r,p)[0]<=0x0200177c:continue
 lines.append(f'REFERENCE {off:x}')
 for i in md.disasm(r[max(0,off-8):off+34],0x8000000+max(0,off-8)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  instruction=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if instruction&0xf800==0x4800:
   pool=((i.address-0x8000000+4)&~3)+(instruction&255)*4
   row+=f' literal={struct.unpack_from("<I",r,pool)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/palette-flag-owners-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
