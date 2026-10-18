exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
assert r[0x2db54:0x2dbb6].hex()=='30b5184c184b194a194980256d0110880004001508602088000400151860508800040015486060880004001558609088000400158860a088000400159860d08800040015c860e08800040015d8600834103308321031043d002dd8d130bc01bc0047'
assert r[0x86a6:0x86c6].hex()=='124d4119096861430912196010495218104d4019006860430012106080204004'
assert r[0x32038:0x3203c].hex()=='08df7047'
def literal(off):
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 return struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]
expected={0x2db56:0x084a2d08,0x2db58:0x020304f0,0x2db5a:0x084a0d08,0x2db5c:0x0202c4f0,0x86a6:0x0202c4f0,0x86b6:0x020304f0}
assert all(literal(k)==v for k,v in expected.items())
ctr=pathlib.Path('outputs/crazy-taxi-native/assets/driving.ctr').read_bytes()
assert ctr[4:8196]==r[0x4a0d08:0x4a2d08] and ctr[8196:16388]==r[0x4a2d08:0x4a4d08]
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'initializer':'0802DB54..0802DBB4,4096 entries each, signed16 ASR4 to signed32','x':'0202C4F0 from084A0D08 sine','z':'020304F0 from084A2D08 cosine','impulse':'086A6/086B6 read X/Z, wrapped multiply ASR8','root_wrapper':'08032038 svc8; native unsigned floor root mathematical replacement, independent BIOS execution not verified','scope':'Exact static ROM bytes/literals and CTR1 table source match; not live GBA gameplay proof'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-axis-binding.json').write_text(json.dumps(report,indent=2))
print('PASS SHA/opcode/literal guarded Ball X=sine/Z=cosine initializer and originalCTR tables; SWI8 wrapper located. No BIOS execution oracle.')
