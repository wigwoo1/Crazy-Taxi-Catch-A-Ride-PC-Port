"""Frame blend update contexts; hash verified; literal pools remain DATA."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Original frame26318 calls EE10,27CD0,28EE0. These contexts are static evidence, not runtime proof.']
for start,end in [(0xee10,0xee9c),(0x27cd0,0x27e68),(0x28ee0,0x28fe4),(0xf490,0xf570),(0xf51e,0xf570),(0xea74,0xeac4),(0xeabc,0xeac4)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-blend-frame-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
