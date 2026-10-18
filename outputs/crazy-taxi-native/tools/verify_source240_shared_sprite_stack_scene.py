"""Original billboard calls sharing actual consumer registers and stack byte memory."""
from pathlib import Path
import json,struct,types
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_frame_background_scene.py').read_text().split('\np=BoundScene();')[0])
sprite_scope={};sprite_source=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=self.return_pc:').replace("elif m=='bx':nextpc=self.val(p[0])","elif m=='bx':nextpc=self.val(p[0])&~1");exec(sprite_source,sprite_scope)
class SharedSpriteScene(BoundScene):
 def raster_request(self,callback):
  if callback==background_target:return super().raster_request(callback)
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx];entry_vertex_pointer=self.reg[1]
  if packet['kind']!=2:return super().raster_request(callback)
  self.actual_targets.append(callback);return_address=QueueProbe.raster_request(self,installed_reverse[callback]);f=families[packet['callback']];cls,base=probes[f]
  q=cls(int(textures[packet['texture']]['pixels'],16),tuple(packet['rect']),f==5);q.reg=self.reg.copy();q.return_pc=q.reg[14]&~1;q.n,q.z,q.c,q.v=self.n,self.z,self.c,self.v;before=q.reg[4:15].copy()
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
  fb=sprite_scope['MarkerRasterProbe'].run(q);assert q.reg[4:15]==before
  counts=[idx,q.reads-q.descriptor_byte_reads,q.writes,0,0];seq=len(self.requests)-1
  assert fb+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(fb):self.shared[0x10080000+i]=b
  self.reg=q.reg.copy();self.n,self.z,self.c,self.v=q.n,q.z,q.c,q.v
  self.raster_steps+=q.steps;self.descriptor_reads+=q.descriptor_byte_reads
  self.sprite_calls.append({'source_index':packet['source_index'],'actual_vertex_pointer':f'{entry_vertex_pointer:08x}','shared_consumer_stack':True,'saved_state_restored':True})
  assert q.return_pc==return_address
  return q.return_pc
p=SharedSpriteScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.sprite_calls)==4 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'sprites':p.sprite_calls,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Four originalbillboardcalls consumeactualpoolR1pointer/ROMdescriptor andshareconsumerstackbyte-memory plusallregisters/NZCV; actualcalleeoutputs returnedtoThumbconsumer withoutcaller-preservingcontract. All37intermediateimages/work nativeexact. Rastercode/frame/bounds aliases andnormal/nearouterbridges remain; notcontinuousfullframe/actors/palette/cycles/cadence proof.'}
(root/'reports/source240-shared-sprite-stack-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
