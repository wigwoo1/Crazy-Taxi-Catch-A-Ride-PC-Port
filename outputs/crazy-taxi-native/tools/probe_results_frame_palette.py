"""Per-frame palette processing contexts; ROM hash verified."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Static EE10 original-palette loop contexts; literal DATA annotated.']
for start,end in [(0xeebc,0xef3e),(0xefa0,0xf000),(0xf060,0xf0c0),(0xf11a,0xf162),(0xf168,0xf1d0)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/results-frame-palette-source.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
