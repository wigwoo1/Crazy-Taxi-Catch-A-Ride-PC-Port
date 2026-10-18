"""Original Control text IDs and source formatting; no reworded instructions."""
import pathlib,hashlib,struct,json
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
records=[];base=0x49dfe0
for text_id in [1,9,*range(130,146)]:
    at=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    text=rom[at:rom.index(0,at)].decode('ascii')
    records.append(dict(id=text_id,address=hex(at+0x08000000),text=text))
fmt=rom[0x49d668:rom.index(0,0x49d668)].decode('ascii')
doc=dict(records=records,format=fmt,pages=[[130+page*2,131+page*2] for page in range(7)],heading=145,layout_label=144,
         qualification='Original English text. Native composition/footer placement and original format helper validation pending.')
pathlib.Path('outputs/crazy-taxi-native/assets/control-menu/text.json').write_text(json.dumps(doc,indent=2)+'\n')
print(json.dumps(doc,indent=2))
