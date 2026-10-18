exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Hash verified blend setup contexts; literal islands are DATA.']
for start,end in [(0x25c18,0x25ce0),(0x26318,0x26468),(0x263a0,0x26440),(0x263d0,0x26440),(0x263f0,0x26440),(0xaf5c,0xaf7a),(0x5778,0x57a8)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
for addr in (0x08034264,):
 a,b=struct.unpack_from('<HH',r,addr-0x08000000+0x80)
 lines.append(f'Header {addr:08x}: +80={a} +82={b}, not proof of gameplay owner')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-blend-setup-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
