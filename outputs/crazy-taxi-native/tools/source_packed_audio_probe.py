"""Bounded offline actual ARM0320 decoder; never a native game dependency."""
import re
from capstone import Cs,CS_ARCH_ARM,CS_MODE_ARM
def packed_audio_original(p,rom):
 code={i.address:i for i in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(rom[0x320:0x468],0x8000320)}
 regs=p.reg[:];stop=regs[14]&~1;pc=0x8000320;steps=0;z=n=v=False;mask=0xffffffff
 def ri(name):
  name=name.strip();return {'sp':13,'lr':14,'pc':15,'sb':9,'sl':10,'fp':11,'ip':12}.get(name,int(name[1:]) if name.startswith('r') else -1)
 def val(name):return int(name.strip()[1:],0) if name.strip().startswith('#') else regs[ri(name)]
 while pc!=stop:
  assert pc in code,hex(pc);i=code[pc];assert rom[pc-0x8000000:pc-0x8000000+4]==bytes(i.bytes);steps+=1;assert steps<20000;regs[15]=pc+8;nextpc=pc+4;m=i.mnemonic;o=i.op_str
  if m in ['andeq','lsrne']:
   if (m.endswith('eq') and not z) or (m.endswith('ne') and z):pc=nextpc;continue
   m=m[:-2]
  if m in ['push','pop','stmdb','ldm']:
   names=o[o.index('{')+1:o.index('}')].split(',');ids=[ri(name) for name in names]
   if m in ['push','stmdb']:
    regs[13]-=4*len(ids)
    for k,index in enumerate(ids):p.write(regs[13]+k*4,regs[index])
   else:
    for k,index in enumerate(ids):regs[index]=p.read(regs[13]+k*4)
    regs[13]+=4*len(ids)
  elif m in ['ldr','ldrb','ldrsb','ldrsh','strh','strb']:
   dest,rest=o.split(',',1);inside=rest[rest.index('[')+1:rest.index(']')].split(',');address=regs[ri(inside[0])]
   if len(inside)>1:
    offset=val(inside[1])
    if len(inside)>2:
     shift,amount=inside[2].strip().split();assert shift in ['lsr','lsl'];offset=offset>>val(amount) if shift=='lsr' else offset<<val(amount)
    address=(address+offset)&mask
   size=4 if m=='ldr' else 2 if m in ['ldrsh','strh'] else 1
   if m.startswith('str'):p.write(address,regs[ri(dest)],size)
   else:
    value=p.read(address,size)
    if m in ['ldrsb','ldrsh'] and value&(1<<(size*8-1)):value-=1<<(size*8)
    regs[ri(dest)]=value&mask
   tail=rest[rest.index(']')+1:].strip()
   if tail:assert tail.startswith(',');regs[ri(inside[0])]=(regs[ri(inside[0])]+val(tail[1:]))&mask
  elif m in ['beq','bgt']:
   if z if m=='beq' else not z and n==v:nextpc=int(o[1:],0)
  elif m=='bx':nextpc=val(o)&~1
  else:
   parts=[part.strip() for part in o.split(',')];dest=ri(parts[0])
   if m=='mov':regs[dest]=val(parts[1])
   elif m in ['add','mul','and','ands','subs','lsr','lsrs']:
    a,b=val(parts[1]),val(parts[2])
    if len(parts)==4:
     shift,amount=parts[3].split();assert shift=='lsl';b=(b<<val(amount))&mask
    value=(a+b if m=='add' else a*b if m=='mul' else a&b if m in ['and','ands'] else a-b if m=='subs' else a>>b)&mask;regs[dest]=value
    if m in ['ands','subs','lsrs']:
     z=value==0;n=bool(value&0x80000000)
     if m=='subs':v=bool(((a^b)&(a^value))&0x80000000)
   else:raise AssertionError((hex(pc),m,o))
  pc=nextpc
 p.reg[:15]=regs[:15];return steps
