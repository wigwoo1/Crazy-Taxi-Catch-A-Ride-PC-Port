from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
steps=0
j=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());matched=[];differences=[]
scene=0x7556cc;mb,tb=struct.unpack_from('<II',rom,scene+28)
for p in j['packets']:
 if p['kind']!=0:continue
 o=original([0,[p['source_index']],*j['camera'],j['yaw'],j['pitch']])
 if not o['cursor']:differences.append({'packet':p,'reason':'source rejected'});continue
 assert o['cursor']==144
 b=bytes.fromhex(o['packet']);flags=struct.unpack_from('<H',b,4)[0];mp,tp=struct.unpack_from('<II',b,8)
 heads=bytes.fromhex(o['heads']);bucket=next(i for i in range(128) if struct.unpack_from('<I',heads,i*4)[0])
 expected=[(mp-mb)//28,255 if not tp else (tp-tb)//12,bucket,flags]
 actual=[p['material'],p['texture'],p['bucket'],p['raster_flags']]
 if expected!=actual:differences.append({'source_index':p['source_index'],'source':expected,'native':actual})
 else:matched.append(p['source_index'])
report={'status':'PASS' if not differences else 'MISMATCH','matched_normal_packets':len(matched),'differences':differences,'original_arm_instructions':steps,'scope':'Each submitted isolatednative source240 normalreference against fulloriginal31EC atactualcamera; material/texture/bucket/flags only. Originalcachezero/phasezero/generation1/trig/division contracts; clipped preparedpolygon differs from source rawpacket representation; not celltraversal/rejectedrefs/cachelifecycle/pixels/cyclesproof.'}
(root/'reports/source240-normal-producer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
