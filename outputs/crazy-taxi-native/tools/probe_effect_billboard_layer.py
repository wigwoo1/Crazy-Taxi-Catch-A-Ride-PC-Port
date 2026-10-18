"""Original billboard layer ownership; Thumb producers and ARM raster writes."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Hash verified. Literal pools are DATA; entry-qualified billboard contexts.']
for start,end in [(0x31a50,0x31b80),(0x31ba4,0x31cc0),(0x31cf0,0x31de0),(0x31df6,0x31e80),(0x31e80,0x31f00),(0x3201c,0x320b0),(0x31cbe,0x31cf0),(0x2b6a0,0x2b82c),(0x2b82c,0x2b86c),(0x2b8ca,0x2b974)]:
 lines.append(f'THUMB {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
for start,end in [(0x49f8,0x4bac),(0x4bac,0x4d00)]:
 lines.append(f'ARM {start:x}..{end:x}')
 for i in arm.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  if i.address==0x08004afc: row+=f' raw_pool_word_runtime_owner_unresolved={struct.unpack_from(chr(60)+chr(73),r,0x49ec)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-billboard-layer-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
