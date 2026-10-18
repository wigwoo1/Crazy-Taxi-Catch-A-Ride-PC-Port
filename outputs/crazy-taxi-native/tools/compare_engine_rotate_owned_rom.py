"""Bounded diagnostic execution of original ARM matrix code, never a game runtime.
Only unconditional instructions in the explicitly allowed source routines are
supported. Unmapped reads/writes and unrecognized instructions fail closed.
No GBA devices, game loop, display, audio, interrupts or BIOS are modeled.
"""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import subprocess,random,json
MASK=0xffffffff
def signed(v):
 v&=MASK;return v-0x100000000 if v&0x80000000 else v
def ror(v,n):
 n%=32;return ((v>>n)|(v<<(32-n)))&MASK if n else v
allowed=[(0x718,0x738),(0x738,0x954),(0x954,0x9bc),(0xa40,0xaa8),(0xaa8,0xb10),(0xb10,0xc48)]
class Probe:
 def __init__(self):
  self.reg=[0]*16;self.reg[13]=0x10010000;self.mem={};self.steps=0
  # Original ARM readers consume relocated signed Q8 words.
  for base,source in [(0x202c4f0,0x84a0d08),(0x20304f0,0x84a2d08)]:
   for i in range(4096):self.mem[base+i*4]=(struct.unpack_from('<h',r,source-0x8000000+i*2)[0]>>4)&MASK
 def read(self,addr):
  if 0x8000000<=addr<0x8000000+len(r):return struct.unpack_from('<I',r,addr-0x8000000)[0]
  assert addr in self.mem,('unmapped read',hex(addr));return self.mem[addr]
 def write(self,addr,value):
  assert (0x10000000<=addr<=0x10010000) or (0x3006a50<=addr<0x3006a90),('write',hex(addr))
  self.mem[addr]=value&MASK
 def run(self,entry,args):
  self.reg[:len(args)]=args;self.reg[14]=0xffffffff;pc=entry
  while pc!=0xffffffff:
   assert any(0x8000000+a<=pc<0x8000000+b for a,b in allowed),hex(pc)
   self.steps+=1;assert self.steps<100000
   w=self.read(pc);assert w>>28==14,('conditional',hex(pc),hex(w))
   self.reg[15]=(pc+8)&MASK;nextpc=pc+4
   if w&0x0ffffff0==0x012fff10:nextpc=self.reg[w&15]
   elif w&0x0e000000==0x0a000000:
    off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off
    if w&(1<<24):self.reg[14]=pc+4
    nextpc=pc+8+off*4
   elif w&0x0e000000==0x08000000:
    rn=(w>>16)&15;regs=[i for i in range(16) if w&(1<<i)];base=self.reg[rn];n=len(regs)
    up=bool(w&(1<<23));pre=bool(w&(1<<24));load=bool(w&(1<<20))
    addr=base+(4 if pre else 0) if up else base-4*n+(0 if pre else 4)
    values=[self.read(addr+4*j) for j in range(n)] if load else [self.reg[i] for i in regs]
    for j,i in enumerate(regs):
     if load:self.reg[i]=values[j]
     else:self.write(addr+4*j,values[j])
    if w&(1<<21):self.reg[rn]=(base+(4*n if up else -4*n))&MASK
   elif w&0x0c000000==0x04000000:
    assert not w&(1<<25) and not w&(1<<22) and w&(1<<24) and not w&(1<<21)
    rn=(w>>16)&15;rd=(w>>12)&15;addr=self.reg[rn]+((w&4095) if w&(1<<23) else -(w&4095))
    if w&(1<<20):self.reg[rd]=self.read(addr)
    else:self.write(addr,self.reg[rd])
   elif w&0x0fc000f0==0x00000090:
    rd=(w>>16)&15;rn=(w>>12)&15;rs=(w>>8)&15;rm=w&15
    self.reg[rd]=(self.reg[rm]*self.reg[rs]+(self.reg[rn] if w&(1<<21) else 0))&MASK
   elif w&0x0c000000==0:
    assert not w&(1<<20),('flags',hex(w))
    rd=(w>>12)&15;rn=(w>>16)&15;op=(w>>21)&15
    if w&(1<<25):v=ror(w&255,((w>>8)&15)*2)
    else:
     assert not w&16
     v=self.reg[w&15];n=(w>>7)&31;shift=(w>>5)&3
     if shift==0:v=(v<<n)&MASK
     elif shift==1:v=v>>(n or 32)
     elif shift==2:v=(signed(v)>>(n or 32))&MASK
     else:raise AssertionError(('unsupported shift',hex(w)))
    if op==0:value=self.reg[rn]&v
    elif op==1:value=self.reg[rn]^v
    elif op==2:value=self.reg[rn]-v
    elif op==3:value=v-self.reg[rn]
    elif op==4:value=self.reg[rn]+v
    elif op==13:value=v
    else:raise AssertionError(('unsupported op',hex(w)))
    self.reg[rd]=value&MASK
   else:raise AssertionError(('unsupported',hex(pc),hex(w)))
   pc=nextpc
 def point(self,mode,p,angles):
  out,source,destination=0x10000000,0x10000100,0x10000200
  self.run(0x8000aa8 if mode else 0x8000a40,[out,*[x&MASK for x in angles]])
  self.run(0x8000718,[out])
  for i,v in enumerate(p):self.write(source+i*4,v)
  self.run(0x8000954,[source,destination])
  return [signed(self.read(destination+i*4)) for i in range(3)]
rng=random.Random(0x154cc);cases=[]
for mode in [0,1]:
 for angle in [0,1,79,1024,2048,3073,4095,-1]:
  for p in [(0,468,-1024),(0,-660,1024),(-21455,38075,0),(0,0,0)]:cases.append((mode,*p,0,angle,0))
 for _ in range(256):
  cases.append((mode,*[rng.randrange(-0x80000000,0x80000000) for _ in range(3)],*[rng.randrange(-8192,8193) for _ in range(3)]))
text=''.join(' '.join(map(str,c))+'\n' for c in cases)
native=subprocess.run(['outputs/crazy-taxi-native/bin/engine-rotate-owned-rom-compare.exe'],input=text,text=True,capture_output=True,check=True)
actual=[list(map(int,line.split())) for line in native.stdout.splitlines()];assert len(actual)==len(cases)
steps=0
for case,want in zip(cases,actual):
 probe=Probe();got=probe.point(case[0],case[1:4],case[4:7]);steps+=probe.steps
 assert got==want,{'case':case,'original':got,'native':want}
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'executed_original_instructions':steps,'allowed_rom_ranges':allowed,'finding':'Original A40 Y*X*Z and AA8 Z*X*Y matrix construction,718 matrix copy,954 wrapped point transform agree exactly with native engine_rotate_owned throughoriginalmatrixglobal routing for original Q8 trig, cardinal and3073 rounding witnesses and full signed32 random points.','limits':'Bounded purpose-built diagnostic instruction executor only; supported AL instructions checked, unmapped accesses fail closed. No full ARM CPU/device execution validation, live game camera mode/bootstrap, GBA pixel oracle, game timing or display fidelity proof. No diagnostic execution linked into playable port.'}
pathlib.Path('outputs/crazy-taxi-native/reports/engine-rotate-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(f'PASS {len(cases)} native/original bounded ARM matrix comparisons ({steps} original instructions); no full-game fidelity claim.')
