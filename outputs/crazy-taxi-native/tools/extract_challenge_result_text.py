"""Recover source-referenced English result/HUD IDs600..614 only."""
import pathlib,hashlib,struct,json
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();sha=hashlib.sha256(r).hexdigest();assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
base=0x49dfe0;rows=[]
for text_id in [159,*range(440,444),*range(600,615)]:
 off=base+struct.unpack_from('<I',r,base+4+text_id*4)[0];raw=r[off:r.index(0,off)]
 assert len(raw)<4096 and all(v in (9,10,13) or 32<=v<127 for v in raw)
 rows.append({'id':text_id,'rom_address':hex(0x8000000+off),'bytes_hex':raw.hex(),'text':raw.decode('ascii')})
p=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-menu/result-text.json');p.write_text(json.dumps({'rom_sha256':sha,'scope':'Original English HUD/result text; layouts/state gates separately recovered','entries':rows},indent=2))
for row in json.loads(p.read_text())['entries']:
 off=int(row['rom_address'],16)-0x8000000;raw=bytes.fromhex(row['bytes_hex']);assert r[off:off+len(raw)]==raw and r[off+len(raw)]==0
for row in rows:print(row['id'],repr(row['text']))
print('PASS20 original result/HUD labels saved and re-read byte-identical; ROM preserved.')
