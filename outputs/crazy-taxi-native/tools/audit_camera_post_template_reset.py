exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x11a1e:0x11a24],0x8011a1e));assert [(i.mnemonic,i.op_str) for i in ins]==[('adds','r0, r6, #0'),('bl','#0x8011540')]
pathlib.Path('outputs/crazy-taxi-native/reports/camera-post-template-reset-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'bytes':r[0x11a1e:0x11a24].hex(),'source':'11804 template initializer final11A20 calls11540 reset after1184A template copy','conclusion':'Camera654/650/64C must use post-template reset values. Prior template-zero rationale was incomplete; template values are overwritten by reset.'},indent=2))
print('PASS original post-template11540 reset owner; native startup exports final camera initialization.')
