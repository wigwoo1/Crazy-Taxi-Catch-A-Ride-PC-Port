"""Hash verified effect callback21958 source; pools are data."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Effect callback contexts; disassembly beyond branches/pools needs qualification.']
for start,end in [(0x217d4,0x218ac),(0x218ac,0x21958),(0x21958,0x21a04),(0x21a04,0x21aac),(0x21ab0,0x21b58),(0x21b5c,0x21c04)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-callback-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))



