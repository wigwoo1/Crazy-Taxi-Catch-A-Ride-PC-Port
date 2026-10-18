"""Preserve original Control diagram pixels and layout highlight table."""
import pathlib,struct,hashlib,json
from png_writer import write_png
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
base=0x3e5ec;out=pathlib.Path('outputs/crazy-taxi-native/assets/control-menu');out.mkdir(exist_ok=True)
count=struct.unpack_from('<I',rom,base)[0];assert 0<count<32
rows=[];blob=bytearray()
for group in range(count):
    g=base+struct.unpack_from('<I',rom,base+4+group*4)[0]
    frames,bits,tiles,start,pals=struct.unpack_from('<5I',rom,g)
    assert bits==16 and 0<pals<=16 and frames<1024
    palette=g+20+frames*4;pixels=palette+pals*32
    assert pixels+tiles*32<=len(rom)
    for frame in range(frames):
        descriptor=base+struct.unpack_from('<I',rom,g+20+frame*4)[0]
        metrics=struct.unpack_from('<9H',rom,descriptor)
        tx,ty,ox,oy,cx,cy,w,h,tile=metrics
        assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
        for bank in range(pals):
            colors=struct.unpack_from('<16H',rom,palette+bank*32);rgba=bytearray()
            for y in range(ty*8):
                for x in range(tx*8):
                    at=pixels+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
                    index=rom[at]>>(4*(x&1))&15;color=colors[index]
                    rgba.extend([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if index else 0])
            rows.append(dict(group=group,frame=frame,bank=bank,metrics=list(metrics),offset=len(blob),length=len(rgba)))
            blob.extend(rgba);write_png(out/f'sprite-{group}-{frame}-{bank}.png',tx*8,ty*8,rgba)
table=rom[0x49d4d4:0x49d4d4+3*7*4]
(out/'layout-highlights.bin').write_bytes(table)
(out/'diagram-positions.bin').write_bytes(rom[0x7ebf5c:0x7ebf5c+12*12])
(out/'sprites.rgba').write_bytes(blob)
(out/'sprites.json').write_text(json.dumps(dict(container='0803e5ec',groups=count,frames=rows,rgba_sha256=hashlib.sha256(blob).hexdigest(),layout_table='0849d4d4',highlights=[list(struct.unpack_from('<7I',table,layout*28)) for layout in range(3)],qualification='Original pixels/banks and three-by-seven highlight words; native rendering/demo timing not yet integrated.'),indent=2)+'\n')
print('Original Control container:',count,'groups;',len(rows),'frame/bank variants;',len(blob),'RGBA bytes')
