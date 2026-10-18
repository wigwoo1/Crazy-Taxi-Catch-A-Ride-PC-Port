from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');original=json.loads((root/'reports/world-producer-destination-observation.json').read_text());expected={tuple(o['case']):o['result'] for o in original['emitted_packets']};cases=original['cases']
r=subprocess.run([str(root/'bin/world-full-packet-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw'),str(root/'assets/driving.ctr'),str(root/'assets/indexed-scenes/city1.cti'),str(root/'assets/indexed-scenes/city2.cti')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=r.stdout.splitlines();assert len(rows)==len(cases);matched=0;packets=0
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
for case,row in zip(cases,rows):
 case=tuple(case)
 v=list(map(int,row.split()));assert tuple(v[:2])==case[:2];assert v[2]==int(case in expected),(case,row)
 if not v[2]:continue
 packets+=1;packet=bytes.fromhex(expected[case]['packet']);flags=struct.unpack_from('<H',packet,4)[0];mp,tp=struct.unpack_from('<II',packet,8);scene=[0x7556cc,0x755734][case[0]];mb,tb=struct.unpack_from('<II',rom,scene+28);material=(mp-mb)//28;texture=255 if not tp else (tp-tb)//12
 heads=bytes.fromhex(expected[case]['heads']);bucket=next(i for i in range(128) if struct.unpack_from('<I',heads,i*4)[0]);assert v[3:7]==[material,texture,bucket,flags],(case,v[3:7],[material,texture,bucket,flags])
 if not flags&3:
  n=3 if flags&16 else 4;assert v[7]==n
  for i in range(n):
   sx,u,uv,sy,x,y,z,attr=struct.unpack_from('<8i',packet,16+i*32);assert v[8+i*8:13+i*8]==[sx,sy,x,y,z],(case,'XYZ/screen mismatch')
   if texture!=255:assert v[13+i*8:15+i*8]==[u,uv],(case,'UV mismatch')
   else:assert v[15+i*8]==attr,(case,'flat attribute mismatch')
  matched+=1
report={'pass_match':True,'cases':len(cases),'matched_submissions':packets,'matched_non_depthclipped_packet_fields':matched,'scope':'All first/last actual destination camera normal reference decisions and submitted material/texture/bucket/flags match full31EC observation. Non-depthclipped activeXYZ/screenUV packets match. Prepared clipped geometry/source raw packet differ by representation; uninitialized textured attributes excluded. No full original consumer pixel or preview/cache lifecycle proof.','sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_frame.c','src/world_frame.h','src/world_indexed.c','tests/world-full-packet-rom-compare.c','tools/compare_world_destination_packet_native.py']}}
(root/'reports/world-destination-full-packet-native-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
