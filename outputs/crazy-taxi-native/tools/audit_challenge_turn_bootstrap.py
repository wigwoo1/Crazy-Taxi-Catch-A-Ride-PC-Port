exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0xabc8:('bl','#0x8014a58'),0xabcc:('bl','#0x800b6ec'),0xabd4:('bl','#0x80227ac'),0x22836:('cmp','r0, #0x80'),0x2283a:('b','#0x8022986'),0x22986:('movs','r2, #1'),0x22988:('strb','r2, [r7]'),0x22a28:('cmp','r0, #0x80'),0x22a2c:('b','#0x8022c2a'),0x22c4c:('subs','r0, r1, #3'),0x22c52:('cmp','r0, #2'),0x22c56:('cmp','r1, #7'),0x22c5c:('movs','r1, #0x96'),0x22c5e:('lsls','r1, r1, #2'),0x22c60:('strh','r1, [r7, #0xc]'),0x22c76:('bl','#0x8029b44'),0x22c7a:('str','r0, [r7, #0x14]')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
assert struct.unpack_from('<I',r,0x4a4fdc+16)[0]==0x087ebd30
assert struct.unpack_from('<3H',r,0x7ebd30+14)==(1,5,1)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'finding':'ABC4 calls original227AC. Challenge firstpass skips ordinary route resolution and setsactor flag1; secondpass22C2A initializeskind/position/animation/billboard, modes3/4/5/7 counter600, original29B44 queried using retainedactorY. Original Turn has1actor/5path entries/1destination; onePED route has1point and does not justify ordinary route eligibility.','scope':'Exact local opcode/header guards, external calls opaque. Native course bootstrap tests use explicit prioractorY0, not verified whole original reset scheduling.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-turn-bootstrap-source.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS original challenge initialization bypass/retainedheight/600animationcounter and Turn source header.')
