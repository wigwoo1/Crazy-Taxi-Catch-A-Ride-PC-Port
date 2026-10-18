exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for start,end in [(0x7378,0x73c0),(0x73c0,0x7410),(0x5778,0x5e80)]:
 lines.append(f'CONTEXT {start:x}..{end:x}; pool fragments not execution evidence')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
lines.append('TITLE DESCRIPTOR:'+r[0x3415c:0x341e0].hex())
pathlib.Path('outputs/crazy-taxi-native/reports/title-scroll-initialization-source.txt').write_text('\n'.join(lines));print('\n'.join(lines))

