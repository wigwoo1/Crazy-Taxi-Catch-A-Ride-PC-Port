from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native');j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());packets=j['packets'];textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures'];data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);mb=20+nv*16+np*24
families={'03006950':0,'03006948':1,'03006914':2,'03006934':3,'030069ac':4,'030069a4':5,'03006a28':6};lines=[str(len(packets))]
for p in packets:
 f=families[p['callback']];pattern=struct.unpack_from('<H',data,mb+p['material']*28+14)[0]
 values=[f,p['bucket'],p['texture'],pattern,len(p['vertices'])]
 if f>=5:values+=p['rect']
 else:values+=[value&0xffffffff if i>=2 else value for v in p['vertices'] for i,value in enumerate([v[0],v[1],v[5],v[6],v[4]])]
 lines.append(' '.join(map(str,values)))
cp=subprocess.run([str(root/'bin/source240-ordered-scene-probe.exe'),str(root/'assets/indexed-scenes/city1.cti')],input=('\n'.join(lines)+'\n').encode(),capture_output=True,check=True);assert len(cp.stdout)==len(packets)*38420
ns={};exec((root/'tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0],ns);ns['steps']=0
refs=[p['source_index']|(0x8000 if p['kind']==2 else 0) for p in packets];o=ns['original']([0,refs,*j['camera'],j['yaw'],j['pitch']]);assert o['cursor']==len(packets)*144
heads=bytes.fromhex(o['heads']);pool=bytes.fromhex(o['packet']);order=[]
for bucket in range(127,-1,-1):
 ptr=struct.unpack_from('<I',heads,bucket*4)[0]
 while ptr:
  offset=ptr-0x02009eec;assert offset%144==0;idx=offset//144;assert idx not in order and 0<=idx<len(packets);assert packets[idx]['bucket']==bucket;order.append(idx);ptr=struct.unpack_from('<I',pool,offset)[0]
assert len(order)==len(packets)
probes={}
for f,name in [(0,'verify_source240_full_flat_polygon'),(1,'verify_source240_full_affine_polygon'),(2,'verify_source240_full_clipped_polygon'),(3,'verify_source240_full_perspective_polygon'),(4,'verify_source240_full_column_polygon')]:
 scope={};exec((root/'tools'/(name+'.py')).read_text().split('\nj=json')[0],scope);probes[f]=(scope['Probe'],scope['FB'])
scope={};exec((root/'tools/compare_source240_special_raster_work_rom.py').read_text().split('exe=root')[0],scope);probes[5]=probes[6]=(scope['Probe'],scope['FB'])
fb=bytes([173])*38400;edges=bytes(161*32);reads=half=flat_half=flat_word=steps=0
for seq,idx in enumerate(order):
 p=packets[idx];f=families[p['callback']];cls,base=probes[f]
 if f==0:q=cls(p,struct.unpack_from('<H',data,mb+p['material']*28+14)[0])
 elif f<5:q=cls(p,textures[p['texture']])
 else:q=cls(int(textures[p['texture']]['pixels'],16),tuple(p['rect']),f==5)
 for address,buf in q.blocks:
  if address==base:buf[:]=fb
  if address==0x020344f0:buf[:]=edges[:len(buf)]
 fb=q.run()
 for address,buf in q.blocks:
  if address==0x020344f0:edges=bytes(buf)+edges[len(buf):]
 counts=[idx,0,0,q.half,q.word] if f==0 else [idx,q.reads,q.writes,0,0]
 assert fb+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx,p['source_index'])
 reads+=counts[1];half+=counts[2];flat_half+=counts[3];flat_word+=counts[4];steps+=q.steps
assert (reads,half,flat_half,flat_word)==(8660,8006,73,2121)
(root/'reports/source240-ordered-scenery-indexed.bin').write_bytes(fb)
report={'status':'PASS','packets':len(order),'framebuffer_bytes_after_each_packet':len(order)*38400,'original_producer_queue_instructions':ns['steps'],'original_raster_instructions':steps,'texture_byte_reads':reads,'textured_halfword_writes':half,'flat_halfword_writes':flat_half,'flat_word_writes':flat_word,'consumption_order':order,'indexed_framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Fulloriginal31EC mixedacceptedreference list queue linkage order equals productionnativequeue, all37materialrasterbodies retainingfb andedgeworkspace; everyintermediatefb/count exact. Actualsource240preparedgeometry withnearwrapperseparatelyqualified, sourcecontractsphase/cache0,initial173fb. Sceneryonly excludesoriginalbounds-cell traversal/background/actors/palette/cachelifecycle/trajectory/cycles/cadence.'};(root/'reports/source240-ordered-scenery-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
