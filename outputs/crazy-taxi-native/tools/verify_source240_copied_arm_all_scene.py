"""All original scenery families execute one copied image and shared caller state."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
combined_prefix=(root/'tools/verify_source240_copied_arm_normal_sprite_scene.py').read_text().split('\np=SharedNormalScene();')[0]
all_source=(root/'tools/verify_source240_shared_all_stack_scene.py').read_text().split('\np=SharedAllScene();')[0]
all_source=all_source.replace("exec((root/'tools/verify_source240_shared_normal_stack_scene.py').read_text().split('\\np=SharedNormalScene();')[0])",'exec(combined_prefix)')
all_source=all_source.replace("near_namespace['ns']['BASE']=0x08000700", "near_namespace['ns']['BASE']=0x03000008")
all_source=all_source.replace('wrapper.reg=self.reg.copy();','wrapper.blocks.append((0x03000000,image));wrapper.entry=callback;wrapper.reg=self.reg.copy();')
all_source=all_source.replace('   if a==0x080049e8:return 0x03006818 # Explicit viewport data literal relocation; do not overwrite callback slots.\n','')
all_source=all_source.replace('q=cls(packet,pattern if f==0 else textures[packet[\'texture\']]);',"run.__globals__['BASE']=0x10000000;q=cls(packet,pattern if f==0 else textures[packet['texture']]);q.blocks.append((0x03000000,image));q.entry=obj.material_target;")
all_source=all_source.replace('obj.pixels=run(q);',"run.__globals__['BASE']=0x03000008;obj.pixels=run(q);")
exec(all_source)
p=SharedAllScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.shared_near_calls)==3 and len(p.normal_calls)==30 and len(p.sprite_calls)==4 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'copied_normal_calls':30,'copied_sprite_calls':4,'copied_near_chains':3,'counts':p.counts,'descriptor_byte_reads':p.descriptor_reads,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'ALL37 scenerydraws useONEactualcoldbootcopiedpatchedARMimage atactualinstalledPCs includingnear/nestedraster, sameconsumerstack/pool/engine/ROMdescriptors/registers/NZCV/actualscratchreturns. Allintermediateimages/work nativeexact, cleanup exact. Selectedframebuffer11000000alias andedgefixture/backingbuffers remain; backgroundexecution stillseparateROMbridge. Offlineonly/no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-copied-arm-all-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
