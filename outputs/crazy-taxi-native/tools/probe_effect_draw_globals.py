"""Hash-verified source references for effect drawing globals; no execution."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Static Thumb literal references; candidate contexts may contain DATA.',
       '02002b78 = actor02002634+544 (speed), not contact phase.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4
 value=struct.unpack_from('<I',r,p)[0]
 if value!=0x02005f20:continue
 lines.append(f'REFERENCE {off:08x} literal {value:08x}')
 for i in md.disasm(r[max(0,off-12):off+32],0x8000000+max(0,off-12)):
  lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
lines.append('Qualified entry contexts (not continuation through intervening pools):')
for start,end in [(0xb6ec,0xb74c),(0x11490,0x114f0),(0x2277c,0x22796)]:
 lines.append(f'ENTRY {start:08x}..{end:08x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
lines.append('ROM material preceding-record backing bytes: source signed index -1; runtime table alias still must be established.')
for name,header in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
 material=struct.unpack_from('<I',r,header+0x1c)[0]-0x8000000
 assert material>=28
 lines.append(f'{name} materialbase {material+0x8000000:08x}, preceding record {r[material-28:material].hex()}, flags+20 {r[material-8]}')
pathlib.Path('outputs/crazy-taxi-native/reports/effect-draw-globals-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
