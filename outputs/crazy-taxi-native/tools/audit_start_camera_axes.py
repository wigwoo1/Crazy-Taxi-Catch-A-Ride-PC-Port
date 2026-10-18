exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x14eca:0x14ee2],0x8014eca))
assert [(i.mnemonic,i.op_str) for i in ins]==[('ldr','r2, [pc, #0x2c]'),('ldr','r1, [r2, #4]'),('movs','r0, #0xea'),('lsls','r0, r0, #1'),('muls','r0, r1, r0'),('str','r0, [r3]'),('movs','r3, #0xee'),('lsls','r3, r3, #3'),('adds','r1, r7, r3'),('ldr','r0, [r2, #8]'),('lsls','r0, r0, #0xa'),('str','r0, [r1]')]
assert struct.unpack_from('<I',r,0x14ef8)[0]==0x02001674
pathlib.Path('outputs/crazy-taxi-native/reports/start-camera-axis-owner.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'bytes':r[0x14eca:0x14ee2].hex(),'height':'468 * axes[1]','distance':'axes[2] << 10','scope':'Source preset ownership; full camera mode/offset and bootstrap/facing still qualified.'},indent=2))
print('PASS source startup axes own camera height/distance presets.')
