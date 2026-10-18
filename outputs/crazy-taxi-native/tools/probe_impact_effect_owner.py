exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Impact callback/allocator reference candidates; pools need qualification.']
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 target=0
 if a&0xf800==0xf000 and b&0xf800==0xf800:
  d=((a&2047)<<12)|((b&2047)<<1)
  if d&0x400000:d-=0x800000
  target=off+4+d
 literal=0
 if a&0xf800==0x4800:
  p=((off+4)&~3)+(a&255)*4;literal=struct.unpack_from('<I',r,p)[0]
 if target!=0x21c30 and literal!=0x08021b5d:continue
 lines.append(f'CANDIDATE {off:x} target={target:x} literal={literal:x}')
 for i in md.disasm(r[off-18:off+44],0x8000000+off-18):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/impact-effect-owner-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))

