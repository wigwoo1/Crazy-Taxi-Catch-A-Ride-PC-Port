"""Find original billboard callers, retaining candidate qualification."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM hash verified. BL callers are candidates; surrounding literal data is not execution proof.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0xf000:continue
 ins=list(md.disasm(r[off:off+4],0x08000000+off))
 if len(ins)!=1 or ins[0].mnemonic!='bl' or ins[0].op_str!='#0x8031a50':continue
 lines.append(f'CALL {off:x}')
 for i in md.disasm(r[max(0,off-40):off+6],0x08000000+max(0,off-40)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
for start,end in [(0x1556c,0x155c0),(0x155c0,0x1563a),(0x1ed20,0x1ed70)]:
 lines.append(f'QUALIFICATION {start:x}..{end:x}; pools may be DATA')
 for i in md.disasm(r[start:end],0x08000000+start):
  lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/billboard-callers-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
