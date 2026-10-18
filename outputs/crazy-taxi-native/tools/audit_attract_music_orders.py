"""Audit and extract original attract music order selection; not live integration."""
import pathlib, hashlib, json, struct
root=pathlib.Path(__file__).resolve().parents[1]
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
# AC7c/7e prepares range0..65535; AC84..90 computes signed remainder4.
assert r[0xac7c:0xac80].hex()=='00202449'
assert r[0xac84:0xac92].hex()=='011c002900da c81c80108000081a'.replace(' ','')
# LDRB selected byte then direct call27110 (supplied music order).
assert struct.unpack_from('<H',r,0xac96)[0]==0x7800
h1,h2=struct.unpack_from('<HH',r,0xac98)
d=((h1&2047)<<12)|((h2&2047)<<1)
if d&(1<<22):d-=1<<23
assert 0x8000000+0xac98+4+d==0x8027110
orders=list(r[0x7ec060:0x7ec064]);assert orders==[63,88,115,34]
module=(root/'assets/music/original-module.bin').read_bytes()
# Original module order-byte indirection: header+6+order -> pattern index.
patterns=module[0]
resolved=[module[6+i] for i in orders]
assert all(p<patterns for p in resolved)
out=root/'assets/music/attract-orders.bin';out.write_bytes(bytes(orders))
assert out.read_bytes()==r[0x7ec060:0x7ec064]
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'rom_table':'087ec060','orders':orders,'pattern_indices':resolved,'pattern_count':patterns,'scope':'exact source order selection and raw table; not live title/demo integration; intervening initialization RNG ownership remains to be verified'}
(root/'reports/attract-music-order-audit.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report))
