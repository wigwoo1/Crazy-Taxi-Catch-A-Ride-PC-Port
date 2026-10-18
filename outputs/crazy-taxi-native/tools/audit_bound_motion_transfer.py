"""Exact local ROM motion stores/calls; no full execution claim."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
from pathlib import Path
import json,hashlib
calls=[(0x126f2,0x80129e0,'steering response'),(0x1283c,0x8012c6c,'turning orientation'),(0x12922,0x8012c6c,'straight orientation'),(0x129aa,0x8014394,'traction/body response'),(0x129b0,0x8015bb8,'response update'),(0x129b6,0x8015c24,'original no-op')]
rows=[]
for off,target,role in calls:
 i=next(md.disasm(r[off:off+4],0x8000000+off));assert i.mnemonic=='bl' and int(i.op_str.lstrip('#'),16)==target
 rows.append(dict(address=hex(i.address),target=hex(target),bytes=i.bytes.hex(),role=role))
# Both paths store at+0C/+10/+14 before common vertical negation.
checks={0x12894:'f960',0x128a6:'3961',0x128b4:'7961',0x12974:'f860',0x1297a:'3861',0x12980:'7861',0x12984:'386940423861',0x15c24:'7047'}
for off,raw in checks.items():assert r[off:off+len(bytes.fromhex(raw))].hex()==raw,hex(off)
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),calls=rows,byte_checks={hex(k):v for k,v in checks.items()},finding='Original stores X/Y/Z displacements at offsets0C/10/14 and negates vertical after both paths. Native motion.c uses corresponding common negation; 15C24 is BX LR, so its omission is behaviorally exact.',limits='Local call/store and no-op guards only. Full steering/pivot/traction arithmetic and field transfer oracle remain unresolved.')
Path('outputs/crazy-taxi-native/reports/challenge-bound-motion-transfer-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original local motion displacement stores, vertical negation, call order and15C24 no-op.')
