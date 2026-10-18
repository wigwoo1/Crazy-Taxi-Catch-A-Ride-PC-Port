"""Hash verified frame-tail control flow; pools remain explicitly data."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Frame-tail source contexts, not an original runtime execution trace.']
for start,end in [(0xafd4,0xb056),(0xb0a4,0xb168),(0xb1a0,0xb27c),(0xb27c,0xb33c),(0xb33c,0xb3b0),(0xb560,0xb5f8),(0xb630,0xb694)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-frame-tail-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
