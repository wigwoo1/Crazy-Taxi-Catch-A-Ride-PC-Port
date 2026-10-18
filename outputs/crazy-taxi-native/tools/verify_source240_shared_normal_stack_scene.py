"""Original ordinary polygons sharing consumer stack, packet pointers and register state."""
from pathlib import Path
import json,re,struct,types
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_shared_sprite_stack_scene.py').read_text().split('\np=SharedSpriteScene();')[0])
normal_runs={}
for f,name,end,oldend in [(0,'flat',0x1f94,0x1f88),(1,'affine',0x198c,0x1980),(2,'clipped',0x1730,0x1724),(3,'perspective',0x1044,0x1038),(4,'column',0x1440,0x1434)]:
 scope={};exec((root/'tools'/('verify_source240_full_'+name+'_polygon.py')).read_text().split('\nj=json')[0],scope);cls=scope['Probe'];s=scope['s'];s,n=re.subn(r'while pc!=BASE\+0x[0-9a-f]+-0x700:', 'while pc!=self.return_pc:',s);assert n==1
 s=s.replace('<'+hex(oldend),'<'+hex(end)).replace("elif m=='bx':nextpc=self.val(p[0])","elif m=='bx':nextpc=self.val(p[0])&~1");exec(s,scope);scope['arm'].detail=True;normal_runs[f]=(cls,scope['MarkerRasterProbe'].run,scope['FB'])
class SharedNormalScene(SharedSpriteScene):
 def __init__(self):super().__init__();self.normal_calls=[]
 def raster_request(self,callback):
  if callback==background_target:return super().raster_request(callback)
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx]
  if packet['kind']==2 or packet['raster_flags']&1:return super().raster_request(callback)
  assert not packet['raster_flags']&2
  self.actual_targets.append(callback);return_address=QueueProbe.raster_request(self,installed_reverse[callback]);f=families[packet['callback']];cls,run,base=normal_runs[f]
  pattern=struct.unpack_from('<H',data,mb+packet['material']*28+14)[0]
  q=cls(packet,pattern if f==0 else textures[packet['texture']]);q.reg=self.reg.copy();q.return_pc=q.reg[14]&~1;q.n,q.z,q.c,q.v=self.n,self.z,self.c,self.v;before=q.reg[4:15].copy();entry_vertex_pointer=q.reg[1]
  q.descriptor_byte_reads=0;original_read=q.read;original_write=q.write;descriptor=q.reg[2]
  def is_shared(a,n):return 0x1000f000<=a<a+n<=0x10010000 or 0x02009cec<=a<a+n<=0x02009eec+len(pool)
  def shared_read(obj,a,n=4):
   if is_shared(a,n):return self.read(a,n)
   if n==1 and descriptor<=a<descriptor+12:obj.descriptor_byte_reads+=1
   return original_read(a,n)
  def shared_write(obj,a,v,n=4):
   if is_shared(a,n):return self.write(a,v,n)
   return original_write(a,v,n)
  q.read=types.MethodType(shared_read,q);q.write=types.MethodType(shared_write,q)
  for address,buf in q.blocks:
   if address==base:buf[:]=bytes(self.shared[a] for a in range(0x10080000,0x10089600))
   if address==0x020344f0:buf[:]=self.edges[:len(buf)]
  fb=run(q);assert q.reg[4:15]==before
  for address,buf in q.blocks:
   if address==0x020344f0:self.edges=bytes(buf)+self.edges[len(buf):]
  counts=[idx,0,0,q.half,q.word] if f==0 else [idx,q.reads-q.descriptor_byte_reads,q.writes,0,0];seq=len(self.requests)-1
  assert fb+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx,counts)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(fb):self.shared[0x10080000+i]=b
  self.reg=q.reg.copy();self.n,self.z,self.c,self.v=q.n,q.z,q.c,q.v;self.raster_steps+=q.steps;self.descriptor_reads+=q.descriptor_byte_reads
  self.normal_calls.append({'source_index':packet['source_index'],'entry_vertex_pointer':f'{entry_vertex_pointer:08x}','descriptor_byte_reads':q.descriptor_byte_reads})
  assert q.return_pc==return_address
  return q.return_pc
p=SharedNormalScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.normal_calls)==30 and len(p.sprite_calls)==4 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'shared_normal_calls':p.normal_calls,'shared_sprite_calls':4,'descriptor_byte_reads':p.descriptor_reads,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Thirtyordinary andfourbillboardoriginalcalls shareactualconsumerstack/poolR1/ROMdescriptor/allregisters/NZCV andreturnactualscratchresults toThumb; threenearoutercalls stillbridged. All37intermediates nativeexact, cleanup exact. Relocatedrastercode/frame/bounds/edge backing remains; excludesactors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-shared-normal-stack-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS sharednormal',len(p.normal_calls),'sprites',len(p.sprite_calls),'descriptor reads',p.descriptor_reads,flush=True)
