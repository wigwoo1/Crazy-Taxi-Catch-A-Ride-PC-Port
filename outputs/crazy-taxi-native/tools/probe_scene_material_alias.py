"""Source scene loading contexts, hash verified, literal values annotated."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Scene loader candidate contexts; pools remain data, not executed instructions.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 if struct.unpack_from('<I',r,p)[0]!=0x03006810:continue
 reg='r'+str((h>>8)&7)
 context=list(md.disasm(r[off:off+64],0x8000000+off))
 if not any(i.mnemonic=='str' and i.op_str.endswith(f'[{reg}]') for i in context):continue
 lines.append(f'CANDIDATE scene-global store via{reg}, entryunknown {off:x}')
 for i in md.disasm(r[max(0,off-20):off+64],0x8000000+max(0,off-20)):
  lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
for start,end in [(0x2b44c,0x2b524),(0x1e7f0,0x1e862),(0x2b0b0,0x2b1a0),(0x2b1e0,0x2b298),(0x2b2a0,0x2b320),(0x2c558,0x2c654),(0x2c5bc,0x2c654),(0x2c664,0x2c686),(0x2b1a0,0x2b1e0),(0x2b298,0x2b2a0)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/scene-material-alias-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
