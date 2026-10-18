"""Ordinary polygons execute original cold-bootstrap-copied ARM code/data."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
raw_image=(root/'reports/renderer-cold-copied-arm-image.bin').read_bytes();assert hashlib.sha256(raw_image).hexdigest()=='2749554d7d0a759a8ba906cf9f32c9500e782a5f7df6b745a3fc1f70b78c344e'
image=bytearray(raw_image);image[0x16cc:0x16d0]=(0x11000000).to_bytes(4,'little')
source=(root/'tools/verify_source240_shared_normal_stack_scene.py').read_text().split('\np=SharedNormalScene();')[0]
source=source.replace("cls,run,base=normal_runs[f]", "cls,run,base=normal_runs[f];run.__globals__['BASE']=0x10000000")
source=source.replace("fb=run(q);", "run.__globals__['BASE']=0x03000008;fb=run(q);")
source=source.replace('q.reg=self.reg.copy();', 'q.blocks.append((0x03000000,bytearray(image)));q.entry=callback;q.reg=self.reg.copy();')
exec(source)
p=SharedNormalScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.normal_calls)==30 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'actual_copied_arm_normal_calls':30,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Thirtyordinarycallbacks executeactualinstalled0300PCs/coldbootstrapcopiedpatchedARMcode/data, actualconsumerstack/poolR1/ROMdescriptors/registers/NZCV andfullreturns. Framebuffer11000000alias andretainededgefixture remain; sprite/nearpaths inheritedearlierchecks. All37intermediates nativeexact. Offlineonly, no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-copied-arm-normal-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
