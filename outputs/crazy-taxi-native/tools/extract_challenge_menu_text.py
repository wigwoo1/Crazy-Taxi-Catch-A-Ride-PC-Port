"""ROM-derived English challenge text; labels do not prove rendering layout."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()==EXPECTED
base=0x49dfe0
entries=[]
for text_id in list(range(110,125))+list(range(170,200))+list(range(209,317)):
 address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
 end=rom.index(0,address);value=rom[address:end]
 assert len(value)<4096 and all(x in (9,10,13) or 32<=x<127 for x in value)
 entries.append({'id':text_id,'address':hex(address+0x08000000),'bytes_hex':value.hex(),'text':value.decode('ascii')})
alphabet=rom[0x49d624:0x49d624+42]
assert alphabet==b'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.&!?@ '
out=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-menu');out.mkdir(exist_ok=True)
report={'rom_sha256':EXPECTED,'language':'original English table at0849DFE0','entries':entries,'name_alphabet':alphabet.decode('ascii'),'scope':'Source text only; challenge panel/briefing layout, font flags and rendering not yet integrated.'}
report['briefings']=[{'submode':mode,'title_id':113+mode,'rules_ids':[209+4*mode+line for line in range(4)],'advice_ids':[[245+8*mode+4*blink+line for line in range(4)] for blink in range(2)]} for mode in range(9)]
report['briefing_layout']={'title_anchor':[4,6],'font':1,'body_center_x':'viewport width07542/2','body_y':[56,64,72,80],'body_text_flags':4,'page_header_ids':[211,212],'page_header_y':44,'footer_request':[[5,1],[6,1]],'scope':'Exact source1AB74..1AD58 request mapping; header IDs211/212 resolve to spaces in this English table, unresolved original intent.'}
a=0x49d688;format_bytes=rom[a:rom.index(0,a)];report['title_format']={'address':'0x0849d688','bytes_hex':format_bytes.hex(),'text':format_bytes.decode('ascii')}
(out/'text.json').write_text(json.dumps(report,indent=2)+'\n')
# Verify saved byte records against their original source addresses.
loaded=json.loads((out/'text.json').read_text())
for e in loaded['entries']:
 a=int(e['address'],16)-0x8000000;data=bytes.fromhex(e['bytes_hex']);assert rom[a:a+len(data)]==data and rom[a+len(data)]==0
for e in entries[:15]:print(e['id'],repr(e['text']))
print('PASS saved153 original text entries byte-identical to ROM; name alphabet42 bytes.')

