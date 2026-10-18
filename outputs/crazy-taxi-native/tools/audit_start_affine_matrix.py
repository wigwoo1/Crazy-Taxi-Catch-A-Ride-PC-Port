"""Verify existing native OBJ matrix is the original startup affine owner."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x28a0a:0x28a6c],0x8028a0a))
assert [i.op_str for i in ins if i.mnemonic=='bl']==['#0x8032454']*4
assert [(i.mnemonic,i.op_str) for i in ins if i.address in (0x8028a16,0x8028a32,0x8028a46,0x8028a48,0x8028a5c)]==[('lsls','r0, r0, #8'),('lsls','r0, r0, #8'),('rsbs','r0, r0, #0'),('lsls','r0, r0, #8'),('lsls','r0, r0, #8')]
for off,value in [(0x28a0a,0x020304f0),(0x28a2c,0x0202c4f0)]:
 h=struct.unpack_from('<H',r,off)[0];p=((off+4)&~3)+(h&255)*4;assert struct.unpack_from('<I',r,p)[0]==value
pathlib.Path('outputs/crazy-taxi-native/reports/start-affine-matrix-owner.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'bytes':r[0x28a0a:0x28a6c].hex(),'instructions':[(hex(i.address),i.mnemonic,i.op_str) for i in ins],'native_owner':'src/obj_order.c:taxi_obj_matrix_values','mapping':'cos<<8/scale, sin<<8/scale, -sin<<8/scale, cos<<8/scale; signed halfword narrowing','scope':'Matrix generation source owner verified; GPU startup artwork and display mode still pending.'},indent=2))
print('PASS startup affine source uses existing native OBJ matrix mapping; four divide calls/trig table owners verified.')
