"""Hash-verified shade setup callers and material override source."""
import pathlib,hashlib,struct,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB,CS_MODE_ARM
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);lines=[]
def decode(start,end):
    for i in md.disasm(r[start:end],0x08000000+start):
        row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
        h=struct.unpack_from('<H',r,i.address-0x08000000)[0]
        if h&0xf800==0x4800:
            at=((i.address-0x08000000+4)&~3)+(h&255)*4
            row+=f' ; literal={struct.unpack_from("<I",r,at)[0]:08x}'
        lines.append(row)
for off in range(0,0x34000,2):
    a,b=struct.unpack_from('<HH',r,off)
    if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
    d=((a&2047)<<12)|((b&2047)<<1)
    if d&0x400000:d-=0x800000
    if off+4+d not in (0x2b0b0,0x2ca94):continue
    lines.append(f'\nSHADE SETUP CALL {off:08x} -> {off+4+d:08x} candidate context')
    decode(off-32,off+16)
lines.append('\nMATERIAL OVERRIDE candidate range; validate entry and pools')
decode(0x2ca60,0x2caec)
decode(0xde80,0xdf26)
decode(0xdf44,0xdf8c)
lines.append('\nNormal render service validated instruction boundary')
decode(0xb468,0xb48a)
lines.append('\nRenderer initialization entry and post-pool setup candidates')
decode(0x2ac6c,0x2acb0)
decode(0x2aee0,0x2af34)
decode(0x2b330,0x2b390)
decode(0x2b44c,0x2b524)
decode(0x2adf0,0x2aeac)
lines.append('\nENGINE +44/+48 store candidates (register provenance pending)')
for off in range(0,0x34000,2):
    h=struct.unpack_from('<H',r,off)[0]
    if h&0xf800!=0x4800:continue
    literal=((off+4)&~3)+(h&255)*4
    if struct.unpack_from('<I',r,literal)[0] not in (0x0202b8ec,0x02001764):continue
    lines.append(f'\nFLAT MATERIAL CACHE candidate {off:08x}')
    decode(off,off+64)
for off in range(0,0x34000,2):
    h=struct.unpack_from('<H',r,off)[0]
    if h&0xf800!=0x6000 or ((h>>6)&31) not in (17,18):continue
    # Narrow to contexts with a literal engine-base load. This can miss aliases.
    found=False
    for at in range(max(0,off-48),off,2):
        load=struct.unpack_from('<H',r,at)[0]
        if load&0xf800!=0x4800:continue
        literal=((at+4)&~3)+(load&255)*4
        if struct.unpack_from('<I',r,literal)[0]==0x03006810:found=True
    if found:
        lines.append(f'\nENGINE STORE candidate {off:08x}')
        decode(max(0,off-32),off+16)
lines.append('\nBoot ARM entry candidate')
for ins in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(r[0xc0:0x180],0x080000c0):
    lines.append(f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}')
entry=struct.unpack_from('<I',r,0x1d0)[0]
lines.append(f'Boot BX literal at080001d0={entry:08x}')
if entry&1 and 0x08000000<=entry<0x08034000:
    decode((entry-0x08000000)&~1,((entry-0x08000000)&~1)+0x40)
lines.append('\nValidated boot calls05410/05414 initialize runtime')
decode(0x1e2e4,0x1e390)
decode(0x10a54,0x10ae0)
lines.append('\nRenderer initializer2aef4 calls2cae4 before scene binding')
decode(0x2cae4,0x2cb80)
lines.append('\nExact aligned pointer references; data, not instruction claims')
for off in range(0,len(r)-3,4):
    word=struct.unpack_from('<I',r,off)[0]
    if word in (0x0802b0b0,0x0802b0b1,0x03006858):
        lines.append(f'POINTER {0x08000000+off:08x} = {word:08x}')
lines.append('\nUnfiltered Thumb immediate word stores +44/+48; register provenance pending')
for off in range(0,0x34000,2):
    word=struct.unpack_from('<H',r,off)[0]
    if word&0xf800==0x6000 and ((word>>6)&31) in (17,18):
        lines.append(f'STORE CANDIDATE {0x08000000+off:08x} base=r{(word>>3)&7} value=r{word&7} offset={((word>>6)&31)*4:x}')
lines.append('\nAlias candidate contexts; raw boundaries require validation')
for start,end in [(0x6e40,0x6f30),(0x7100,0x7198),(0x114d0,0x11560),
                  (0x11610,0x11660),(0x15b40,0x15bb0),(0x1d000,0x1d050),
                  (0x27d60,0x27da8),(0x28c90,0x28cfc),(0x28d70,0x28dc0)]:
    decode(start,end)
lines.append('\nPalette registration validated e21a entry and tail')
decode(0xe21a,0xe2aa)
decode(0xe2aa,0xe350)
lines.append('\nStartup scene/palette order before shade override')
decode(0xad30,0xadd8)
lines.append('\nScene-bind tail and following renderer setters')
decode(0x2b530,0x2b6a0)
lines.append('\nComplete initializer tail before shade builder')
decode(0x2af02,0x2af36)
lines.append('\nExact aligned pointers inside shade allocation and engine state')
for off in range(0,len(r)-3,4):
    value=struct.unpack_from('<I',r,off)[0]
    if 0x0202baec<=value<0x0202c4ec or 0x03006810<=value<0x03006860:
        lines.append(f'INTERIOR POINTER {0x08000000+off:08x} = {value:08x}')
pathlib.Path('outputs/crazy-taxi-native/reports/shade-callers-source.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))







