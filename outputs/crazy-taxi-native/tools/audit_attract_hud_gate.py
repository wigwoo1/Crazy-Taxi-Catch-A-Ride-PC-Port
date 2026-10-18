"""Qualify original demo exclusion of ordinary end-of-frame HUD."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xaf7a,0xaf7e,[('ldr','r2, [pc, #0xdc]'),('mov','sb, r2')]),(0xb400,0xb40e,[('mov','r2, sb'),('ldrb','r0, [r2]'),('cmp','r0, #0'),('beq','#0x800b468'),('bl','#0x800d8c8'),('b','#0x800b46c')]),(0xb570,0xb57c,[('mov','r6, sb'),('ldrb','r0, [r6]'),('cmp','r0, #0'),('bne','#0x800b57c'),('bl','#0x801f0e8')])]
checks.append((0xb48a,0xb492,[('mov','r1, sb'),('ldrb','r0, [r1]'),('cmp','r0, #0'),('bne','#0x800b540')]))
checks.append((0xb51e,0xb522,[('bl','#0x8031ef8')]))
assert struct.unpack_from('<I',r,0xb058)[0]==0x02001708
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'SB retains02001708 attract flag. Enabled demo choosesD8C8 overlay and skips1F0E8 ordinary HUD.','scope':'Direct caller gate; individual auxiliary panels, simulation countdown and all other mode presentation require separate ownership qualification.'}
pathlib.Path('outputs/crazy-taxi-native/reports/attract-hud-gate-audit.json').write_text(json.dumps(report,indent=2));print('PASS original attract overlay/ordinaryHUD exclusion gate.')
