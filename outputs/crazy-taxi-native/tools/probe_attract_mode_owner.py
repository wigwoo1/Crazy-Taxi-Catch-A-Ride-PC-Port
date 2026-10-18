exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Locator contexts only; pools/tables are not executable instructions.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4;v=struct.unpack_from('<I',r,p)[0]
 if v not in (0x02001708,):continue
 lines.append(f'REF {off:x} => {v:08x}')
 for i in md.disasm(r[max(0,off-12):off+36],0x8000000+max(0,off-12)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/attract-mode-owner-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))



