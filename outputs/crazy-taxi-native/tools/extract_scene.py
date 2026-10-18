"""Extract the startup-referenced scene geometry for local native rendering.

Geometry layout inferred from ROM tables; textures and scene identity unverified.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from png_writer import write_png

EXPECTED = '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
BASE = 0x08000000
SCENES = {'city1': 0x087556cc, 'city2': 0x08755734, 'startup': 0x087ebb90}


def extract(data, scene='city1'):
    if hashlib.sha256(data).hexdigest() != EXPECTED:
        raise ValueError('Unsupported ROM revision')
    def region(address, length):
        offset = address - BASE
        if offset < 0 or length < 0 or offset + length > len(data):
            raise ValueError('Scene reference outside ROM')
        return data[offset:offset+length]
    scene_address = SCENES[scene]
    header = region(scene_address, 104)
    version, _, materials, textures, vertex_count, face_count = struct.unpack_from('<6H', header)
    if version != 4 or not 0 < vertex_count <= 65535 or not 0 < face_count <= 12288:
        raise ValueError('Unexpected scene header')
    vertex_address, face_address = struct.unpack_from('<2I', header, 0x24)
    vertices = list(struct.iter_unpack('<4i', region(vertex_address, vertex_count * 16)))
    bounds = [(min(v[k] for v in vertices), max(v[k] for v in vertices)) for k in range(3)]
    faces = []
    for raw in struct.iter_unpack('<12H', region(face_address, face_count * 24)):
        indices = list(raw[4:8])
        if indices[-1] == 65535:
            indices.pop()
        if any(i >= vertex_count for i in indices) or len(set(indices)) != len(indices):
            raise ValueError('Invalid polygon indices')
        if (raw[0] & 255) >= materials:
            raise ValueError('Invalid material index')
        faces.append((indices, raw[0]))
    metadata = {'status': 'Recovered static geometry only; not playable. Scene identity and material/texture semantics unverified.',
                'rom_sha256': EXPECTED, 'scene_label': scene, 'scene_address': hex(scene_address), 'vertex_address': hex(vertex_address),
                'face_address': hex(face_address), 'vertex_count': vertex_count, 'polygon_count': face_count,
                'triangles': sum(len(f[0]) == 3 for f in faces), 'quads': sum(len(f[0]) == 4 for f in faces),
                'material_count': materials, 'texture_count': textures, 'raw_bounds': bounds,
                'fourth_vertex_word': 'Preserved in source ROM; not interpreted or used by this geometry viewer.',
                'display_transform': 'Center raw XYZ coordinates and scale largest extent to 2. Camera is a viewer convention, not recovered game behavior.'}
    return vertices, faces, metadata


def appearance(data, scene):
    if hashlib.sha256(data).hexdigest() != EXPECTED:
        raise ValueError('Unsupported ROM revision')
    header=SCENES[scene]-BASE
    material_count,texture_count=struct.unpack_from('<2H',data,header+4)
    palette_address,material_address,texture_address=struct.unpack_from('<3I',data,header+24)
    def region(address,size):
        off=address-BASE
        if off<0 or off+size>len(data): raise ValueError('Appearance reference outside ROM')
        return data[off:off+size]
    def rgb(color):
        channels=[color&31,(color>>5)&31,(color>>10)&31]
        return bytes([(c<<3)|(c>>2) for c in channels]+[255])
    palette=[rgb(c[0]) for c in struct.iter_unpack('<H',region(palette_address,512))]
    textures=[]
    for i in range(texture_count):
        w,h,logw,logh,address,reserved=struct.unpack('<4B2I',region(texture_address+i*12,12))
        if not w or not h or logw>8 or logh>8:
            raise ValueError('Invalid texture dimensions')
        pixels=region(address,w*h)
        # The original keyed rasterizer skips texel index zero (08001d30..08001d38).
        textures.append((w,h,b''.join(palette[p] if p else palette[p][:3]+b'\0' for p in pixels)))
    materials=[]
    for i in range(material_count):
        row=region(material_address+i*28,28)
        texture_index=row[16]
        if texture_index != 255 and texture_index>=texture_count:
            raise ValueError('Invalid material texture reference')
        # First palette shade is used for flat materials; original lighting is not ported.
        materials.append((texture_index,palette[row[0]],row[20]))
    return textures,materials


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--scene', choices=SCENES, default='city1')
    args = parser.parse_args()
    data=args.rom.read_bytes()
    vertices, faces, metadata = extract(data, args.scene)
    textures,materials=appearance(data,args.scene)
    # 0802c558 chooses the first palette entry with minimum RGB555 Manhattan
    # distance. Scene initialization at 0802af20 and 0802b462 requests black.
    palette_address=struct.unpack_from('<I',data,SCENES[args.scene]-BASE+24)[0]
    colors=struct.unpack_from('<256H',data,palette_address-BASE)
    clear_index=min(range(256),key=lambda i:sum((colors[i]>>shift)&31 for shift in (0,5,10)))
    clear_rgb=[((colors[clear_index]>>shift)&31) for shift in (0,5,10)]
    clear_rgba=bytes([(c<<3)|(c>>2) for c in clear_rgb]+[255])
    metadata['framebuffer_clear']={'routine':'0802c558','requested_rgb555':[0,0,0],
        'palette_address':hex(palette_address),'palette_index':clear_index,
        'rgb555':hex(colors[clear_index]),'rgba8':list(clear_rgba),
        'scope':'Initial framebuffer fill; additional background drawing remains unverified.'}
    sky_index=struct.unpack_from('<H',data,SCENES[args.scene]-BASE+20)[0]
    if sky_index>=len(textures) or textures[sky_index][:2]!=(240,160):
        raise ValueError('Unsupported original sky descriptor')
    metadata['sky']={'routine':'08002fe0','texture_index':sky_index,
        'sampling':'Opaque 240x160 bitmap, yaw scroll in four-pixel steps, pitch row offset.'}
    sky_fill=textures[sky_index][2][(159*240)*4:(159*240)*4+3]+bytes([255])
    metadata['sky']['lower_fill']={'routine':'0802b4b6..0802b4e0',
        'source_pixel':[0,159],'rgba8':list(sky_fill)}
    metadata['status']='Recovered static geometry and base textures. Not playable; original lighting, animation, special flags and scene behavior remain unported.'
    metadata['textured_polygons']=sum(materials[flags&255][0]!=255 for _,flags in faces)
    metadata['flat_polygons']=len(faces)-metadata['textured_polygons']
    metadata['hidden_polygons']=sum(bool(materials[flags&255][2]&9) for _,flags in faces)
    metadata['visibility_rule']='Original08003248..08003260 skips polygons when material byte20 &9 is nonzero.'
    args.output.mkdir(parents=True, exist_ok=True)
    bounds = metadata['raw_bounds']
    center = [(lo+hi)/2 for lo,hi in bounds]
    scale = 2/max(hi-lo for lo,hi in bounds)
    normalized = [tuple((v[k]-center[k])*scale for k in range(3)) for v in vertices]
    with (args.output/(args.scene+'.ctm')).open('wb') as stream:
        stream.write(struct.pack('<4s4I', b'CTM6', len(vertices), len(faces),len(textures),len(materials)))
        stream.write(clear_rgba)
        stream.write(struct.pack('<I',sky_index))
        stream.write(sky_fill)
        for v in normalized:
            stream.write(struct.pack('<3f', *v))
        face_address=int(metadata['face_address'],16)-BASE
        for face_index,(indices, flags) in enumerate(faces):
            stream.write(struct.pack('<6H', len(indices), flags, *(indices+[65535]*(4-len(indices)))))
            uv=data[face_address+face_index*24+16:face_address+face_index*24+24]
            texture_index=materials[flags&255][0]
            w,h=textures[texture_index][:2] if texture_index!=255 else (1,1)
            stream.write(struct.pack('<8f',*[value/(w if k%2==0 else h) for k,value in enumerate(uv)]))
        for texture_index,color,flags in materials:
            stream.write(struct.pack('<I4sI',texture_index,color,flags))
        for w,h,pixels in textures:
            stream.write(struct.pack('<II',w,h));stream.write(pixels)
    with (args.output/(args.scene+'.obj')).open('w') as stream:
        stream.write('# ROM-derived geometry; raw coordinate units. Not a playable port.\n')
        for vertex in vertices:
            stream.write('v %d %d %d\n' % vertex[:3])
        for indices,flags in faces:
            stream.write('f '+' '.join(str(i+1) for i in indices)+'\n')
    (args.output/(args.scene+'-metadata.json')).write_text(json.dumps(metadata,indent=2)+'\n')
    # 8 columns of 128-pixel cells; native pixels decoded directly through the ROM palette.
    cell=132; atlas_width=cell*8;atlas_height=cell*((len(textures)+7)//8)
    atlas=bytearray(bytes([20,24,32,255])*atlas_width*atlas_height)
    for number,(w,h,pixels) in enumerate(textures):
        scale=min(128/w,128/h);dw=max(1,int(w*scale));dh=max(1,int(h*scale))
        x0=(number%8)*cell+2;y0=(number//8)*cell+2
        for y in range(dh):
            for x in range(dw):
                source=((y*h//dh)*w+x*w//dw)*4; target=((y0+y)*atlas_width+x0+x)*4
                atlas[target:target+4]=pixels[source:source+4]
    write_png(args.output/(args.scene+'-texture-atlas.png'),atlas_width,atlas_height,atlas)
    print(json.dumps(metadata,indent=2))


if __name__ == '__main__':
    main()
