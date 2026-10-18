"""Native title background against existing original5778 qualification."""
from pathlib import Path
import subprocess,json,hashlib
probe=Path('outputs/crazy-taxi-native/tools/verify_title_bg_loader_rom.py')
verifier_file=__file__;__file__=str(probe)
exec(compile(probe.read_text(),str(probe),'exec'))
__file__=verifier_file
capture=root/'reports/title-background-native.bin';binary=root/'bin/title-background-rom-compare.exe'
run=subprocess.run([str(binary),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],capture_output=True,text=True,check=True)
assert list(map(int,run.stdout.split()))==[control,0x1f40,0x0808,4,128,0]
expected=bytes(p.read(0x6000000+i,1) for i in range(65536))+bytes(p.read(0x2001790+i,1) for i in range(2560))+bytes(p.read(0x5000000+i,1) for i in range(1024))
actual=capture.read_bytes();assert len(actual)==len(expected)
assert actual==expected,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b)
report={'status':'PASS','compared_bytes':len(expected),'original_Thumb_instructions':p.steps,'scope':'Native title descriptor tile decode/map placement/palette registration compared to existing original5778 probe, all64KiB BGgraphics/2560registrybytes/1024palette bytes and control4440/blend1F40/alpha0808/count4/bg128/obj0. Original tile7190 consumes previously direct1E860-qualified decoded fixture and F7B0 allocatorreturn0 contract; no whole caller/title rendering/parent proof. Clean registry/allocator fixture only.','native_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/title-background-native-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native title background',len(expected),'bytes')
