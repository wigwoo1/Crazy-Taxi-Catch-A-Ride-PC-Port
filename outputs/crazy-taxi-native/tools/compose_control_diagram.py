"""Original Control diagram reference; excludes BG/text/footer and OBJ priority oracle."""
import pathlib,json,struct,hashlib
from png_writer import write_png
root=pathlib.Path('outputs/crazy-taxi-native/assets/control-menu')
meta=json.loads((root/'sprites.json').read_text());pixels=(root/'sprites.rgba').read_bytes()
assert hashlib.sha256(pixels).hexdigest()==meta['rgba_sha256']
frames={(r['group'],r['frame'],r['bank']):r for r in meta['frames']}
positions=struct.unpack('<36i',(root/'diagram-positions.bin').read_bytes())
masks=[0x04010040,0x08020080,0x02008020,0x01004010,0x00100401,0x00200802,
       0x00802008,0x00401004,0x20080200,0x20080200,0x10040100,0x10040100]
assert len(meta['highlights'])==3
def draw(dst,frame,bank,xanchor,yanchor):
    row=frames[4,frame,bank];tx,ty,ox,oy,*_=row['metrics']
    source=pixels[row['offset']:row['offset']+row['length']]
    for y in range(ty*8):
        for x in range(tx*8):
            at=(y*tx*8+x)*4;xx=xanchor-ox+x;yy=yanchor-oy+y
            if source[at+3] and 0<=xx<240 and 0<=yy<160:
                dst[(yy*240+xx)*4:(yy*240+xx)*4+4]=source[at:at+4]
for layout in range(3):
    for page in range(7):
        image=bytearray(bytes([0,0,0,255])*(240*160))
        for frame in range(12):draw(image,frame,0,positions[frame*3],positions[frame*3+1])
        for row in range(2):
            for col in range(3):draw(image,12+row*3+col,0,24+col*64,16+row*64)
        word=meta['highlights'][layout][page]
        if layout==0 and page==0:(root/'diagram-base.rgba').write_bytes(image)
        for frame,mask in enumerate(masks):
            if word&mask:draw(image,frame,1,positions[frame*3],positions[frame*3+1])
        (root/f'diagram-{layout}-{page}.rgba').write_bytes(image)
        write_png(root/f'diagram-{layout}-{page}.png',240,160,image)
(root/'diagram-reference.json').write_text(json.dumps(dict(positions=[positions[i:i+3] for i in range(0,36,3)],masks=masks,qualification='Original group4 bank0 diagram, bank1 action highlights at clockbit32 set. Black diagnostic backdrop excludes original BG/text/footer. Overlapping source OBJ request ordering not independently verified.'),indent=2)+'\n')
print('Saved21 original Control diagram references; text/BG/priority excluded')
