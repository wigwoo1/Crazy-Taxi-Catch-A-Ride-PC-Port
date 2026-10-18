"""Results BG palette owner and background verification; ROM read only."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
lines=['ROM SHA verified. BL callers static candidates; pools qualified DATA.']
for off in range(0x5778,0x5e80,2):
 ins=list(md.disasm(r[off:off+4],0x8000000+off))
 if len(ins)!=1 or ins[0].mnemonic!='bl' or ins[0].op_str!='#0x800e388':continue
 lines.append(f'BG palette registration call {off:x}')
 for i in md.disasm(r[off-24:off+4],0x8000000+off-24):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
b=0x3436c;palette=struct.unpack_from('<I',r,b+16)[0]-0x8000000;mp=struct.unpack_from('<I',r,b)[0]-0x8000000
values=struct.unpack_from('<256H',r,palette)
tiles=pathlib.Path('outputs/crazy-taxi-native/assets/results/background.tiles').read_bytes();rgba=pathlib.Path('outputs/crazy-taxi-native/assets/results/background.rgba').read_bytes();assert len(rgba)==240*160*4
zero=0
for y in range(160):
 for x in range(240):
  entry=struct.unpack_from('<H',r,mp+2*((y//8)*30+x//8))[0];tile=entry&1023
  u=(7-x%8) if entry&1024 else x%8;v=(7-y%8) if entry&2048 else y%8
  index=tiles[tile*64+v*8+u];color=values[index];zero+=index==0
  expected=bytes([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255])
  assert rgba[(y*240+x)*4:(y*240+x+1)*4]==expected
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),palette=hex(palette+0x8000000),palette0=values[0],zero_index_pixels=zero,pixels_verified=38400,qualification='ROM map/palette and existing extracted tile buffer exact. Does not independently re-decode compressed tiles or prove all later palette mutations.')
p=pathlib.Path('outputs/crazy-taxi-native/reports/results-background-palette-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
lines.append(f'PASS38400 BG pixels; original palette0={values[0]:04x}, zero-index pixels{zero}.')
pathlib.Path('outputs/crazy-taxi-native/reports/results-background-palette-source.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines))
