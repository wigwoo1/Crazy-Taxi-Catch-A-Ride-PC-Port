import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
tables={}
for name,base in [('sound',0x7ec658),('music',0x7ec048)]:
    values=[]
    for i in range(1024):
        value=struct.unpack_from('<I',rom,base+i*4)[0]
        values.append(value)
        if value==0:break
    assert values[-1]==0
    tables[name]=dict(address=hex(base+0x08000000),count=len(values)-1,pointers=[hex(x) for x in values])
document=dict(rom_sha256=EXPECTED,tables=tables)
document['test_formats']=[rom[off:rom.index(0,off)].decode('ascii') for off in (0x49d69c,0x49d6a0)]
document['options_time_format']=rom[0x49d698:rom.index(0,0x49d698)].decode('ascii')
pathlib.Path('outputs/crazy-taxi-native/reports/audio-test-tables.json').write_text(json.dumps(document,indent=2)+'\n')
for name,table in tables.items():print(name,table['count'],'nonzero entries then null sentinel')
print('Original test formats:',document['test_formats'])
print('Original Options Time format:',document['options_time_format'])
