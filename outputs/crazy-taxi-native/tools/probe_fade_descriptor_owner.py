"""Fade descriptor owner candidates; source hash checked, pools DATA."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM hash verified. Candidate contexts include literal pools; no runtime/frame owner claim.']
for start,end in [(0x16250,0x162ac),(0x16390,0x163d4),(0x1654e,0x165cc)]:
 lines.append(f'CONTEXT {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
row=struct.unpack_from('<7I',r,0x49d560+5*28)
lines.append('TITLE state5 descriptor DATA: '+','.join(hex(v) for v in row))
lines.append(f'state5 masks +0xc/+0xe={struct.unpack_from("<2H",r,0x49d560+5*28+12)}')
pathlib.Path('outputs/crazy-taxi-native/reports/fade-descriptor-owner-source.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
