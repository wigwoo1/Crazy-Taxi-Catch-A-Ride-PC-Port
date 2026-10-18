"""Hash-verified static action-query callers; candidate contexts aren't execution proof."""
import pathlib, hashlib, struct, sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
lines=['Static BL candidates to original08015d98. Validate branches and literal pools before translation.']
for off in range(0,0x34000,2):
    a,b=struct.unpack_from('<HH',rom,off)
    if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
    displacement=((a&0x7ff)<<12)|((b&0x7ff)<<1)
    if displacement&0x400000:displacement-=0x800000
    target=off+4+displacement
    if target not in (0x15d98,0x31860):continue
    lines.append(f'\nCALL {0x08000000+off:08x} -> {0x08000000+target:08x}')
    for ins in md.disasm(rom[max(0,off-16):off+64],0x08000000+max(0,off-16)):
        row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        half=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
        if half&0xf800==0x4800:
            literal=((ins.address-0x08000000+4)&~3)+(half&255)*4
            row+=f' ; literal={struct.unpack_from("<I",rom,literal)[0]:08x}'
        lines.append(row)
count=sum(x.lstrip().startswith('CALL ') for x in lines)
lines.append('\nDISTANCE ACTION CONTEXT c200..c2fc; validate pool c2d4..c2dc')
for ins in md.disasm(rom[0xc200:0xc2fc],0x0800c200):
    row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
    half=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
    if half&0xf800==0x4800:
        literal=((ins.address-0x08000000+4)&~3)+(half&255)*4
        row+=f' ; literal={struct.unpack_from("<I",rom,literal)[0]:08x}'
    lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/action-callers-source.txt').write_text('\n'.join(lines)+'\n')
print('Saved action-query candidate callers',count)
refs=[]
for off in range(0,0x34000,2):
    half=struct.unpack_from('<H',rom,off)[0]
    if half&0xf800!=0x4800:continue
    literal=((off+4)&~3)+(half&255)*4
    value=struct.unpack_from('<I',rom,literal)[0]
    if value not in (0x020016c0,0x020016c2,0x020016c4,0x020016c8,0x020358f0,0x030069a0,0x0202c4f0,0x020304f0):continue
    refs.append(f'\nPANEL literal {value:08x} at {0x08000000+off:08x}; candidate context')
    for ins in md.disasm(rom[off:off+80],0x08000000+off):
        row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        h=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
        if h&0xf800==0x4800:
            at=((ins.address-0x08000000+4)&~3)+(h&255)*4
            row+=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        refs.append(row)
for start,end in [(0x1edb0,0x1ee50),(0xdf8c,0xdff0),(0xcb84,0xcc50),(0xb4d0,0xb522),
                  (0x2954c,0x29658),(0x295a8,0x29744),(0xb6ec,0xb824),(0xb908,0xb974),(0xb970,0xba04),
                  (0x2c790,0x2c888),(0x2b6a0,0x2b756),(0x2b78c,0x2b7aa),
                  (0x2b7bc,0x2b7de),(0x2b7e8,0x2b820),(0x2b820,0x2b97c),
                  (0x32448,0x3245c),(0x2db54,0x2dbbc),
                  (0x317d0,0x31884),(0x31860,0x31a38),(0x2c918,0x2ca30),(0x31ef8,0x3201c)]:
    refs.append(f'\nINITIALIZER/DRAW context {start:06x}..{end:06x}; raw pools require validation')
    for ins in md.disasm(rom[start:end],0x08000000+start):
        row=f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}'
        h=struct.unpack_from('<H',rom,ins.address-0x08000000)[0]
        if h&0xf800==0x4800:
            at=((ins.address-0x08000000+4)&~3)+(h&255)*4
            row+=f' ; literal={struct.unpack_from("<I",rom,at)[0]:08x}'
        refs.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/distance-panel-source.txt').write_text('\n'.join(refs)+'\n')
print('Saved panel reference contexts')
