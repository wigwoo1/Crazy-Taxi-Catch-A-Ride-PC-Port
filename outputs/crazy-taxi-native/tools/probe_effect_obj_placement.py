"""Original OBJ producer contexts; hash verified, literal islands are DATA."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['28658 source placement candidates, not runtime execution evidence.']
for start,end in [(0x28658,0x28800),(0x28800,0x28900),(0x28966,0x28a14),(0x289d0,0x28a14),(0x28a14,0x28ac0),(0x28ac0,0x28b94),(0x28bbc,0x28bd0),(0x28288,0x283f0)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-obj-placement-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
