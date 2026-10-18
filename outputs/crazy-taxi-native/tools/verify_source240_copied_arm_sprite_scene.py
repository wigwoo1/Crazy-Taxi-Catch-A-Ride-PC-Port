"""Execute sprites at actual cold-bootstrap-copied ARM addresses and patched data."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
boot={};exec((root/'tools/audit_graphics_bootstrap_cold_source.py').read_text().split("report={'status'")[0],boot)
image=bytearray(boot['p'].regions[0x03000000][:0x6810]);assert int.from_bytes(image[0x42f0:0x42f4],'little')==0x030016b8
(root/'reports/renderer-cold-copied-arm-image.bin').write_bytes(image)
# Explicit isolated raster framebuffer identity; mutable block updated as original handoff does.
image[0x16cc:0x16d0]=(0x11000000).to_bytes(4,'little')
source=(root/'tools/verify_source240_shared_sprite_stack_scene.py').read_text().split('\np=SharedSpriteScene();')[0]
source=source.replace('class SharedSpriteScene(BoundScene):',"sprite_scope['BASE']=0x03000008\nclass SharedSpriteScene(BoundScene):")
source=source.replace('q.reg=self.reg.copy();', 'q.blocks.append((0x03000000,bytearray(image)));q.entry=callback;q.reg=self.reg.copy();')
exec(source)
p=SharedSpriteScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.sprite_calls)==4 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'actual_copied_arm_sprite_calls':p.sprite_calls,'cold_copied_arm_image_sha256':hashlib.sha256((root/'reports/renderer-cold-copied-arm-image.bin').read_bytes()).hexdigest(),'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Fouroriginalspritecalls executeactualinstalled0300addresses usingactualcoldbootcopied/patchedARMcode/viewport/frameblock image, actualconsumerstack/pool/registers/NZCV. Explicit selectedframebuffer11000000 alias retained; normal/nearpaths inherited earlierbridges inthischeck. All37intermediates nativeexact. Bootstrap helpercontracts inherited; no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-copied-arm-sprite-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
