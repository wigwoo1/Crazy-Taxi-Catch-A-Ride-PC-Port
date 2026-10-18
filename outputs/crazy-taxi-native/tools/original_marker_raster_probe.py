"""Bounded relocated original ARM marker raster diagnostic. Never linked into game."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import re
from capstone import CS_MODE_ARM
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
BASE=0x10000000;FB=0x11000000;PACK=0x12000000;DESC=0x12000100;STACK=0x13001000
MASK=0xffffffff
def signed(x):return x-0x100000000 if x&0x80000000 else x
class MarkerRasterProbe:
 def __init__(self,pixels,rect,clipped):
  self.blocks=[(BASE,bytearray(r[0x700:0x4f5c])),(FB,bytearray([173])*38400),(PACK,bytearray(512)),(STACK-4096,bytearray(4096))]
  self.reg=[0]*16;self.reg[13]=STACK;self.reg[14]=0xffffffff;self.reg[1]=PACK;self.reg[2]=DESC
  self.steps=0;self.z=False;self.n=False;self.v=False;self.c=False
  self.write(BASE+0x49e8-0x700,BASE+0x1db0-0x700,4)
  self.write(BASE+0x49ec-0x700,BASE+0x1dc4-0x700,4)
  for j,x in enumerate([0,0,240,160]):self.write(BASE+0x1db0-0x700+j*4,x,4)
  self.write(BASE+0x1dc4-0x700,FB,4)
  for off,x in zip([32,44,96,108],rect):self.write(PACK+off,x,4)
  self.write(DESC,0x06064040,4);self.write(DESC+4,pixels,4)
  self.entry=BASE+(0x49f8 if clipped else 0x4a94)-0x700
 def locate(self,a,n):
  for b,data in self.blocks:
   if b<=a and a+n<=b+len(data):return data,a-b
  raise AssertionError(('unmapped',hex(a),n))
 def read(self,a,n=4):
  if 0x8000000<=a and a+n<=0x8000000+len(r):return int.from_bytes(r[a-0x8000000:a-0x8000000+n],'little')
  data,o=self.locate(a,n);return int.from_bytes(data[o:o+n],'little')
 def write(self,a,x,n=4):
  data,o=self.locate(a,n);data[o:o+n]=(x&((1<<(8*n))-1)).to_bytes(n,'little')
 def ri(self,s):return {'sb':9,'sl':10,'fp':11,'ip':12,'sp':13,'lr':14,'pc':15}.get(s,int(s[1:]) if s.startswith('r') else -1)
 def val(self,s):return int(s[1:],0)&MASK if s.startswith('#') else self.reg[self.ri(s)]
 def operand(self,parts):
  x=self.val(parts[0])
  if len(parts)==1:return x
  kind,arg=parts[1].split();n=self.val(arg)&255
  if kind=='lsl':return (x<<n)&MASK
  if kind=='lsr':return x>>n
  if kind=='asr':return (signed(x)>>n)&MASK
  raise AssertionError(kind)
 def flags_sub(self,a,b):
  x=(a-b)&MASK;self.z=x==0;self.n=bool(x>>31);self.c=a>=b;self.v=bool(((a^b)&(a^x))>>31);return x
 def run(self):
  pc=self.entry
  while pc!=0xffffffff:
   self.steps+=1;assert self.steps<1000000
   off=pc-BASE+0x700
   assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)
   w=self.read(pc);cond=w>>28
   ok={14:True,0:self.z,1:not self.z,11:self.n!=self.v,12:not self.z and self.n==self.v,13:self.z or self.n!=self.v,2:self.c,3:not self.c,4:self.n,5:not self.n,8:self.c and not self.z,9:not self.c or self.z}.get(cond)
   assert ok is not None,cond
   self.reg[15]=pc+8;nextpc=pc+4
   if not ok:pc=nextpc;continue
   i=next(arm.disasm(w.to_bytes(4,'little'),pc,count=1));m=i.mnemonic
   if cond!=14:m=m[:-2]
   p=i.op_str.split(', ')
   if m in ('push','pop'):
    regs=[self.ri(x) for x in i.op_str[1:-1].split(', ')];n=len(regs)*4
    if m=='push':
     self.reg[13]-=n
     for j,k in enumerate(regs):self.write(self.reg[13]+j*4,self.reg[k])
    else:
     for j,k in enumerate(regs):self.reg[k]=self.read(self.reg[13]+j*4)
     self.reg[13]+=n
   elif m in ('ldr','ldrb','str','strh'):
    rd,addr=i.op_str.split(', ',1);terms=addr[1:-1].split(', ');a=(self.val(terms[0])+ (self.val(terms[1]) if len(terms)>1 else 0))&MASK
    n=1 if m=='ldrb' else 2 if m=='strh' else 4
    if m.startswith('ldr'):self.reg[self.ri(rd)]=self.read(a,n)
    else:self.write(a,self.val(rd),n)
   elif m in ('b','bl'):
    if m=='bl':self.reg[14]=pc+4
    nextpc=self.val(p[0])
   elif m=='bx':nextpc=self.val(p[0])
   elif m=='cmp':self.flags_sub(self.val(p[0]),self.operand(p[1:]))
   elif m=='tst':
    x=self.val(p[0])&self.operand(p[1:]);self.z=x==0;self.n=bool(x>>31)
   elif m in ('mov','add','sub','subs','bic','and','orrs','mul','lsl','asr','lsr','rsb','adc','eor','ands'):
    rd=self.ri(p[0]);a=self.val(p[1]);b=self.operand(p[2:]) if len(p)>2 else 0
    if m=='mov':x=a
    elif m=='add':x=a+b
    elif m=='sub':x=a-b
    elif m=='subs':x=self.flags_sub(a,b)
    elif m=='bic':x=a&~b
    elif m=='and':x=a&b
    elif m=='ands':
     x=a&b;self.z=x==0;self.n=bool(x>>31)
    elif m=='rsb':x=b-a
    elif m=='adc':x=a+b+int(self.c)
    elif m=='eor':x=a^b
    elif m=='lsr':x=a>>(b&255)
    elif m=='orrs':
     x=a|b;self.z=(x&MASK)==0;self.n=bool((x&MASK)>>31)
    elif m=='mul':x=a*b
    elif m=='lsl':x=a<<(b&255)
    else:x=signed(a)>>(b&255)
    self.reg[rd]=x&MASK
   else:raise AssertionError((hex(pc),m,i.op_str))
   pc=nextpc
  return bytes(self.blocks[1][1])
