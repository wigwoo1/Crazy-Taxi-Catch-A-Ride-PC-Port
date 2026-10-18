"""Static effect frame callers; candidates require reachable control-flow validation."""
import pathlib,hashlib,struct,sys,json
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);targets={0x145e4,0x20d2c,0x21078,0x21220,0x214f8,0x208d4};calls=[];lines=[]
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&0x7ff)<<12)|((b&0x7ff)<<1)
 if d&0x400000:d-=0x800000
 target=off+4+d
 if target not in targets:continue
 calls.append(dict(call=hex(0x8000000+off),target=hex(0x8000000+target)))
 lines.append('\nCALL '+hex(0x8000000+off)+' -> '+hex(0x8000000+target))
 for i in md.disasm(r[max(0,off-32):off+48],0x8000000+max(0,off-32)):lines.append(f'{i.address:08x}: {i.mnemonic} {i.op_str}')
q=pathlib.Path('outputs/crazy-taxi-native/reports');(q/'effect-frame-callers-source.txt').write_text('\n'.join(lines)+'\n');(q/'effect-frame-callers-candidates.json').write_text(json.dumps(dict(rom_sha256=hashlib.sha256(r).hexdigest(),scope='Static BL candidates only; not reachable execution proof',calls=calls),indent=2));print(json.dumps(calls,indent=2))
