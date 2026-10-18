from pathlib import Path
import hashlib,json,struct,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
sha=hashlib.sha256(rom).hexdigest()
assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
assert rom[0x3244c:0x3244e]==bytes.fromhex('0047')
descriptor=struct.unpack_from('<8I',rom,0x4a4ff4)
assert descriptor[0]==0x0800aad1
instructions=list(md.disasm(rom[0xaad0:0xaaf0],0x0800aad0))
assert instructions[0].mnemonic=='push' and instructions[-1].mnemonic=='bx'
calls=[i.operands[0].imm for i in instructions if i.mnemonic=='bl']
assert calls==[0x0800abc4]
regs={};stores=[]
for i in instructions:
 if i.mnemonic=='ldr' and 'pc,' in i.op_str:
  ptr=((i.address+4)&~3)+i.operands[1].mem.disp
  regs[i.reg_name(i.operands[0].reg)]=struct.unpack_from('<I',rom,ptr-0x08000000)[0]
 elif i.mnemonic=='movs': regs[i.reg_name(i.operands[0].reg)]=i.operands[1].imm
 elif i.mnemonic=='lsls': regs[i.reg_name(i.operands[0].reg)]=regs[i.reg_name(i.operands[1].reg)]<<i.operands[2].imm
 elif i.mnemonic=='adds': regs[i.reg_name(i.operands[0].reg)]=regs[i.reg_name(i.operands[1].reg)]+i.operands[2].imm
 elif i.mnemonic in ('strb','strh'):
  address=regs[i.reg_name(i.operands[1].mem.base)]+i.operands[1].mem.disp
  stores.append({'address':hex(address),'width':1 if i.mnemonic=='strb' else 2,'value':regs[i.reg_name(i.operands[0].reg)]})
assert stores==[{'address':'0x2001660','width':2,'value':2400},{'address':'0x200165e','width':1,'value':0},{'address':'0x2005f2f','width':1,'value':4}]
assert all(int(s['address'],16)+s['width']<=0x020034dc or int(s['address'],16)>=0x0200351e for s in stores)
report={'status':'PASS_STATIC_CALLBACK_IDENTITY','rom_sha256':sha,'dispatcher':'0x0803244C bx r0','descriptor':'0x084A4FF4','descriptor_words':[hex(x) for x in descriptor],'callback_entry':'0x0800AAD0','callback_thumb_pointer':hex(descriptor[0]),'calls':[hex(x) for x in calls],'direct_stores_after_call':stores,'disassembly':[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in instructions],'scope':'SHA guarded callback identity and symbolic straight-line stores after ABC4. ABC4 remains an explicit unexecuted dependency. No whole callback execution, allocator proof, retained input or full restart qualification.'}
Path('outputs/crazy-taxi-native/reports/bound-restart-descriptor-callback-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],report['calls'],stores)
