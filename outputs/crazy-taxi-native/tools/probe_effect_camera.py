exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM);lines=[]
for i in arm.disasm(r[0xa1c:0xb78],0x8000a1c):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-camera-matrix-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
