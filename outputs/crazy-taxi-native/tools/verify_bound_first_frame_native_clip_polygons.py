from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');rom_path=Path('Crazy Taxi - Catch a Ride (USA).gba');rom=rom_path.read_bytes()
ram=(root/'reports/bound-first-frame-packet-native-clips-dependency-02000000.bin').read_bytes()
calls=json.loads((root/'reports/bound-first-frame-packet-clip-joined-returns.json').read_text())['calls'];assert len(calls)==101
clips={c['packet_index']:c for c in json.loads((root/'reports/bound-first-frame-native-clip-inputs.json').read_text())}
initial=(root/'reports/bound-first-frame-original-background-indexed.bin').read_bytes();assert len(initial)==38400
blob=bytearray(initial)+struct.pack('<I',len(calls))
for index,c in enumerate(calls):
 n,address,material,level=c['before'][:4];off=material-0x08000000;assert 0<=off<len(rom)-4-level*4
 width,height,log=rom[off:off+3];pointer=struct.unpack_from('<I',rom,off+4+level*4)[0]
 blob+=struct.pack('<7I',int(c['entry'],16),n,width,height,log,level,pointer)
 if index in clips:
  clip=clips[index];blob+=struct.pack('<9I',*clip['projection'],*clip['bounds'])
  for v in clip['input_vertices']:blob+=struct.pack('<8I',*v)
 else:
  assert 0x02000000<=address and address+n*32<=0x02040000
  for i in range(n):blob+=struct.pack('<5I',*[struct.unpack_from('<I',ram,address-0x02000000+i*32+o)[0] for o in (0,12,4,8,24)])
fixture=root/'reports/bound-first-frame-native-clip-polygons.fixture';fixture.write_bytes(blob)
cp=subprocess.run([str(root/'bin/bound-first-frame-native-clip-polygon-compare.exe'),str(rom_path),str(fixture)],capture_output=True)
expected=(root/'reports/bound-first-frame-original-polygons-indexed.bin').read_bytes()
diff=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b]
passed=cp.returncode==0 and len(cp.stdout)==len(expected)==38400 and not diff
if cp.returncode==0:(root/'reports/bound-first-frame-native-clip-polygons-indexed.bin').write_bytes(cp.stdout)
report=dict(status='NATIVE_ORIGINAL_CLIP_POLYGON_FRAME_PASS' if passed else 'NATIVE_CLIP_POLYGON_FRAME_DEPENDENCY',native_exit=cp.returncode,native_stderr=cp.stderr.decode(errors='replace'),original_packet_inputs=len(calls),native_farclip_calls=len(clips),compared_bytes=min(len(cp.stdout),len(expected)),different_pixels=len(diff),first_differences=diff[:32],native_sha256=hashlib.sha256(cp.stdout).hexdigest(),original_sha256=hashlib.sha256(expected).hexdigest(),scope='All101 raw original packet inputs; native C far clipping, projection and polygon drawing with ROM textures vs original indexed frame. Original background and prepared packet queue remain captured fixtures; not whole native startup/frame/palette/presentation/gameplay proof.')
(root/'reports/bound-first-frame-native-clip-polygons-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
