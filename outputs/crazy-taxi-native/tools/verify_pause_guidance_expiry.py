"""Qualify ordinary HUD palette ownership; no original full-frame claim."""
import hashlib,json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(r).hexdigest()==expected
# Source stores record.source at bank*20+16; literal values guard actual owners.
for address,value in [(0x207e0,0x2001790),(0x207d4,0x8290520),(0xc1ec,0x2001790),(0xc1e0,0x8290a20),(0xc1e4,0x5000360)]:
 assert struct.unpack_from('<I',r,address)[0]==value
assert r[0x2077e:0x20788].hex()=='0025605f400180184867'
assert r[0xc124:0xc128].hex()=='ec300260'
# Ordinary HUD cumulative palette records from original group headers.
base=0x70e50;records=[];next_bank=0
for group in range(11):
 g=base+struct.unpack_from('<I',r,base+4+group*4)[0]
 count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
 records.append((next_bank,pals));next_bank+=pals
assert records[2]==(5,1) and records[7]==(11,1)
rows=[]
for name,frames,w,h,palettes,bank in [('guidance',32,64,32,5,5),('hud/time-expiry',15,32,32,1,11)]:
 path=root/('assets/guidance/guidance.rgba' if bank==5 else 'assets/hud/time-expiry.rgba')
 data=path.read_bytes();assert len(data)==frames*palettes*w*h*4
 for variant in range(palettes):
  palette=(0x290520+variant*32) if bank==5 else 0x290a20
  values=set(struct.unpack_from('<16H',r,palette));opaque=0
  for off in range(variant*frames*w*h*4,(variant+1)*frames*w*h*4,4):
   red,green,blue,alpha=data[off:off+4]
   if alpha:
    value=sum(((c*31+127)//255)<<(5*i) for i,c in enumerate((red,green,blue)))
    assert value in values,(name,off,value);opaque+=1
  rows.append(dict(asset=name,variant=variant,registered_record=bank,palette_source=hex(palette+0x8000000),opaque_pixels=opaque,alternate=bank!=11,asset_sha256=hashlib.sha256(data).hexdigest()))
report=dict(rom_sha256=expected,banks=rows,source_owners=['0802077c..08020786 guidance dynamic source record5','0800c122..0800c126 expiry source record11'],scope='Source pointer/bank registration and opaque palette membership; not full original HUD frame equivalence')
(root/'reports/pause-guidance-expiry-provenance.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS guidance five palette variants and expiry record11 retained ownership')
