"""Bounded original Thumb numeric-library diagnostic. Never linked into game."""
from pathlib import Path
import sys,struct
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm import ARM_OP_IMM
ROM=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
MD=Cs(CS_ARCH_ARM,CS_MODE_THUMB);MD.detail=True
MASK=0xffffffff
CODE_CACHE={}
class NumericProbe:
 def __init__(self):self.reg=[0]*16;self.reg[13]=0x10010000;self.mem={};self.n=self.z=self.c=self.v=False;self.steps=0
 def signed(self,x):return x-0x100000000 if x&0x80000000 else x
 def read(self,a,size=4):
  if 0x08000000<=a and a+size<=0x08800000:return int.from_bytes(ROM[a-0x08000000:a-0x08000000+size],'little')
  assert 0x10000000<=a and a+size<=0x10010000,('read',hex(a))
  return sum(self.mem.get(a+i,0)<<(8*i) for i in range(size))
 def write(self,a,x,size=4):
  assert 0x10000000<=a and a+size<=0x10010000,('write',hex(a))
  for i in range(size):self.mem[a+i]=(x>>(8*i))&255
 def rn(self,ins,o):return {'sb':9,'sl':10,'fp':11,'ip':12,'sp':13,'lr':14,'pc':15}.get(ins.reg_name(o.reg),int(ins.reg_name(o.reg)[1:]) if ins.reg_name(o.reg).startswith('r') else -1)
 def val(self,ins,o):return o.imm&MASK if o.type==ARM_OP_IMM else self.reg[self.rn(ins,o)]
 def nz(self,x):self.n=bool(x&0x80000000);self.z=(x&MASK)==0
 def arith(self,a,b,sub,carry=0):
  t=a-b-carry if sub else a+b+carry;x=t&MASK;self.nz(x);self.c=t>=0 if sub else t>MASK;self.v=bool(((a^b)&(a^x) if sub else ~(a^b)&(a^x))&0x80000000);return x
 def run(self,entry,stop=0x15000000):
  pc=entry;self.reg[14]=stop|1
  while pc!=stop:
   assert 0x08032000<=pc<0x08033500,('outside',hex(pc));self.steps+=1;assert self.steps<20000
   if pc not in CODE_CACHE:CODE_CACHE[pc]=next(MD.disasm(ROM[pc-0x08000000:pc-0x08000000+4],pc))
   ins=CODE_CACHE[pc];o=ins.operands;m=ins.mnemonic;nxt=pc+ins.size;self.reg[15]=pc+4
   rn=lambda q:self.rn(ins,q);val=lambda q:self.val(ins,q)
   if m=='push':
    ids=sorted(rn(q) for q in o);self.reg[13]-=len(ids)*4
    for i,k in enumerate(ids):self.write(self.reg[13]+i*4,self.reg[k])
   elif m=='pop':
    ids=sorted(rn(q) for q in o)
    for i,k in enumerate(ids):self.reg[k]=self.read(self.reg[13]+i*4)
    self.reg[13]+=len(ids)*4
    if 15 in ids:nxt=self.reg[15]&~1
   elif m in ['mov','movs']:
    x=val(o[1]);self.reg[rn(o[0])]=x
    if m=='movs':self.nz(x)
   elif m in ['add','adds','sub','subs','rsbs','adcs','sbcs','cmp','cmn']:
    if m in ['cmp','cmn']:a=val(o[0]);b=val(o[1])
    else:a=val(o[1]) if len(o)==3 else self.reg[rn(o[0])];b=val(o[2]) if len(o)==3 else val(o[1])
    if m=='rsbs':a,b=b,a
    sub=m in ['sub','subs','rsbs','sbcs','cmp'];carry=(0 if self.c else 1) if m=='sbcs' else int(self.c) if m=='adcs' else 0
    x=self.arith(a,b,sub,carry) if m.endswith('s') or m in ['cmp','cmn'] else (a-b if sub else a+b)&MASK
    if m not in ['cmp','cmn']:self.reg[rn(o[0])]=x
   elif m in ['ands','orrs','eors','bics','muls','tst']:
    a=val(o[0]);b=val(o[1]);x={'ands':a&b,'orrs':a|b,'eors':a^b,'bics':a&~b,'muls':a*b,'tst':a&b}[m]&MASK;self.nz(x)
    if m!='tst':self.reg[rn(o[0])]=x
   elif m in ['lsls','lsrs','asrs']:
    a=val(o[1]) if len(o)==3 else val(o[0]);n=(val(o[2]) if len(o)==3 else val(o[1]))&255
    if not n:x=a
    elif m=='lsls':self.c=bool((a>>(32-n))&1) if n<=32 else False;x=(a<<n)&MASK
    elif m=='lsrs':self.c=bool((a>>(n-1))&1) if n<=32 else False;x=a>>n
    else:self.c=bool((a>>(n-1))&1) if n<32 else bool(a&0x80000000);x=(self.signed(a)>>n)&MASK
    self.reg[rn(o[0])]=x;self.nz(x)
   elif m in ['ldr','ldrb','ldrh','str','strb','strh']:
    t=o[1].mem;base=MD.reg_name(t.base);a=(((pc+4)&~3) if base=='pc' else self.reg[{'sp':13}.get(base,int(base[1:]) if base.startswith('r') else -1)])+t.disp+(self.reg[self.rn(ins,type('O',(),{'reg':t.index})())] if t.index else 0);size=1 if m.endswith('b') else 2 if m.endswith('h') else 4
    if m.startswith('str'):self.write(a,val(o[0]),size)
    else:self.reg[rn(o[0])]=self.read(a,size)
   elif m=='bl':self.reg[14]=nxt|1;nxt=o[0].imm
   elif m=='bx':nxt=val(o[0])&~1
   elif m.startswith('b'):
    take={'b':True,'beq':self.z,'bne':not self.z,'bge':self.n==self.v,'blt':self.n!=self.v,'bgt':not self.z and self.n==self.v,'ble':self.z or self.n!=self.v,'bhi':self.c and not self.z,'bls':not self.c or self.z,'bhs':self.c,'blo':not self.c,'bpl':not self.n,'bmi':self.n}[m]
    if take:nxt=o[0].imm
   else:raise AssertionError(('unsupported',hex(pc),m,ins.op_str))
   pc=nxt
  assert self.reg[13]==0x10010000
  return self.reg[:2]
