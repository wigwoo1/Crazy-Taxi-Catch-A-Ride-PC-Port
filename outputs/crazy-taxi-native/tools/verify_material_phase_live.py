"""Actual integrated native material phase and full source scene producer check."""
from pathlib import Path
import sys,json,struct,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
clock={};exec((root/'tools/verify_material_phase_owner.py').read_text().split('values=')[0],clock)
trace=root/'reports/material-phase-live-120.txt';rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert len(rows)==120;prior=0;steps=0
for a,delta,freeze,b in rows:
 assert a==prior;expected,n=clock['original'](a,delta,freeze);steps+=n;assert b==expected;prior=b
j=json.loads((root/'reports/material-phase-live-120.json.source240.json').read_text());assert j['phase']==prior==120
producer={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],producer)
s=producer['s'].replace('def original(case):','def original(case,phase=0):').replace(' write(0x11000018,len(refs))',' write(0x03006904,phase)\n write(0x11000018,len(refs))');exec(s,producer);producer['steps']=0
o=producer['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']],j['phase']);assert o['cursor']==144*len(j['packets']);pool=bytes.fromhex(o['packet']);mb,tb=struct.unpack_from('<II',producer['rom'],0x7556cc+28);buckets={}
for bucket in range(128):
 ptr=struct.unpack_from('<I',bytes.fromhex(o['heads']),bucket*4)[0]
 while ptr:
  i=(ptr-0x02009eec)//144;assert i not in buckets;buckets[i]=bucket;ptr=struct.unpack_from('<I',pool,i*144)[0]
near={};exec((root/'tools/verify_source240_near_consumer.py').read_text().split('steps=0;j=json')[0],near)
vertices=specials=near_packets=0
for i,p in enumerate(j['packets']):
 packet=pool[i*144:(i+1)*144];flags=struct.unpack_from('<H',packet,4)[0];mp,tp=struct.unpack_from('<II',packet,8)
 assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12,buckets[i]]==[p['raster_flags'],p['material'],p['texture'],p['bucket']]
 if p['kind']==2:
  assert [struct.unpack_from('<i',packet,k)[0] for k in [48,60,112,124]]==p['rect'];specials+=1;continue
 if flags&1:
  q=near['NearClip'](packet);q.run();n=q.reg[8];ptr=q.reg[13]+0x94;v=[]
  for k in range(n):
   sx,u,uv,sy,x,y,z,attr=[near['signed'](q.read(ptr+k*32+t*4)) for t in range(8)];v.append([sx,sy,x,y,z,u,uv,attr])
  near_packets+=1
 else:
  assert not flags&2;n=3 if flags&16 else 4;v=[]
  for k in range(n):
   sx,u,uv,sy,x,y,z,attr=struct.unpack_from('<8i',packet,16+k*32);v.append([sx,sy,x,y,z,u,uv,attr])
 fields=5 if p['texture']==255 else 7;assert n==len(p['vertices']);assert [x[:fields] for x in v]==[x[:fields] for x in p['vertices']],(i,p['source_index'],p['material'],v,p['vertices'])
 if p['texture']==255:assert [x[7] for x in v]==[x[7] for x in p['vertices']]
 vertices+=n
pause=(root/'reports/material-phase-pause.log').read_text();assert 'PASS native pause freeze: 120 frames' in pause;assert not (root/'reports/material-phase-pause.err').read_text()
report={'status':'PASS','native_phase_updates':len(rows),'phase_start':0,'phase_final':prior,'source_capture_phase':j['phase'],'submitted_packets':len(j['packets']),'matched_normal_vertices':vertices,'matched_special_rectangles':specials,'near_consumer_packets':near_packets,'original_clock_instructions':steps,'original_continuous_producer_instructions':producer['steps'],'paused_frames_with_material_phase_preserved':120,'native_binary_sha256':hashlib.sha256((root/'bin/scene-viewer-material-phase-live.exe').read_bytes()).hexdigest(),'scope':'Actual native gameplay120updates engine6904 phase0to120 matches original2C858 on each recorded globaldelta/freeze; sourcewidth scene inheritsphase120; fulloriginal2D18/31EC metadata/buckets/geometry and originalnearconsumer match nativecapture. Pause check asserts retained6904 unchanged for120frames. This pose does not establish animated-material coverage or allphase values/cadence, wholeoriginalframe/pixels/palette/audio/bootstrap/60fps. Diagnostic executable; user launchers unchanged.'}
(root/'reports/material-phase-live-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
