"""Fail-closed diagnostic original ARM046C..055C, not a game runtime."""
import sys,struct,json,hashlib,itertools,subprocess
from pathlib import Path
sys.path.insert(0,'work/python-packages')
from capstone import *
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
code={i.address:i for i in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(rom[0x46c:0x560],0x800046c)}
def signed(v):return (v&0xffffffff)-0x100000000 if v&0x80000000 else v&0xffffffff
class ArmLoop:
 def __init__(self,pointer,length,count,phase,step,volume):
  self.reg=[0]*16;self.reg[:4]=[0x10000000,pointer,phase,step];self.reg[13]=0x10001000;self.reg[14]=0xffffffff;self.mem={};self.z=self.n=self.v=False;self.steps=0
  for j,x in enumerate([length,0,count,volume]):self.write(self.reg[13]+j*4,x,4)
  for j in range(count):self.write(0x10000000+j*2,j*257-30000,2)
 def read(self,a,n):
  if 0x8000000<=a and a+n<=0x8800000:return int.from_bytes(rom[a-0x8000000:a-0x8000000+n],'little')
  assert all(a+j in self.mem for j in range(n)),('unmapped',hex(a),n)
  return sum(self.mem[a+j]<<(j*8) for j in range(n))
 def write(self,a,v,n):
  assert 0x10000000<=a and a+n<=0x10001020
  for j in range(n):self.mem[a+j]=(v>>(j*8))&255
 def ri(self,s):
  s=s.strip();return {'sp':13,'lr':14,'pc':15,'sb':9,'sl':10}.get(s,int(s[1:]) if s.startswith('r') else -1)
 def val(self,s):return int(s[1:],0) if s.startswith('#') else self.reg[self.ri(s)]
 def flags_sub(self,a,b):
  x=(a-b)&0xffffffff;self.z=x==0;self.n=bool(x&0x80000000);self.v=bool(((a^b)&(a^x))&0x80000000);return x
 def run(self):
  pc=0x800046c
  while pc!=0xffffffff:
   assert pc in code,hex(pc);ins=code[pc];assert self.read(pc,4)==int.from_bytes(ins.bytes,'little');self.steps+=1;assert self.steps<10000
   m,o=ins.mnemonic,ins.op_str;nextpc=pc+4
   if m.endswith('ge'):
    if self.n!=self.v:pc=nextpc;continue
    m=m[:-2]
   if m in ['beq','bgt']:
    if (self.z if m=='beq' else not self.z and self.n==self.v):nextpc=int(o[1:],0)
   elif m=='bx':nextpc=self.val(o)
   elif m in ['push','pop']:
    ids=[self.ri(x) for x in o.strip('{}').split(',')]
    if m=='push':
     self.reg[13]-=4*len(ids)
     for j,k in enumerate(ids):self.write(self.reg[13]+j*4,self.reg[k],4)
    else:
     for j,k in enumerate(ids):self.reg[k]=self.read(self.reg[13]+j*4,4)
     self.reg[13]+=4*len(ids)
   elif m in ['ldr','ldrsb','ldrsh','strh']:
    dest,addr=o.split(',',1);a=addr.strip();inside=a[a.index('[')+1:a.index(']')].split(',');base=self.ri(inside[0]);address=self.reg[base]+(self.val(inside[1].strip()) if len(inside)>1 else 0);n=4 if m=='ldr' else 1 if m=='ldrsb' else 2
    if m=='strh':self.write(address,self.reg[self.ri(dest)],n)
    else:
     x=self.read(address,n)
     if m!='ldr' and x&(1<<(n*8-1)):x-=1<<(n*8)
     self.reg[self.ri(dest)]=x&0xffffffff
    tail=a[a.index(']')+1:].strip()
    if tail:assert tail.startswith(',');self.reg[base]=(self.reg[base]+self.val(tail[1:].strip()))&0xffffffff
   else:
    op=[x.strip() for x in o.split(',')]
    if m=='cmp':self.flags_sub(self.val(op[0]),self.val(op[1]))
    elif m in ['lsr','lsrs','lsl']:
     x=self.val(op[1]);shift=self.val(op[2]);x=(x>>shift) if m.startswith('lsr') else (x<<shift);self.reg[self.ri(op[0])]=x&0xffffffff
     if m=='lsrs':self.z=(x&0xffffffff)==0;self.n=bool(x&0x80000000)
    elif m=='mov':self.reg[self.ri(op[0])]=self.val(op[1])
    elif m in ['add','subs','ands','mul']:
     a,b=self.val(op[1]),self.val(op[2]);x=a+b if m=='add' else a*b if m=='mul' else self.flags_sub(a,b) if m=='subs' else a&b;self.reg[self.ri(op[0])]=x&0xffffffff
     if m=='ands':self.z=x==0;self.n=bool(x&0x80000000)
    else:raise AssertionError((hex(pc),m,o))
   pc=nextpc
  return self.reg[0]
cases=[]
for index,count,step,volume,boundary in itertools.product([9,10,58],[0,1,3,4,5,176,352],[0,128,256,511,1024],[1,64],[0,1]):
 off=struct.unpack_from('<I',rom,0x7ec658+index*4)[0]-0x8000000;length=struct.unpack_from('<I',rom,off+4)[0];cases.append((index,count,(length-1)*256+128 if boundary else 0,step,volume))
cp=subprocess.run([str(root/'bin/audio-repeat-rom-compare.exe'),str(root/'assets/audio')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0;samples=0
for c,want in zip(cases,rows):
 index,count,phase,step,volume=c;pointer=struct.unpack_from('<I',rom,0x7ec658+index*4)[0];length=struct.unpack_from('<I',rom,pointer-0x8000000+4)[0];p=ArmLoop(pointer+8,length,count,phase,step,volume);nextphase=p.run();values=[]
 for j in range(count):
  v=p.read(0x10000000+j*2,2);values.append(v if v<32768 else v-65536)
 actual=[count,nextphase]+values;assert actual==want,(c,actual,want);steps+=p.steps;samples+=count
report=dict(cases=len(cases),samples=samples,arm_instructions=steps,equal=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual original ARM046C..055C output samples and returned phase vs native protected raw channel mix using original engine assets9/10/58 including post-length bytes. Bounded diagnostic ARM executor, not emulated game or host audio; steps0..1024 and selected counts only.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/audio_channel.c','src/audio_mix.c','src/audio_asset.c','tools/compare_audio_repeat_arm.py','tools/extract_audio.py','tests/audio_repeat_rom_compare.c']})
(root/'reports/audio-repeat-arm-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
