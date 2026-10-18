import pathlib,struct,hashlib,json,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=pathlib.Path('outputs/crazy-taxi-native');rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba');r=rom.read_bytes();digest=hashlib.sha256(r).hexdigest();assert digest=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);targets=set(range(0x02001670,0x02001680,4));refs=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 pool=((off+4)&~3)+(h&255)*4;value=struct.unpack_from('<I',r,pool)[0]
 if value not in targets:continue
 ins=list(md.disasm(r[off:off+64],0x08000000+off))
 refs.append(dict(address=hex(off+0x08000000),pool=hex(pool+0x08000000),target=hex(value),instructions=[dict(address=hex(i.address),text=i.mnemonic+' '+i.op_str) for i in ins]))
report=dict(rom_sha256=digest,scan_end='0x08034000',literal_references=refs,scope='Static Thumb literal-load candidates, including possible data false positives; not execution reachability or exhaustive indirect/ARM writer proof.')
(root/'reports/camera-preset-literal-reference-audit.json').write_text(json.dumps(report,indent=2)+'\n')
for a in refs:
 print(a['address'],a['target'], '; '.join(i['text'] for i in a['instructions'][:12]))

