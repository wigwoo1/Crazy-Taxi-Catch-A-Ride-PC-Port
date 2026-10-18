"""One original copied ARM image shared by normal and sprite scene calls."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
image=bytearray((root/'reports/renderer-cold-copied-arm-image.bin').read_bytes());assert hashlib.sha256(image).hexdigest()=='2749554d7d0a759a8ba906cf9f32c9500e782a5f7df6b745a3fc1f70b78c344e'
image[0x16cc:0x16d0]=(0x11000000).to_bytes(4,'little')
sprite_prefix=(root/'tools/verify_source240_shared_sprite_stack_scene.py').read_text().split('\np=SharedSpriteScene();')[0]
sprite_prefix=sprite_prefix.replace('class SharedSpriteScene(BoundScene):',"sprite_scope['BASE']=0x03000008\nclass SharedSpriteScene(BoundScene):")
sprite_prefix=sprite_prefix.replace('q.reg=self.reg.copy();','q.blocks.append((0x03000000,image));q.entry=callback;q.reg=self.reg.copy();')
source=(root/'tools/verify_source240_shared_normal_stack_scene.py').read_text().split('\np=SharedNormalScene();')[0]
source=source.replace("exec((root/'tools/verify_source240_shared_sprite_stack_scene.py').read_text().split('\\np=SharedSpriteScene();')[0])",'exec(sprite_prefix)')
source=source.replace('cls,run,base=normal_runs[f]',"cls,run,base=normal_runs[f];run.__globals__['BASE']=0x10000000")
source=source.replace('fb=run(q);',"run.__globals__['BASE']=0x03000008;fb=run(q);")
source=source.replace('q.reg=self.reg.copy();','q.blocks.append((0x03000000,image));q.entry=callback;q.reg=self.reg.copy();')
exec(source)
p=SharedNormalScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.normal_calls)==30 and len(p.sprite_calls)==4 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'actual_copied_arm_normal_calls':30,'actual_copied_arm_sprite_calls':4,'counts':p.counts,'descriptor_byte_reads':p.descriptor_reads,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Thirtynormalandfourbillboards executeONE sharedactualcoldbootstrapcopiedpatchedARMimage/mutabledata withactualinstalledPCs andsharedconsumerstack/registers/NZCV/pool. All37intermediates nativeexact. Three nearpaths stilloldbridge; selectedfb11000000alias/edgefixture/backingconstructorbuffers remain. Offlineonly/no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-copied-arm-normal-sprite-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
