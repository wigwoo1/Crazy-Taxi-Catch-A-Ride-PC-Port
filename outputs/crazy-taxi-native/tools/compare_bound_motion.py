"""Bounded Thumb motion diagnostic, not a CPU/game runtime.
External steering result and no-drift orientation are explicit fixture contracts.
Matrix operations execute original bounded ARM code. Division is a scalar contract.
"""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_pivot.py').read().split('# Source motion')[0])
from capstone.arm_const import ARM_OP_REG,ARM_OP_IMM,ARM_OP_MEM
allowed.append((0xa04,0xa1c))
md.detail=True
cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-motion-cfg.json').read_text())
allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
def trunc(a,b):
 assert b,'zero division contract';return (abs(a)//abs(b))*(-1 if (a<0)!=(b<0) else 1)
def regno(name):
 return {'sp':13,'lr':14,'pc':15,'sb':9,'sl':10,'fp':11,'ip':12}.get(name,int(name[1:]) if name.startswith('r') and name[1:].isdigit() else -1)
class MotionProbe:
 def __init__(self,case,pre):
  self.arm=Probe();self.reg=[0]*16;self.reg[0]=0x10000000;self.reg[13]=0x10010000;self.flags=(False,False,False,False);self.steps=0;self.pre=pre
  for base,end in [(0x10000000,0x10001000),(0x1000fe00,0x10010000)]:
   for a in range(base,end,4):self.arm.mem[a]=0
  speed,steer,pitch,heading,roll,delta=case
  self.delta=delta;self.base=0x10000000
  for off,v in {0x18:pitch&65535,0x1a:roll&65535,0x1c:heading*4096,0x20:heading*4096,0x540:12345,0x544:speed,0x594:2000,0x7c8:4096,0xe84:5324}.items():self.write(self.base+off,v,2 if off in [0x18,0x1a] else 4)
  self.arm.mem[0x30053c0]=delta
  for slot,target in [(0x3006a90,0x8000718),(0x3006a94,0x80009d4),(0x3006ab4,0x80009ec),(0x3006ab8,0x80009bc),(0x3006ac4,0x8000a40),(0x3006ab0,0x8000954),(0x3006a40,0x8000a04)]:self.arm.mem[slot]=target
 def read(self,a,n=4):
  if 0x8000000<=a<0x8000000+len(r):return int.from_bytes(r[a-0x8000000:a-0x8000000+n],'little')
  assert n in [1,2,4] and a%4+n<=4,(hex(a),n)
  return (self.arm.read(a&~3)>>(8*(a&3)))&((1<<(8*n))-1)
 def write(self,a,v,n=4):
  assert 0x10000000<=a<0x10010000 and a%4+n<=4,('Thumb write',hex(a),n)
  old=self.arm.read(a&~3);shift=8*(a&3);mask=((1<<(8*n))-1)<<shift
  self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
 def nz(self,v):
  _,_,c,o=self.flags;self.flags=(bool(v&0x80000000),(v&MASK)==0,c,o)
 def arithmetic(self,a,b,subtract):
  if subtract:
   value=(a-b)&MASK;c=a>=b;v=bool(((a^b)&(a^value))&0x80000000)
  else:
   total=a+b;value=total&MASK;c=total>MASK;v=bool((~(a^b)&(a^value))&0x80000000)
  self.flags=(bool(value&0x80000000),value==0,c,v);return value
 def operand(self,ins,o):
  if o.type==ARM_OP_REG:return self.reg[regno(ins.reg_name(o.reg))]
  if o.type==ARM_OP_IMM:return o.imm&MASK
  raise AssertionError(('operand',ins.mnemonic,ins.op_str))
 def external(self,target):
  if target==0x80129e0:
   self.reg[0]=self.pre&MASK;self.write(self.base+0x28,64,1)
  elif target==0x8012c6c:
   assert self.read(self.base+0x358,1)&4==0
   self.write(self.base+0x20,self.read(self.base+0x1c));self.write(self.base+0x69c,0)
  elif target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  elif target in [0x803245c,0x8032450,0x803244c,0x8032458]:
   index={0x803245c:4,0x8032450:1,0x803244c:0,0x8032458:3}[target];entry=self.reg[index]
   assert entry in [0x8000a40,0x8000718,0x80009bc,0x80009d4,0x80009ec,0x8000954,0x8000a04],hex(entry)
   self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  else:raise AssertionError(('external',hex(target)))
 def run(self):
  pc=0x80126dc
  while pc!=0x80129a8:
   assert pc in allowed_thumb,('outside',hex(pc));self.steps+=1;assert self.steps<1000
   raw=allowed_thumb[pc];assert r[pc-0x8000000:pc-0x8000000+len(raw)]==raw
   ins=next(md.disasm(raw,pc));ops=ins.operands;m=ins.mnemonic;nextpc=pc+ins.size;self.reg[15]=pc+4
   rn=lambda o:regno(ins.reg_name(o.reg))
   val=lambda o:self.operand(ins,o)
   if m=='push':
    regs=sorted(rn(o) for o in ops);self.reg[13]-=4*len(regs)
    for j,k in enumerate(regs):self.write(self.reg[13]+4*j,self.reg[k])
   elif m in ['mov','movs']:
    value=val(ops[1]);self.reg[rn(ops[0])]=value
    if m=='movs':self.nz(value)
   elif m in ['add','adds','sub','subs','rsbs']:
    dest=rn(ops[0]);a=val(ops[1]) if len(ops)==3 else self.reg[dest];b=val(ops[2]) if len(ops)==3 else val(ops[1])
    if m=='rsbs':a,b=b,a
    if m.endswith('s'):value=self.arithmetic(a,b,m in ['sub','subs','rsbs'])
    else:value=(a-b if m=='sub' else a+b)&MASK
    self.reg[dest]=value
   elif m in ['lsls','lsrs','asrs']:
    a=val(ops[1]);n=val(ops[2]);value=(a<<n)&MASK if m=='lsls' else (a>>n if m=='lsrs' else signed(a)>>n)&MASK
    self.reg[rn(ops[0])]=value;self.nz(value)
   elif m in ['muls','ands']:
    a=val(ops[1]);b=val(ops[2]) if len(ops)==3 else self.reg[rn(ops[0])];value=(a*b if m=='muls' else a&b)&MASK;self.reg[rn(ops[0])]=value;self.nz(value)
   elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)
   elif m in ['ldr','ldrh','ldrsh','str','strh']:
    mem=ops[1].mem;a=self.reg[regno(ins.reg_name(mem.base))]
    if ins.reg_name(mem.base)=='pc':a&=~3
    if mem.index:a+=self.reg[regno(ins.reg_name(mem.index))]
    a=(a+mem.disp)&MASK;n=2 if m in ['ldrh','ldrsh','strh'] else 4
    if m.startswith('str'):self.write(a,val(ops[0]),n)
    else:
     value=self.read(a,n)
     if m=='ldrsh' and value&32768:value-=65536
     self.reg[rn(ops[0])]=value&MASK
   elif m=='bl':self.reg[14]=nextpc;self.external(val(ops[0]))
   elif m.startswith('b'):
    n,z,c,v=self.flags;take={'b':True,'bne':not z,'bhi':c and not z,'ble':z or n!=v,'bge':n==v}[m]
    if take:nextpc=val(ops[0])
   else:raise AssertionError(('unsupported',hex(pc),m,ins.op_str))
   pc=nextpc
  return [signed(self.read(self.base+off)) for off in [0x1c,0x20,0xc,0x10,0x14,0x540]]
cases=[(speed,steer,pitch,heading,-3,delta) for speed in [71424,122309,129807] for steer in [0,64,-64,256,-256] for pitch in [-178,-91,-5,60] for heading in [-105,-30,54] for delta in [1,2]]
text=''.join(' '.join(map(str,c))+'\n' for c in cases)
result=subprocess.run(['outputs/crazy-taxi-native/bin/motion-rom-compare.exe'],input=text,text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in result.stdout.splitlines()];steps=0;arm_steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=MotionProbe(case,row[1]);got=probe.run();steps+=probe.steps;arm_steps+=probe.arm.steps
 response=probe.read(probe.base+0x57c,2);response=response-65536 if response&32768 else response;assert response==row[2]
 assert got==row[3:],dict(case=case,original=got,native=row[3:])
report=dict(cases=len(cases),thumb_instructions=steps,arm_instructions=arm_steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original reachable motion-generation instructions through129A6 with explicit native steering-result/no-drift orientation/division contracts; original ARM matrix helpers executed. Not whole original CPU/frame or GBA trajectory.')
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-motion-comparison.json').write_text(json.dumps(report,indent=2))
print('PASS',len(cases),'bounded motion comparisons;',steps,'Thumb instructions.')
