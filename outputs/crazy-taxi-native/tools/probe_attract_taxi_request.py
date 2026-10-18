"""Hash verified taxi rendering entry and mode-specific request ownership."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Rendering contexts; literal pools and embedded data are not executable claims.']
for start,end in [(0x1556c,0x15644),(0x15640,0x15840),(0x15860,0x15910),(0x15910,0x159c8),(0x159f0,0x15ac0),(0xaf76,0xafd0),(0xafb2,0xaff2),(0xba04,0xba64),(0xb2a0,0xb33c),(0xb33c,0xb36a)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-taxi-request-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
