"""Guard original local turn-radius/angle arithmetic and pivot subtraction sites."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
from pathlib import Path
import json,hashlib
checks={0x12756:'c001',0x12758:'fa218900',0x12760:'0003',0x12774:'0901',0x1277a:'0238',0x1277e:'4100',0x12784:'4443e411',0x1278e:'4143',0x1279a:'0003',0x127ee:'2403',0x127f2:'fa694244fa61',0x1288e:'189a801a0918',0x1289c:'586854466268801a0918',0x128ac:'9868a268801a0918'}
for off,raw in checks.items():assert r[off:off+len(bytes.fromhex(raw))].hex()==raw,hex(off)
constants={}
for off,want in [(0x1276a,4095),(0x12780,12867),(0x12788,0x30053c0),(0x12792,0x84a59dc)]:
 h=struct.unpack_from('<H',r,off)[0];a=((off+4)&~3)+(h&255)*4;got=struct.unpack_from('<I',r,a)[0];assert got==want;constants[hex(off)]=hex(got)
assert struct.unpack_from('<I',r,0x4a59dc)[0]==60
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),byte_checks={hex(k):v for k,v in checks.items()},constants=constants,finding='Original dimension*128/1000 then<<12 division by sine*16, radius-2, wrapped circumference(2*radius*12867)>>7, delta*scaled speed/60 then<<12/circumference and heading angle<<12 agree with native motion.c arithmetic. X/Y/Z add new pivot minus old pivot before common vertical negation.',limits='Exact local opcode/literal guards and manual arithmetic comparison only. Complete original Thumb execution and external division semantics are not independently executed here.')
Path('outputs/crazy-taxi-native/reports/challenge-bound-turn-arithmetic-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original turn arithmetic constants, wrap/shift order and pivot subtraction guards.')
