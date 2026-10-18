"""Verify original ordinary initial start scales and hold initializer."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xb928,0xb938,[('ldr','r0, [pc, #0x30]'),('movs','r2, #0xc0'),('lsls','r2, r2, #6'),('str','r2, [r0]'),('movs','r1, #0x80'),('lsls','r1, r1, #5'),('str','r1, [r0, #4]'),('str','r2, [r0, #8]')]),(0xb70a,0xb710,[('movs','r1, #0'),('str','r1, [r0]'),('str','r1, [r2]')]),(0xb73e,0xb744,[('ldr','r0, [pc, #0x11c]'),('movs','r4, #0'),('strh','r1, [r0]')])]
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
for off,value in [(0xb928,0x02001674),(0xb73e,0x020016b6)]:
 h=struct.unpack_from('<H',r,off)[0];assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'ordinary_axes':[12288,4096,12288],'hold':0,'scope':'Ordinarysettings!=128/nonattractB908..938 defaults. DemoB80Aaxes4096 andspecialsettings128axes1024 remain separate branches; completeactor startup not proven.'}
pathlib.Path('outputs/crazy-taxi-native/reports/game-start-defaults-audit.json').write_text(json.dumps(report,indent=2));print('PASS original ordinary start axes12288/4096/12288 and zero hold owner.')
