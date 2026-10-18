"""Hash-verified renderer callback installation and ARM entries; static evidence."""
import pathlib, hashlib, struct, sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB,CS_MODE_ARM
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
thumb=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
lines=['Original callback installation. Literal annotations; relocation base must be validated.']
for ins in list(thumb.disasm(rom[0x2ac6c:0x2adf0],0x0802ac6c))+list(thumb.disasm(rom[0x2b0b0:0x2b31e],0x0802b0b0))+list(thumb.disasm(rom[0x2b1a0:0x2b31e],0x0802b1a0))+list(thumb.disasm(rom[0x33f2c:0x33f88],0x08033f2c)):
    row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
    half=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
    if half&0xf800==0x4800:
        at=((ins.address-0x08000000+4)&~3)+(half&255)*4
        row+=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
    lines.append(row)
for start,end in [(0x1448,0x1730),(0x1730,0x198c),(0x198c,0x1dd4),
                  (0x1dd4,0x1f94),(0x1f94,0x20fc),(0x20fc,0x23d0),
                  (0x23d0,0x2668),(0x2668,0x29f8),
                  (0x4424,0x4704),(0x4704,0x49f8),
                  (0xc5c,0x1048),(0x1048,0x1448),
                  (0x49f8,0x4a94),(0x4a94,0x4bac),(0x4bac,0x4d5c),
                  (0x4d5c,0x4da0),(0x4da0,0x4e40),(0x4e40,0x4f5c),
                  (0x38d0,0x3af8),(0x36d0,0x38d0)]:
    lines.append(f'\nARM candidate code {start:x}..{end:x}; pools need validation')
    for ins in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(rom[start:end],0x08000000+start):
        extra=''
        word=struct.unpack_from('<I',rom,ins.address-0x08000000)[0]
        if word&0x0f7f0000==0x051f0000:
            disp=word&4095
            at=ins.address-0x08000000+8+(disp if word&(1<<23) else -disp)
            extra=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        lines.append(f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}{extra}')
for off in range(0,0x34000,2):
    half=struct.unpack_from('<H',rom,off)[0]
    if half&0xf800!=0x4800:continue
    at=((off+4)&~3)+(half&255)*4
    if struct.unpack_from('<I',rom,at)[0]!=0x0202baec:continue
    lines.append(f'\nSHADE TABLE candidate reference {0x08000000+off:08x}')
    for ins in thumb.disasm(rom[off:off+120],0x08000000+off):
        row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        h=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
        if h&0xf800==0x4800:
            literal=((ins.address-0x08000000+4)&~3)+(h&255)*4
            row+=f' ; literal={struct.unpack_from("<I",rom,literal)[0]:08x}'
        lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/raster-callbacks-source.txt').write_text('\n'.join(lines)+'\n')
print('Saved renderer callback and ARM entry evidence')



