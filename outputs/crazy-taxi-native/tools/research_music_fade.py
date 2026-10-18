"""Locate original music fade literal references; candidates need branch validation."""
import pathlib, hashlib, struct, sys
sys.path.insert(0, 'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from capstone.arm import ARM_REG_PC
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB); md.detail=True
lines=['Hash-verified literal-reference candidates. Linear contexts can contain pools; not execution proof.']
for begin,end in [(0x0b540,0x0b628),(0x079c0,0x07ae0),(0x174dc,0x17530),(0x0dec8,0x0df26),(0x0df44,0x0df8c),(0x0af5c,0x0afb8),(0x0b27c,0x0b320),(0x10b54,0x10c40)]:
    lines.append('\nFACTOR lifecycle context')
    for x in md.disasm(rom[begin:end],0x08000000+begin):
        extra=''
        if x.mnemonic=='ldr' and len(x.operands)>1 and x.operands[1].mem.base==ARM_REG_PC:
            at=((x.address+4)&~3)+x.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        lines.append(f'{x.address:08x}: {x.mnemonic} {x.op_str}{extra}')
for off in range(0,0x34000,2):
    ins=next(md.disasm(rom[off:off+2],0x08000000+off),None)
    if not ins or ins.mnemonic!='ldr' or len(ins.operands)<2 or ins.operands[1].mem.base!=ARM_REG_PC: continue
    lit=((ins.address+4)&~3)+ins.operands[1].mem.disp-0x08000000
    value=struct.unpack_from('<I',rom,lit)[0]
    if value not in (0x020025da,0x02001ea6): continue
    lines.append('\n'+('FACTOR' if value==0x020025da else 'MAINMODE')+' reference '+hex(ins.address))
    for x in md.disasm(rom[off:off+80],ins.address):
        extra=''
        if x.mnemonic=='ldr' and len(x.operands)>1 and x.operands[1].mem.base==ARM_REG_PC:
            at=((x.address+4)&~3)+x.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        lines.append(f'{x.address:08x}: {x.mnemonic} {x.op_str}{extra}')
pathlib.Path('outputs/crazy-taxi-native/reports/music-fade-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(x for x in lines if 'FACTOR reference' in x))
