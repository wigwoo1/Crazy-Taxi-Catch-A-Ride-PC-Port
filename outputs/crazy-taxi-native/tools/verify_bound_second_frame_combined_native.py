from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');rom_path=Path('Crazy Taxi - Catch a Ride (USA).gba');rom=rom_path.read_bytes()
ram=(root/'reports/bound-second-frame-pre-consumer-owner-02000000.bin').read_bytes();base=0x02009eec;seen={}
for bucket in range(128):
 address=struct.unpack_from('<I',ram,0x9cec+bucket*4)[0]
 while address:
  assert base<=address<base+101*144 and (address-base)%144==0 and address not in seen
  seen[address]=bucket;address=struct.unpack_from('<I',ram,address-0x02000000)[0]
assert len(seen)==101
calls=json.loads((root/'reports/bound-second-frame-packet-clip-joined-returns.json').read_text())['calls'];assert len(calls)==101 and all(c['before'][3]==0 for c in calls)
clips=json.loads((root/'reports/bound-second-frame-native-clip-inputs.json').read_text());projection=clips[0]['projection'];assert all(c['projection']==projection for c in clips)
blob=bytearray((root/'reports/bound-second-frame-native-background.fixture').read_bytes())+struct.pack('<5I',*projection)+struct.pack('<I',101)
for i in range(101):
 address=base+i*144;_,flags,unused,material,texture=struct.unpack_from('<IHHII',ram,address-0x02000000);assert texture
 n=3 if flags&16 else 4;off=texture-0x08000000;width,height,log=rom[off:off+3];pointer=struct.unpack_from('<I',rom,off+4)[0];material_flags=rom[material-0x08000000+0x14]
 blob+=struct.pack('<9I',seen[address],flags,material_flags,0,width,height,log,pointer,n)
 blob+=ram[address-0x02000000+16:address-0x02000000+16+n*32]
fixture=root/'reports/bound-second-frame-combined-native.fixture';fixture.write_bytes(blob)
cp=subprocess.run([str(root/'bin/bound-first-frame-combined-native-compare.exe'),str(rom_path),str(fixture)],capture_output=True)
expected=(root/'reports/bound-second-frame-original-polygons-indexed.bin').read_bytes();diff=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b];passed=cp.returncode==0 and len(cp.stdout)==len(expected)==38400 and not diff
if cp.returncode==0:(root/'reports/bound-second-frame-combined-native-indexed.bin').write_bytes(cp.stdout)
report=dict(status='COMBINED_NATIVE_ORIGINAL_FRAME_PASS' if passed else 'COMBINED_NATIVE_FRAME_DEPENDENCY',native_exit=cp.returncode,native_stderr=cp.stderr.decode(errors='replace'),raw_allocated_packets=101,different_pixels=len(diff),first_differences=diff[:32],native_sha256=hashlib.sha256(cp.stdout).hexdigest(),original_sha256=hashlib.sha256(expected).hexdigest(),scope='Reusable direct native C background+queue ordering/material dispatch+farclip/projection+polygon pipeline vs actual original indexed frame. Raw prepared producer packet memory and owner parameters are fixtures; all observed mip levels0 supplied, mip selection outside scope. No captured background pixels, preordered callback list, or clipped vertices feed native frame. Not whole startup/gameplay/4K/widescreen/presentation proof.')
(root/'reports/bound-second-frame-combined-native-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert passed
