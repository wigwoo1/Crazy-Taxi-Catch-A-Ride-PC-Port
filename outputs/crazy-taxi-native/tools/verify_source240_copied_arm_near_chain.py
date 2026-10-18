"""Original near wrapper/nested raster execute actual startup-copied ARM image."""
from pathlib import Path
import json,hashlib,struct
root=Path('outputs/crazy-taxi-native')
image=bytearray((root/'reports/renderer-cold-copied-arm-image.bin').read_bytes());assert hashlib.sha256(image).hexdigest()=='2749554d7d0a759a8ba906cf9f32c9500e782a5f7df6b745a3fc1f70b78c344e'
image[0x16cc:0x16d0]=(0x11000000).to_bytes(4,'little')
source=(root/'tools/verify_source240_near_raster_chain.py').read_text().split('\nbindings=json')[0]
source=source.replace('  q=cls(self.packet,value);',"  run.__globals__['BASE']=0x10000000;q=cls(self.packet,value);q.blocks.append((0x03000000,image));q.entry=self.material_target;")
source=source.replace('before=q.reg[4:15].copy();pixels=run(q);',"before=q.reg[4:15].copy();run.__globals__['BASE']=0x03000008;pixels=run(q);")
exec(source)
ns['BASE']=0x03000008
class CopiedNear(JoinedNear):
 def __init__(self,raw,target,packet):super().__init__(raw,target,packet);self.blocks.append((0x03000000,image));self.entry=0x03003d2c
bindings=json.loads((root/'reports/renderer-installed-targets-verification.json').read_text());qualified=json.loads((root/'reports/source240-near-raster-chain-verification.json').read_text());byid={x['source_index']:x for x in qualified['packets']};rows=[]
for idx,packet in enumerate(j['packets']):
 if not packet['raster_flags']&1:continue
 target=next(int(x['installed_value'],16) for x in bindings['installed_slots'] if x['slot']==packet['callback'])
 q=CopiedNear(pool[idx*144:(idx+1)*144],target,packet);saved=[0xf8100000+k*0x10203 for k in range(9)];q.reg[4:13]=saved;sp=q.reg[13];q.reg[2:4]=args[idx][2:4];q.run()
 assert q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==0xffffffff
 expected=byid[packet['source_index']];assert hashlib.sha256(q.pixels).hexdigest()==expected['framebuffer_sha256']
 counts=[q.raster.half,q.raster.word] if packet['callback']=='03006950' else [q.raster.reads-q.raster.descriptor_byte_reads,q.raster.writes];assert counts==expected['counts']
 rows.append({'source_index':packet['source_index'],'actual_copied_wrapper_entry':'03003d2c','actual_copied_material_target':f'{target:08x}','counts':counts,'wrapper_instructions':q.steps,'raster_instructions':q.raster.steps})
assert len(rows)==3
report={'status':'PASS','packets':rows,'scope':'Allthree originalnearwrappers andnestednormalrasterbody/fullreturns executeactualcopied0300ARMimage withstartup-patchedviewport/frame pointers, sameisolatedwrapperstack andactualcallee scratchstate. Pixels/work matchnativequalifiedchains. Isolatedwrapper input/SP/engine fixtures andfb11000000alias remain; notfullconsumerintegration/actors/palette/cycles/cadence proof.'}
(root/'reports/source240-copied-arm-near-chain-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
