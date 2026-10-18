exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for address in (0x0803409c,0x080340a4):
 o=address-0x8000000;lines.append(f'String{address:08x}: {r[o:r.index(0,o)].decode("ascii")}')
table=struct.unpack_from('<I',r,0x75579c+12)[0];lines.append(f'Citytable={table:08x}')
for city in range(2):
 o=table-0x8000000+city*104;count=struct.unpack_from('<H',r,o+16)[0];records=struct.unpack_from('<I',r,o+52)[0]
 lines.append(f'city{city} descriptor={o+0x8000000:08x} count={count} records={records:08x}')
 for index in range(count):
  q=records-0x8000000+index*20
  lines.append(f'entry{index}: {r[q:q+20].hex()} name={r[q+4:q+20].split(bytes([0]))[0]!r}')
pathlib.Path('outputs/crazy-taxi-native/reports/attract-city-path-table.txt').write_text('\n'.join(lines));print('\n'.join(lines))
