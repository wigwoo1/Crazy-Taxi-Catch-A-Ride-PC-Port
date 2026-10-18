# Offline diagnostic execution of supplied ROM050AC only; never game runtime.
def execute_search(owner,rom):
 mask=0xffffffff;before=owner.reg.copy();regs=owner.reg;flags=list(owner.flags);pc=0x080050ac;steps=0;stores=[]
 assert rom[0x34074:0x3407c].hex()=='7847c0460b44ffea'
 def sign(x):return x-0x100000000 if x&0x80000000 else x
 scene=owner.read(0x03006810,4)
 count=owner.read(scene+14,2);assert 0<=count<=128
 customers=0x02005f2c;assert int.from_bytes(rom[0x5098:0x509c],'little')==customers
 spawns=owner.read(scene+0x30,4)
 prior=bytes(owner.read(customers+i,1) for i in range(count*44))
 spawn_xy=[[sign(owner.read(spawns+32*i+4,4)),sign(owner.read(spawns+32*i+12,4))] for i in range(count)]
 while True:
  assert 0x080050ac<=pc<0x080051c8 and pc%4==0;steps+=1;assert steps<=128*80+32
  w=int.from_bytes(rom[pc-0x08000000:pc-0x08000000+4],'little');n,z,c,v=flags;cond=w>>28
  run={0:z,1:not z,10:n==v,11:n!=v,12:not z and n==v,13:z or n!=v,14:True}[cond]
  if not run:pc+=4;continue
  regs[15]=pc+8
  if w&0x0ffffff0==0x012fff10:
   assert w&15==14;break
  if w&0x0e000000==0x0a000000:
   assert not w&(1<<24);off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;pc+=8+off*4;continue
  if w&0x0e000000==0x08000000:
   assert (w>>16)&15==13;ids=[i for i in range(16) if w&(1<<i)];load=bool(w&(1<<20));assert (load and w==0xe8bd5ff0) or (not load and w==0xe92d5ff0)
   address=regs[13] if load else regs[13]-4*len(ids)
   for i,k in enumerate(ids):
    if load:regs[k]=owner.read(address+4*i,4)
    else:assert before[13]-40<=address+4*i<before[13];owner.write(address+4*i,regs[k],4)
   regs[13]+=4*len(ids) if load else -4*len(ids);pc+=4;continue
  if w&0x0fc000f0==0x00000090:
   assert not w&(1<<20);rd=(w>>16)&15;value=regs[w&15]*regs[(w>>8)&15]
   if w&(1<<21):value+=regs[(w>>12)&15]
   regs[rd]=value&mask;pc+=4;continue
  if w&0x0e000090==0x00000090:
   assert w&(1<<20) and w&(1<<22) and w&(1<<24) and not w&(1<<21) and w&0x60==0x20
   off=((w>>4)&0xf0)|(w&15);address=(regs[(w>>16)&15]+(off if w&(1<<23) else -off))&mask;regs[(w>>12)&15]=owner.read(address,2);pc+=4;continue
  if w&0x0c000000==0x04000000:
   assert not w&(1<<25) and w&(1<<24) and not w&(1<<21)
   address=(regs[(w>>16)&15]+((w&4095) if w&(1<<23) else -(w&4095)))&mask;rd=(w>>12)&15;size=1 if w&(1<<22) else 4
   if w&(1<<20):regs[rd]=owner.read(address,size)
   else:
    assert (size==1 and customers<=address<customers+44*count and (address-customers)%44==0) or (size==4 and address==before[2]);owner.write(address,regs[rd],size);stores.append([address,size])
   pc+=4;continue
  assert w&0x0c000000==0
  op=(w>>21)&15;rd=(w>>12)&15;left=regs[(w>>16)&15]
  if w&(1<<25):
   val=w&255;rot=((w>>8)&15)*2;right=((val>>rot)|(val<<(32-rot)))&mask if rot else val
  else:
   assert not w&16;right=regs[w&15];shift=(w>>7)&31;kind=(w>>5)&3
   if kind==0:right=(right<<shift)&mask
   elif kind==2:right=(sign(right)>>(shift or 32))&mask
   else:raise AssertionError(('unsupportedshift',hex(pc),kind))
  if op==0:result=left&right
  elif op==1:result=left^right
  elif op in (2,10):result=(left-right)&mask
  elif op==3:result=(right-left)&mask
  elif op==4:result=(left+right)&mask
  elif op==8:result=left&right
  elif op==12:result=left|right
  elif op==13:result=right
  else:raise AssertionError(('unsupportedoperation',hex(pc),op))
  if op in (8,10):
   flags[0]=bool(result&0x80000000);flags[1]=result==0
   if op==10:flags[2]=left>=right;flags[3]=bool((left^right)&(left^result)&0x80000000)
  else:assert not w&(1<<20);regs[rd]=result
  pc+=4
 assert regs[4:15]==before[4:15] and regs[13]==before[13]
 owner.flags=flags;owner.steps+=steps
 return {'count':count,'arguments':before[:3],'before':prior.hex(),'after':bytes(owner.read(customers+i,1) for i in range(count*44)).hex(),'spawns':spawn_xy,'index':sign(regs[0]),'distance':sign(owner.read(before[2],4)),'instructions':steps,'stores':stores}
