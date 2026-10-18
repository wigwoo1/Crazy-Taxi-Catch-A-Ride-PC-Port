exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Static direct-BL candidates; callsite control flow requires validation.']
for off in range(0,0x34000-4,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 delta=((a&2047)<<12)|((b&2047)<<1)
 if delta&0x400000:delta-=0x800000
 if off+4+delta!=0x554c:continue
 lines.append(f'554C caller {off:06x}')
 for i in md.disasm(r[off-20:off+12],0x8000000+off-20):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/attract-ai-controller-callers.txt').write_text('\n'.join(lines));print('\n'.join(lines))

