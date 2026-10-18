"""Original Flag HUD callback owner references, SHA guarded."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4;v=struct.unpack_from('<I',r,p)[0]
 if not 0x084a4f7c<=v<=0x084a4f90:continue
 lines.append(f'SHA verified table literal at{off:05x}: {v:08x}; local context only')
 for i in md.disasm(r[max(0,off-12):off+42],0x8000000+max(0,off-12)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
Path=pathlib.Path
Path('outputs/crazy-taxi-native/reports/flag-hud-owner-references.txt').write_text('\n'.join(lines))
print('\n'.join(lines))

for start,end in [(0x7934,0x79d4)]:
 lines.append('SHA verified phase2 HUD ordering; bounded straight-line source')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}';off=i.address-0x8000000;h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:row+=f' literal DATA={struct.unpack_from("<I",r,((off+4)&~3)+(h&255)*4)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/flag-hud-owner-references.txt').write_text('\n'.join(lines))

arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
lines.append('SHA verified3407C Thumb BX-PC ->34080 ARM B->04F6C opaque8bpp bitmap copy; data after5094 not disassembled')
for i in arm.disasm(r[0x4f6c:0x5098],0x8004f6c):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/flag-hud-owner-references.txt').write_text('\n'.join(lines))
