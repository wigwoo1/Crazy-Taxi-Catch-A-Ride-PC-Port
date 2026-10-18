"""Verify original ordinary/alternate camera yaw limit owner."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0x1530e,0x15320,[('ldr','r0, [r7, #0x20]'),('asrs','r0, r0, #0xc'),('ldr','r1, [pc, #0x24]'),('adds','r4, r7, r1'),('ldr','r1, [r4]'),('asrs','r1, r1, #0xc'),('bl','#0x802dd5c'),('adds','r3, r0, #0')]),(0x15320,0x1532c,[('ldr','r0, [pc, #0x18]'),('ldrb','r0, [r0]'),('lsls','r0, r0, #0x18'),('asrs','r0, r0, #0x18'),('cmp','r0, #0'),('beq','#0x801535c')])]
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
for address,value in [(0x15338,0x654),(0x1533c,0x02001696),(0x15344,1991),(0x1536c,853)]:assert struct.unpack_from('<I',r,address)[0]==value
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'ordinary_limit':853,'alternate_limit':1991,'selector':'signed byte02001696, not attract1708','heading_source':'vehicle+0x20 ASR12','camera_heading':'vehicle+0x654 ASR12','scope':'Exact yaw follow inputs and limit selector. Current ordinary hostalternate0 agrees only while1696zero; source phase1696 branch and bootstrap offsets remain independently required.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-yaw-limit-owner-audit.json').write_text(json.dumps(report,indent=2));print('PASS original camera follows heading20; ordinary853/alternate1991 selected by1696, not demo1708.')
