from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native');j=json.loads((root/'reports/live-taxi-gpu-120-scenery.json.source240.json').read_text());packets=j['packets'];textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures'];data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);mb=20+nv*16+np*24
shadow=json.loads((root/'reports/taxi-shadow-joined-renderer-verification.json').read_text())['cases'][-1]
assert shadow['ticks']==120
packets=[{'callback':'SHADOW','bucket':0,'texture':0,'material':0,'vertices':[],'rect':shadow['shadow_rectangle'],'source_index':-1,'kind':2}]+packets
families={'SHADOW':7,'03006950':0,'03006948':1,'03006914':2,'03006934':3,'030069ac':4,'030069a4':5,'03006a28':6};lines=[str(len(packets))]
for p in packets:
 f=families[p['callback']];pattern=struct.unpack_from('<H',data,mb+p['material']*28+14)[0]
 values=[f,p['bucket'],p['texture'],pattern,len(p['vertices'])]
 if f>=5:values+=p['rect']
 else:values+=[value&0xffffffff if i>=2 else value for v in p['vertices'] for i,value in enumerate([v[0],v[1],v[5],v[6],v[4]])]
 lines.append(' '.join(map(str,values)))
cp=subprocess.run([str(root/'bin/source240-shadow-scene-probe.exe'),str(root/'assets/indexed-scenes/city1.cti'),str(root/'reports/source240-real-background-city1.bin'),str(root/'assets/taxi-software/taxi-software.ctx')],input=('\n'.join(lines)+'\n').encode(),capture_output=True,check=True);assert len(cp.stdout)==len(packets)*38420
ns={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],ns)
seed=json.loads((root/'reports/taxi-shadow-producer-verification.json').read_text())['cases'][-1];ns['_shadow_seed']=bytes.fromhex(seed['packet_hex'])
assert ns['_shadow_seed'][:4]==bytes(4)
needle=' N=Z=V=False;C=False;pc=0x08002d18;case_steps=0'
assert needle in ns['s']
injection=" regions[0x02009eec][:144]=_shadow_seed;write(0x03006910,144);write(0x02009cec,0x02009eec)\n"
exec(ns['s'].replace(needle,injection+needle),ns);ns['steps']=0
o=ns['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']]);assert o['cursor']==len(packets)*144
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
_thumb=Cs(CS_ARCH_ARM,CS_MODE_THUMB);_thumb.detail=True
for address,target in [(0x0800b364,0x0801556c),(0x0800b474,0x0802c840),(0x0800b482,0x0802b6a0)]:
 instruction=next(_thumb.disasm(ns['rom'][address-0x08000000:address-0x08000000+4],address));assert instruction.mnemonic=='bl' and instruction.operands[0].imm==target
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
shadow_ns={};exec((root/'tools/verify_taxi_shadow_raster.py').read_text().split('\njoined=json')[0],shadow_ns)
probes[7]=(shadow_ns['ShadowRaster'],shadow_ns['FB'])
fb=(root/'reports/source240-real-background-city1.bin').read_bytes();edges=bytes(161*32);reads=half=flat_half=flat_word=steps=0
for seq,idx in enumerate(order):
 p=packets[idx];f=families[p['callback']];cls,base=probes[f]
 if f==0:q=cls(p,struct.unpack_from('<H',data,mb+p['material']*28+14)[0])
 elif f<5:q=cls(p,textures[p['texture']])
 elif f==7:q=cls(int(shadow['shadow_call']['pixels'],16),tuple(p['rect']))
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
assert (reads,half,flat_half,flat_word)==(9444,8204,73,2121)
assert order[-1]==0 and all(packets[i]['callback']!='SHADOW' for i in order[:-1])
(root/'reports/source240-scenery-shadow-indexed.bin').write_bytes(fb)
report={'status':'PASS','packets':len(order),'framebuffer_bytes_after_each_packet':len(order)*38400,'original_producer_queue_instructions':ns['steps'],'original_raster_instructions':steps,'texture_byte_reads':reads,'textured_halfword_writes':half,'flat_halfword_writes':flat_half,'flat_word_writes':flat_word,'consumption_order':order,'indexed_framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Original continuous2D18/31EC producer with actual original31A50 shadow packet preseededbefore scenery; all38queue links/order match native and all38 intermediateindexedframes/work exact. OriginalB364 cab shadow precedes B474 scene dispatch (static source-call evidence). Actualsource240preparedgeometry withnearwrapperseparatelyqualified, sourcecontractsstaticcitymaterials/cache0 and verifiedoriginalbackground; actuallivecamera20625/19666/-35593 yaw-485 pitch1. Joinedscenery/shadow, independentlyoriginalqualified bodylayer not yet added. Rastercontexts retain common framebuffer/edgecontents acrosscalls; continuousoriginalThumbconsumer sharedABI/descriptorowners andlivepalette/otheractors/HUD/cadence remainunverified for38packetjoin.'};(root/'reports/source240-scenery-shadow-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
