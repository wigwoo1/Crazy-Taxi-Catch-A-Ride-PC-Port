exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
for i in md.disasm(r[0xd764:0xd7e0],0x800d764):
 row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
 if i.size==2:
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
 print(row)
