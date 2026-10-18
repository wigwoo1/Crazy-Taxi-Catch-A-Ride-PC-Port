"""Qualify demo mode1 fade source independently of initial palette DMA flags."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xad7e,0xad8a,[('movs','r0, #1'),('movs','r1, #0x20'),('movs','r2, #1'),('movs','r3, #1'),('bl','#0x800e5a8')]),(0xe466,0xe46a,[('ldr','r1, [sp, #0x20]'),('str','r1, [r0]')]),(0xebd2,0xebdc,[('ldr','r6, [sp, #0x210]'),('ldr','r0, [r6]'),('ldr','r1, [sp, #0x22c]'),('adds','r0, r1, r0'),('ldrh','r1, [r0]')]),(0xebf6,0xebfe,[('ldr','r4, [pc, #0x24]'),('ldrb','r1, [r4]'),('cmp','r1, #2'),('bne','#0x800ec44')]),(0xec44,0xec4a,[('ldr','r0, [sp, #0x20c]'),('cmp','r0, #1'),('bne','#0x800ec98')])]
rows=[]
for start,end,expected in checks:
 ins=list(md.disasm(r[start:end],0x8000000+start));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(start+0x8000000),'bytes':r[start:end].hex(),'instructions':[f'{i.mnemonic} {i.op_str}' for i in ins]})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checked_spans':rows,'finding':'Demo setup AD86 requests mode1/masks1,1. Registration retains original ROM palette pointer. E5A8 mode!=2 bypasses1770 substitution and selects original palette cap path.','scope':'Supports original palette source during this fade; initial DMA, EE10 frame service, post-fade mutations and complete OAM bank state remain separate. Do not color-key blue indices.'}
p=pathlib.Path('outputs/crazy-taxi-native/reports/attract-palette-source-audit.json');p.write_text(json.dumps(report,indent=2));assert json.loads(p.read_text())==report
print('PASS five exact ROM spans: original demo mode1 cap source bypasses mode2-only1770 substitution.')
