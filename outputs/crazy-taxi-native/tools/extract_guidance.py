"""Original guidance frames:802072a source, group2 descriptor, five palettes.
No ROM code execution. Native HUD placement and route-hint selection pending.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from png_writer import write_png
from extract_scene import SCENES

EXPECTED='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
def extract(rom, output):
    if hashlib.sha256(rom).hexdigest()!=EXPECTED:
        raise ValueError('Expected verified USA ROM')
    base=0x70e50
    group=base+struct.unpack_from('<I',rom,base+12)[0]
    count,bits=struct.unpack_from('<II',rom,group)
    frame=base+struct.unpack_from('<I',rom,group+20)[0]
    tiles_x,tiles_y=struct.unpack_from('<HH',rom,frame)
    if (count,bits,tiles_x,tiles_y)!=(1,16,8,4):
        raise ValueError('Unexpected original guidance descriptor')
    width,height=tiles_x*8,tiles_y*8
    size=width*height//2
    output.mkdir(parents=True,exist_ok=True)
    sheet=bytearray(width*8*height*20*4)
    all_pixels=bytearray()
    for palette in range(5):
        values=struct.unpack_from('<16H',rom,0x290520+palette*32)
        colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(values)]
        for direction in range(32):
            bank=rom[0xbee8c+direction*size:0xbee8c+(direction+1)*size]
            pixels=bytearray(width*height*4)
            for y in range(height):
                for x in range(width):
                    index=(bank[((y//8)*tiles_x+x//8)*32+(y%8)*4+(x%8)//2]>>(4*(x&1)))&15
                    color=colors[index]
                    pixels[(y*width+x)*4:(y*width+x+1)*4]=color
                    sx=(direction%8)*width+x
                    sy=(palette*4+direction//8)*height+y
                    gray=32 if ((sx//8+sy//8)&1) else 48
                    offset=(sy*width*8+sx)*4
                    sheet[offset:offset+4]=color if index else bytes((gray,gray,gray,255))
            all_pixels.extend(pixels)
    (output/'guidance.rgba').write_bytes(all_pixels)
    write_png(output/'guidance-atlas.png',width*8,height*20,sheet)
    report={'rom_sha256':EXPECTED,'width':width,'height':height,'directions':32,'palettes':5,
            'pixel_source':'0x080bee8c','palette_source':'0x08290520','descriptor':hex(frame+0x8000000),
            'format':'RGBA8 palette-major, frame-major; index0 transparent provisional',
            'pixels_sha256':hashlib.sha256(all_pixels).hexdigest()}
    (output/'guidance.json').write_text(json.dumps(report,indent=2)+'\n')
    routes={}
    for scene,address in SCENES.items():
        header=address-0x8000000
        path_count=struct.unpack_from('<H',rom,header+16)[0]
        paths=struct.unpack_from('<I',rom,header+52)[0]-0x8000000
        points=[]
        for index in range(path_count):
            row=paths+index*20
            if rom[row+4:row+10]!=b'ARROW\0':
                continue
            count=struct.unpack_from('<I',rom,row)[0]
            start=struct.unpack_from('<I',rom,row+16)[0]-0x8000000
            if not 0<=count<=80 or start<0 or start+count*16>len(rom):
                raise ValueError('Invalid original ARROW route')
            points=list(struct.iter_unpack('<4i',rom[start:start+count*16]))
            break
        routes[scene]={'points_raw':points,'count':len(points)}
        with (output/(scene+'.ctg')).open('wb') as out:
            out.write(struct.pack('<4sI',b'CTG1',len(points)))
            for point in points:out.write(struct.pack('<4i',*point))
    (output/'routes.json').write_text(json.dumps(routes,indent=2)+'\n')
    return report
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('rom',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();print(json.dumps(extract(args.rom.read_bytes(),args.output)))
