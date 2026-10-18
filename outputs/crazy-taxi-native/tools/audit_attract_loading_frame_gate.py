"""Verify demo loader skips synchronous frame service; no runtime inference."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xd78c,0xd796,[('ldr','r0, [pc, #0x124]'),('ldrb','r7, [r0]'),('cmp','r7, #0'),('beq','#0x800d796'),('b','#0x800d89c')]),(0xd898,0xd8a8,[('bl','#0x8026318'),('add','sp, #0x20'),('pop','{r3}'),('mov','r8, r3'),('pop','{r4, r5, r6, r7}'),('pop','{r0}'),('bx','r0')])]
assert struct.unpack_from('<I',r,((0xd78c+4)&~3)+0x124)[0]==0x02001708
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'Enabled demo atD794 branches directly toD89C epilogue, bypassingD898 synchronous26318 call.','consequence':'Do not add52 demo prompt-counter increments merely because native loader presents52 work frames. Native loading audio/render pacing is a host policy; IRQ audio can continue independently and requires separate timing qualification.','scope':'Exact local branch proof; all other loader26318 calls and fullIRQ scheduling remain separate.'}
pathlib.Path('outputs/crazy-taxi-native/reports/attract-loading-frame-gate-audit.json').write_text(json.dumps(report,indent=2));print('PASS original attract loader callback bypasses synchronous frame service.')
