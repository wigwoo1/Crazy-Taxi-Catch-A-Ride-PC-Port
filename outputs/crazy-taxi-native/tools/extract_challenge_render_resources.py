"""Preserve original scene texture indices and descriptor fields, without RGBA conversion."""
import hashlib,json,pathlib,struct
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(r).hexdigest()==expected
out=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-worlds');out.mkdir(exist_ok=True)
for mode in range(9):
    name=f'challenge{mode}';scene=struct.unpack_from('<I',r,0x4a4f7c+24*mode+16)[0]-0x08000000
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

from extract_world_special import extract
from extract_scene import SCENES
special=[]
for mode in range(9):
    name=f'challenge{mode}';SCENES[name]=struct.unpack_from('<I',r,0x4a4f7c+24*mode+16)[0]
    bundle,info=extract(r,name);p=out/(name+'-special.cts');p.write_bytes(bundle);assert p.read_bytes()==bundle
    special.append(info)
    indexed=(out/(name+'.cti')).read_bytes();count,length=struct.unpack_from('<2I',indexed,4)
    assert len(indexed)==12+12*count+length
    rows=json.loads((out/(name+'.json')).read_text())['textures']
    for row in rows:
        off=int(row['pixels'],16)-0x08000000;start=12+12*count+row['byte_offset'];size=row['byte_length']
        assert indexed[start:start+size]==r[off:off+size]
    print('PASS',name,'special',info['count'],'and',count,'indexed descriptors re-read verified')
(out/'special-manifest.json').write_text(json.dumps(special,indent=2)+'\n')
assert (out/'challenge0.cti').read_bytes()==pathlib.Path('outputs/crazy-taxi-native/assets/indexed-scenes/startup.cti').read_bytes()
assert (out/'challenge0-special.cts').read_bytes()==pathlib.Path('outputs/crazy-taxi-native/assets/startup-special.cts').read_bytes()
print('PASS Jump indexed textures and special scenery byte-identical to original startup exports; no live rendering claim.')

prefixes=[]
for mode in range(9):
    scene=struct.unpack_from('<I',r,0x4a4f7c+24*mode+16)[0]-0x08000000
    address=struct.unpack_from('<I',r,scene+28)[0];offset=address-0x08000000-28
    assert 0<=offset<=len(r)-28
    data=r[offset:offset+28];p=out/(f'challenge{mode}-material-prefix.bin');p.write_bytes(data);assert p.read_bytes()==data
    prefixes.append(dict(submode=mode,source=hex(address-28),sha256=hashlib.sha256(data).hexdigest(),qualification='Original signed material index -1 backing bytes only; later palette mutations not established'))
(out/'material-prefix-manifest.json').write_text(json.dumps(prefixes,indent=2)+'\n')
assert (out/'challenge0-material-prefix.bin').read_bytes()==pathlib.Path('outputs/crazy-taxi-native/assets/material-prefix/startup.bin').read_bytes()
print('PASS nine original material prefixes preserved; Jump matches startup prefix.')

for mode in range(9):
    scene=struct.unpack_from('<I',r,0x4a4f7c+24*mode+16)[0]-0x08000000
    address=struct.unpack_from('<I',r,scene+24)[0]-0x08000000
    assert 0<=address<=len(r)-512
    p=out/(f'challenge{mode}-palette.rgb555');p.write_bytes(r[address:address+512]);assert p.read_bytes()==r[address:address+512]
print('PASS original nine scene palettes, 256 RGB555 entries each.')
