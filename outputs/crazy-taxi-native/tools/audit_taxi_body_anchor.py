"""Source-bound taxi body/shadow anchor evidence; not a live GBA camera oracle."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
expected={0x155ce:('str','r2, [r6]'),0x155d2:('bl','#0x8029b44'),0x155d6:('subs','r0, #0xc0'),0x155d8:('str','r0, [r4]'),0x15636:('bl','#0x8031a50'),0x15640:('ldr','r2, [r7, #4]'),0x15642:('subs','r2, #0xc0'),0x15644:('str','r2, [sp, #0x18]'),0x1565c:('subs','r2, r2, r0'),0x1565e:('str','r2, [r6, #4]'),0x156ca:('ldr','r1, [r6, #8]'),0x156d4:('subs','r1, r1, r0'),0x15752:('str','r0, [r1, #4]'),0x15814:('bl','#0x8028658')}
checked={}
for off,want in expected.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
 checked[hex(i.address)]=' '.join(want)
for off,want in [(0x155be,0xe8c),(0x155ca,0xe88),(0x15656,0x49c)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==want
result={'rom_sha256':hashlib.sha256(r).hexdigest(),'checked_instructions':checked,'finding':'155D2 ground-query minus192 feeds retained E88/E8C/E90 point and 31A50 shadow submission. At15640 vehicle Y from offset4 minus192 replaces stack body anchor; body Y subtracts view origin49C, rotates and projects, then supplies original28658 sprite call. Native body anchor vehicle Y minus192 agrees with this source path. Changing body anchor to ground Y would incorrectly conflate body and shadow.','limits':'Static bounded instructions and literals, external calls opaque. Does not prove running camera offsets, exact GBA framing, depth-derived sprite scale or complete renderer fidelity.'}
pathlib.Path('outputs/crazy-taxi-native/reports/taxi-body-anchor-audit.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS original taxi body vehicle-Y minus192 distinguished from retained ground shadow anchor; live camera matching still unresolved.')
