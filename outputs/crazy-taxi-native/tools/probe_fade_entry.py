"""Original fade entry qualification; hash verified, pools remain DATA."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['ROM hash verified. Entry candidates and literal data distinguished; static evidence.']
for i in md.disasm(r[0xe5a8:0xe5f2],0x0800e5a8):
 lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0xf000:continue
 ins=list(md.disasm(r[off:off+4],0x08000000+off))
 if len(ins)!=1 or ins[0].mnemonic!='bl' or ins[0].op_str!='#0x800e5a8':continue
 lines.append(f'CALLER candidate {off:x}; nearby data is not execution proof')
 for i in md.disasm(r[max(0,off-24):off+6],0x08000000+max(0,off-24)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/fade-entry-qualification.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
