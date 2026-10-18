"""Static ROM evidence; excludes a projection dispatch hypothesis, not a live camera oracle."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={}
for off,want in [(0x2ad8e,0x03006940),(0x2ad90,0x080029fc),(0x2c784,0x03006810)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 value=struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0];assert value==want
 checks[hex(off+0x08000000)]=hex(value)
assert [(i.mnemonic,i.op_str) for i in md.disasm(r[0x2c786:0x2c78a],0x802c786)]==[('str','r0, [r2, #0x2c]'),('str','r1, [r2, #0x30]')]
from capstone import Cs,CS_ARCH_ARM,CS_MODE_ARM
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
body=[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in arm.disasm(r[0x29fc:0x2ce8],0x80029fc)]
result={'rom_sha256':hashlib.sha256(r).hexdigest(),'literal_bindings':checks,'division_body':body,'finding':'Projection callback06940 binds original signed integer division29FC. Setter2C784 writes near/far2C/30, not focal28; it does not replace192 focal after viewport setup. These hypotheses do not explain cropped taxi. Native mode2 bootstrap/current offsets and view composition still require independent source execution evidence. Provisional PC pullback is not original fidelity proof.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-projection-dispatch-audit.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS exact ROM division binding and near/far setter opcode/literal checks; live camera mismatch unresolved.')
