"""Validate all supported ROM geometry layouts and unsupported-ROM rejection."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
from extract_scene import extract,appearance

rom=Path(sys.argv[1]).read_bytes()
for name,counts in [('city1',(14250,9880)),('city2',(12320,8670)),('startup',(823,492))]:
    vertices,faces,metadata=extract(rom,name)
    textures,materials=appearance(rom,name)
    assert (len(vertices),len(faces))==counts
    assert metadata['triangles']+metadata['quads']==len(faces)
    assert len(textures)==metadata['texture_count'] and len(materials)==metadata['material_count']
    for w,h,pixels in textures:
        assert w>0 and h>0 and len(pixels)==w*h*4
        assert all(alpha in (0,255) for alpha in pixels[3::4])
    for indices,flags in faces:
        assert len(indices) in (3,4)
        assert len(set(indices))==len(indices)
        assert all(0<=i<len(vertices) for i in indices)
        assert (flags&255)<metadata['material_count']
    print(f'PASS: {name}: {len(vertices)} vertices, {len(faces)} validated polygons')
    print(f'PASS: {len(textures)} decoded textures, {len(materials)} materials')
try:
    extract(b'invalid')
except ValueError:
    print('PASS: unsupported ROM rejected')
else:
    raise AssertionError('Unsupported ROM accepted')
