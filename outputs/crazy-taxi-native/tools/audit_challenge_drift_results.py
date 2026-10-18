exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0x1ff1c:('cmp','r4, #3'),0x1ff26:('ldrsh','r2, [r0, r4]'),0x1ff2a:('bl','#0x801e420'),0x2010a:('cmp','r6, #3'),0x2010e:('ldr','r2, [r2, #0x6c]'),0x20110:('lsrs','r2, r2, #0xf'),0x20116:('bl','#0x801e420')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want
for off,value in [(0x1ff20,0x0849d6d8),(0x1ff22,0x0200164c),(0x20114,0x0849d6d8)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
assert r[0x49d6d8:0x49d6db]==b'%d\x00'
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-results-source.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'current_result':'0801FF1C..0801FF2E signed164C formatted by original1E420 with %d at0849D6D8','record_result':'0802010A..0802011A packedgroup144+208profileword>>15 with same %d','registration':'Existing recovered sharedmode128 profilecommit now dispatches native9AB0submode3 snapshot recalculate; no new diskIO','scope':'Exact local formatter opcode/literals; originalprintf1E420 dependency is opaque, host decimal snprintf used. Native fixtures do not prove full GBA fidelity/earned Drift.'},indent=2))
print('PASS original Drift signed current/unsigned decoded record %d requests and exact ROM format bytes.')
