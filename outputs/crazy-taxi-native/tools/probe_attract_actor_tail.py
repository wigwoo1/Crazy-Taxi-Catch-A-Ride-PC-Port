"""Hash verified post-vehicle actor owner contexts."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Actor contexts; pools/data are not executable claims. 1488c begins a different routine; 145e4 ends1485c.']
for start,end in [(0x145e4,0x1485e),(0x20d2c,0x20fbc),(0x20fd0,0x21078),(0x21078,0x211f8),(0x21220,0x214ba),(0x214f8,0x21792),(0x208d4,0x20a18)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-actor-tail-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
