exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
md.skipdata=True
lines=['Locator disassembly with skip-data enabled; pool/data lines NOT executable evidence.']
for start,end in [(0xdb60,0xde64),(0xac38,0xac62)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  if i.size==2:
   h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
   if h&0xf800==0x4800:
    p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-playback-locator.txt').write_text('\n'.join(lines));print('\n'.join(lines))
