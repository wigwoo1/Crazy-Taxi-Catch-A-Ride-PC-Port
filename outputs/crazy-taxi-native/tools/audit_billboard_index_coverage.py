"""Verify software billboard indices/zero key against supplied ROM only."""
import hashlib,json,struct
from pathlib import Path
EXPECTED='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
root=Path('outputs/crazy-taxi-native'); rows=[]
for kind,off in enumerate([0x22ff08,0x250110,0x270318]):
 p=root/f'assets/traffic/traffic-{kind}.ctb'; b=p.read_bytes()
 assert b[:12]==b'CTB1'+struct.pack('<HHI',128,64,16)
 assert b[12:]==r[off+8:off+520+131072]
 indices=b[524:]; assert len(indices)==131072
 rows.append(dict(owner='traffic',kind=kind,frames=16,pixels=len(indices),keyed_zero=indices.count(0),sha256=hashlib.sha256(b).hexdigest()))
p=root/'assets/taxi-software/taxi-software.ctx';b=p.read_bytes();off=0x20fd00
assert b[:12]==b'CTX1'+struct.pack('<HHI',64,32,64)
assert b[12:]==r[off+8:off+520+131072]
indices=b[524:];assert len(indices)==131072
rows.append(dict(owner='normal-shadow',frames=64,pixels=len(indices),keyed_zero=indices.count(0),sha256=hashlib.sha256(b).hexdigest()))
for kind in range(8):
 for stage,address in enumerate(struct.unpack_from('<4I',r,0x7ec51c+kind*16)):
  off=address-0x08000000;frames=[1,4,4,9][stage]
  indices=r[off:off+32*64*frames];palette=struct.unpack_from('<256H',r,off-512)
  colors=[bytes([(((c>>s)&31)<<3)|(((c>>s)&31)>>2) for s in (0,5,10)]+[255 if i else 0]) for i,c in enumerate(palette)]
  p=root/f'assets/passengers/passenger-{kind}-stage-{stage}.rgba';b=p.read_bytes()
  assert b==b''.join(colors[i] for i in indices)
  assert b[3::4]==bytes(255 if i else 0 for i in indices)
  rows.append(dict(owner='passenger',kind=kind,stage=stage,frames=frames,pixels=len(indices),keyed_zero=indices.count(0),sha256=hashlib.sha256(b).hexdigest()))
report=dict(rom_sha256=EXPECTED,frames=sum(x['frames'] for x in rows),pixels=sum(x['pixels'] for x in rows),banks=rows,qualification='Exact raw traffic/shadow banks and embedded-palette passenger RGBA. Zero-key coverage only; does not prove runtime global palette ownership, alternate branches, or full port fidelity.')
p=root/'reports/billboard-index-coverage.json';p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
print(f"PASS {report['frames']} frames, {report['pixels']} pixels; ROM-identical indices and passenger zero-key alpha.")
