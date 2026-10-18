"""Bounded original ARM0560 diagnostic; never linked into native runtime."""
from pathlib import Path
import struct,json,hashlib
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert rom[0x3406c:0x34074]==bytes.fromhex('7847c0463a31ffea')
rows=[]
for count in (0,4,28,32,36,224,252,256,260,512,1024,17168):
 src,dst,sp=0x8071000,0x6010000,0x3007f00
 reg=[0]*16;reg[:3]=[src,dst,count];reg[4:12]=[0x12340000+i for i in range(8)];saved=reg[4:12].copy();reg[13]=sp;reg[14]=0x10020001
 mem={};stores=[];pc=0x8000560;z=False;steps=0
 def read(a):
  if 0x8000000<=a<=0x8000000+len(rom)-4:return struct.unpack_from('<I',rom,a-0x8000000)[0]
  assert a in mem,hex(a)
  return mem[a]
 def write(a,v):
  assert dst<=a<dst+count or sp-32<=a<sp,hex(a)
  assert not 0x20034dc<=a<0x200351e
  mem[a]=v&0xffffffff;stores.append(a)
 while pc!=reg[14]:
  assert 0x8000560<=pc<=0x80005f0 and pc%4==0,hex(pc)
  steps+=1;assert steps<20000
  w=read(pc);nxt=pc+4;cond=w>>28
  assert cond in (0,1,14)
  if (cond==0 and not z) or (cond==1 and z):pc=nxt;continue
  if w==0xe92d0ff0:
   reg[13]-=32
   for j in range(8):write(reg[13]+j*4,reg[j+4])
  elif w==0xe8bd0ff0:
   reg[4:12]=[read(reg[13]+j*4) for j in range(8)];reg[13]+=32
  elif w in (0xe8b00ff0,0xe8a10ff0):
   if w==0xe8b00ff0:reg[4:12]=[read(reg[0]+j*4) for j in range(8)];reg[0]+=32
   else:
    for j in range(8):write(reg[1]+j*4,reg[j+4])
    reg[1]+=32
  elif w==0xe4904004:reg[4]=read(reg[0]);reg[0]+=4
  elif w==0xe4814004:write(reg[1],reg[4]);reg[1]+=4
  elif w==0xe12fff1e:nxt=reg[14]
  elif w&0x0f000000==0x0a000000:
   off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;nxt=pc+8+off*4
  elif w in (0xe1b03422,0xe1b032a3,0xe1b03123):
   n={0xe1b03422:8,0xe1b032a3:5,0xe1b03123:2}[w];reg[3]=(reg[2] if w==0xe1b03422 else reg[3])>>n;z=reg[3]==0
  elif w in (0xe20230e0,0xe212301c):reg[3]=reg[2]&(0xe0 if w==0xe20230e0 else 0x1c);z=reg[3]==0 if w==0xe212301c else z
  elif w==0xe2533001:reg[3]=(reg[3]-1)&0xffffffff;z=reg[3]==0
  else:raise AssertionError((hex(pc),hex(w)))
  pc=nxt
 assert reg[0]==src+count and reg[1]==dst+count and reg[3]==0 and reg[13]==sp and reg[4:12]==saved
 assert bytes((mem[dst+j//4*4]>>(j%4*8))&255 for j in range(count))==rom[src-0x8000000:src-0x8000000+count]
 assert len([a for a in stores if dst<=a<dst+count])==count//4
 rows.append(dict(bytes=count,instructions=steps,destination=hex(dst),stack=hex(sp-32)))
report=dict(status='PASS',rom_sha256=hashlib.sha256(rom).hexdigest(),cases=rows,scope='Exact original3406C Thumb interwork wrapper and ARM0560 body. Actual instruction execution for aligned byte counts0..17168; exact uploaded ROM bytes, pointers, scratch, stack and callee saved registers. Stores restricted to destination count bytes plus32-byte stack; no retained-input overlap. No hardware DMA, compression, installed32454copy, complete bootstrap or runtime emulation claim.')
(root/'reports/bound-raw-copy-arm-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),'original ARM raw-copy cases')
