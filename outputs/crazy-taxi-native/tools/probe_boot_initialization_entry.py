exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for offset in (0x1cc,0x1d0):
 v=struct.unpack_from('<I',r,offset)[0];lines.append(f'BOOT literal {offset:x}={v:08x}')
 if 0x08000000<=v<0x08034000:
  start=(v&~1)-0x8000000
  mode=Cs(CS_ARCH_ARM,CS_MODE_THUMB if v&1 else CS_MODE_ARM)
  for i in mode.disasm(r[start:start+180],0x8000000+start):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/boot-initialization-entry-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
