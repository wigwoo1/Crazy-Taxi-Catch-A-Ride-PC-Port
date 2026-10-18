"""Extract only the immutable original four noncustomer bitmap identities."""
from pathlib import Path
import argparse,hashlib,json,struct
B=Path(__file__).absolute().parent.parent;P=B.parent.parent
ap=argparse.ArgumentParser();ap.add_argument('--rom',type=Path,default=P.parent/'Crazy Taxi - Catch a Ride (USA).gba');ap.add_argument('--output',type=Path,default=B/'assets/noncustomer-bitmaps.bin');ap.add_argument('--report',type=Path,default=B/'reports/noncustomer-bitmap-identities.json');a=ap.parse_args()
for p in [a.rom,a.output,a.report]:assert p.absolute().as_posix().lower().startswith('./'),p
rom=a.rom.read_bytes();sha=lambda d:hashlib.sha256(d).hexdigest();assert sha(rom)=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
raw=rom[0x7ec068:0x7ec078];ids=struct.unpack('<4I',raw);assert all(0x08000000<=v<0x08800000 for v in ids)
for city in [1,2]:
 d=(B/f'assets/city{city}.ctp').read_bytes();n=struct.unpack_from('<I',d,4)[0];assert all(d[16+32*i+3]<=13 for i in range(n))
a.output.parent.mkdir(exist_ok=True,parents=True)
if a.output.exists():assert a.output.read_bytes()==raw,'Preserve an unexpected existing asset'
else:
 temp=a.output.with_suffix('.bin.tmp');temp.write_bytes(raw);temp.replace(a.output)
a.report.write_text(json.dumps(dict(status='PASS',rom_sha256=sha(rom),source_identity='087EC068',bytes=16,source_kinds=[10,11,12,13],bitmap_identities=[f'{v:08x}' for v in ids],output=str(a.output),sha256=sha(raw),scope='Immutable table only, no generated art/source code/runtime emulation.'),indent=2)+'\n',encoding='utf-8');print('PASS original noncustomer bitmap identities extracted,16bytes',flush=True)
