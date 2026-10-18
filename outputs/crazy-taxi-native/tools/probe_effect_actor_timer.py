exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[];arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
for start,end in [(0,4),(0xc0,0x180)]:
 for i in arm.disasm(r[start:end],0x8000000+start):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf8ff!=0x20f4:continue
 lines.append(f'TIMER OFFSET CANDIDATE {off:x}')
 for i in md.disasm(r[max(0,off-8):off+28],0x8000000+max(0,off-8)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-actor-timer-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
