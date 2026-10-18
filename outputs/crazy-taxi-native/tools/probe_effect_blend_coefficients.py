exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Hash verified. Blend restore entry26440 callers and coefficient references; DATA-qualified candidates.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 match=False
 if h&0xf800==0x4800:
  p=((off+4)&~3)+(h&255)*4
  match=struct.unpack_from('<I',r,p)[0] in (0x020000e0,0x020000e2)
 if h&0xf800==0xf000:
  ins=list(md.disasm(r[off:off+4],0x08000000+off))
  match=match or any(i.mnemonic=='bl' and i.op_str=='#0x8026440' for i in ins)
 if not match: continue
 lines.append(f'\nCANDIDATE {off:x}')
 start=max(0,off-20)
 for i in md.disasm(r[start:off+48],0x08000000+start): lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-blend-coefficient-candidates.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
