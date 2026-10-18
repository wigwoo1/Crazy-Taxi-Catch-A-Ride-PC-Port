"""Read-only recorded/demo input ownership audit; never executes ROM code."""
import hashlib
import pathlib
import struct
import sys
sys.path.insert(0, 'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB, CS_MODE_ARM
rom = pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest() == '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
decoder = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
lines = ['Hash verified. Candidate literal references require control-flow validation; data islands are not code.']
for start,end in [(0x1448,0x1730),(0x1730,0x198c)]:
    for ins in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(rom[start:end],0x08000000+start):
        lines.append(f'BITMAP ARM {ins.address:08x}: {ins.mnemonic} {ins.op_str}')
for start,end,mode in [(0x34094,0x34098,CS_MODE_THUMB),(0x34098,0x3409c,CS_MODE_ARM),(0x52b4,0x531c,CS_MODE_ARM),(0x31f8,0x3380,CS_MODE_ARM),(0x36d0,0x37c0,CS_MODE_ARM),(0x39c0,0x3ad0,CS_MODE_ARM),(0x3aec,0x3c80,CS_MODE_ARM)]:
    for ins in Cs(CS_ARCH_ARM,mode).disasm(rom[start:end],0x08000000+start):
        lines.append(f'MODE AUDIT {ins.address:08x}: {ins.mnemonic} {ins.op_str}')
def emit(start, end):
    lines.append(f'RANGE {start:06x}..{end:06x}')
    for ins in decoder.disasm(rom[start:end], 0x08000000 + start):
        offset = ins.address - 0x08000000
        half = struct.unpack_from('<H', rom, offset)[0]
        row = f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        if half & 0xf800 == 0x4800:
            literal = ((offset + 4) & ~3) + (half & 255) * 4
            row += f' => {struct.unpack_from("<I", rom, literal)[0]:08x}'
        lines.append(row)
emit(0x15cdc, 0x15ce0)  # Verified branch target, avoids preceding literal pool.
emit(0x15ce0, 0x15d02)
emit(0x15d0c, 0x15d90)
emit(0xdec8, 0xdfc0)
emit(0xdfc0, 0xe100)
emit(0x15db6, 0x15e20)
emit(0x160b8, 0x16130)
emit(0xdb70, 0xdbf8)
emit(0xdc00, 0xde64)
emit(0xd920, 0xd970)
emit(0xdbf8, 0xdc00)
emit(0xd8c0, 0xd920)
emit(0xd970, 0xda40)
emit(0x11a80, 0x11ae0)
emit(0x11ae0, 0x11b40)
emit(0x13490, 0x13530)
emit(0x13e20, 0x13ec0)
emit(0x554c, 0x55a0)
emit(0x55b8, 0x5660)
emit(0x11734, 0x11800)
emit(0x11800, 0x11890)
emit(0x55b0, 0x55f6)
emit(0x5608, 0x5634)
emit(0x5634, 0x5714)
emit(0x5714, 0x5778)
emit(0x5760, 0x5778)
emit(0x5460, 0x549a)
emit(0x54b4, 0x554c)
emit(0x54a8, 0x54b4)
emit(0x56d8, 0x570c)
emit(0x2dd5c, 0x2ddd0)
emit(0x2dcd0,0x2dd40)
emit(0x2dd14,0x2dd5c)
emit(0x32454,0x324c0)
emit(0x26700,0x267a4)
emit(0x14bc0,0x14ca0)
emit(0x14a00,0x14b00)
emit(0x14b00,0x14bc0)
emit(0x14ca0,0x14d60)
emit(0xadf0,0xae52)
emit(0x6580,0x65f0)
emit(0x6600,0x6680)
emit(0x6610,0x6700)  # Verified dispatch targets after literal island.
emit(0x66b4,0x6784)  # Traffic update entry; stop before its literal pool.
emit(0x67a0,0x68f0)  # Traffic spawn/update continuation after literal pool.
emit(0x6bc0,0x6ce8)
emit(0x6ce8,0x6e20)
emit(0x6e20,0x6f90)
emit(0x6f7a,0x7050)
emit(0x6fc4,0x7050)
emit(0x6908,0x69c0)
emit(0x69ae,0x69c0)
emit(0x6b38,0x6bcc)
emit(0x6b9c,0x6bc0)
emit(0x2958c,0x29660)
emit(0x7050,0x7200)
emit(0x31a50,0x31b80)
emit(0x31ba4,0x31f00)
emit(0x2bcc0,0x2bd90)
emit(0x2bd90,0x2bf00)
emit(0x2c550,0x2c688)
emit(0x2c688,0x2c834)
emit(0x2b680,0x2b7a0)
emit(0x2b78c,0x2b9da)
emit(0x2e190,0x2e290)
emit(0x2e264,0x2e420)
emit(0x32520,0x32580)
emit(0x32580,0x325f8)
emit(0x69c0,0x6bc0)
emit(0x65bc,0x65f0)
emit(0x2ad8e,0x2ada0)
for offset in range(0, 0x34000 - 4, 2):
    half = struct.unpack_from('<H', rom, offset)[0]
    if half & 0xf800 != 0x4800:
        continue
    literal = ((offset + 4) & ~3) + (half & 255) * 4
    value = struct.unpack_from('<I', rom, literal)[0]
    if value in (0x05000000, 0x05000002, 0x03006914, 0x03006948, 0x02009cec, 0x02009eec, 0x02001eaa, 0x02001708, 0x0200170c, 0x02001710, 0x02001709, 0x0200170a, 0x030053d0, 0x03004c60, 0x03000000, 0x03006940, 0x020034d4):
        lines.append(f'OWNER CANDIDATE {offset:06x} global{value:08x}')
        emit(max(0, offset - 8), offset + 40)
path = pathlib.Path('outputs/crazy-taxi-native/reports/recorded-input-ownership.txt')
lines.append('ANGLE MASK 080051c8: '+hex(struct.unpack_from('<I',rom,0x51c8)[0]))
lines.append('TRAFFIC DENSITY TABLE 084a54b0: '+repr(list(rom[0x4a54b0:0x4a54c0])))
for offset in range(0,len(rom)-4,4):
    if struct.unpack_from('<I',rom,offset)[0] in (0x02009cec,0x02009eec):
        lines.append(f'BILLBOARD QUEUE RAW POINTER {offset:08x}')
    if struct.unpack_from('<I',rom,offset)[0]==0x03006940:
        lines.append(f'DIVISION DISPATCH RAW POINTER {offset:08x}')
emit(0x1e6fc,0x1e720)
emit(0x1e72c,0x1e790)
lines.append('AI PATH NAME: '+repr(rom[0x3409c:0x340a4]))
for name, address in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
    count=struct.unpack_from('<H',rom,address+16)[0]
    pointer=struct.unpack_from('<I',rom,address+52)[0]
    lines.append(f'PATH DESCRIPTOR OWNER {name}: count{count} pointer{pointer:08x}')
    if 0x08000000<=pointer<0x08000000+len(rom):
        base=pointer-0x08000000
        for i in range(count):
            lines.append(f'DESCRIPTOR CANDIDATE {name} {i}: '+repr(rom[base+i*20:base+i*20+20]))
path.write_text('\n'.join(lines) + '\n')
print(f'Wrote {path}: {len(lines)} lines')
