"""Guard exact local ROM world sequencing. Not a whole execution oracle."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json,hashlib
from pathlib import Path
checks=[(0x11fc4,0x802a27c,'movement/impact'),(0x12076,0x802aba0,'ground query/contact'),(0x12084,0x8015b00,'vertical gravity'),(0x120a6,0x8015b54,'contact decay'),(0x124ca,0x801488c,'grounded body springs'),(0x124ea,0x8014968,'grounded normal alignment'),(0x12588,0x8015b54,'no-hit contact decay')]
rows=[]
for off,target,name in checks:
 ins=next(md.disasm(r[off:off+4],0x8000000+off));assert ins.mnemonic=='bl' and int(ins.op_str.lstrip('#'),16)==target,(hex(off),ins.op_str)
 rows.append(dict(address=hex(ins.address),bytes=r[off:off+4].hex(),target=hex(target),role=name))
assert r[0x1207c:0x12082].hex()=='002c00d172e2' # no contact ->12568
assert r[0x1258c:0x12594].hex()=='d321c90068180460' # state698 reset after no-hit decay
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),calls=rows,finding='Native world_update order matches these exact local ROM call sites, contact gate and no-hit reset; no production change justified.',limits='Local call/byte guards and manual sequencing comparison only. Full motion/world execution, hidden services and GBA trajectory remain unverified.')
Path('outputs/crazy-taxi-native/reports/challenge-bound-world-sequence-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original local world sequence, contact gate and no-hit reset guards.')
