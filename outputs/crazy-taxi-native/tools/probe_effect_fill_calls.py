exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Static fill-call candidates; validate control flow and literal pools.']
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&2047)<<12)|((b&2047)<<1)
 if d&0x400000:d-=0x800000
 if off+4+d!=0x1e784:continue
 lines.append(f'FILL CALL {off:x}')
 for i in md.disasm(r[max(0,off-40):off+8],0x8000000+max(0,off-40)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-fill-call-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
