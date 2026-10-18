"""Verify guidance entry and normal gameplay wrapper exclusion in demo."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0x206bc,0x206c0,[('push','{r4, r5, r6, lr}'),('sub','sp, #4')]),(0x1f060,0x1f06a,[('push','{r4, r5, r6, lr}'),('bl','#0x80206bc'),('bl','#0x802083c')]),(0xafc2,0xafcc,[('ldrb','r0, [r4]'),('cmp','r0, #0'),('bne','#0x800afd4'),('bl','#0x801f060')])]
# AFA6 R4 holds original attract flag; this is not an arbitrary driver flag.
h=struct.unpack_from('<H',r,0xafa6)[0];assert h&0xf800==0x4800
assert struct.unpack_from('<I',r,((0xafa6+4)&~3)+(h&255)*4)[0]==0x02001708
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'Guidance206BC entry,2072A is internal upload site. Normal1F060 wrapper skipped bymainAFC6 whenattract1708enabled.','scope':'Main gameplay wrapper gate only; other callers8B1A/8D70/9070/9404/972C belong to additional flows needing independent semantic qualification.'}
pathlib.Path('outputs/crazy-taxi-native/reports/attract-guidance-gate-audit.json').write_text(json.dumps(report,indent=2));print('PASS original main-loop demo guidance exclusion and wrapper entry.')
