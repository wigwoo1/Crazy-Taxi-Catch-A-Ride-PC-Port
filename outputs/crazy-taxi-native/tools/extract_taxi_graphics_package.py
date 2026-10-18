"""Extract immutable original graphics data; never execute source instructions."""
from pathlib import Path
import argparse,hashlib,json,struct
B=Path(__file__).absolute().parent.parent;P=B.parent.parent
parser=argparse.ArgumentParser()
parser.add_argument('--rom',type=Path,default=P.parent/'Crazy Taxi - Catch a Ride (USA).gba')
parser.add_argument('--output',type=Path,default=B/'assets/taxis/graphics.ctg')
parser.add_argument('--report',type=Path,default=B/'reports/taxi-graphics-package.json')
args=parser.parse_args()
def scoped(path):
 p=path.absolute();normal=p.as_posix().lower()
 if not (normal.startswith('./') or normal.startswith('./')):raise ValueError('Project data must stay under ./')
 return p
rom_path,output,report_path=map(scoped,[args.rom,args.output,args.report])
sha=lambda data:hashlib.sha256(data).hexdigest()
baseline=json.loads((B/'tools/taxi_graphics_source_ranges.json').read_text())
rom=rom_path.read_bytes();assert sha(rom)==baseline['rom_sha256']
ranges=[tuple(x) for x in baseline['ranges']];scenes=[]
for scene in ['city1','city2','startup']:
 descriptor=(B/'assets/scene-descriptors'/(scene+'.bin')).read_bytes()
 palette=(B/'assets'/(scene+'-palette.rgb555')).read_bytes()
 assert len(descriptor)==104 and len(palette)==512
 identity=struct.unpack_from('<I',descriptor,24)[0];off=identity-0x8000000
 assert 0<=off<=len(rom)-512 and rom[off:off+512]==palette
 ranges.append((off,off+512));scenes.append({'scene':scene,'identity':hex(identity),'palette_sha256':sha(palette)})
# Actual1EE28 registered-palette writes; DMA-only inheritance missed bank8.
# Include their immutable source palettes for all4session drivers and5languages.
hud_ids={0x08290520,0x08290720,0x08290af8,0x08290b40,struct.unpack_from('<I',rom,0x7ec200)[0]}
hud_ids.update(struct.unpack_from('<I',rom,0x4a551c+12*driver)[0] for driver in range(4))
hud_ids.update(struct.unpack_from('<I',rom,0x7ec24c+4*language)[0] for language in range(5))
for identity in sorted(hud_ids):
 off=identity-0x8000000;assert 0<=off<=len(rom)-32;ranges.append((off,off+32))
# Original renderer15A34 failure-flash palette; immutable source data only.
renderer_tail_palette=0x0820fce0
tail_off=renderer_tail_palette-0x08000000
assert 0<=tail_off<=len(rom)-32
ranges.append((tail_off,tail_off+32))
merged=[]
for a,b in sorted(ranges):
 assert 0<=a<b<=len(rom)
 if merged and a<=merged[-1][1]:merged[-1]=(merged[-1][0],max(b,merged[-1][1]))
 else:merged.append((a,b))
data=bytearray(struct.pack('<4sII',b'CTG1',len(rom),len(merged)))
for a,b in merged:data+=struct.pack('<II',a,b-a)+rom[a:b]
output.parent.mkdir(parents=True,exist_ok=True);report_path.parent.mkdir(parents=True,exist_ok=True)
temporary=output.with_name(output.name+'.tmp')
temporary.write_bytes(data);temporary.replace(output)
report={'rom_sha256':sha(rom),'package_sha256':sha(data),'package_bytes':len(data),'ranges':merged,'scenes':scenes,'hud_palette_identities':[hex(x) for x in sorted(hud_ids)],'renderer_tail_palette_identity':hex(renderer_tail_palette),'baseline_range_manifest_sha256':sha((B/'tools/taxi_graphics_source_ranges.json').read_bytes()),'scope':'Every inherited original immutable graphics/HUD/taxi/cash range retained; original city1/city2/startup descriptor palettes included for common registered palette ownership. Source identities retained as data, no source CPU execution or generated/upscaled art.'}
report_path.write_text(json.dumps(report,indent=2)+'\n')
print('Graphics package',len(data),'bytes;',len(merged),'source ranges;',len(scenes),'verified scene palette identities')