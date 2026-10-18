exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Display mode switch25c18: table is DATA; each selected target decoded independently.']
for mode in (3,4):
 target=struct.unpack_from('<I',r,0x25c44+mode*4)[0]
 lines.append(f'MODE {mode} target {target:08x}')
 start=target-0x08000000
 for i in md.disasm(r[start:start+112],target):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-background-display-mode-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
