"""Original English demo instructions and score overlay headings; ROM read-only."""
import pathlib,hashlib,struct,json
root=pathlib.Path(__file__).resolve().parents[1]
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
sha=hashlib.sha256(r).hexdigest();assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
base=0x49dfe0;count=struct.unpack_from('<I',r,base)[0]
ids=[30,31]+list(range(320,330));blob=bytearray(b'CRT1'+struct.pack('<I',len(ids)));entries=[]
for i in ids:
 assert i<count
 a=base+struct.unpack_from('<I',r,base+4+4*i)[0];z=r.index(0,a);text=r[a:z]
 assert 0<len(text)<128 and all(c>=32 for c in text)
 blob.extend(struct.pack('<HH',i,len(text)));blob.extend(text)
 entries.append({'id':i,'address':hex(a+0x8000000),'text':text.decode('latin1'),'bytes_hex':text.hex()})
out=root/'assets/attract';out.mkdir(exist_ok=True)
(out/'text.crt').write_bytes(blob)
report={'rom_sha256':sha,'entries':entries,'instructions':{'owner':'0800d8c8','frame_gate':'(source0755c &127)<=111','font_slot':0,'alignment':4,'anchors':['viewportWidth/2, viewportHeight-22','viewportWidth/2, viewportHeight-12']},'score_rows':{'enabled':'02001709','page':'0200170a','reveal_counter':'0200170b','row_count':20,'counter_under20':'draw rows < counter','counter_at_least20':'draw all20','names_source':'settings+0x1a8+8*(page*5+(row%5))','money_source':'settings+0x1ac+8*(page*5+(row%5))','driver_source':'settings+0x1ac+8*(page*5+(row%5)); source byte bits5..6 plus sprite group3'},'scope':'exact original text bytes and selected source layout; runtime font registration, money unpacking and score record/save defaults need independent qualification before live overlay integration'}
(out/'text.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
