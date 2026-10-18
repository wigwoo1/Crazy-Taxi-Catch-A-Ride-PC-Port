"""Bounded register-only execution of original ARM52B4 angle routine, diagnostic."""
def original_angle_difference(a,b):
 pc=0x52b4;regs=[a&0xffffffff,b&0xffffffff,0,0];comparison=0
 for _ in range(40):
  assert 0x52b4<=pc<0x531c and pc%4==0
  w=int.from_bytes(r[pc:pc+4],'little');condition=w>>28
  execute={14:True,11:comparison<0,12:comparison>0,10:comparison>=0}[condition]
  if not execute:pc+=4;continue
  if w&0x0ffffff0==0x012fff10:
   assert w&15==14;return regs[0]
  if w&0x0e000000==0x0a000000:
   assert not w&(1<<24);off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;pc+=8+off*4;continue
  if w&0x0c000000==0x04000000:
   assert w==0xe51f20f4;regs[2]=int.from_bytes(r[pc+8-0xf4:pc+12-0xf4],'little');pc+=4;continue
  assert w&0x0c000000==0
  rn=(w>>16)&15;rd=(w>>12)&15;op=(w>>21)&15
  if w&(1<<25):
   v=w&255;n=((w>>8)&15)*2;v=((v>>n)|(v<<(32-n if n else 0)))&0xffffffff if n else v
  else:assert w&0xff0==0;v=regs[w&15]
  if op==10:
   assert w&(1<<20);comparison=(regs[rn]-v);pc+=4;continue
  assert not w&(1<<20)
  if op==0:value=regs[rn]&v
  elif op==2:value=regs[rn]-v
  elif op==3:value=v-regs[rn]
  elif op==4:value=regs[rn]+v
  elif op==13:value=v
  else:raise AssertionError(('unsupported angle instruction',hex(pc),hex(w)))
  regs[rd]=value&0xffffffff;pc+=4
 raise AssertionError('angle instruction bound')
# Confirm Thumb veneer switches to ARM and branches to this exact body.
assert r[0x34094:0x34098].hex()=='7847c046'
assert int.from_bytes(r[0x34098:0x3409c],'little')==0xeaff4485
