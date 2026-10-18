"""Original renderer dispatch bridged to recovered near/raster bodies; ABI relocation explicit."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_joined_render_requests.py').read_text().split('p=QueueProbe();')[0])
textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures']
probes={}
for f,name in enumerate(['flat','affine','clipped','perspective','column']):
 scope={};exec((root/'tools'/('verify_source240_full_'+name+'_polygon.py')).read_text().split('\nj=json')[0],scope);probes[f]=(scope['Probe'],scope['FB'])
scope={};exec((root/'tools/compare_source240_special_raster_work_rom.py').read_text().split('exe=root')[0],scope);probes[5]=probes[6]=(scope['Probe'],scope['FB'])
near={};exec((root/'tools/verify_source240_near_consumer.py').read_text().split('steps=0;j=json')[0],near)
data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);mb=20+nv*16+np*24
families={'03006950':0,'03006948':1,'03006914':2,'03006934':3,'030069ac':4,'030069a4':5,'03006a28':6}
lines=[str(len(packets))]
for packet in packets:
 f=families[packet['callback']];pattern=struct.unpack_from('<H',data,mb+packet['material']*28+14)[0]
 values=[f,packet['bucket'],packet['texture'],pattern,len(packet['vertices'])]
 if f>=5:values+=packet['rect']
 else:values+=[value&0xffffffff if k>=2 else value for v in packet['vertices'] for k,value in enumerate([v[0],v[1],v[5],v[6],v[4]])]
 lines.append(' '.join(map(str,values)))
cp=subprocess.run([str(root/'bin/source240-background-scenery-probe.exe'),str(root/'assets/indexed-scenes/city1.cti'),str(root/'reports/source240-real-background-city1.bin')],input=('\n'.join(lines)+'\n').encode(),capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
assert len(cp.stdout)==len(packets)*38420
class DrawingProbe(QueueProbe):
 def __init__(self):
  super().__init__();self.edges=bytes(161*32);self.raster_steps=0;self.near_steps=0;self.counts=[0]*4
 def raster_request(self,callback):
  return_address=super().raster_request(callback);idx=self.requests[-1]['payload'];captured=packets[idx];raw=pool[idx*144:(idx+1)*144];packet=dict(captured);f=families[packet['callback']]
  if f>=5:
   packet['rect']=[struct.unpack_from('<i',raw,k)[0] for k in (48,60,112,124)];assert packet['rect']==captured['rect']
  else:
   if captured['raster_flags']&1:
    q=near['NearClip'](raw);q.run();self.near_steps+=q.steps;n=q.reg[8];ptr=q.reg[13]+0x94
    words=[[near['signed'](q.read(ptr+i*32+k*4)) for k in range(8)] for i in range(n)]
   else:
    assert not captured['raster_flags']&2
    n=3 if captured['raster_flags']&16 else 4;words=[list(struct.unpack_from('<8i',raw,16+i*32)) for i in range(n)]
   packet['vertices']=[[w[0],w[3],w[4],w[5],w[6],w[1],w[2],w[7]] for w in words]
   fields=[0,1,2,3,4,7] if f==0 else list(range(7))
   assert [[v[k] for k in fields] for v in packet['vertices']]==[[v[k] for k in fields] for v in captured['vertices']],idx
  cls,base=probes[f]
  if f==0:q=cls(packet,struct.unpack_from('<H',data,mb+packet['material']*28+14)[0])
  elif f<5:q=cls(packet,textures[packet['texture']])
  else:q=cls(int(textures[packet['texture']]['pixels'],16),tuple(packet['rect']),f==5)
  fb=bytes(self.shared[a] for a in range(0x10080000,0x10089600))
  for address,buf in q.blocks:
   if address==base:buf[:]=fb
   if address==0x020344f0:buf[:]=self.edges[:len(buf)]
  fb=q.run();self.raster_steps+=q.steps
  for address,buf in q.blocks:
   if address==0x020344f0:self.edges=bytes(buf)+self.edges[len(buf):]
  counts=[idx,0,0,q.half,q.word] if f==0 else [idx,q.reads,q.writes,0,0]
  seq=len(self.requests)-1;assert fb+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(fb):self.shared[0x10080000+i]=b
  return return_address
p=DrawingProbe();p.run(0x0802b6a0)
assert len(p.requests)==37 and all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
assert p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'intermediate_framebuffer_bytes_compared':37*38400,'counts':p.counts,'original_dispatch_thumb_instructions':p.steps,'original_background_arm_instructions':p.background_steps,'original_near_arm_instructions':p.near_steps,'original_raster_arm_instructions':p.raster_steps,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Original full renderer control flow/background/queue dispatch bridged to actual near and raster instruction bodies. Original pool supplies meaningful geometry and special rectangles; all37 intermediate pixels/work exact native production queue. Retained framebuffer and edge workspace. Explicit callback ABI contract: relocated fixture contexts, near stops before material dispatch, raster stops before epilogue, caller registers preserved by bridge. Not continuous callee ABI/full game/frame/palette/actors/cycles/cadence proof.'}
(root/'reports/source240-joined-render-bodies-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
