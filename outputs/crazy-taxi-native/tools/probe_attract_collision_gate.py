"""Verify original movement collision gate contexts, including mode0x60."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Collision contexts only; literal pools remain data.']
for start,end in [(0x2a5c8,0x2a6b2),(0x2a328,0x2a3d0),(0x2a3e0,0x2a49c),(0x29e84,0x29f40),(0x29f60,0x2a050),(0x2a060,0x2a160)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-collision-gate-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
