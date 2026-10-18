"""Verify startup world dispatcher, excluding literal pool from instructions."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x15ac8:0x15af8],0x8015ac8))
assert len(ins)==23
assert [(i.mnemonic,i.op_str) for i in ins if i.mnemonic=='bl']==[('bl','#0x8011f80')]
assert [(i.mnemonic,i.op_str) for i in ins[8:16]]==[('adds','r6, r1, #0'),('adds','r0, r4, #0'),('adds','r1, r5, #0'),('bl','#0x8011f80'),('adds','r4, #1'),('movs','r0, #0xea'),('lsls','r0, r0, #4'),('adds','r5, r5, r0')]
assert struct.unpack_from('<I',r,0x15afc)[0]==0x020034d4
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'actor_base':hex(struct.unpack_from('<I',r,0x15af8)[0]),'actor_count_address':'0x020034d4','stride':3744,'callee':'0x08011f80','instructions':[(hex(i.address),i.mnemonic,i.op_str) for i in ins],'scope':'World update dispatcher, not camera update. Startup gear update precedes this dispatcher; full startup integration remains pending.'}
pathlib.Path('outputs/crazy-taxi-native/reports/start-world-dispatch-audit.json').write_text(json.dumps(report,indent=2))
print('PASS startup world dispatcher: actor stride3744, live count020034d4, sole call08011f80; no camera call.')
