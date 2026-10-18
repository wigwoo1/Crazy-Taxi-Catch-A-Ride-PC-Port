"""Hash-verified music code/data evidence. Disassembly is not execution."""
import pathlib,struct,hashlib,sys,json
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB,CS_MODE_ARM
from capstone.arm import ARM_REG_PC
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
lines=['Hash-verified original music routines. Literal pools may be data; validate branch boundaries.']
for offset in range(0,0x34000,2):
    item=next(md.disasm(rom[offset:offset+4],0x08000000+offset),None)
    if item and item.mnemonic=='bl' and item.op_str.startswith('#0x80268'):
        lines.append('Audio service caller '+hex(item.address)+' -> '+item.op_str)
for city in range(2):
    lines.append('Gameplay city descriptor '+str(city)+': '+','.join(hex(x) for x in struct.unpack_from('<6I',rom,0x4a4f7c+city*24)))
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
for item in arm.disasm(rom[0x29fc:0x2ce8],0x080029fc):
    lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}')
for offset in range(0,0x34000,2):
    item=next(md.disasm(rom[offset:offset+2],0x08000000+offset),None)
    if not item or item.mnemonic!='ldr' or len(item.operands)<2 or item.operands[1].mem.base!=ARM_REG_PC:continue
    literal=((item.address+4)&~3)+item.operands[1].mem.disp-0x08000000
    if struct.unpack_from('<I',rom,literal)[0]!=0x03006940:continue
    reg=item.op_str.split(',')[0]
    following=list(md.disasm(rom[offset+2:offset+26],item.address+2))
    stores=[x for x in following if x.mnemonic.startswith('str') and '['+reg in x.op_str]
    if stores:
        lines.append('DIVISION initializer candidate '+hex(item.address))
        for x in [item]+following:lines.append(f'{x.address:08x}: {x.mnemonic} {x.op_str}')
for begin,end in [(0x0d764,0x0d89c),(0x0d89c,0x0d8ec),(0x16140,0x161b8),(0x161d8,0x16208),(0x16208,0x16278),(0x227ac,0x2283c),(0x26318,0x2637a),(0x0ac9c,0x0ad26),(0x0ad26,0x0ae34),(0x0ae34,0x0af20),(0x11490,0x11520),(0x1ee28,0x1eea0),(0x2277c,0x227ac),(0x21c10,0x21c90),(0x2ad20,0x2add0),(0x174dc,0x175ae),(0x073d0,0x07530),(0x07522,0x0759e),(0x0abc4,0x0acc0),(0x0a97c,0x0a998),(0x0a9dc,0x0aa20),(0x07ab0,0x07b00),(0x17a24,0x17bb8),(0x27730,0x27770)]:
    for item in md.disasm(rom[begin:end],0x08000000+begin):
        extra=''
        if item.mnemonic=='ldr' and len(item.operands)>1 and item.operands[1].mem.base==ARM_REG_PC:
            off=((item.address+4)&~3)+item.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,off)[0]:08x}'
        lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}{extra}')
dispatch=list(struct.unpack_from('<10I',rom,0x27324))
for offset in range(0,0x26000,2):
    item=next(md.disasm(rom[offset:offset+4],0x08000000+offset),None)
    if item and item.mnemonic=='bl' and item.op_str in ('#0x8027110','#0x8026ef8'):
        lines.append('Music ownership caller '+hex(item.address)+' -> '+item.op_str)
for offset in range(0x26000,0x27044,2):
    item=next(md.disasm(rom[offset:offset+4],0x08000000+offset),None)
    if item and item.mnemonic=='bl' and item.op_str in ('#0x80271e0','#0x80274bc'):
        lines.append('Tracker caller '+hex(item.address)+' -> '+item.op_str)
for offset in range(0x27040,0x27110,2):
    item=next(md.disasm(rom[offset:offset+2],0x08000000+offset),None)
    if item and item.mnemonic=='push' and 'lr' in item.op_str:print('Tracker update entry candidate:',hex(item.address))
lines.append('Source effect dispatch6..15: '+', '.join(hex(x) for x in dispatch))
for begin,end in [(0x26b14,0x26bd8),(0x26c18,0x26c76),(0x26c7c,0x26caa),(0x26918,0x269ce),(0x269fc,0x26a1e),(0x26a2c,0x26a4e),(0x26ef8,0x2702c),(0x27044,0x270fc),(0x27110,0x271ca),(0x271e0,0x27302),(0x2734c,0x273f6),(0x2740c,0x27558)]:
    lines.append(f'\nSource {begin:05x}..{end:05x}')
    for item in md.disasm(rom[begin:end],0x08000000+begin):
        extra=''
        if item.mnemonic=='ldr' and len(item.operands)>1 and item.operands[1].mem.base==ARM_REG_PC:
            offset=((item.address+4)&~3)+item.operands[1].mem.disp-0x08000000
            extra=f' ; literal={struct.unpack_from("<I",rom,offset)[0]:08x}'
        lines.append(f'{item.address:08x}: {item.mnemonic} {item.op_str}{extra}')
tables=[]
for i in range(5):
    pointer=struct.unpack_from('<I',rom,0x7ec048+i*4)[0]
    assert 0x08000000<=pointer<0x08000000+len(rom)
    offset=pointer-0x08000000
    tables.append(dict(index=i,address=hex(pointer),first64=rom[offset:offset+64].hex(),words=[hex(x) for x in struct.unpack_from('<16I',rom,offset)]))
patterns=list(rom[0x7ec060:0x7ec065])
base=tables[0]['address'];offset=int(base,16)-0x08000000
cursor=offset+0x106
pattern_records=[]
for i in range(rom[offset]):
    rows=rom[cursor];size=1+rows*rom[offset+4]*5
    assert cursor+size<=len(rom)
    pattern_records.append(dict(index=i,address=hex(cursor+0x08000000),rows=rows,size=size,sha256=hashlib.sha256(rom[cursor:cursor+size]).hexdigest()))
    cursor+=size
sample_records=[]
for i in range(rom[offset+3]):
    length=struct.unpack_from('<I',rom,cursor)[0];size=length+15
    assert cursor+size<=len(rom)
    sample_records.append(dict(index=i,address=hex(cursor+0x08000000),length_field=length,size=size,header15=rom[cursor:cursor+15].hex(),sha256=hashlib.sha256(rom[cursor:cursor+size]).hexdigest()))
    cursor+=size
starts=[]
for order in patterns:
    pattern=rom[offset+6+order]
    assert pattern<len(pattern_records)
    starts.append(dict(order=order,pattern=pattern,pattern_address=pattern_records[pattern]['address']))
pathlib.Path('outputs/crazy-taxi-native/reports/music-source.txt').write_text('\n'.join(lines)+'\n')
pathlib.Path('outputs/crazy-taxi-native/reports/music-data.json').write_text(json.dumps(dict(rom_sha256=expected,table='0x087ec048',music_headers=tables,menu_pattern_bytes=patterns,
    menu_start_orders=starts,order_table=list(rom[offset+6:offset+0x106]),
    source_loader_walk=dict(start=base,end=hex(cursor+0x08000000),channels=rom[offset+4],patterns=pattern_records,samples=sample_records),
    qualification='Loader byte layout recovered; event/sample semantics and audible playback remain unported.'),indent=2)+'\n')
print('Original music pointers:',[entry['address'] for entry in tables])
print('Pattern bytes at087ec060:',patterns)
print('Source loader walk:',len(pattern_records),'pattern records;',len(sample_records),'sample records;',cursor-offset,'bytes')
print('Source2713a..2714a menu order to pattern mapping:',starts)
out=pathlib.Path('outputs/crazy-taxi-native/assets/music');out.mkdir(parents=True,exist_ok=True)
blob=rom[offset:cursor]
(out/'original-module.bin').write_bytes(blob)
# Source26506 DMA relocates 768 halfword pitch values to03006160.
pitch=rom[0x49d780:0x49dd80]
assert len(pitch)==1536
(out/'pitch-table.bin').write_bytes(pitch)
reference=bytearray()
for sample in sample_records:
    h=bytes.fromhex(sample['header15'])
    transpose=int.from_bytes(h[13:14],'little',signed=True)
    fine=int.from_bytes(h[12:13],'little',signed=True)
    for event in range(1,256):
        if event==128:continue
        note=(event+transpose-1)%256
        period=(7616-note*64-fine//2)%65536
        distance=(7680-period)%65536
        octave,remainder=divmod(distance,768)
        frequency=struct.unpack_from('<H',pitch,remainder*2)[0]*4
        shift=(7-octave)%256
        frequency=frequency>>shift if shift<32 else 0
        assert frequency*256<2**31  # Installed signed divide: original positive music domain
        reference.extend(struct.pack('<BBBBHII',event,h[13],h[12],note,period,frequency,((frequency*256)%2**32)//21024))
pathlib.Path('outputs/crazy-taxi-native/reports/music-note-reference.bin').write_bytes(reference)
lines.append('Pitch source0849d780: 768 halfwords SHA256 '+hashlib.sha256(pitch).hexdigest())
pathlib.Path('outputs/crazy-taxi-native/reports/music-source.txt').write_text('\n'.join(lines)+'\n')
(out/'module.json').write_text(json.dumps(dict(rom_sha256=expected,address=base,length=len(blob),sha256=hashlib.sha256(blob).hexdigest(),menu_start_orders=patterns,
    qualification='Original byte block; native loader only, tracker playback unported.'),indent=2)+'\n')
















