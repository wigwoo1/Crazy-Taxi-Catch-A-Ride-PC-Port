"""Original English normal main/mode labels, source IDs from 1b1d4 callers."""
import hashlib
import json
import pathlib
import struct
from extract_guidance import EXPECTED
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
base=0x49dfe0
out=pathlib.Path('outputs/crazy-taxi-native/assets/normal-menu')
out.mkdir(exist_ok=True)
menus=[]
for name,heading,count,owner in [('main',40,5,0x0200387d),('mode',50,4,0x0200387e)]:
    entries=[]
    for text_id in range(heading,heading+count+1):
        address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
        text=rom[address:rom.index(0,address)]
        assert text and len(text)<128 and all(32<=byte<127 for byte in text)
        entries.append(dict(id=text_id,address=hex(address+0x08000000),text=text.decode('ascii')))
    menus.append(dict(name=name,owner=hex(owner),heading=entries[0],choices=entries[1:],
        heading_anchor=[4,6],heading_font=0,list_font=1,list_x=72,
        list_y=(160-(count*16+44))//2+28,list_step_y=16,
        qualification='English source table; settled viewport240x160. Shared1b1d4 heading4/6, main/mode row x16+112/2=72. Height owner07544 and width07542 must remain explicit in runtime. Selection highlight/border/secondary text still require recovery.'))
document=dict(rom_sha256=EXPECTED,background_descriptor='0x080340d8',
    background_asset='../profile/background.indices',container='0x08034634',menus=menus)
document['footer']=[]
for text_id in (0,1):
    address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    value=rom[address:rom.index(0,address)].decode('ascii')
    document['footer'].append(dict(id=text_id,text=value,address=hex(address+0x08000000)))
print('Footer:',document['footer'])
(out/'text.json').write_text(json.dumps(document,indent=2)+'\n')
for menu in menus:
    print(menu['name'],menu['heading']['text'],[entry['text'] for entry in menu['choices']])
