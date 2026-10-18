"""Original Control submenu evidence; linear pool regions need branch validation."""
import pathlib, hashlib, struct, sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm import ARM_REG_PC
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
lines=['Hash-verified original Control callback08017bb9. Disassembly, not execution; pool boundaries need validation.']
for start,end in [(0x17bb8,0x17f12),(0x17f9c,0x18036),(0x1805c,0x180f2),(0x263d0,0x26418)]:
    for x in md.disasm(rom[start:end],0x08000000+start):
        extra=''
        if x.mnemonic=='ldr' and len(x.operands)>1 and x.operands[1].mem.base==ARM_REG_PC:
            at=((x.address+4)&~3)+x.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        lines.append(f'{x.address:08x}: {x.mnemonic} {x.op_str}{extra}')
for off in range(0,0x34000,2):
    ins=next(md.disasm(rom[off:off+2],0x08000000+off),None)
    if not ins or ins.mnemonic!='ldr' or len(ins.operands)<2 or ins.operands[1].mem.base!=ARM_REG_PC:continue
    at=((ins.address+4)&~3)+ins.operands[1].mem.disp-0x08000000
    value=struct.unpack_from('<I',rom,at)[0]
    if value not in (0x0200755c,0x02001eb0):continue
    lines.append('\n'+('DEMO CLOCK' if value==0x0200755c else 'SETTINGS')+' literal reference '+hex(ins.address))
    for x in md.disasm(rom[off:off+(32 if value==0x0200755c else 96)],ins.address):
        lines.append(f'{x.address:08x}: {x.mnemonic} {x.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/control-menu-source.txt').write_text('\n'.join(lines)+'\n')
print('Saved original Control submenu entry evidence',len(lines),'lines')
