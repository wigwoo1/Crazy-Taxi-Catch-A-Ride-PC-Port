"""Extract original passenger bitmap banks using their embedded palettes.

This preserves source art, not the game's runtime palette/lighting pipeline.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from extract_scene import BASE, EXPECTED
from png_writer import write_png

def extract(data):
    if hashlib.sha256(data).hexdigest()!=EXPECTED:
        raise ValueError('Unsupported ROM revision')
    def region(address,size):
        offset=address-BASE
        if offset<0 or size<0 or offset+size>len(data):raise ValueError('Bitmap reference outside ROM')
        return data[offset:offset+size]
    banks=[]
    for kind in range(8):
        pointers=struct.unpack('<4I',region(0x087ec51c+kind*16,16))
        for stage,address in enumerate(pointers):
            width,height,colors,reserved=struct.unpack('<4H',region(address-520,8))
            if (width,height,colors,reserved)!=(32,[64,256,256,576][stage],256,0):
                raise ValueError('Unexpected passenger bitmap header')
            palette=region(address-512,512)
            rgba=[]
            for i,(color,) in enumerate(struct.iter_unpack('<H',palette)):
                channels=[color&31,(color>>5)&31,(color>>10)&31]
                rgba.append(bytes([(c<<3)|(c>>2) for c in channels]+[255 if i else 0]))
            pixels=region(address,width*height)
            banks.append((kind,stage,address,height//64,palette,b''.join(rgba[p] for p in pixels)))
    return banks

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('rom',type=Path);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
    banks=extract(args.rom.read_bytes());args.output.mkdir(parents=True,exist_ok=True)
    rows=[]
    # Eight kinds, each row contains idle / two gesture banks / boarding frames.
    atlas_width=32*18;atlas_height=64*8;atlas=bytearray(atlas_width*atlas_height*4)
    for kind,stage,address,frames,palette,pixels in banks:
        name=f'passenger-{kind}-stage-{stage}'
        (args.output/(name+'.rgba')).write_bytes(pixels)
        write_png(args.output/(name+'.png'),32,64*frames,pixels)
        first=[0,1,5,9][stage]
        for frame in range(frames):
            for y in range(64):
                source=(frame*64+y)*32*4;target=((kind*64+y)*atlas_width+(first+frame)*32)*4
                for x in range(32):
                    sample=pixels[source+x*4:source+x*4+4]
                    shade=48 if ((first+frame)*32+x)//8%2 == (kind*64+y)//8%2 else 64
                    atlas[target+x*4:target+x*4+4]=sample if sample[3] else bytes([shade,shade,shade,255])
        rows.append({'kind':kind,'stage':stage,'pixels_address':hex(address),'width':32,'frame_height':64,'frames':frames,
                     'rgba_file':name+'.rgba','rgba_sha256':hashlib.sha256(pixels).hexdigest(),
                     'palette_sha256':hashlib.sha256(palette).hexdigest()})
    write_png(args.output/'passenger-source-atlas.png',atlas_width,atlas_height,atlas)
    (args.output/'sprites.json').write_text(json.dumps({'rom_sha256':EXPECTED,
        'status':'Original linear 8-bit bitmap art decoded with embedded RGB555 palette; runtime palette mapping, lighting and sprite placement unverified.',
        'evidence':'087ec51c bank table;0802318e..231aa frame stride2048; record descriptor bytes32,64,5,6; bitmap header512-byte palette before pixels.',
        'transparency':'Index zero treated as transparent for source preview; verify passenger rasterizer keying before integration.',
        'banks':rows},indent=2)+'\n')
    print(f'Extracted {len(banks)} banks, {sum(b[3] for b in banks)} frames using embedded palettes.')
if __name__=='__main__':main()
