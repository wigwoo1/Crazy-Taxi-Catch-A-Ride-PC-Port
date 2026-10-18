exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for i in md.disasm(r[0x153f4:0x1544c],0x80153f4):
 line=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
 o=i.address-0x8000000;h=struct.unpack_from('<H',r,o)[0]
 if h&0xf800==0x4800:
  p=((o+4)&~3)+(h&255)*4;line+=' DATA=%08x'%struct.unpack_from('<I',r,p)[0]
 lines.append(line)
pathlib.Path('outputs/crazy-taxi-native/reports/camera-offset2618-source.txt').write_text('\n'.join(lines));print('\n'.join(lines))
