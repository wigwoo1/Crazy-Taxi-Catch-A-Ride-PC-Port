from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');j=json.loads((root/'reports/world-producer-multi-reference-observation.json').read_text());rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
lines=[]
for city,refs,x,y,z,yaw,pitch in j['cases']:lines.append(' '.join(map(str,[city,len(refs),x,y,z,yaw,pitch,*refs]))+'\n')
r=subprocess.run([str(root/'bin/world-multi-packet-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw'),str(root/'assets/driving.ctr'),str(root/'assets/indexed-scenes/city1.cti'),str(root/'assets/indexed-scenes/city2.cti')],input=''.join(lines),text=True,capture_output=True,check=True);rows=r.stdout.splitlines();assert len(rows)==len(lines);total=0
for case,source,row in zip(j['cases'],j['observations'],rows):
 pool=bytes.fromhex(source['packet']);heads=bytes.fromhex(source['heads']);scene=[0x7556cc,0x755734][case[0]];mb,tb=struct.unpack_from('<II',rom,scene+28);expected=[];visited=[]
 for bucket in range(127,-1,-1):
  ptr=struct.unpack_from('<I',heads,bucket*4)[0]
  while ptr:
   off=ptr-0x02009eec;assert off%144==0 and 0<=off<source['cursor'];assert off not in visited;visited.append(off);nxt=struct.unpack_from('<I',pool,off)[0];flags=struct.unpack_from('<H',pool,off+4)[0];mp,tp=struct.unpack_from('<II',pool,off+8);expected.extend([(mp-mb)//28,255 if not tp else (tp-tb)//12,bucket,flags]);ptr=nxt
 count=source['cursor']//144;assert len(visited)==count;assert [count,*expected]==list(map(int,row.split())),(case,expected,row);total+=count
report={'pass_match':True,'cases':len(rows),'source_instructions':j['actual_arm_instructions'],'matched_consumed_packets':total,'scope':'Full31EC producer on several actualcity normal references incl reversed/duplicate/material-rejected inputs; native realworld_frame_build + productionqueueconsume matches source final-head linked traversal bucket127..0 newestfirst and complete packet material/texture/bucket/flags order. Native heads/count empty afterconsume asserted. Source draw callbackbody notexecuted; generation restore/repeatedframe persistentcache lifecycle notproved.','sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['tools/probe_world_producer_multi.py','tests/world-multi-packet-rom-compare.c','tools/compare_world_multi_packet_native.py','src/world_frame.c','src/render_queue.c']}}
(root/'reports/world-multi-packet-native-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
