"""Bounded original fade diagnostic. Never imported by the game runtime.
Executes only three allowlisted ROM routines; signed division and DMA word
copies are explicit scalar/data contracts. No BIOS, CPU/device/game emulator.
"""
import hashlib, json, os, pathlib, sys
P=pathlib.Path(r'./')
B=P/'outputs/crazy-taxi-native'
sys.path.insert(0,str(P/'work/python-packages'))
os.environ['CAPSTONE_LIBRARY_PATH']=str(P/'work/python-packages/capstone/lib')
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB
from capstone.arm_const import ARM_OP_REG, ARM_OP_IMM
ROM=pathlib.Path(r'Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
SHA='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(ROM).hexdigest()==SHA
MD=Cs(CS_ARCH_ARM,CS_MODE_THUMB);MD.detail=True
DECODE={}
U=0xffffffff
signed=lambda v:v-0x100000000 if v&0x80000000 else v

def rn(name):
 return {'sp':13,'lr':14,'pc':15,'sb':9,'sl':10,'fp':11,'ip':12}.get(name,int(name[1:]) if name.startswith('r') and name[1:].isdigit() else -1)

class FadeProbe:
 def __init__(self):
  self.reg=[0]*16;self.reg[13]=0x10002000;self.reg[14]=0x10020001
  self.flags=(False,False,False,False);self.steps=0;self.visited={};self.calls={};self.dma={}
  self.maps=[(a,bytearray(n)) for a,n in [(0x02001770,0xa20),(0x02003b00,4),(0x02007542,4),(0x020094f0,24),(0x03004cb0,1536),(0x0400004c,12),(0x05000000,1024),(0x09010000,512),(0x10000000,0x2000)]]
 def region(self,a,n):
  for base,data in self.maps:
   if base<=a and a+n<=base+len(data):return data,a-base
  raise AssertionError(('outside data',hex(a),n))
 def read(self,a,n=4):
  if 0x08000000<=a and a+n<=0x08000000+len(ROM):return int.from_bytes(ROM[a-0x08000000:a-0x08000000+n],'little')
  if a in self.dma:return self.dma[a]
  d,o=self.region(a,n);return int.from_bytes(d[o:o+n],'little')
 def write(self,a,v,n=4):
  v&=(1<<(8*n))-1
  if a in (0x040000d4,0x040000d8,0x040000dc):
   assert n==4;self.dma[a]=v
   if a==0x040000dc:
    assert v&0xffff0000==0x84000000,hex(v)
    src,dst=self.dma[0x040000d4],self.dma[0x040000d8]
    data=[self.read(src+j*4) for j in range(v&65535)]
    for j,value in enumerate(data):self.write(dst+j*4,value)
   return
  d,o=self.region(a,n);d[o:o+n]=v.to_bytes(n,'little')
 def nz(self,v):
  n,z,c,o=self.flags;self.flags=(bool(v&0x80000000),v==0,c,o)
 def arith(self,a,b,sub):
  value=(a-b if sub else a+b)&U
  c=a>=b if sub else a+b>U
  v=bool(((a^b) if sub else ~(a^b))&(a^value)&0x80000000)
  self.flags=(bool(value&0x80000000),value==0,c,v);return value
 def run(self,entry,stop,budget=200000):
  allowed={0x0800e5a8:(0x0800e5a8,0x0800edde),0x0800ee10:(0x0800ee10,0x0800f5c6),0x0800f69c:(0x0800f69c,0x0800f732)}
  lo,hi=allowed[entry];pc=entry;start=self.steps
  while pc!=stop:
   assert lo<=pc<hi and pc%2==0,('outside code',hex(pc))
   assert self.steps-start<budget,('instruction budget',hex(pc))
   if pc not in DECODE:DECODE[pc]=next(MD.disasm(ROM[pc-0x08000000:pc-0x08000000+4],pc,count=1))
   ins=DECODE[pc]
   self.steps+=1;self.visited[pc]=bytes(ins.bytes).hex()
   ops=ins.operands;m=ins.mnemonic;npc=pc+ins.size;self.reg[15]=pc+4
   reg=lambda o:rn(ins.reg_name(o.reg))
   def val(o):
    if o.type==ARM_OP_REG:return self.reg[reg(o)]
    assert o.type==ARM_OP_IMM,(hex(pc),m,ins.op_str)
    return o.imm&U
   if m in ('push','pop'):
    regs=sorted(reg(o) for o in ops)
    if m=='push':
     self.reg[13]-=4*len(regs)
     for j,k in enumerate(regs):self.write(self.reg[13]+4*j,self.reg[k])
    else:
     for j,k in enumerate(regs):
      value=self.read(self.reg[13]+4*j)
      if k==15:npc=value&~1
      else:self.reg[k]=value
     self.reg[13]+=4*len(regs)
   elif m in ('mov','movs'):
    value=val(ops[1]);self.reg[reg(ops[0])]=value
    if m=='movs':self.nz(value)
   elif m in ('add','adds','sub','subs','rsbs'):
    dst=reg(ops[0]);a=val(ops[1]) if len(ops)==3 else self.reg[dst];b=val(ops[2]) if len(ops)==3 else val(ops[1])
    if m=='rsbs':a,b=b,a
    self.reg[dst]=self.arith(a,b,m in ('sub','subs','rsbs')) if m.endswith('s') else (a-b if m=='sub' else a+b)&U
   elif m in ('lsls','lsrs','asrs'):
    a=val(ops[1]) if len(ops)==3 else self.reg[reg(ops[0])]
    bits=val(ops[-1]) if len(ops)==3 else val(ops[1])&255
    oldc=self.flags[2]
    if m=='lsls':value=(a<<bits)&U if bits<32 else 0;c=bool((a>>(32-bits))&1) if 0<bits<=32 else (oldc if bits==0 else False)
    elif m=='lsrs':value=a>>bits if bits<32 else 0;c=bool((a>>(bits-1))&1) if 0<bits<=32 else (oldc if bits==0 else False)
    else:value=(signed(a)>>min(bits,32))&U;c=bool((a>>(min(bits,32)-1))&1) if bits else oldc
    self.reg[reg(ops[0])]=value;self.flags=(bool(value&0x80000000),value==0,c,self.flags[3])
   elif m in ('muls','ands','orrs','eors','bics'):
    a=val(ops[1]);b=val(ops[2]) if len(ops)==3 else self.reg[reg(ops[0])]
    value={'muls':lambda:a*b,'ands':lambda:a&b,'orrs':lambda:a|b,'eors':lambda:a^b,'bics':lambda:b&~a}[m]()&U
    self.reg[reg(ops[0])]=value;self.nz(value)
   elif m=='cmp':self.arith(val(ops[0]),val(ops[1]),True)
   elif m=='tst':self.nz(val(ops[0])&val(ops[1]))
   elif m in ('ldr','ldrh','ldrsh','ldrb','ldrsb','str','strh','strb'):
    mem=ops[1].mem;a=self.reg[rn(ins.reg_name(mem.base))]
    if ins.reg_name(mem.base)=='pc':a&=~3
    if mem.index:a+=self.reg[rn(ins.reg_name(mem.index))]
    a=(a+mem.disp)&U;n=1 if m in ('ldrb','ldrsb','strb') else (2 if m in ('ldrh','ldrsh','strh') else 4)
    if m.startswith('str'):self.write(a,val(ops[0]),n)
    else:
     value=self.read(a,n)
     if m in ('ldrsh','ldrsb') and value&(1<<(8*n-1)):value-=1<<(8*n)
     self.reg[reg(ops[0])]=value&U
   elif m=='bl':
    target=val(ops[0]);assert target==0x08032488,('unexpected call',hex(target))
    self.calls[target]=self.calls.get(target,0)+1;self.reg[14]=npc|1
    a,b=map(signed,self.reg[:2]);assert b
    self.reg[0]=((abs(a)//abs(b))*(-1 if (a<0)!=(b<0) else 1))&U
   elif m=='bx':npc=val(ops[0])&~1
   elif m in ('b','beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl'):
    n,z,c,v=self.flags
    take={'b':True,'beq':z,'bne':not z,'bhs':c,'blo':not c,'bhi':c and not z,'bls':not c or z,'bge':n==v,'blt':n!=v,'bgt':not z and n==v,'ble':z or n!=v,'bmi':n,'bpl':not n}[m]
    if take:npc=val(ops[0])
   else:raise AssertionError(('unsupported instruction',hex(pc),m,ins.op_str))
   pc=npc
  return self

def lookup():
 p=FadeProbe().run(0x0800f69c,0x10020000)
 data=bytes(p.read(0x02001a10+i,1) for i in range(1024))
 assert data==bytes(progress*channel//31 for progress in range(32) for channel in range(32))
 assert p.calls=={0x08032488:1024}
 return data,p

if __name__=='__main__':
 lut,p=lookup()
 print('PASS original lookup generator:',len(lut),'bytes,',p.steps,'instructions,',len(p.visited),'instruction sites')
 (B/'work/native-normal-parent-20261015/lookup-original-result.json').write_text(json.dumps({'status':'PASS','rom_sha256':SHA,'lookup_sha256':hashlib.sha256(lut).hexdigest(),'bytes':1024,'actual_thumb_instructions':p.steps,'division_contract_calls':1024,'instructions':{f'{k:08x}':v for k,v in sorted(p.visited.items())}},indent=2)+'\n')
