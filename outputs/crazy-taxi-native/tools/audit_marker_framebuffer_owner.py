"""Source-bound normal renderer framebuffer handoff; static evidence only."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native')
cfg=json.loads((root/'reports/normal-loop-cfg.json').read_text())
rows=[x for x in cfg['instructions'] if 0x800b468<=int(x['address'],16)<=0x800b488]
assert any(x['address']=='0x800b482' and x['operands']=='#0x802b6a0' for x in rows)
def literal(off):
 h=struct.unpack_from('<H',r,off)[0]
 assert h&0xf800==0x4800
 return struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]
owner=literal(0xb47e)
assert r[0xb480:0xb482]==bytes.fromhex('7068')
assert literal(0x2b6ac)==0x030069a8
assert r[0x2b6ae:0x2b6b2]==bytes.fromhex('09680860')
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Static source instruction and literal audit, not runtime framebuffer/palette/pixel verification.', 'normal_frame_source':rows,'framebuffer_descriptor':hex(owner),'framebuffer_pointer_field':hex(owner+4),'handoff':['0800B480 loads descriptor+4 into r0','0800B482 calls 0802B6A0','0802B6AC loads 030069A8','0802B6AE dereferences relocated framebuffer-word address','0802B6B0 stores incoming framebuffer pointer'], 'next_dependency':'Recover descriptor initialization and palette registration; execute relocated ARM raster callbacks with patched clip/framebuffer words. Do not equate embedded marker palettes or city palette with active palette without source ownership proof.'}
(root/'reports/marker-framebuffer-owner-audit.json').write_text(json.dumps(report,indent=2))
print('Verified normal-frame framebuffer descriptor',hex(owner),'pointer field',hex(owner+4))
