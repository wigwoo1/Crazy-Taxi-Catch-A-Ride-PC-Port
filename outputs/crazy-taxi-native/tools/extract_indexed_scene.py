"""Preserve original scene texture indices and descriptor fields, without RGBA conversion."""
import hashlib,json,pathlib,struct
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(r).hexdigest()==expected
out=pathlib.Path('outputs/crazy-taxi-native/assets/indexed-scenes');out.mkdir(exist_ok=True)
for name,scene in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
    count=struct.unpack_from('<H',r,scene+6)[0]
    address=struct.unpack_from('<I',r,scene+32)[0];base=address-0x08000000
    blob=bytearray();rows=[]
    for index in range(count):
        w,h,lw,lh,pointer,extra=struct.unpack_from('<4B2I',r,base+index*12)
        offset=pointer-0x08000000
        assert w and h and 0<=offset<=len(r)-w*h
        pixels=r[offset:offset+w*h]
        rows.append(dict(index=index,width=w,height=h,log_width=lw,log_height=lh,
            descriptor=hex(address+index*12),pixels=hex(pointer),extra_word=hex(extra),
            byte_offset=len(blob),byte_length=len(pixels),sha256=hashlib.sha256(pixels).hexdigest(),
            power_of_two=(w==(1<<lw) and h==(1<<lh))))
        blob.extend(pixels)
    (out/(name+'.indices')).write_bytes(blob)
    descriptors=b''.join(struct.pack('<4B2I',t['width'],t['height'],t['log_width'],
        t['log_height'],t['byte_offset'],t['byte_length']) for t in rows)
    (out/(name+'.cti')).write_bytes(b'CTI1'+struct.pack('<2I',count,len(blob))+descriptors+blob)
    (out/(name+'.json')).write_text(json.dumps(dict(rom_sha256=expected,
        scene=name,background_descriptor=struct.unpack_from('<h',r,scene+20)[0],texture_count=count,blob_sha256=hashlib.sha256(blob).hexdigest(),textures=rows,
        qualification='Original level0 raw indices only. Extra descriptor word preserved, not interpreted as verified mipmap pointer.'),indent=2)+'\n')
    print(name,count,'textures',len(blob),'original indexed bytes',
        sum(x['power_of_two'] for x in rows),'power-of-two descriptors')
