exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
expected={
(0x9aec,0x9afc):[('mov','r1, r8'),('ldr','r0, [r1]'),('asrs','r4, r0, #8'),('movs','r3, #0xff'),('ands','r0, r3'),('movs','r5, #0x64'),('muls','r0, r5, r0'),('asrs','r6, r0, #8')],
(0x9b42,0x9b4e):[('asrs','r1, r5, #0x18'),('movs','r2, #0x80'),('lsls','r2, r2, #0x11'),('adds','r0, r5, r2'),('lsrs','r0, r0, #0x18'),('str','r0, [sp]')],
(0x9c7a,0x9c7e):[('bl','#0x800a96a')],
(0xa96a,0xa97c):[('ldr','r0, [sp]'),('add','sp, #4'),('pop','{r3, r4, r5}'),('mov','r8, r3'),('mov','sb, r4'),('mov','sl, r5'),('pop','{r4, r5, r6, r7}'),('pop','{r1}'),('bx','r1')]
}
rows=[]
for (a,b),want in expected.items():
 got=[(i.mnemonic,i.op_str) for i in md.disasm(r[a:b],0x8000000+a)];assert got==want,(hex(a),got)
 rows.append({'start':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':got})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'Distance fraction floor(lowbyte*100/256); insertion position+1 stored stack;9C7A calls sharedA96A epilogue restoring caller return, no unlock callback or fallthrough. Return rank0..3.','scope':'Exact distance conversion, rank store and terminal return; other challenge modes and all table writes not audited here.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-distance-return-audit.json').write_text(json.dumps(report,indent=2))
print('PASS source distance conversion and terminal rank return; A96A is shared epilogue, not unlock.')
