from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
steps=0;j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());cells=json.loads((root/'reports/source240-bounds-and-cells-observation.json').read_text())['cells'];refs=[r for c in cells for r in c['references']]
assert len(refs)==425
result=original([0,refs,*j['camera'],j['yaw'],j['pitch']]);assert result['cursor']==len(j['packets'])*144
pool=bytes.fromhex(result['packet']);heads=bytes.fromhex(result['heads']);mb,tb=struct.unpack_from('<II',rom,0x7556cc+28)
for i,p in enumerate(j['packets']):
 off=i*144;flags=struct.unpack_from('<H',pool,off+4)[0];mp,tp=struct.unpack_from('<II',pool,off+8)
 assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12]==[p['raster_flags'],p['material'],p['texture']],(i,p)
order=[]
for bucket in range(127,-1,-1):
 ptr=struct.unpack_from('<I',heads,bucket*4)[0]
 while ptr:
  off=ptr-0x02009eec;assert off%144==0;i=off//144;assert i not in order and j['packets'][i]['bucket']==bucket;order.append(i);ptr=struct.unpack_from('<I',pool,off)[0]
expected=json.loads((root/'reports/source240-ordered-scenery-verification.json').read_text())['consumption_order'];assert order==expected
normal=set();special=set();skip=0
for r in refs:
 if r&0x8000:
  if r&0x4000:skip+=1
  else:special.add(r&0x3fff)
 else:normal.add(r)
report={'status':'PASS','raw_candidate_references':len(refs),'unique_normal_references':len(normal),'unique_special_references':len(special),'bit14_special_skips':skip,'submitted_packets':len(j['packets']),'original_arm_instructions':steps,'consumption_order':order,'scope':'Fulloriginal31EC runs all425real candidatecell references with duplicate normal/special stampgates and persistent generation1 withinone invocation; 37submissionorder/material/texture/flags/heads match nativecurrentcapture andorderedsceneproof. Cell list extracted fromrealCTW boundedrectangle; original2E70pointerloop not yetexecuted and no per-frame retainedcache/cycles/cadenceproof.'};(root/'reports/source240-full-candidate-producer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
