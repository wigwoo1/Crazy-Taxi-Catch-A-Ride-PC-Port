"""Compare native title tile/palette loading against qualified original27994 probe."""
from pathlib import Path
import subprocess,struct,json,hashlib
probe=Path('outputs/crazy-taxi-native/tools/probe_title_sprite_allocation_rom.py')
verifier_file=__file__;__file__=str(probe)
exec(compile(probe.read_text(),str(probe),'exec'))
__file__=verifier_file
binary=root/'bin/title-assets-rom-compare.exe';capture=root/'reports/title-assets-native.bin'
run=subprocess.run([str(binary),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],capture_output=True,text=True,check=True)
assert [g['palette_base'] for g in groups]==[0,2,5,6,8,10,11,12,13,14,15]
assert run.stdout.strip()=='count=20 bg=128 obj=512',run.stdout
expected=b''.join(struct.pack('<I',p.read(0x2007e3c+g*4)) for g in range(11))
expected+=bytes(p.read(0x6010000+i,1) for i in range(32768))
expected+=bytes(p.read(0x2001790+i,1) for i in range(2560))
expected+=bytes(p.read(0x5000000+i,1) for i in range(1024))
actual=capture.read_bytes();assert actual==expected,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b)
report={'status':'PASS','compared_bytes':len(expected),'original_Thumb_instructions':p.steps,'native_counts':run.stdout.strip(),'scope':'Original27994 title asset raw tiles/all32768OBJbytes/all2560palette records/all1024palette-bankbytes/11groupbases match. Retained titleBG fixture count4/bg128. Original7190 upload uses logical ARM0560 copy and28BD0 objectsetup return0 contract from probe. No OAM/title rendering/full startup/parent/gameplay claim.','native_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/title-assets-native-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS native title asset comparison',len(expected),'bytes')
