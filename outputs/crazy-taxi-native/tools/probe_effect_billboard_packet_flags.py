"""Hash verified billboard packet flags; branch-qualified entries, no CPU execution."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM SHA256 verified. Static source; branch-qualified ranges; known literal pools omitted.']
for start,end in [(0x31b80,0x31b8e),(0x31c84,0x31cc8),(0x31df6,0x31e16),(0x31e20,0x31e86),(0x3201c,0x32022)]:
 lines.append(f'ENTRY {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-billboard-packet-flags-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
