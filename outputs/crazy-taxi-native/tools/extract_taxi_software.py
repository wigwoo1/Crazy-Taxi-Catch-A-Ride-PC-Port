"""Original155f6..15636 software taxi bank; preserve raw palette/indices."""
import argparse,hashlib,json,struct
from pathlib import Path
from png_writer import write_png
EXPECTED='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
def extract(rom,out):
    if hashlib.sha256(rom).hexdigest()!=EXPECTED:raise ValueError('Unsupported ROM')
    base=0x20fd00
    if struct.unpack_from('<4H',rom,base)!=(64,2048,256,0):raise ValueError('Unexpected bitmap header')
    palette=rom[base+8:base+520];indices=rom[base+520:base+520+64*32*64]
    if len(indices)!=131072:raise ValueError('Truncated original bank')
    colors=[]
    for i,(c,) in enumerate(struct.iter_unpack('<H',palette)):
        colors.append(bytes([(((c>>s)&31)<<3)|(((c>>s)&31)>>2) for s in (0,5,10)]+[255 if i else 0]))
    atlas=bytearray(512*256*4)
    for f in range(64):
        for y in range(32):
            row=b''.join(colors[i] for i in indices[f*2048+y*64:f*2048+(y+1)*64])
            dst=((f//8*32+y)*512+f%8*64)*4;atlas[dst:dst+256]=row
    out.mkdir(parents=True,exist_ok=True)
    (out/'taxi-software.ctx').write_bytes(b'CTX1'+struct.pack('<HHI',64,32,64)+palette+indices)
    write_png(out/'taxi-software-atlas.png',512,256,atlas)
    report=dict(rom_sha256=EXPECTED,header_address='0820fd00',pixels_address='0820ff08',width=64,height=32,frames=64,
        palette_sha256=hashlib.sha256(palette).hexdigest(),indices_sha256=hashlib.sha256(indices).hexdigest(),
        status='Original software taxi bank. Embedded-palette preview only; original branch ownership and runtime palette must be verified before rendering.')
    (out/'taxi-software.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report))
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rom',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();extract(a.rom.read_bytes(),a.output)
