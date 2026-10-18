"""Preserve original traffic grid cells and referenced road paths; no ROM execution."""
import hashlib,json,pathlib,struct
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
def raw(address,size):
    offset=address-0x08000000
    assert 0<=offset and offset+size<=len(rom)
    return rom[offset:offset+size]
def word(offset):return struct.unpack_from('<I',rom,offset)[0]
out=pathlib.Path('outputs/crazy-taxi-native/assets/traffic');out.mkdir(exist_ok=True)
reports=[]
for city,scene in [('city1',0x7556cc),('city2',0x755734)]:
    width,height=struct.unpack_from('<HH',rom,scene+0x3c)
    table=word(scene+0x60);descriptors=word(scene+0x34)
    descriptor_count=struct.unpack_from('<H',rom,scene+0x10)[0]
    pointers=struct.unpack('<'+'I'*(width*height),raw(table,width*height*4))
    grid=bytearray();routes=set();nonempty=0;slots=0
    for pointer in pointers:
        cell=raw(pointer,24) if pointer else bytes(24)
        grid+=struct.pack('<I',pointer)+cell
        if not pointer:continue
        nonempty+=1
        phases=struct.unpack_from('<8H',cell,8)
        count=next((i+1 for i in range(7,-1,-1) if phases[i]),0)
        for i in range(count):
            route=cell[i];assert route<descriptor_count
            routes.add(route);slots+=1
    (out/f'{city}-grid.ctg').write_bytes(b'CTG1'+struct.pack('<HH',width,height)+rom[scene+0x40:scene+0x60]+grid)
    paths=[]
    for index in sorted(routes):
        descriptor=raw(descriptors+index*20,20)
        count=struct.unpack_from('<I',descriptor)[0]
        pointer=struct.unpack_from('<I',descriptor,16)[0]
        assert 1<=count<=65536
        points=raw(pointer,count*16)
        distances=[p[0] for p in struct.iter_unpack('<IiiI',points)]
        assert count==1 or (distances[0]==0 and all(a<b for a,b in zip(distances,distances[1:])))
        (out/f'{city}-road-{index:03d}.ctrp').write_bytes(b'CTP1'+struct.pack('<I',count)+points)
        paths.append(dict(index=index,name=descriptor[4:16].split(b'\0')[0].decode('ascii'),
            descriptor_address=hex(descriptors+index*20),point_address=hex(pointer),count=count,
            length=distances[-1],sha256=hashlib.sha256(points).hexdigest()))
    item=dict(city=city,scene_address=hex(scene+0x08000000),width=width,height=height,
        cells=len(pointers),nonempty_cells=nonempty,spawn_slots=slots,
        grid_sha256=hashlib.sha256(grid).hexdigest(),paths=paths)
    (out/f'{city}-roads.json').write_text(json.dumps(dict(rom_sha256=expected,**item),indent=2)+'\n')
    reports.append(item)
pathlib.Path('outputs/crazy-taxi-native/reports/traffic-road-extraction.json').write_text(json.dumps(reports,indent=2)+'\n')
for item in reports:print(item['city'],item['width'],item['height'],'cells',item['nonempty_cells'],'slots',item['spawn_slots'],'routes',len(item['paths']))
