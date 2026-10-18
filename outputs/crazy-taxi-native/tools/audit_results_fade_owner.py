"""Results fade owner ROM audit; exact instructions and literal evidence."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
rows=[]
for call,mode,owner in [(0x23996,1,'results initialization'),(0x24036,0,'results timer greater than300')]:
 ins=list(md.disasm(r[call-8:call+4],0x8000000+call-8))
 assert [(i.mnemonic,i.op_str) for i in ins]==[('movs',f'r0, #{mode}'),('movs','r1, #0x20'),('movs','r2, #1'),('movs','r3, #1'),('bl','#0x800e5a8')]
 rows.append(dict(call=hex(call+0x8000000),mode=mode,endpoint=32,bg_mask=1,obj_mask=1,owner=owner))
checks=[(0x24026,0x2402e,[('movs','r2, #0x96'),('lsls','r2, r2, #1'),('cmp','r0, r2'),('ble','#0x802403e')]),(0x2403e,0x24042,[('bl','#0x8026318')])]
for start,end,expected in checks:
 assert [(i.mnemonic,i.op_str) for i in md.disasm(r[start:end],0x8000000+start)]==expected
lines=['ROM SHA256 verified. Results entry/exit contexts; literal references annotated as DATA.']
for start,end in [(0x2392c,0x239b0),(0x24000,0x24072)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),calls=rows,qualification='Exact fade arguments and adjacent timer comparison verified; full render/audio/global owner fidelity remains separate.')
p=pathlib.Path('outputs/crazy-taxi-native/reports/results-fade-owner-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
pathlib.Path('outputs/crazy-taxi-native/reports/results-fade-owner-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines));print('PASS results fade-in/out exact ROM call arguments: mode1/0, endpoint32, BG/OBJ1/1.')
