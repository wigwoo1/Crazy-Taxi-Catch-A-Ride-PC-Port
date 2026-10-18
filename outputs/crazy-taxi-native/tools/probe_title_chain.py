exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
for n in range(9):
 row=struct.unpack_from('<7I',r,0x49d560+28*n)
 print(n,[hex(v) for v in row])
for start,end in [(0x5778,0x57d0),(0xf5e8,0xf634)]:
 for i in md.disasm(r[start:end],0x8000000+start):print(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
