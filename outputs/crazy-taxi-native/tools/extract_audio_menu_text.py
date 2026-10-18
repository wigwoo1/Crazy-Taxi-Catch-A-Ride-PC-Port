"""Original English audio submenu labels and caller-owned count."""
import pathlib,hashlib,struct,json
from extract_guidance import EXPECTED
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
entries=[]
for text_id in range(32,37):
    base=0x49dfe0;address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    value=rom[address:rom.index(0,address)]
    assert value and all(32<=x<127 for x in value)
    entries.append(dict(id=text_id,address=hex(address+0x08000000),text=value.decode('ascii')))
document=dict(rom_sha256=EXPECTED,owner='0x02003887',caller='0x08017a24',
    count=4,count_store='0x08017a36..0x08017a3a',heading=entries[0],choices=entries[1:],
    heading_anchor=[4,6],heading_font=0,list_font=1,
    qualification='Four source rows. English label37 Exit is not counted by this caller. B cancellation returns to options. Layout, volume bars and test values need complete renderer recovery.')
document['footer']=[]
for text_id in (9,1):
    base=0x49dfe0;address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    document['footer'].append(dict(id=text_id,text=rom[address:rom.index(0,address)].decode('ascii')))
out=pathlib.Path('outputs/crazy-taxi-native/assets/normal-menu/audio-text.json')
out.write_text(json.dumps(document,indent=2)+'\n')
print('Recovered audio menu:',[entry['text'] for entry in document['choices']])
print('Original footer:',document['footer'])
options=[]
for text_id in range(60,66):
    base=0x49dfe0;address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    value=rom[address:rom.index(0,address)]
    assert value and all(32<=x<127 for x in value)
    options.append(dict(id=text_id,address=hex(address+0x08000000),text=value.decode('ascii')))
parent=dict(rom_sha256=EXPECTED,owner='0x0200387f',caller='0x08017918',
    count=5,count_store='0x0801791c..0x08017920',heading=options[0],choices=options[1:],
    audio_row=3,audio_callback='0x08017a25',cancel_callback='0x080174dd',
    qualification='Source count five. Branch for row5 exists but is outside the normal row range. Settings controls and live flow require integration.')
pathlib.Path('outputs/crazy-taxi-native/assets/normal-menu/options-text.json').write_text(json.dumps(parent,indent=2)+'\n')
print('Recovered Options menu:',[entry['text'] for entry in parent['choices']])
