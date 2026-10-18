"""Extract original6580 traffic bitmap banks; preview uses embedded palette."""
import hashlib,json,pathlib,struct
from png_writer import write_png
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
out=pathlib.Path('outputs/crazy-taxi-native/assets/traffic');out.mkdir(exist_ok=True)
rows=[];atlas=bytearray(128*16*64*3*4)
for kind,address in enumerate([0x0822ff08,0x08250110,0x08270318]):
    offset=address-0x08000000
    assert struct.unpack_from('<4H',rom,offset)==(128,1024,256,0)
    palette=rom[offset+8:offset+520];indices=rom[offset+520:offset+520+128*1024]
    assert len(palette)==512 and len(indices)==128*1024
    colors=[]
    for i,(color,) in enumerate(struct.iter_unpack('<H',palette)):
        channels=[color&31,(color>>5)&31,(color>>10)&31]
        colors.append(bytes([(c<<3)|(c>>2) for c in channels]+[255 if i else 0]))
    rgba=b''.join(colors[index] for index in indices)
    (out/f'traffic-{kind}.ctb').write_bytes(b'CTB1'+struct.pack('<HHI',128,64,16)+palette+indices)
    (out/f'traffic-{kind}.rgba').write_bytes(rgba)
    write_png(out/f'traffic-{kind}.png',128,1024,rgba)
    for frame in range(16):
        for y in range(64):
            source=(frame*64+y)*128*4;target=((kind*64+y)*2048+frame*128)*4
            atlas[target:target+512]=rgba[source:source+512]
    rows.append(dict(kind=kind,header_address=hex(address),pixels_address=hex(address+520),
        width=128,frame_height=64,frames=16,frame_stride=8192,
        palette_sha256=hashlib.sha256(palette).hexdigest(),indices_sha256=hashlib.sha256(indices).hexdigest(),
        rgba_sha256=hashlib.sha256(rgba).hexdigest()))
write_png(out/'traffic-source-atlas.png',2048,192,atlas)
(out/'traffic-sprites.json').write_text(json.dumps(dict(rom_sha256=expected,
    evidence='Source6580 sprite groups, +0x208 pixels,16 frames,stride0x2000.',
    status='Original raw palette and indices preserved. Embedded palette RGBA preview only; runtime palette lighting and rasterizer transparency require integration verification.',
    banks=rows),indent=2)+'\n')
print('Extracted three original traffic bitmap banks,48 directional frames; raw palette/indices preserved.')
