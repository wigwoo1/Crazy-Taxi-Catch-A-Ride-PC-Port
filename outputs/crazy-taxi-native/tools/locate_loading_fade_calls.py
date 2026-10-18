"""ROM encoding candidates only; call sites require reachable-code validation."""
import struct,json,hashlib
from pathlib import Path
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
found=[]
for off in range(0,len(r)-4,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 n=((a&2047)<<12)|((b&2047)<<1)
 if n&(1<<22):n-=1<<23
 target=0x8000000+off+4+n
 if target in [0x800ee10,0x800d764]:found.append(dict(address=hex(0x8000000+off),target=hex(target)))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),candidates=found,scope='Whole immutable ROM aligned Thumb BL encoding search. Candidates only, not proof of all dynamic/ARM/indirect callers or reachable instructions. D764/26318 and AE34 direct sites separately inspected; asynchronous fade remains unresolved.')
(root/'reports/loading-fade-direct-call-candidates.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
