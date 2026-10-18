"""Locate original attract identity owners; candidates may include data."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['PC-relative literal candidates only; data/pools are not proven instructions.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 value=struct.unpack_from('<I',r,p)[0]
 if value not in (0x02001690,0x02001840):continue
 lines.append(f'CANDIDATE {off:x} literal {value:08x}')
 for i in md.disasm(r[max(0,off-12):off+40],0x08000000+max(0,off-12)):
  lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
lines.append('DATA084a551c is palette member+8 of four character records084a5514, NOT a camera table:')
import json
taxis=json.loads(pathlib.Path('outputs/crazy-taxi-native/assets/taxis/taxis.json').read_text())
for index in range(4):
 palette=struct.unpack_from('<I',r,0x4a551c+index*12)[0]
 assert palette==int(taxis['characters'][index]['palette'],16)
 lines.append(f'driver{index}: palette{palette:08x}, registered palette address02001790 +8*20 +16=02001840')
pathlib.Path('outputs/crazy-taxi-native/reports/attract-identity-owner-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
