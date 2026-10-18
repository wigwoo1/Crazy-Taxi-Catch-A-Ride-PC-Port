from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_special_producer.py').read_text().split('steps=0;cases=[]')[0])
steps=0
j=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());matched=[]
assert j['phase']==0
for p in j['packets']:
 if p['kind']!=2:continue
 o=original([0,p['source_index']|0x8000,*j['camera'],j['yaw'],j['pitch']]);assert o['cursor']==144
 b=bytes.fromhex(o['packet']);flags=struct.unpack_from('<H',b,4)[0]
 rect=[struct.unpack_from('<i',b,x)[0] for x in [48,60,112,124]]
 assert flags==p['raster_flags'] and rect==p['rect'],(p,flags,rect)
 heads=bytes.fromhex(o['heads']);bucket=next(i for i in range(128) if struct.unpack_from('<I',heads,i*4)[0]);assert bucket==p['bucket']
 matched.append(p['source_index'])
assert len(matched)==4
report={'status':'PASS','matched_special_source_indices':matched,'original_arm_instructions':steps,'scope':'Four isolated native source240 submitted scenery sprites at actual presented camera vs full original31EC special producer: rectangle/flags/bucket exact. Zero source phase and patched cache/generation/trig/division contracts; not complete cell traversal, rejected references, retained caches, consumer pixels or full frame timing.'}
(root/'reports/source240-special-producer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
