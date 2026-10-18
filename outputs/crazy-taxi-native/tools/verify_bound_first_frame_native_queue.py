from pathlib import Path
import struct,json,subprocess
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
ram=(root/'reports/bound-first-frame-background-return-dependency-02000000.bin').read_bytes()
base=0x02009eec;size=144;seen={};original_order=[]
for bucket in range(127,-1,-1):
 pointer=struct.unpack_from('<I',ram,0x9cec+bucket*4)[0]
 while pointer:
  assert base<=pointer<base+101*size and (pointer-base)%size==0 and pointer not in seen
  seen[pointer]=bucket;original_order.append((pointer-base)//size);pointer=struct.unpack_from('<I',ram,pointer-0x02000000)[0]
assert len(seen)==101
blob=bytearray()
for i in range(101):
 address=base+i*size;_,flags,level,material,texture=struct.unpack_from('<IHHII',ram,address-0x02000000)
 material_flags=rom[material-0x08000000+0x14]
 blob+=struct.pack('<4I',seen[address],flags,int(texture!=0),material_flags)
fixture=root/'reports/bound-first-frame-native-queue.fixture';fixture.write_bytes(blob)
cp=subprocess.run([str(root/'bin/bound-first-frame-queue-native-compare.exe'),str(fixture)],capture_output=True)
actual=[list(struct.unpack_from('<3I',cp.stdout,i)) for i in range(0,len(cp.stdout),12)] if cp.returncode==0 else []
calls=json.loads((root/'reports/bound-first-frame-packet-clip-joined-returns.json').read_text())['calls']
expected=[[(c['before'][1]-16-base)//size,int(c['entry'],16),c['before'][0]] for c in calls]
assert [row[0] for row in expected]==original_order
passed=cp.returncode==0 and actual==expected
report=dict(status='NATIVE_ORIGINAL_QUEUE_DISPATCH_PASS' if passed else 'NATIVE_QUEUE_DISPATCH_DEPENDENCY',native_exit=cp.returncode,packets=len(actual),original_queue_buckets=128,order_dispatch_vertex_counts_match=passed,actual=actual,expected=expected,scope='Native prepend/127to0 consumption and source material callback identity/count vs101 actual original packet headers/links and observed calls. Prepared producer RAM snapshot remains fixture; native queue reset asserted, not complete playable frame/gameplay.')
(root/'reports/bound-first-frame-native-queue-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],cp.returncode,len(actual))
