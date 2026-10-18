"""Native moving scenery geometry against continuous original producer and near consumer."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
producer={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],producer);producer['steps']=0
near={};near_src=(root/'tools/verify_source240_near_consumer.py').read_text().split('steps=0;j=json')[0];near_src=near_src.replace("pc!=0x080046d8","pc not in (0x080046d8,0xffffffff)");near_src=near_src.replace('exec(src)',"src=src.replace('  return bytes(self.blocks[1][1])','  self.stopped_pc=pc\\n  return bytes(self.blocks[1][1])');exec(src)");exec(near_src,near)
trace=root/'reports/camera-heading-chain-120-camera.txt';rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()]
indices=[0,29,59,89,119];cases=[[0,i,*rows[i][25:28],rows[i][29],rows[i][28]] for i in indices]
exe=root/'bin/source240-trajectory-geometry-probe.exe'
cp=subprocess.run([str(exe),*[str(root/'assets'/a) for a in ['city1.ctw','city2.ctw','driving.ctr','indexed-scenes/city1.cti','indexed-scenes/city2.cti','city1-special.cts','city2-special.cts']]],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
captures=[json.loads(l) for l in cp.stdout.splitlines()];assert len(captures)==len(cases)
results=[];mb,tb=struct.unpack_from('<II',producer['rom'],0x7556cc+28)
for case,capture in zip(cases,captures):
 assert capture['trace_index']==case[1]
 o=producer['original']([0,[0]*425,*case[2:]]);pool=bytes.fromhex(o['packet']);assert o['cursor']==144*len(capture['packets'])
 buckets={}
 for bucket in range(128):
  ptr=struct.unpack_from('<I',bytes.fromhex(o['heads']),bucket*4)[0]
  while ptr:
   i=(ptr-0x02009eec)//144;assert i not in buckets;buckets[i]=bucket;ptr=struct.unpack_from('<I',pool,i*144)[0]
 matched_vertices=normal_packets=special_packets=near_packets=near_steps=flat_attribute_vertices=rejected_near_packets=0
 for i,p in enumerate(capture['packets']):
  packet=pool[i*144:(i+1)*144];flags=struct.unpack_from('<H',packet,4)[0];mp,tp=struct.unpack_from('<II',packet,8)
  assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12,buckets[i]]==[p['flags'],p['material'],p['texture'],p['bucket']],(case,i,p)
  if p['kind']==2:
   rect=[struct.unpack_from('<i',packet,k)[0] for k in [48,60,112,124]];assert rect==p['rect'],(case,i,rect,p);special_packets+=1;continue
  normal_packets+=1
  assert not flags&2,'Far consumer requires separate source entry'
  if flags&1:
   q=near['NearClip'](packet);q.run();near_packets+=1;near_steps+=q.steps
   if q.stopped_pc==0xffffffff:
    assert p['draw_culled'],(case,i,'original rejected near polygon but native draws',p)
    rejected_near_packets+=1;continue
   n=q.reg[8];ptr=q.reg[13]+0x94
   vertices=[]
   for k in range(n):
    sx,u,v,sy,x,y,z,attr=[near['signed'](q.read(ptr+k*32+j*4)) for j in range(8)];vertices.append([sx,sy,x,y,z,u,v,attr])
  else:
   n=3 if flags&16 else 4;vertices=[]
   for k in range(n):
    sx,u,v,sy,x,y,z,attr=struct.unpack_from('<8i',packet,16+k*32);vertices.append([sx,sy,x,y,z,u,v,attr])
  assert n==len(p['vertices']),(case,i,n,p)
  fields=5 if p['texture']==255 else 7
  assert [v[:fields] for v in vertices]==[v[:fields] for v in p['vertices']],(case,i,p['source_index'],vertices,p['vertices'])
  if p['texture']==255:
   assert [v[7] for v in vertices]==[v[7] for v in p['vertices']],(case,i,'flat attribute');flat_attribute_vertices+=n
  matched_vertices+=n
 results.append({'trace_index':case[1],'normal_packets':normal_packets,'special_rectangles':special_packets,'near_consumer_packets':near_packets,'matched_vertices':matched_vertices,'rejected_near_packets':rejected_near_packets,'flat_attribute_vertices':flat_attribute_vertices,'original_near_consumer_instructions':near_steps})
 print('PASS',results[-1],flush=True)
report={'status':'PASS','poses':results,'original_continuous_producer_instructions':producer['steps'],'trace_sha256':hashlib.sha256(trace.read_bytes()).hexdigest(),'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'scope':'Five recorded native moving-camera poses, source240/focal192/cache0/fresh scene. All prepared normal screenXY/viewXYZ, textured UV, flat attributes, special rectangles and metadata/buckets match; four original near-consumer early rejections match native draw_culled (their prepared vertices are not compared) continuous original2D18/31EC plus original4424 nearconsumer including source divide. Unused flat UV/textured attributes excluded; unused fourth triangle vertex excluded by source count. Not raster pixels across poses, retained stamps/material phase, original bootstrap/actors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-recorded-trajectory-geometry-verification.json').write_text(json.dumps(report,indent=2)+'\n');(root/'reports/source240-recorded-trajectory-geometry-capture.json').write_text(json.dumps(captures,indent=2)+'\n');print(json.dumps(report,indent=2))
