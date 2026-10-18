"""Original kind3 volume descriptors6..10 from normal menu container."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
base=0x34634;group=base+struct.unpack_from('<I',rom,base+16)[0]
count,bits,tiles,start,palettes=struct.unpack_from('<5I',rom,group)
assert bits==16 and count>=11 and palettes>0
palette=group+20+count*4;pixels=palette+32*palettes
colors=struct.unpack_from('<16H',rom,palette)
out=pathlib.Path('outputs/crazy-taxi-native/assets/normal-menu')
blob=bytearray();rgba_blob=bytearray();records=[]
for index in range(6,11):
    address=base+struct.unpack_from('<I',rom,group+20+index*4)[0]
    metrics=struct.unpack_from('<9H',rom,address)
    tx,ty,ox,oy,cx,cy,w,h,tile=metrics
    assert tx and ty and tile+tx*ty<=tiles
    indices=bytearray();rgba=bytearray()
    for y in range(ty*8):
        for x in range(tx*8):
            location=pixels+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
            value=(rom[location]>>(4*(x&1)))&15;indices.append(value)
            color=colors[value]
            rgba.extend([*((color>>shift&31)*255//31 for shift in (0,5,10)),255 if value else 0])
    records.append(dict(index=index,descriptor=hex(address+0x08000000),metrics=list(metrics),
        indices_offset=len(blob),indices_size=len(indices),rgba_offset=len(rgba_blob)))
    blob.extend(indices);rgba_blob.extend(rgba)
    write_png(out/f'volume-{index}.png',tx*8,ty*8,rgba)
(out/'volume.indices').write_bytes(blob);(out/'volume.rgba').write_bytes(rgba_blob)
(out/'volume.rgb555').write_bytes(rom[palette:palette+32])
(out/'volume.crs').write_bytes(b'CRS1'+struct.pack('<I',5)+b''.join(struct.pack('<9H',*r['metrics']) for r in records))
(out/'volume.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container='0x08034634',group=3,
    palette=hex(palette+0x08000000),pixels=hex(pixels+0x08000000),frames=records,
    indices_sha256=hashlib.sha256(blob).hexdigest(),qualification='Original bank0 source graphics. Live palette ownership and native menu composition pending.'),indent=2)+'\n')
print('Recovered original volume sprites:',[(r['index'],r['metrics']) for r in records])
