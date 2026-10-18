"""Actual installed scenery arguments with full near/normal/sprite return checks."""
from pathlib import Path
import json,struct,types
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_near_chain_scene.py').read_text().split('\np=FullNearScene();')[0])
class FullSpriteScene(FullNearScene):
 def __init__(self):super().__init__();self.sprite_calls=[]
 def raster_request(self,callback):
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx]
  if packet['kind']!=2:return super().raster_request(callback)
  self.actual_targets.append(callback);return_address=QueueProbe.raster_request(self,installed_reverse[callback]);f=families[packet['callback']];cls,base=probes[f]
  raw=pool[idx*144:(idx+1)*144];rect=[struct.unpack_from('<i',raw,k)[0] for k in (48,60,112,124)];assert rect==packet['rect']
  q=cls(int(textures[packet['texture']]['pixels'],16),tuple(rect),f==5);q.reg[0]=self.reg[0];q.reg[2:4]=self.reg[2:4];q.reg[4:13]=self.reg[4:13];saved=q.reg[4:13].copy();sp=q.reg[13];lr=q.reg[14]
  q.descriptor_byte_reads=0;original_read=q.read;descriptor=self.reg[2]
  def counted_read(obj,a,n=4):
   if n==1 and descriptor<=a<descriptor+12:obj.descriptor_byte_reads+=1
   return original_read(a,n)
  q.read=types.MethodType(counted_read,q)
  for address,buf in q.blocks:
   if address==base:buf[:]=bytes(self.shared[a] for a in range(0x10080000,0x10089600))
  fb=q.run();assert q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==lr
  counts=[idx,q.reads-q.descriptor_byte_reads,q.writes,0,0];seq=len(self.requests)-1
  assert fb+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(fb):self.shared[0x10080000+i]=b
  self.raster_steps+=q.steps;self.descriptor_reads+=q.descriptor_byte_reads
  self.sprite_calls.append({'source_index':packet['source_index'],'descriptor':f'{descriptor:08x}','descriptor_reads':q.descriptor_byte_reads,'saved_state_restored':True})
  return return_address
p=FullSpriteScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.sprite_calls)==4 and len(p.chain_calls)==3 and p.counts==[8660,8006,73,2121] and p.descriptor_reads==26
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'sprites':p.sprite_calls,'near_chains':p.chain_calls,'descriptor_byte_reads':p.descriptor_reads,'counts':p.counts,'intermediate_framebuffer_bytes_compared':1420800,'original_raster_arm_instructions':p.raster_steps,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'All37 actualinstalledconsumerdraws nativeexact eachintermediate. Fourbillboards useactualconsumerROMdescriptor/R0/R3 andfullsaved-state returns; threenear chains useactualmaterialowner/fullrasterreturns/sharedwrapperstack. Outerconsumerbridge/code/frame/bounds/R1relocations remain, descriptorloads separatelycounted forchangedpaths only; no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-installed-all-returns-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
