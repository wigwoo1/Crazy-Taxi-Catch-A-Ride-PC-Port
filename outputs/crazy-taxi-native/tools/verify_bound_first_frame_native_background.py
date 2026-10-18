from pathlib import Path
import struct,json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');rom_path=Path('Crazy Taxi - Catch a Ride (USA).gba');rom=rom_path.read_bytes()
iw=(root/'reports/bound-first-frame-background-return-dependency-03000000.bin').read_bytes();ew=(root/'reports/bound-first-frame-background-return-dependency-02000000.bin').read_bytes()
def word(a):
 data,off=(iw,a-0x03000000) if 0x03000000<=a<0x03010000 else (ew,a-0x02000000) if 0x02000000<=a<0x02040000 else (rom,a-0x08000000)
 return struct.unpack_from('<I',data,off)[0]
block=word(0x03000008+0x3abc-0x700);scene=word(block);descriptor=word(scene+0x20)+(word(scene+0x14)&65535)*12
width,height=rom[descriptor-0x08000000:descriptor-0x08000000+2];assert width==240
texture=word(descriptor+4);pitch=json.loads((root/'reports/bound-first-frame-background-return-audit.json').read_text())['before'][0];yaw=word(word(block+4)+0x10)
left,top,right,bottom=[word(block+offset) for offset in (8,12,16,20)];fill=word(block+0x40)
fixture=root/'reports/bound-first-frame-native-background.fixture';fixture.write_bytes(struct.pack('<9I',texture,height,pitch,yaw,left,top,right,bottom,fill))
cp=subprocess.run([str(root/'bin/bound-first-frame-background-native-compare.exe'),str(rom_path),str(fixture)],capture_output=True)
expected=(root/'reports/bound-first-frame-original-background-indexed.bin').read_bytes();diff=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b];passed=cp.returncode==0 and len(cp.stdout)==len(expected)==38400 and not diff
if cp.returncode==0:(root/'reports/bound-first-frame-native-background-indexed.bin').write_bytes(cp.stdout)
report=dict(status='NATIVE_ORIGINAL_BACKGROUND_PASS' if passed else 'NATIVE_BACKGROUND_DEPENDENCY',native_exit=cp.returncode,parameters=dict(block=hex(block),descriptor=hex(descriptor),texture=hex(texture),height=height,pitch=pitch,yaw=yaw,bounds=[left,top,right,bottom],fill=hex(fill)),different_pixels=len(diff),first_differences=diff[:32],native_sha256=hashlib.sha256(cp.stdout).hexdigest(),original_sha256=hashlib.sha256(expected).hexdigest(),scope='Actual original2FE0 background caller pitch, source-owned scene descriptor/camera yaw/bounds/fill and ROM texture vs direct native background C,38400bytes; retained snapshot owners, not whole startup/presentation/gameplay.')
(root/'reports/bound-first-frame-native-background-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
