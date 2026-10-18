from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
steps=0;j=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());packets=[p for p in j['packets'] if p['kind']==0]
exe=root/'bin/world-full-packet-rom-compare.exe'
cp=subprocess.run([str(exe),*[str(root/'assets'/a) for a in ['city1.ctw','city2.ctw','driving.ctr','indexed-scenes/city1.cti','indexed-scenes/city2.cti']]],input=''.join(' '.join(map(str,[0,p['source_index'],*j['camera'],j['yaw'],j['pitch']]))+'\n' for p in packets),text=True,capture_output=True,check=True)
rows=cp.stdout.splitlines();assert len(rows)==len(packets)
matched=vertices=depth=0;differences=[];textured_uv_vertices=flat_unused_uv_vertices=0
for p,row in zip(packets,rows):
 v=list(map(int,row.split()));assert v[:3]==[0,p['source_index'],1]
 assert v[3:7]==[p['material'],p['texture'],p['bucket'],p['raster_flags']]
 o=original([0,[p['source_index']],*j['camera'],j['yaw'],j['pitch']]);b=bytes.fromhex(o['packet']);flags=struct.unpack_from('<H',b,4)[0]
 if flags&3:depth+=1;continue
 n=3 if flags&16 else 4;assert v[7]==n
 for i in range(n):
  sx,u,uv,sy,x,y,z,attr=struct.unpack_from('<8i',b,16+i*32)
  fields=5 if p['texture']==255 else 7
  if fields==5:
   assert (u,uv)==(-1515870811,-1515870811),'Flat UV source slots unexpectedly written'
   flat_unused_uv_vertices+=1
  else:textured_uv_vertices+=1
  if v[8+i*8:16+i*8][:fields]!=[sx,sy,x,y,z,u,uv][:fields]:differences.append({'source_index':p['source_index'],'vertex':i,'native':v[8+i*8:16+i*8][:fields],'source':[sx,sy,x,y,z,u,uv][:fields]})
  vertices+=1
 matched+=1
report={'status':'PASS' if not differences else 'MISMATCH','differences':differences,'submitted_normal_packets':len(packets),'non_depthclipped_geometry_packets':matched,'matched_vertices':vertices,'textured_uv_vertices':textured_uv_vertices,'flat_unused_uv_vertices':flat_unused_uv_vertices,'depthclipped_representation_excluded':depth,'original_arm_instructions':steps,'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'scope':'Existing native fullpacket comparator at actualsource240camera vsfulloriginal31EC. Nondepthclipped screenXY/viewXYZ/UV comparison; consult differences before claiming equivalence; depthclipped prepared vsraw representation excluded; flat unused UV slots asserted unchanged source A5 sentinel and excluded; no attribute/producertraversal/cachelifecycle/fullframe/cyclesproof.'}
(root/'reports/source240-normal-geometry-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
