"""Extract original taxi angle sprites; ROM is input data, never executed.

080155ac..b6 selects one of64 views;0801591c..594c loads two banks via
084a5514 character records (left,right,palette). Each view has two64x64
4bpp tiled halves, combined left-to-right. Dimensions verified visually
against all64 decoded views; actor metadata/camera integration is pending.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from png_writer import write_png

EXPECTED='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'

def region(rom,address,size):
    offset=address-0x08000000
    if offset<0 or size<0 or offset+size>len(rom):
        raise ValueError('Sprite reference outside ROM')
    return rom[offset:offset+size]

def decode_frame(left,right,palette):
    if len(left)!=2048 or len(right)!=2048 or len(palette)!=32:
        raise ValueError('Invalid sprite data size')
    colors=[]
    for index,color in enumerate(struct.unpack('<16H',palette)):
        colors.append(bytes([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if index else 0]))
    pixels=bytearray(128*64*4)
    for side,bank in enumerate((left,right)):
        for y in range(64):
            for x in range(64):
                offset=((y//8)*8+x//8)*32+(y%8)*4+(x%8)//2
                index=(bank[offset]>>(4*(x&1)))&15
                dst=(y*128+side*64+x)*4
                pixels[dst:dst+4]=colors[index]
    return pixels

def extract(rom,output):
    if hashlib.sha256(rom).hexdigest()!=EXPECTED:
        raise ValueError('Expected the verified USA ROM revision')
    output.mkdir(parents=True,exist_ok=True)
    root=0x70e50
    group=root+struct.unpack_from('<I',rom,root+8)[0]
    header=struct.unpack_from('<5I',rom,group)
    if header!=(2,16,128,109,1):
        raise ValueError('Unexpected original taxi sprite group')
    descriptors=[]
    for half in range(2):
        address=root+struct.unpack_from('<I',rom,group+20+half*4)[0]
        descriptor=struct.unpack_from('<9H',rom,address)
        if descriptor[:2]!=(8,8) or descriptor[6:8]!=(64,64):
            raise ValueError('Unexpected original taxi half dimensions')
        descriptors.append({'address':hex(address+0x08000000),'values':list(descriptor)})
    report={'rom_sha256':EXPECTED,'frame_width':128,'frame_height':64,'frames':64,
            'format':'RGBA8, frame-major, row-major, index0 transparent','characters':[],
            'source_group':hex(group+0x08000000),'source_group_header':list(header),
            'half_descriptors':descriptors,'source_half_stride':8*8*32}
    for character in range(4):
        left,right,palette=struct.unpack('<III',region(rom,0x084a5514+character*12,12))
        frames=bytearray();sheet=bytearray(1024*512*4)
        for frame in range(64):
            pixels=decode_frame(region(rom,left+frame*2048,2048),region(rom,right+frame*2048,2048),region(rom,palette,32))
            frames.extend(pixels)
            for y in range(64):
                for x in range(128):
                    src=(y*128+x)*4;px=(frame%8)*128+x;py=(frame//8)*64+y
                    dst=(py*1024+px)*4
                    if pixels[src+3]:sheet[dst:dst+4]=pixels[src:src+4]
                    else:
                        gray=32 if ((px//8+py//8)&1) else 42
                        sheet[dst:dst+4]=bytes((gray,gray,gray,255))
        name=f'taxi-{character}'
        (output/f'{name}.rgba').write_bytes(frames)
        write_png(output/f'{name}-angles.png',1024,512,sheet)
        report['characters'].append({'index':character,'left':hex(left),'right':hex(right),'palette':hex(palette),
            'pixels':f'{name}.rgba','preview':f'{name}-angles.png','sha256':hashlib.sha256(frames).hexdigest()})
    (output/'taxis.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    return report

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('rom',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();extract(args.rom.read_bytes(),args.output)
    print('Extracted4 original taxi sets,64 angles each; native RGBA data and checkerboard previews.')
