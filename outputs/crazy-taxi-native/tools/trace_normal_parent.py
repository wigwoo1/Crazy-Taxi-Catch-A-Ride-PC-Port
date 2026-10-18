"""Read-only normal-session parent/return audit, no execution of ROM code."""
import hashlib
import pathlib
import struct
import sys
sys.path.insert(0, 'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB

rom = pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest() == '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
decoder = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
lines = ['ROM hash verified; source audit only. Linear data islands must not be treated as executed code.']
def emit(start, end):
    lines.append(f'RANGE {start:06x}..{end:06x}')
    for ins in decoder.disasm(rom[start:end], 0x08000000 + start):
        row = f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        offset = ins.address - 0x08000000
        half = struct.unpack_from('<H', rom, offset)[0]
        if half & 0xf800 == 0x4800:
            literal = ((offset + 4) & ~3) + (half & 255) * 4
            row += f' => {struct.unpack_from("<I", rom, literal)[0]:08x}'
        lines.append(row)
for start, end in [(0xabec, 0xac62), (0xb674, 0xb694),
                   (0xb6a8, 0xb6b6), (0xb6c4, 0xb6e0),
                   (0x174dc, 0x17560), (0x17740, 0x177e0),
                   (0x17560, 0x17590), (0x175bc, 0x176c0),
                   (0x176d0, 0x17740), (0x177a0, 0x177d0),
                   (0x177f0, 0x178ec), (0x178f0, 0x17914),
                   (0x1b1d4, 0x1b450), (0x1c0c4, 0x1c310),
                   (0x1ae50, 0x1af40),
                   (0x1c224, 0x1c328), (0x1c328, 0x1c460),
                   (0x1c700, 0x1c888),
                   (0x1c4d6, 0x1c700),
                   (0x1b452, 0x1b550), (0x1bf60, 0x1c0c4),
                   (0x1b744, 0x1b830), (0x1be80, 0x1bf60),
                   (0x1ae50, 0x1b040),
                   (0x1b83e, 0x1b940),
                   (0x1b930, 0x1bab0), (0x1af00, 0x1b1d4),
                   (0x28542, 0x28564), (0x28574, 0x28650),
                   (0x25c18, 0x25e00),
                   (0x5778, 0x58e0), (0x25e12, 0x25e54),
                   (0xf7dc, 0xf8dc), (0xf8e0, 0xf9a0),
                   (0xe5a8, 0xe640), (0xe640, 0xe6dc),
                   (0xe72c, 0xe79c), (0xebac, 0xecb0),
                   (0xe600, 0xe62c), (0xe658, 0xe66c),
                   (0xe670, 0xe6d8), (0xe794, 0xe834),
                   (0xe834, 0xe8a0), (0xe8a0, 0xe940),
                   (0xec98, 0xed70), (0xed70, 0xee10),
                   (0xeac0, 0xebac),
                   (0xee10, 0xee9c), (0xf3c4, 0xf42a),
                   (0xf468, 0xf490),
                   (0xe940, 0xe97c), (0xe9ac, 0xea12),
                   (0xe630, 0xe658), (0xea4c, 0xea90),
                   (0x28288, 0x2835c), (0x28380, 0x283c0),
                   (0x28650, 0x287a8), (0x287d0, 0x28810),
                   (0xb976, 0xb9c6), (0xb9d0, 0xb9f8),
                   (0xb5b0, 0xb630), (0x24674, 0x24758),
                   (0x24768, 0x24880),
                   (0xb606, 0xb60e),
                   (0x1e274, 0x1e2e0)]:
    emit(start, end)
for offset in range(0, 0x34000 - 4, 2):
    high, low = struct.unpack_from('<HH', rom, offset)
    if high & 0xf800 != 0xf000 or low & 0xf800 != 0xf800:
        continue
    displacement = ((high & 0x7ff) << 12) | ((low & 0x7ff) << 1)
    if displacement & 0x400000:
        displacement -= 0x800000
    if offset + 4 + displacement == 0xabec:
        lines.append(f'NORMAL PARENT DIRECT CALL {0x08000000+offset:08x}')
        emit(max(0, offset - 32), offset + 24)
path = pathlib.Path('outputs/crazy-taxi-native/reports/normal-parent-return-source.txt')
lines.append('VERIFIED 28288 DISPATCH TABLE 08028344: ' + ', '.join(
    f'{value:08x}' for value in struct.unpack_from('<5I', rom, 0x28344)))
lines.append('CLOCK BASE 084a59dc: ' + str(struct.unpack_from('<I', rom, 0x4a59dc)[0]))
for offset in range(0, 0x34000 - 4, 2):
    half = struct.unpack_from('<H', rom, offset)[0]
    if half & 0xf800 != 0x4800:
        continue
    literal = ((offset + 4) & ~3) + (half & 255) * 4
    if struct.unpack_from('<I', rom, literal)[0] in (0x020094f0, 0x020094f4, 0x020094f8):
        lines.append(f'MOTION LITERAL CANDIDATE {0x08000000+offset:08x}; validate control flow before interpreting')
        emit(offset, offset + 40)
    if struct.unpack_from('<I', rom, literal)[0] == 0x02003897:
        lines.append(f'REINITIALIZE LITERAL CANDIDATE {0x08000000+offset:08x}; validate control flow')
        emit(offset, offset + 28)
path.write_text('\n'.join(lines) + '\n')
print('\n'.join(lines))
