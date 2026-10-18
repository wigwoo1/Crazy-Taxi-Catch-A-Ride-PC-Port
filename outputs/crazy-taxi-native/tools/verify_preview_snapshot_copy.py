"""Execute original aligned248-byte copy body; stack prologue/return explicit boundary."""
import sys,json,hashlib
from pathlib import Path
sys.path.insert(0,'work/python-packages')
from capstone import *
from capstone.arm import *
root=Path('outputs/crazy-taxi-native')
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
code={i.address:i for i in md.disasm(rom[0x33ece:0x33f2a],0x08033ece)}
steps=0
def copy(memory,dst,src):
 global steps
 regs={'r0':dst,'r1':src,'r2':248};pc=0x08033ece;eq=False;above=False
 def value(op):return op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
 while pc!=0x08033f28:
  i=code[pc];steps+=1;ops=i.operands;mn=i.mnemonic;nxt=pc+i.size
  if mn in ['adds','subs','orrs','ands','rsbs']:
   a=value(ops[0] if len(ops)==2 else ops[1]);b=value(ops[1] if len(ops)==2 else ops[2]);v=a+b if mn=='adds' else a-b if mn=='subs' else a|b if mn=='orrs' else a&b if mn=='ands' else b-a
   regs[md.reg_name(ops[0].reg)]=v&0xffffffff
  elif mn=='movs':regs[md.reg_name(ops[0].reg)]=value(ops[1])&0xffffffff
  elif mn=='cmp':a=value(ops[0]);b=value(ops[1]);eq=a==b;above=a>b
  elif mn in ['bhi','bls','beq','bne']:
   if {'bhi':above,'bls':not above,'beq':eq,'bne':not eq}[mn]:nxt=ops[0].imm
  elif mn in ['ldm','stm']:
   name=md.reg_name(ops[0].reg);at=regs[name]
   for op in ops[1:]:
    reg=md.reg_name(op.reg)
    if mn=='ldm':regs[reg]=int.from_bytes(bytes(memory[at+j] for j in range(4)),'little')
    else:
     for j,b in enumerate(regs[reg].to_bytes(4,'little')):memory[at+j]=b
    at+=4
   assert i.writeback;regs[name]=at
  elif mn in ['ldrb','strb']:
   at=regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp;reg=md.reg_name(ops[0].reg)
   if mn=='ldrb':regs[reg]=memory[at]
   else:memory[at]=regs[reg]&255
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 assert regs['r0']==dst
for seed in range(64):
 owner=0x03006810;snapshot=0x10000050
 memory={owner+j:(seed*19+j*37)&255 for j in range(280)}
 memory.update({snapshot+j:165 for j in range(-16,264)})
 original=bytes(memory[owner+j] for j in range(280))
 copy(memory,snapshot,owner)
 assert bytes(memory[snapshot+j] for j in range(248))==original[:248]
 for j in range(280):memory[owner+j]=(seed+j*11)&255
 outside=bytes(memory[owner+j] for j in range(248,280))
 copy(memory,owner,snapshot)
 assert bytes(memory[owner+j] for j in range(248))==original[:248]
 assert bytes(memory[owner+j] for j in range(248,280))==outside
 assert all(memory[snapshot+j]==165 for j in list(range(-16,0))+list(range(248,264)))
report=dict(cases=64,copy_calls=128,actual_thumb_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual33ECC aligned248-byte copy body twice, snapshot/restore and guard retention. Stack prologue/return boundary excluded. Restored03006810..03006907 includes generation03006844; bytecursor03006910 and all external caches/heads excluded. Caller31860 helpers and native typed mapping NOT qualified.')
(root/'reports/preview-snapshot-copy-source-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
