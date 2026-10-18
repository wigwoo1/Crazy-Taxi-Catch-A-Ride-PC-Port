"""Extract the unchanged original rank-label indices. No image upscaling."""
from pathlib import Path
import argparse,hashlib,json,struct
base=Path(__file__).resolve().parents[1];project=base.parents[1]
ap=argparse.ArgumentParser()
ap.add_argument('--rom',type=Path,default=project.parent/'Crazy Taxi - Catch a Ride (USA).gba')
ap.add_argument('--output',type=Path,default=base/'assets/hud/rank-labels.indices')
args=ap.parse_args();rom=args.rom.read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected,'Unexpected source ROM'
addresses=[0x080efed4,0x080f31fc,0x080f6524,0x080f984c,0x080fcb74]
data=bytearray();labels=[]
for language,address in enumerate(addresses):
 descriptor=0x7ec33c+8*language;width,height,pointer=struct.unpack_from('<HHI',rom,descriptor)
 assert (width,height,pointer)==(36,32,address)
 start=pointer-0x8000000;pixels=rom[start:start+width*height];assert len(pixels)==1152
 data+=pixels;labels.append(dict(language=language,descriptor=hex(0x8000000+descriptor),pixels=hex(pointer),width=width,height=height,sha256=hashlib.sha256(pixels).hexdigest()))
if args.output.exists():assert args.output.read_bytes()==data,'Existing rank art differs; preserve it for investigation'
else:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_bytes(data)
report=dict(status='PASS_ORIGINAL_RANK_ART_EXTRACTION',rom_sha256=expected,output_sha256=hashlib.sha256(data).hexdigest(),bytes=len(data),labels=labels,scope='Immutable indexed art only. No palette, text, face icon or gameplay modification.')
(base/'reports/native-rank-art-source-20261018.json').write_text(json.dumps(report,indent=2))
print(report['status'],len(data),'unchanged source bytes')
