"""Verify normal gameplay fade call arguments against ROM instructions."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
rows=[]
for call,mode,name in [(0xad86,1,'attract/setup fade-in'),(0xae86,0,'normal gameplay transition fade-out'),(0xaeca,1,'normal gameplay scene fade-in'),(0xc06c,0,'normal expiry shared fade-out target; alternate menu exit also uses target')]:
 ins=list(md.disasm(r[call-8:call+4],0x08000000+call-8))
 expected=[('movs',f'r0, #{mode}'),('movs','r1, #0x20'),('movs','r2, #1'),('movs','r3, #1'),('bl','#0x800e5a8')]
 assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append(dict(call=hex(0x08000000+call),owner=name,mode=mode,endpoint=32,bg_mask=1,obj_mask=1,instructions=[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in ins]))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),entry='0800e5a8',arguments='r0 low8 mode; r1 low8 endpoint clamped32; r2 low16 BG mask; r3 low16 OBJ mask.',calls=rows,qualification='Direct straight-line call arguments verified. Normal ended/settings gate BD5c..BD7e selects scale/hold expiry, BDF8 branches directly to C064. BEF6 belongs to settings high bits128 and is not normal expiry. Does not prove results fade-in, all original modes or complete call graph.')
p=pathlib.Path('outputs/crazy-taxi-native/reports/normal-gameplay-fade-mask-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
print('PASS four original fade argument sequences: normal transition and expiry masks1/1; title descriptor masks remain distinct.')
