"""Extract source5460-selected PATH_AI, preserving original16byte points."""
import hashlib,json,pathlib,struct
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
assert rom[0x3409c:0x340a4]==b'PATH_AI\0'
out=pathlib.Path('outputs/crazy-taxi-native/assets/ai');out.mkdir(exist_ok=True)
report=[]
for city,scene in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
    total=struct.unpack_from('<H',rom,scene+16)[0]
    base=struct.unpack_from('<I',rom,scene+52)[0]-0x08000000
    matches=[]
    for i in range(total):
        descriptor=base+i*20
        if rom[descriptor+4:descriptor+16].split(b'\0')[0]==b'PATH_AI':matches.append((i,descriptor))
    if not matches:
        report.append(dict(city=city,found=False));continue
    i,descriptor=matches[0]
    count=struct.unpack_from('<I',rom,descriptor)[0]
    pointer=struct.unpack_from('<I',rom,descriptor+16)[0]
    start=pointer-0x08000000
    assert count>=2 and start>=0 and start+count*16<=len(rom)
    raw=rom[start:start+count*16]
    points=list(struct.iter_unpack('<IiiI',raw))
    distances=[p[0] for p in points]
    item=dict(city=city,found=True,descriptor_index=i,descriptor_address=hex(descriptor+0x08000000),pointer=hex(pointer),count=count,
        raw_sha256=hashlib.sha256(raw).hexdigest(),first_distance=distances[0],last_distance=distances[-1],
        increasing=all(a<b for a,b in zip(distances,distances[1:])),matches=len(matches))
    (out/f'{city}-path.cta').write_bytes(b'CTA1'+struct.pack('<I',count)+raw)
    def signed(value):
        return (value+2**31)%2**32-2**31
    reference=bytearray();segment=0
    for query in range(65536):
        while query>points[segment+1][0]:segment+=1
        a,b=points[segment:segment+2]
        ratio=((query-a[0])*4096)//(b[0]-a[0])
        coords=[signed(a[axis]+(signed((b[axis]-a[axis])*ratio)//4096)) for axis in (1,2)]
        reference+=struct.pack('<ii',*coords)
    pathlib.Path(f'outputs/crazy-taxi-native/reports/{city}-ai-path-reference.bin').write_bytes(reference)
    (out/f'{city}-path.json').write_text(json.dumps(dict(rom_sha256=expected,**item),indent=2)+'\n')
    report.append(item)
pathlib.Path('outputs/crazy-taxi-native/reports/ai-path-extraction.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
