"""Find blend register owners; candidates include DATA and require caller audit."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Hash verified ROM. Thumb literal references to blend registers or display base; static candidates only.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800: continue
 p=((off+4)&~3)+(h&255)*4
 v=struct.unpack_from('<I',r,p)[0]
 if v not in (0x04000050,0x04000052,0x04000054,0x04000000): continue
 lines.append(f'\nREFERENCE {off:x} literal {v:08x}')
 start=max(0,off-16); end=min(len(r),off+64)
 for i in md.disasm(r[start:end],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  q=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if q&0xf800==0x4800:
   at=((i.address-0x08000000+4)&~3)+(q&255)*4
   row+=f' literal={struct.unpack_from("<I",r,at)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-blend-owner-candidates.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
