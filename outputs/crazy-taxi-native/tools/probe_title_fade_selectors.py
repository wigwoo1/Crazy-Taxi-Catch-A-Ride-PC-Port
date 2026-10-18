exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Locator contexts only; literal pools and tables are data, not executable evidence.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 v=struct.unpack_from('<I',r,p)[0]
 if v not in (0x02007e2e,0x02001786,0x02001788):continue
 lines.append(f'\nREFERENCE {off:x} => {v:08x}')
 for i in md.disasm(r[max(0,off-20):off+42],0x8000000+max(0,off-20)):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  q=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if q&0xf800==0x4800:
   a=((i.address-0x8000000+4)&~3)+(q&255)*4;row+=f' literal={struct.unpack_from("<I",r,a)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/title-fade-selector-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
