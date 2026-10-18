from pathlib import Path
import struct,json,subprocess
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba')
calls=json.loads((root/'reports/bound-first-frame-glyph-queue-calls.json').read_text());assert len(calls)==5,('incomplete glyph calls',len(calls))
rows=[]
for index,call in enumerate(calls):
 frame,group,position,priority=call['arguments'];bucket,flags,palette,scale,angle=call['stacked']
 values=[int(call['entry'],16),frame,group,priority,bucket,flags,palette,scale,angle,call['resource'],call['group_base'],call['palette_base'],call['mask'],call['width'],call['height'],*call['position'],call['sine'],call['cosine'],call['returned']]
 fixture=root/'reports'/('bound-first-frame-glyph-%d.fixture'%index);fixture.write_bytes(struct.pack('<21I',*values)+bytes.fromhex(call['before']))
 cp=subprocess.run([str(root/'bin/bound-first-frame-glyph-queue-native-compare.exe'),str(fixture),str(rom)],capture_output=True);expected=bytes.fromhex(call['after']);differences=[i for i,(a,b) in enumerate(zip(cp.stdout,expected)) if a!=b]
 assert cp.returncode==0 and len(cp.stdout)==len(expected)==4490 and not differences,(index,cp.returncode,len(cp.stdout),differences[:32])
 rows.append(dict(index=index,entry=call['entry'],glyph=frame,compared_bytes=4490,different_bytes=0))
report=dict(status='NATIVE_ORIGINAL_GLYPH_QUEUE_PASS',calls=rows,scope='Actual five shared firstframe Bound font calls, native queue/affine producers from actual registeredROM/group/palette/trig owners and captured beforestate; exact4490 bytes each andreturnpointer, not final OAMpublication/visiblepixels/liveownership equivalence.')
(root/'reports/bound-first-frame-native-glyph-queue-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
