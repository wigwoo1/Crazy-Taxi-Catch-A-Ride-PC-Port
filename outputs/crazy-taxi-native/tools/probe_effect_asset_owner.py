exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
for p in range(0x7ebf48,0x7ebf58,4):
 v=struct.unpack_from('<I',r,p)[0];print(hex(p+0x8000000),hex(v),r[v-0x8000000:v-0x8000000+24].hex())
for start,end in [(0x162ac,0x162f0),(0x16328,0x1637a)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  text=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;text+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  print(text)
