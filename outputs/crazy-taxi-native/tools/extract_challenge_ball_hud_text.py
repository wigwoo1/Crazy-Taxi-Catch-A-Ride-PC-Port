from pathlib import Path
import hashlib,struct,json
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
sha=hashlib.sha256(r).hexdigest();assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
id=615;base=0x49dfe0;off=base+struct.unpack_from('<I',r,base+4+id*4)[0];raw=r[off:r.index(0,off)]
assert len(raw)<4096 and all(32<=v<127 for v in raw)
p=Path('outputs/crazy-taxi-native/assets/challenge-ball/hud-text.json')
p.write_text(json.dumps(dict(rom_sha256=sha,id=id,rom_address=hex(off+0x08000000),bytes_hex=raw.hex(),text=raw.decode('ascii'),scope='Original8430 HUD label via28FE4; F7DC drawing/request flags separately recovered'),indent=2)+'\n')
j=json.loads(p.read_text());assert r[off:off+len(raw)].hex()==j['bytes_hex'] and r[off+len(raw)]==0
print('PASS original Ball HUD label',id,repr(j['text']),'source bytes/readback/SHA exact')
