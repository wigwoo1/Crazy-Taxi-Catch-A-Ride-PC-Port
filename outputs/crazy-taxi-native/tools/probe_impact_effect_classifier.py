exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for start,end in [(0x2abf4,0x2ad20)]:
 for i in md.disasm(r[start:end],0x8000000+start):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/impact-effect-classifier-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
