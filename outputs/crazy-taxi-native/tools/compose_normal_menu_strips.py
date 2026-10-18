"""Source RGB555 reference for normal-menu BG/row strips, excluding text/footer."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
root=pathlib.Path('outputs/crazy-taxi-native/assets');out=root/'normal-menu'
front=(root/'profile/background.indices').read_bytes();back=(root/'profile-layer1/background.indices').read_bytes()
assert len(front)==len(back)==512*256
assert r[0x340d8+120:0x340d8+122]==bytes([3,1])
pal=struct.unpack_from('<256H',r,0xb458c)
b=0x34634;g=b+struct.unpack_from('<I',r,b+16)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g);assert bits==16
palette=g+20+count*4;pixels=palette+pals*32;obj=struct.unpack_from('<16H',r,palette)
def mix(a,b):return sum((((a>>s&31)+(b>>s&31))//2)<<s for s in (0,5,10))
def rgba(values):return bytes(c for v in values for c in [*((v>>s&31)*255//31 for s in (0,5,10)),255])
for name,n in [('main',5),('mode',4)]:
    for selected in range(n):
        raw=[];target=[];eligible=[]
        for y in range(160):
            for x in range(240):
                a=front[y*512+x];bb=back[y*512+x]
                target.append(pal[a or bb])
                eligible.append(bool(a or bb))
                raw.append(mix(pal[a],pal[bb]) if a and bb else pal[a or bb])
        for row in range(n):
            base=3 if row==selected else 0;yanchor=(160-(n*16+44))//2+28+row*16-4
            for frame,xanchor in [(base,16),(base+1,24),(base+1,56),(base+1,88),(base+2,120)]:
                a=b+struct.unpack_from('<I',r,g+20+frame*4)[0]
                tx,ty,ox,oy,cx,cy,w,h,tile=struct.unpack_from('<9H',r,a)
                assert tile+tx*ty<=tiles
                for y in range(ty*8):
                    for x in range(tx*8):
                        p=pixels+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
                        index=r[p]>>(4*(x&1))&15;xx=xanchor-ox+x;yy=yanchor-oy+y
                        if index and 0<=xx<240 and 0<=yy<160:
                            dst=yy*240+xx
                            # Semi-transparent OBJ blends with highest eligible
                            # underlying BG, not with already blended BG output.
                            raw[dst]=obj[index] if row==selected or not eligible[dst] else mix(obj[index],target[dst])
        image=rgba(raw);(out/f'{name}-strips-{selected}.rgba').write_bytes(image)
        write_png(out/f'{name}-strips-{selected}.png',240,160,image)
(out/'strip-reference.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,blend_control='1f4f',coefficients=[8,8],qualification='Source BG flags3/1; selected OBJ opaque bank0, unselected semi-transparent bank0. Raw highest BG is second target, not preblended BG. Diagnostic reference excludes all text/footer and palette fade; native composition still pending.'),indent=2)+'\n')
print('Nine original background/row-strip reference compositions exported; text/footer excluded.')
