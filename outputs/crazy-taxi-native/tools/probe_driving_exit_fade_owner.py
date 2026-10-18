"""Recover driving exit fade contexts, preserving literal DATA qualification."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM SHA256 verified. Static Thumb contexts; literal islands are DATA, not executed instructions.']
for start,end in [(0xbd5c,0xbdba),(0xbdd8,0xbdfa),(0xbe04,0xbe24),(0xbe30,0xbe54),(0xbe60,0xbe80),(0xbe8c,0xbeaa),(0xbeb8,0xbf32),(0xbf78,0xbfec),(0xc004,0xc038),(0xc040,0xc072)]:
 lines.append(f'CONTEXT {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal DATA @{p:08x}={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/driving-exit-fade-owner-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
