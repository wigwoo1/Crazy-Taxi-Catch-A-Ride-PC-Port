exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 if struct.unpack_from('<I',r,p)[0]!=0x0200261c:continue
 lines.append('CANDIDATE %08x'%(0x8000000+off))
 for i in md.disasm(r[max(0,off-8):off+24],0x8000000+max(0,off-8)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/camera-target261c-candidates.txt').write_text('\n'.join(lines));print('\n'.join(lines))
