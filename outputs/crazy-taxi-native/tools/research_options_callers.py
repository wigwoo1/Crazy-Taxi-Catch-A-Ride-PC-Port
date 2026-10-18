import pathlib, hashlib, sys, struct
sys.path.insert(0,'work/python-packages')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from capstone.arm import ARM_REG_PC
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
lines=['Hash-verified ROM. Raw Thumb BL candidates; validate surrounding code before use.']
calls=[]
for offset in range(0,0x33000-4,2):
    a,b=struct.unpack_from('<HH',rom,offset)
    if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
    displacement=((a&0x7ff)<<12)|((b&0x7ff)<<1)
    if displacement&0x400000:displacement-=0x800000
    if 0x08000000+offset+4+displacement==0x0801c0c4:calls.append(offset)
for call in calls+[0x17918,0x17a80,0x1b1d4,0xf7dc,0xf850]:
    lines.append(f'\nCaller {call+0x08000000:08x}:')
    begin,end=(0x17918,0x17a24) if call==0x17918 else (0xf850,0xf8c0) if call==0xf850 else (0xf7dc,0xf9e0) if call==0xf7dc else (0x1b1d4,0x1c0ac) if call==0x1b1d4 else (0x17a10,0x17ba0) if call==0x17a80 else (call-100,call+70)
    for item in md.disasm(rom[begin:end],begin+0x08000000):
        # Parent branch-over literal pools. These are data, not Thumb code.
        if call==0x17918 and any(a<=item.address-0x08000000<b for a,b in
            ((0x17968,0x17978),(0x179b4,0x179c4),(0x179d6,0x179e4),
             (0x179ee,0x179f8),(0x17a1a,0x17a24))):continue
        extra=''
        if item.mnemonic=='ldr' and len(item.operands)>1 and item.operands[1].mem.base==ARM_REG_PC:
            loc=((item.address+4)&~3)+item.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,loc)[0]:08x}'
        lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}{extra}')
for text_id in range(32,40):
    base=0x49dfe0;address=base+struct.unpack_from('<I',rom,base+4+text_id*4)[0]
    value=rom[address:rom.index(0,address)]
    lines.append(f'English label{text_id} at{address+0x08000000:08x}: {value!r}')
pathlib.Path('outputs/crazy-taxi-native/reports/options-callers.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
