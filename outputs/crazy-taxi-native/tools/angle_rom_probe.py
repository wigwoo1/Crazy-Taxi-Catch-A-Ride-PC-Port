"""Bounded original ARM angle routine executor; diagnostic only."""
import struct
class AngleProbe:
 def __init__(self,rom):
  self.rom=rom;self.steps=0
  self.allowed={0x52b4:0xe51f20f4,0x52b8:0xe0000002,0x52bc:0xe0011002,0x52c0:0xe1500001,0x52c4:0xba000002,0x52c8:0xca00000a,0x52cc:0xe3a00000,0x52d0:0xe12fff1e,0x52d4:0xe0412000,0x52d8:0xe2803a01,0x52dc:0xe0433001,0x52e0:0xe1520003,0x52e4:0xaa000001,0x52e8:0xe1a00002,0x52ec:0xe12fff1e,0x52f0:0xe2630000,0x52f4:0xe12fff1e,0x52f8:0xe0402001,0x52fc:0xe2813a01,0x5300:0xe0433000,0x5304:0xe1520003,0x5308:0xaa000001,0x530c:0xe2620000,0x5310:0xe12fff1e,0x5314:0xe1a00003,0x5318:0xe12fff1e}
 def run(self,a,b):
  regs=[a&0xffffffff,b&0xffffffff,0,0];pc=0x52b4;n=z=v=False;count=0
  while True:
   count+=1;assert count<32;self.steps+=1
   w=struct.unpack_from('<I',self.rom,pc)[0];assert self.allowed.get(pc)==w,hex(pc)
   cond=w>>28;take={14:True,11:n!=v,12:not z and n==v,10:n==v}[cond]
   if not take:pc+=4;continue
   if w==0xe12fff1e:return regs[0]-0x100000000 if regs[0]&0x80000000 else regs[0]
   if (w>>25)&7==5:
    disp=w&0xffffff;disp=disp-0x1000000 if disp&0x800000 else disp;pc+=8+disp*4;continue
   if pc==0x52b4:regs[2]=struct.unpack_from('<I',self.rom,pc+8-0xf4)[0]
   else:
    opcode=(w>>21)&15;rn=(w>>16)&15;rd=(w>>12)&15
    if w&(1<<25):
     imm=w&255;rot=((w>>8)&15)*2;operand=((imm>>rot)|(imm<<(32-rot)))&0xffffffff if rot else imm
    else:assert (w&0xff0)==0;operand=regs[w&15]
    left=regs[rn] if rn<4 else 0
    if opcode==10:
     out=(left-operand)&0xffffffff;n=bool(out&0x80000000);z=out==0;v=bool((left^operand)&(left^out)&0x80000000)
    else:
     out={0:lambda:left&operand,2:lambda:left-operand,3:lambda:operand-left,4:lambda:left+operand,13:lambda:operand}[opcode]()
     regs[rd]=out&0xffffffff
   pc+=4
