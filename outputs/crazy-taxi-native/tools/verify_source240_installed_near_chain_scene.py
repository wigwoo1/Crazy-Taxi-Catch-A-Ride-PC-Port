"""Installed scene consumer with shared-stack original near/raster chains."""
from pathlib import Path
import json,struct
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_normal_returns.py').read_text().split('\np=InstalledDrawingProbe();')[0])
chain_source=(root/'tools/verify_source240_near_raster_chain.py').read_text().split('\nbindings=json')[0]
chain_source=chain_source.replace('  q.reg=self.reg.copy();', '''  for address,buf in q.blocks:
   if address==0x11000000:buf[:]=self.initial_fb
   if address==0x020344f0:buf[:]=self.initial_edges[:len(buf)]
  q.reg=self.reg.copy();''')
chain_source=chain_source.replace('  return q.return_pc','''  self.output_edges=self.initial_edges
  for address,buf in q.blocks:
   if address==0x020344f0:self.output_edges=bytes(buf)+self.initial_edges[len(buf):]
  return q.return_pc''')
chain={};exec(chain_source,chain)
class FullNearScene(InstalledDrawingProbe):
 def __init__(self):super().__init__();self.chain_calls=[];self.descriptor_reads=0
 def raster_request(self,callback):
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx]
  if not packet['raster_flags']&1:return super().raster_request(callback)
  assert callback==installed[0x0300691c]
  self.actual_targets.append(callback);return_address=QueueProbe.raster_request(self,0x0300691c)
  target=self.read(0x03006920);assert target==installed[int(packet['callback'],16)]
  q=chain['JoinedNear'](pool[idx*144:(idx+1)*144],target,packet);q.reg[2:4]=self.reg[2:4];q.reg[4:13]=self.reg[4:13];saved=q.reg[4:13].copy();sp=q.reg[13]
  q.initial_fb=bytes(self.shared[a] for a in range(0x10080000,0x10089600));q.initial_edges=self.edges;q.run()
  assert q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==0xffffffff
  flat=packet['callback']=='03006950'
  counts=[idx,0,0,q.raster.half,q.raster.word] if flat else [idx,q.raster.reads-q.raster.descriptor_byte_reads,q.raster.writes,0,0]
  seq=len(self.requests)-1;assert q.pixels+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(q.pixels):self.shared[0x10080000+i]=b
  self.edges=q.output_edges;self.near_steps+=q.steps;self.raster_steps+=q.raster.steps;self.descriptor_reads+=q.raster.descriptor_byte_reads
  self.chain_calls.append({'source_index':packet['source_index'],'target':f'{target:08x}','arguments':q.calls[0]['arguments']})
  return return_address
p=FullNearScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.chain_calls)==3 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'near_chains':p.chain_calls,'descriptor_byte_reads':p.descriptor_reads,'counts':p.counts,'intermediate_framebuffer_bytes_compared':1420800,'original_dispatch_thumb_instructions':p.steps,'original_near_arm_instructions':p.near_steps,'original_raster_arm_instructions':p.raster_steps,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Actual installed consumer targets and actual6920materialowner feed full3original near/raster/return chains sharing wrapperstack andcallee flags/registers, retain realbackground/framebuffer/edge workspace throughall37nativeexactintermediates. Full33normalreturns. Outerconsumer-to-raster bridge stillrelocates contexts/preserves caller; billboardABI/frameblock/backgroundaliases remain, no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-installed-near-chain-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
