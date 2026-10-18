exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
expected={
(0xac68,0xac6c):[('movs','r1, #1'),('mov','sl, r1')],
(0xaece,0xaee0):[('ldr','r6, [pc, #0x58]'),('ldrb','r0, [r6]'),('cmp','r0, #0'),('beq','#0x800af5c'),('mov','r0, sl'),('ldr','r1, [pc, #0x68]'),('str','r0, [r1]'),('movs','r2, #0'),('str','r2, [r1, #4]')],
(0xaf5c,0xaf64):[('mov','r3, sl'),('ldr','r5, [pc, #0xec]'),('str','r3, [r5]'),('str','r3, [r5, #4]')]
}
rows=[]
for (a,b),want in expected.items():
 got=[(i.mnemonic,i.op_str) for i in md.disasm(r[a:b],0x8000000+a)]
 assert got==want,(hex(a),got)
 rows.append({'start':hex(0x8000000+a),'bytes':r[a:b].hex(),'instructions':got})
for off,value in [(0xaece,0x02001708),(0xaed8,0x020034d4),(0xaf5e,0x020034d4)]:
 h=struct.unpack_from('<H',r,off)[0];pool=((off+4)&~3)+(h&255)*4
 assert struct.unpack_from('<I',r,pool)[0]==value
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'AC68 defines SL=1. AECE reads demo1708: zero branches AF5C which writes SL to actor and human counts; nonzero writes SL actor count and zero human count. Camera selector later forces demo human count1.','scope':'Exact local instructions and literals; register preservation across intervening calls and whole boot reachability remain unproven.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-human-boot-audit.json').write_text(json.dumps(report,indent=2))
print('PASS camera human-count boot branch literals and local instructions; intervening-call preservation remains qualified.')
