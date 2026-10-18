"""Extract source HUD group0 font, registered at0800ad48..ad50.

Source renderer0800f930 maps character minus32 to descriptor index.
Palette selection/layout remains subject to runtime verification.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from png_writer import write_png
from extract_guidance import EXPECTED

def extract(rom, output):
    if hashlib.sha256(rom).hexdigest()!=EXPECTED:
        raise ValueError('Unsupported ROM revision')
    base=0x70e50
    group=base+struct.unpack_from('<I',rom,base+4)[0]
    count,bits,tiles,start,palettes=struct.unpack_from('<5I',rom,group)
    if (count,bits,tiles,start,palettes)!=(224,16,109,0,4):
        raise ValueError('Unexpected font group')
    palette_start=group+20+count*4
    pixel_start=palette_start+palettes*32
    # 16x16 cells retain complete source tile bounds, including descenders.
    atlas=bytearray(256*896*4)
    frames=bytearray()
    descriptors=[]
    for bank in range(palettes):
        values=struct.unpack_from('<16H',rom,palette_start+bank*32)
        colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(values)]
        for index in range(count):
            address=base+struct.unpack_from('<I',rom,group+20+index*4)[0]
            tx,ty,ox,oy,cropx,cropy,width,height,tile=struct.unpack_from('<9H',rom,address)
            if tx>2 or ty>2 or tile+tx*ty>tiles:
                raise ValueError('Font descriptor outside source tiles')
            pixels=bytearray(16*16*4)
            for y in range(ty*8):
                for x in range(tx*8):
                    offset=pixel_start+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
                    nibble=(rom[offset]>>(4*(x&1)))&15
                    color=colors[nibble]
                    pixels[(y*16+x)*4:(y*16+x+1)*4]=color
                    sx=(index%16)*16+x;sy=(bank*14+index//16)*16+y
                    dst=(sy*256+sx)*4
                    atlas[dst:dst+4]=color if nibble else bytes((40,40,40,255))
            frames.extend(pixels)
            if bank==0:
                descriptors.append({'character_byte':(index+32)&255,'index':index,'address':hex(address+0x8000000),
                    'tiles':[tx,ty],'offset':[ox,oy],'crop':[cropx,cropy],'bounds':[width,height],'tile':tile})
    output.mkdir(parents=True,exist_ok=True)
    (output/'font.rgba').write_bytes(frames)
    write_png(output/'font-atlas.png',256,896,atlas)
    report={'rom_sha256':EXPECTED,'registration':'0x0800ad48','group':hex(group+0x8000000),
        'pixel_source':hex(pixel_start+0x8000000),'palette_source':hex(palette_start+0x8000000),
        'format':'RGBA8, palette-major,224 glyphs per bank,16x16 cells; index0 transparency provisional',
        'pixels_sha256':hashlib.sha256(frames).hexdigest(),'glyphs':descriptors,
        'kerning_source':'0x08415bbc','kerning_count':224,
        'kerning_hex':rom[0x415bbc:0x415bbc+2+224*224].hex()}
    (output/'font.json').write_text(json.dumps(report,indent=2)+'\n')
    # Compact native metrics preserve source16-bit descriptor fields and
    # unsigned pair table; original normal registration has spacing1.
    metrics=bytearray(b'CTF1')
    metrics.extend(struct.pack('<HH',count,1))
    for glyph in descriptors:
        metrics.extend(struct.pack('<9H',*glyph['tiles'],*glyph['offset'],*glyph['crop'],*glyph['bounds'],glyph['tile']))
    if struct.unpack_from('<H',rom,0x415bbc)[0]!=count:
        raise ValueError('Unexpected original kerning count')
    metrics.extend(rom[0x415bbe:0x415bbe+count*count])
    (output/'font-metrics.ctf').write_bytes(metrics)
    # Main countdown uses larger group3 sprites directly, not font0 text.
    group3=base+struct.unpack_from('<I',rom,base+16)[0]
    if struct.unpack_from('<5I',rom,group3)!=(26,16,48,269,2):
        raise ValueError('Unexpected original timer group')
    palette3=group3+20+26*4
    pixels3=palette3+64
    clock=bytearray();clock_descriptors=[]
    for bank in range(2):
        values=struct.unpack_from('<16H',rom,palette3+bank*32)
        colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(values)]
        for index in range(26):
            address=base+struct.unpack_from('<I',rom,group3+20+index*4)[0]
            tx,ty,ox,oy,cx,cy,w,h,tile=struct.unpack_from('<9H',rom,address)
            if tx>2 or ty>2 or tile+tx*ty>48:
                raise ValueError('Invalid timer descriptor')
            frame=bytearray(1024)
            for y in range(ty*8):
                for x in range(tx*8):
                    source=pixels3+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
                    color=colors[(rom[source]>>(4*(x&1)))&15]
                    frame[(y*16+x)*4:(y*16+x+1)*4]=color
            clock.extend(frame)
            if bank==0:clock_descriptors.append({'index':index,'tiles':[tx,ty],'offset':[ox,oy],'crop':[cx,cy],'bounds':[w,h],'tile':tile})
    (output/'clock.rgba').write_bytes(clock)
    (output/'clock.json').write_text(json.dumps({'rom_sha256':EXPECTED,'group':'0x080734a8',
        'format':'RGBA8,palette-major,26 frames of16x16,2 banks; index0 transparency provisional',
        'pixels_sha256':hashlib.sha256(clock).hexdigest(),'glyphs':clock_descriptors,
        'source_layout':'0801f3be..1f430 normalmode: decimal ticks/60; x8+12*index,y1; group3 bank0'},indent=2)+'\n')
    cash=bytearray();cash_rows=[]
    # Runtime copies six transition rows per digit into groups5/6.
    for number,count,tiles,source,steps in ((5,10,20,0xc6e8c,6),(6,11,22,0xc7d8c,8)):
        g=base+struct.unpack_from('<I',rom,base+4+number*4)[0]
        if struct.unpack_from('<5I',rom,g)[:3]!=(count,16,tiles):
            raise ValueError('Unexpected cash group')
        pal=g+20+count*4
        # Palette records are allocated per bank, not per sprite group:
        # group5 starts slot9 (+c4), group6 slot10 (+d8).
        # Original801f026/02c replaces those palette pointers.
        runtime_palette=0x290af8 if number==5 else 0x290b40
        values=struct.unpack_from('<16H',rom,runtime_palette)
        colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(values)]
        for frame in range(steps*10+count):
            if frame<steps*10:
                bank=source+frame*64
                tile=0
            else:
                p=base+struct.unpack_from('<I',rom,g+20+(frame-steps*10)*4)[0]
                tx,ty,ox,oy,cx,cy,w,h,tile=struct.unpack_from('<9H',rom,p)
                if (tx,ty,ox,oy)!=(1,2,0,0):raise ValueError('Unexpected cash sprite bounds')
                bank=pal+32
            pixels=bytearray(512)
            for y in range(16):
                for x in range(8):
                    packed=rom[bank+(tile+y//8)*32+(y%8)*4+x//2]
                    pixels[(y*8+x)*4:(y*8+x+1)*4]=colors[(packed>>(4*(x&1)))&15]
            cash.extend(pixels)
        cash_rows.append({'group':number,'source':hex(source+0x8000000),'embedded_palette':hex(pal+0x8000000),
            'runtime_palette':hex(runtime_palette+0x8000000),'palette_slot':9 if number==5 else 10,
            'transition_frames':steps*10,'static_frames':count})
    (output/'cash.rgba').write_bytes(cash)
    (output/'cash.json').write_text(json.dumps({'rom_sha256':EXPECTED,'banks':cash_rows,
        'format':'RGBA8,group5 then6; 60 total/80 pending transition frames then source static frames;8x16',
        'pixels_sha256':hashlib.sha256(cash).hexdigest(),'status':'Original cash art and normal runtime palette pointers; global palette effects remain unported'},indent=2)+'\n')
    labels=[]
    for name,table,width,height in (('total',0x7ec29c,88,24),('pending',0x7ec2ec,80,32)):
        combined=bytearray()
        for active in range(2):
            w,h,address=struct.unpack_from('<HHI',rom,table+active*8)
            start=address-0x8000000
            if (w,h)!= (width,height) or struct.unpack_from('<4H',rom,start-520)!=(w,h,256,0):
                raise ValueError('Unexpected original cash label')
            colors=[]
            for value in struct.unpack_from('<256H',rom,start-512):
                colors.append(bytes([((value>>s)&31)*255//31 for s in (0,5,10)]+[255]))
            pixels=b''.join(colors[i] for i in rom[start:start+w*h])
            combined.extend(pixels)
            write_png(output/f'{name}-label-{active}.png',w,h,pixels)
            labels.append({'name':name,'active':active,'pixel_source':hex(address),'width':w,'height':h})
        (output/f'{name}-labels.rgba').write_bytes(combined)
    (output/'labels.json').write_text(json.dumps({'rom_sha256':EXPECTED,'labels':labels,
        'evidence':'0801f5a0..1f624 chooses English state variant; ARM08004f6c copies opaque8-bit pixels',
        'palette':'Embedded RGB555 source bitmap palette; global software palette effects unported'},indent=2)+'\n')
    return report

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rom',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();result=extract(a.rom.read_bytes(),a.output)
    print('Extracted original224 glyph descriptors and four palette banks.')
