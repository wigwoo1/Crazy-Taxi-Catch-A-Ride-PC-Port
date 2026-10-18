"""Recovered E154 transfer domain, actual ROM bytes rather than guessed colors."""
from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert r[0x4a50b0:0x4a52b0]==bytes(512)
assert r[0x4a52b0:0x4a54b0]==bytes.fromhex('ff07')*256
cases=0
for city in ['city1','city2']:
 asset=root/f'assets/{city}-palette.rgb555';assert asset.read_bytes()==r[0x4a5a34:0x4a5c34]
 for flags in range(8):
  for mode in [0,1]:
   source=0x4a50b0 if flags&1 else 0x4a52b0 if flags&2 else 0x4a5a34 if flags&4 else 0x4a50b0
   expected=r[source:source+512]
   native=subprocess.run([str(root/'bin/palette-bank-rom-compare.exe'),str(flags),str(mode),str(asset)],capture_output=True,check=True).stdout
   assert len(native)==1024 and native==(expected+bytes([0xa5])*512 if mode else bytes([0xa5])*512+expected),(city,flags,mode)
   cases+=1
report={'status':'PASS','cases':cases,'scope':'All1024bank bytes exact against actual ROM source for eight flags/background+object modes/both city palette assets. Oppositebank preserved, native invalid control/destination/source guards preserve bothbanks. Transfer domain exact512bytes incrementing32bit E154 request; no general hardware/DMA/CPU emulation, actualGPUupload/livefadeflags excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-bank-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
