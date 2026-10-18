"""Results font registration source contexts; SHA verified, DATA qualified."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM SHA verified. Literal values are DATA; static registration contexts only.']
for start,end in [(0xfec8,0xff18),(0xf8e0,0xf982),(0xf98c,0xfa38),(0x28288,0x28330),(0x2839a,0x283ec),(0x28408,0x28416),(0x28420,0x28448),(0x28450,0x28468),(0x2846c,0x28498),(0x2849c,0x284b8),(0x284b8,0x284d6),(0x284dc,0x284f2),(0x284f8,0x28562),(0x28574,0x28580),(0x28580,0x2862e)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/results-font-registration-source.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
