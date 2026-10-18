"""Original normal menu strips and prompt icons; no palette-offset guess."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
b=0x34634;out=pathlib.Path('outputs/crazy-taxi-native/assets/normal-menu')
out.mkdir(exist_ok=True);rows=[];blob=bytearray()
for group,frames in [(2,[4,5]),(3,list(range(6)))]:
    g=b+struct.unpack_from('<I',r,b+4+group*4)[0]
    count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
    assert bits==16 and 0<pals<=16 and max(frames)<count
    pal=g+20+count*4;pix=pal+32*pals
    assert pix+tiles*32<=len(r)
    for index in frames:
        a=b+struct.unpack_from('<I',r,g+20+index*4)[0]
        m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
        assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
        for bank in range(pals):
            colors=struct.unpack_from('<16H',r,pal+bank*32);rgba=bytearray()
            for y in range(ty*8):
                for x in range(tx*8):
                    p=pix+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
                    i=r[p]>>(4*(x&1))&15;c=colors[i]
                    rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0])
            rows.append(dict(group=group,index=index,bank=bank,metrics=list(m),offset=len(blob),length=len(rgba),descriptor=hex(a+0x8000000),palette=hex(pal+bank*32+0x8000000)))
            blob.extend(rgba);write_png(out/f'sprite-{group}-{index}-{bank}.png',tx*8,ty*8,rgba)
(out/'sprites.rgba').write_bytes(blob)
(out/'footer-icons.rgba').write_bytes(blob[:2048])
(out/'sprites.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container=hex(b+0x8000000),sha256=hashlib.sha256(blob).hexdigest(),frames=rows,qualification='All original banks exported. Production group3 frames0/1/2 unselected and3/4/5 selected both use bank0: caller third stack parameter is0 and28288 adds that to group base palette at2855a. Second stack parameter4 unselected/0 selected is OAM flags, NOT palette offset:285ba bit4 sets attr0 bit0x400 (semi-transparent OBJ). First stack parameter1 is request queue1. Group2 frames4/5 original Confirm/Back icons. Exact BLDCNT/BLDALPHA composition still requires audit.'),indent=2)+'\n')
print('Original normal-menu sprites:',len(rows),'frames,',len(blob),'bytes')
g=b+struct.unpack_from('<I',r,b+16)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
pal=g+20+count*4;pix=pal+32*pals;indexed=bytearray(b'NMS1')
for frame in range(6):
    a=b+struct.unpack_from('<I',r,g+20+frame*4)[0]
    tx,ty,ox,oy,cx,cy,w,h,tile=struct.unpack_from('<9H',r,a)
    indexed.extend(struct.pack('<4H',tx*8,ty*8,ox,oy))
    for y in range(ty*8):
        for x in range(tx*8):
            p=pix+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
            indexed.append(r[p]>>(4*(x&1))&15)
(out/'strips.nms').write_bytes(indexed)
(out/'strip-palette.rgb555').write_bytes(r[pal:pal+32])
(out/'background-palette.rgb555').write_bytes(r[0xb458c:0xb458c+512])
