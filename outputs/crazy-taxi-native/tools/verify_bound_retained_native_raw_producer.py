from pathlib import Path
import json,struct,subprocess
root=Path('outputs/crazy-taxi-native');results=[]
for label in ('second','third'):
 args=[root/'bin/bound-retained-native-raw-producer.exe',root/'assets/challenge-worlds/challenge5.ctw',root/'assets/challenge-worlds/challenge5-special.cts',root/'assets/challenge-worlds/challenge5.cti',root/f'reports/bound-{label}-frame-pre-raster-owner-02000000.bin',root/f'reports/bound-{label}-frame-pre-raster-owner-03000000.bin']
 cp=subprocess.run(list(map(str,args)),capture_output=True,text=True)
 if cp.returncode:results.append({'frame':label,'native_exit':cp.returncode,'stderr':cp.stderr});continue
 data=json.loads(cp.stdout);(root/f'reports/bound-{label}-frame-native-raw-producer-output.json').write_text(json.dumps(data,indent=2)+'\n')
 source=(root/f'reports/bound-{label}-frame-pre-consumer-owner-02000000.bin').read_bytes();differences=[];fields=0
 for index,packet in enumerate(data['packets']):
  offset=0x9eec+144*index;flags=struct.unpack_from('<H',source,offset+4)[0];count=3 if flags&16 else 4
  if len(packet['raw_vertices'])!=count:differences.append({'packet':index,'source_count':count,'native_count':len(packet['raw_vertices'])});continue
  for vertex,row in enumerate(packet['raw_vertices']):
   original=struct.unpack_from('<8i',source,offset+16+32*vertex)
   # Textured attribute word and unused triangle fourth slot are retained scratch.
   selected=list(range(8) if packet['texture']==255 else range(7))
   # Original 3598..35A4 branches past projection when far-plane bit 0x20
   # is present, setting packet bit 2. +0 retains clip flags; +12 is scratch.
   if flags&2:selected=[field for field in selected if field not in (0,3)]
   for field in selected:
    fields+=1
    if original[field]!=row[field]:differences.append({'packet':index,'vertex':vertex,'field':field,'source':original[field],'native':row[field]})
 results.append({'frame':label,'native_exit':0,'packets':len(data['packets']),'compared_fields':fields,'different_fields':len(differences),'first_differences':differences[:16]})
passed=all(row.get('native_exit')==0 and row.get('packets')==101 and row.get('different_fields')==0 for row in results)
report={'status':'NATIVE_ORIGINAL_DEFINED_PRODUCER_COORDINATES_PASS' if passed else 'NATIVE_RAW_PRODUCER_VERTEX_DEPENDENCY','cases':results,'scope':'Production world_frame_build retained raw_polygon coordinates generated from native assets and original prior owners vs actualoriginalpreconsumer packets. Defined XYZ/UV and projected screen coordinates checked; original far-plane branch 3598..35A4 skips projection and sets packet bit 2, so those packets retain clip flags at screen_x and scratch at screen_y. These two fields excluded for far-clipped packets; textured unused attribute excluded. This does not claim raw144byte identity. Producer packetmetadata/stamps qualified separately. Raw geometry is retained directly in production packet API; test adapter only serializes those vertices; no whole nativegame/liveframe claim.'}
(root/'reports/bound-retained-native-raw-producer-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
