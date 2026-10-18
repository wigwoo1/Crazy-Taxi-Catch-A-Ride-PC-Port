from pathlib import Path
import sys,json,hashlib
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
steps=0;j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());refs=[p['source_index']|(0x8000 if p['kind']==2 else 0) for p in j['packets']];expected=original([0,refs,*j['camera'],j['yaw'],j['pitch']]);actual=json.loads((root/'reports/source240-continuous-scene-producer-raw.json').read_text())
assert actual['cursor']==expected['cursor']==37*144
assert actual['heads']==expected['heads']
a=bytes.fromhex(actual['packet'])[:actual['cursor']];b=bytes.fromhex(expected['packet'])[:expected['cursor']];assert a==b
report={'status':'PASS','raw_packet_bytes':len(a),'queue_head_bytes':512,'packet_sha256':hashlib.sha256(a).hexdigest(),'scope':'Continuousoriginalbounds/cellloop/nestedproducer output entire37raw144bytepackets and512headbytes equal sourceacceptedreferencefixture underlying priorgeometry/pixelproof. Includes nextlinks/flags/materialtexturepointers/rawscreenXYZUV/attributes including unusedsentinels. Doesnot addbootstrap/actor/palette/retainedtrajectory/cycleproof.'};(root/'reports/source240-continuous-packet-byte-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
