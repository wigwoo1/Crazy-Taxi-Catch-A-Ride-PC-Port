exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&2047)<<12)|((b&2047)<<1)
 if d&0x400000:d-=0x800000
 if off+4+d!=0x21c10:continue
 lines.append(f'RESET CALL CANDIDATE {off:x}')
 for i in md.disasm(r[off-8:off+8],0x8000000+off-8):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-reset-callers.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
