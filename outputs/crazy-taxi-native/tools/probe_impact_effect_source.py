exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for target in (0x08021b5d,0x08021c31):
 for off in range(0,len(r)-4,2):
  if struct.unpack_from('<I',r,off)[0]==target:lines.append(f'RAW DATA POINTER {off:x}={target:x}')
for start,end in [(0x21c5c,0x21d60)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/impact-effect-owner-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
