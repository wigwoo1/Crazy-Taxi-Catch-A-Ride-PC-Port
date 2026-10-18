import pathlib,hashlib,struct,sys,json
sys.path.insert(0,'work/python-packages')
from capstone import *
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);rows=[];lines=['Static Thumb BL candidates only; validate code reachability and data pools.']
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&2047)<<12)|((b&2047)<<1)
 if d&0x400000:d-=0x800000
 target=off+4+d
 if target not in (0x28288,0x28658):continue
 rows.append(dict(call=hex(off),target=hex(target)))
 lines.append(f'\nCANDIDATE {off:x}->{target:x}')
 for i in md.disasm(r[max(0,off-48):off+8],0x8000000+max(0,off-48)):
  lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/obj-request-call-candidates.json').write_text(json.dumps(rows,indent=2))
pathlib.Path('outputs/crazy-taxi-native/reports/obj-request-call-contexts.txt').write_text('\n'.join(lines))
print(json.dumps(rows,indent=2))
