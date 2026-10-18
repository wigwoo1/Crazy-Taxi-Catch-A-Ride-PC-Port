"""Verify all decoded scene textures retain original keyed index coverage."""
from extract_scene import appearance,EXPECTED
import pathlib,hashlib,struct,json
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
rows=[];root=pathlib.Path('outputs/crazy-taxi-native/assets')
for name in ('city1','city2','startup'):
 raw=(root/(name+'.ctm')).read_bytes();magic,nv,nf,nt,nm=struct.unpack_from('<4s4I',raw)
 assert magic==b'CTM6'
 offset=32+nv*12+nf*44+nm*12
 textures,_=appearance(rom,name);assert len(textures)==nt
 pixel_count=transparent=0
 for width,height,expected in textures:
  w,h=struct.unpack_from('<II',raw,offset);offset+=8;assert (w,h)==(width,height)
  data=raw[offset:offset+w*h*4];offset+=len(data);assert data==expected
  assert all(a in (0,255) for a in data[3::4])
  pixel_count+=w*h;transparent+=data[3::4].count(0)
 assert offset==len(raw)
 rows.append(dict(scene=name,texture_count=nt,pixels=pixel_count,keyed_zero_pixels=transparent,asset_sha256=hashlib.sha256(raw).hexdigest()))
report=dict(rom_sha256=EXPECTED,records=rows,qualification='Complete CTM6 original palette RGBA equality, alpha0 only rawindex0, alpha255 otherwise. Identity unshaded keyed raster only. Shaded lookup output/flat writing index0 and software billboard shade ownership remain separate; not full frame fidelity.')
p=pathlib.Path('outputs/crazy-taxi-native/reports/effect-unshaded-asset-coverage.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
for row in rows:print(row)
