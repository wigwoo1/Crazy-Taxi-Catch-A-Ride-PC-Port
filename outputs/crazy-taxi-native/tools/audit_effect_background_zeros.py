"""Audit original index-zero coverage; no RGB color inference."""
import pathlib,json,hashlib,struct
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
rows=[]
root=pathlib.Path('outputs/crazy-taxi-native/assets/indexed-scenes')
for name in ('city1','city2','startup'):
 meta=json.loads((root/(name+'.json')).read_text());assert meta['rom_sha256']==expected
 scene={'city1':0x7556cc,'city2':0x755734,'startup':0x7ebb90}[name]
 row=meta['textures'][struct.unpack_from('<h',rom,scene+20)[0]]
 addr=int(row['pixels'],16)-0x08000000
 original=rom[addr:addr+row['byte_length']]
 blob=(root/(name+'.cti')).read_bytes();count,size=struct.unpack_from('<II',blob,4)
 offset=12+count*12+row['byte_offset'];data=blob[offset:offset+row['byte_length']]
 assert data==original and hashlib.sha256(data).hexdigest()==row['sha256']
 rows.append(dict(scene=name,descriptor=row['index'],width=row['width'],height=row['height'],zero_pixels=data.count(0),last_row_fill_index=data[(row['height']-1)*row['width']],zero_by_row=[data[y*row['width']:(y+1)*row['width']].count(0) for y in range(row['height'])]))
report=dict(rom_sha256=expected,records=rows,qualification='Exact original sky indices. Polygon/material writes may introduce or replace zero: this is not proof of final BG2 bitmap coverage. Do not infer coverage from faded/RGBA colors.')
path=pathlib.Path('outputs/crazy-taxi-native/reports/effect-background-zero-coverage.json');path.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(path.read_text())==report
for row in rows: print(row['scene'],'sky zero pixels',row['zero_pixels'],'fill index',row['last_row_fill_index'])
