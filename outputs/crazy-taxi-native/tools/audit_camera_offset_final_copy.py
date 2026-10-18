"""Verify final original camera target/current overwrite, after smoothing."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
expected=[('ldr','r0, [pc, #0x58]'),('adds','r1, r7, r0'),('movs','r2, #0xed'),('lsls','r2, r2, #3'),('adds','r0, r7, r2'),('ldr','r0, [r0]'),('str','r0, [r1]'),('movs','r3, #0xef'),('lsls','r3, r3, #3'),('adds','r1, r7, r3'),('ldr','r4, [pc, #0x48]'),('adds','r0, r7, r4'),('ldr','r0, [r0]'),('str','r0, [r1]'),('ldr','r5, [pc, #0x38]'),('adds','r1, r7, r5'),('adds','r2, #8'),('adds','r0, r7, r2'),('ldr','r0, [r0]'),('str','r0, [r1]')]
ins=list(md.disasm(r[0x15218:0x15240],0x8015218));assert [(i.mnemonic,i.op_str) for i in ins]==expected
for off,value in [(0x15218,0x774),(0x1522c,0x76c),(0x15234,0x77c)]:
 h=struct.unpack_from('<H',r,off)[0];p=((off+4)&~3)+(h&255)*4;assert struct.unpack_from('<I',r,p)[0]==value
pathlib.Path('outputs/crazy-taxi-native/reports/camera-offset-final-copy-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'bytes':r[0x15218:0x15240].hex(),'instructions':expected,'copies':{'0x774':'0x768','0x778':'0x76c','0x77c':'0x770'},'scope':'Unconditional final target/current copy in14DD8 routine. Earlier quarter-step smoothing overwritten; full path caller gates/mode still separate.'},indent=2))
print('PASS final camera target copies overwrite earlier smoothing:768->774,76c->778,770->77c.')
