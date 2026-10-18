"""Verify non-affine startup frame anchoring in28288."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x28304:0x2831e],0x8028304))
expected=[('mov','r5, sp'),('lsls','r1, r1, #0x10'),('asrs','r1, r1, #0x10'),('ldr','r0, [r6]'),('subs','r0, r0, r1'),('str','r0, [sp]'),('lsls','r2, r2, #0x10'),('asrs','r2, r2, #0x10'),('ldr','r0, [r6, #4]'),('subs','r0, r0, r2'),('str','r0, [sp, #4]'),('ldr','r0, [r6, #8]'),('str','r0, [sp, #8]')]
assert [(i.mnemonic,i.op_str) for i in ins]==expected
base=0x70e50;group=base+struct.unpack_from('<I',r,base+4+7*4)[0];frames=[]
for frame in range(3):
 descriptor=base+struct.unpack_from('<I',r,group+20+4*frame)[0]
 metrics=struct.unpack_from('<9H',r,descriptor)
 assert metrics[:4]==(4,4,0,0)
 frames.append({'frame':frame,'descriptor':hex(descriptor+0x8000000),'metrics':metrics})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'anchor_source_bytes':r[0x28304:0x2831e].hex(),'instructions':expected,'frames':frames,'conclusion':'28288 subtracts signed descriptor offset_x/offset_y; original group7 offsets both zero, so startup submitted x/y are top-left before later display-mode adjustment.','scope':'Descriptor subtraction only. Later display mode adjustments/OAM clipping/wrapping and phase2 affine scaling remain distinct.'}
pathlib.Path('outputs/crazy-taxi-native/reports/start-art-anchor-audit.json').write_text(json.dumps(report,indent=2))
print('PASS group7 original32x32 descriptors offsets0/0 and signed anchor subtraction.')
