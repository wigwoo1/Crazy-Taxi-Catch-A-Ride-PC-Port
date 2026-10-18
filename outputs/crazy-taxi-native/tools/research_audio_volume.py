"""ROM-only volume ownership evidence; disassembly is data, not emulation."""
import hashlib
import pathlib
import struct
import sys
sys.path.insert(0, 'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from capstone.arm import ARM_REG_PC

rom = pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest() == '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
md.detail = True
ins = next(md.disasm(rom[0x26cf2:0x26cf4], 0x08026cf2))
literal = ((ins.address + 4) & ~3) + ins.operands[1].mem.disp
target = struct.unpack_from('<I', rom, literal - 0x08000000)[0]
lines = [f'SHA256 {hashlib.sha256(rom).hexdigest()}', f'Global volume literal {literal:08x} -> {target:08x}']
refs = [i for i in range(0, 0x33000, 4) if struct.unpack_from('<I', rom, i)[0] == target]
lines.append('Aligned code-region literals: ' + ', '.join(f'{i+0x08000000:08x}' for i in refs))
for ref in refs:
    for instruction in md.disasm(rom[max(0,ref-1024):ref], 0x08000000+max(0,ref-1024)):
        if instruction.mnemonic != 'ldr' or len(instruction.operands) < 2:
            continue
        mem = instruction.operands[1].mem
        if mem.base != ARM_REG_PC or ((instruction.address+4)&~3)+mem.disp != ref+0x08000000:
            continue
        lines.append(f'\nReference {instruction.address:08x} to literal {ref+0x08000000:08x}:')
        begin = instruction.address-0x08000000-24
        for item in md.disasm(rom[begin:begin+100],begin+0x08000000):
            lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}')
for ref in range(0,0x33000,4):
    if struct.unpack_from('<I',rom,ref)[0]!=0x02007602:continue
    for off in range(max(0,ref-1024),ref,2):
        instruction=next(md.disasm(rom[off:off+2],0x08000000+off),None)
        if not instruction or instruction.mnemonic!='ldr' or len(instruction.operands)<2:continue
        mem=instruction.operands[1].mem
        if mem.base!=ARM_REG_PC or ((instruction.address+4)&~3)+mem.disp!=ref+0x08000000:continue
        lines.append(f'\nEffects-enable raw PC-load candidate {instruction.address:08x}; surrounding code requires validation:')
        for following in md.disasm(rom[off+2:off+18],0x08000000+off+2):
            if following.mnemonic=='strb' and len(following.operands)>1 and following.operands[1].mem.base==instruction.operands[0].reg:
                lines.append(f'ENABLE WRITE CANDIDATE {following.address:08x}, load {instruction.address:08x}')
        for item in md.disasm(rom[max(0,off-16):off+32],0x08000000+max(0,off-16)):
            lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}')
for begin,end in [(0x5424,0x543e),(0xffe0,0x10036),(0x101cc,0x10278),(0x104f0,0x1058c),(0x26488,0x2659a),(0x26630,0x266ca),(0x27558,0x27584),(0x1c080,0x1c59c),(0x1c5ac,0x1c892),(0x2763c,0x27778)]:
    lines.append('\nOwnership context (PC literals annotated):')
    for item in md.disasm(rom[begin:end],begin+0x08000000):
        extra = ''
        if item.mnemonic == 'ldr' and len(item.operands)>1 and item.operands[1].mem.base == ARM_REG_PC:
            offset = ((item.address+4)&~3)+item.operands[1].mem.disp-0x08000000
            extra = f' ; literal={struct.unpack_from("<I",rom,offset)[0]:08x}'
        lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}{extra}')
pathlib.Path('outputs/crazy-taxi-native/reports/audio-volume-ownership.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
