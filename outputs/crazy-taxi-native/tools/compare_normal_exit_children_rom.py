from pathlib import Path
import hashlib,itertools,json,os,subprocess,sys
P=Path.cwd();B=P/'outputs/crazy-taxi-native';W=B/'work/native-results-parent-20261016';sys.path.insert(0,str(P/'work/python-packages'));os.environ['CAPSTONE_LIBRARY_PATH']=str(P/'work/python-packages/capstone/lib');os.environ['TMP']=os.environ['TEMP']=str(W)
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm_const import ARM_OP_REG,ARM_OP_IMM
ROM=(P.parent/'Crazy Taxi - Catch a Ride (USA).gba').read_bytes();SHA='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5';assert hashlib.sha256(ROM).hexdigest()==SHA
MD=Cs(CS_ARCH_ARM,CS_MODE_THUMB);MD.detail=True;DECODE={};MASK=0xffffffff
addresses=[0x02001ea8,0x02001697,0x02005f20,0x02001ea6,0x02003897,0x0200389a,0x02003878,0x02001691,0x02005f1a]
targets={0x08010aac,0x08027754,0x0802771c,0x08027730,0x080243a4,0x0802392c,0x080253e4,0x08024098,0x08024674}
class Probe:
 def __init__(self,case):
  self.mem=bytearray(0x10000);self.reg=[0]*16;self.z=False;self.c=False;self.steps=0;self.trace=[];self.visited={};self.children=0;self.scenario=case[3]
  for a,v in zip(addresses,[2,9,case[0],55,77,88,123,case[1],case[2]]):self.write(a,v,4 if a==0x02003878 else 1)
 def read(self,a,n=4):
  if 0x08000000<=a and a+n<=0x08800000:return int.from_bytes(ROM[a-0x08000000:a-0x08000000+n],'little')
  assert 0x02000000<=a and a+n<=0x02010000,(hex(a),n);return int.from_bytes(self.mem[a-0x02000000:a-0x02000000+n],'little')
 def write(self,a,v,n=4):
  assert 0x02000000<=a and a+n<=0x02010000,(hex(a),n);self.mem[a-0x02000000:a-0x02000000+n]=(v&((1<<(8*n))-1)).to_bytes(n,'little')
 def owners(self):return [self.read(a,4 if a==0x02003878 else 1) for a in addresses]
 def external(self,t):
  assert t in targets,hex(t);self.trace.append([t,*self.owners()])
  if t in (0x080243a4,0x0802392c,0x080253e4,0x08024098,0x08024674):
   self.children+=1;i=self.children;k=self.scenario
   if t==0x080243a4 and k in (1,2):self.write(0x02005f1a,k-1,1)
   if k==3 or k==4 and t==0x08024674:
    vals=[11+i,21+i,31+i,41+i,51+i,61+i,0x10203001+2*i,81+i]
    for a,v in zip(addresses[:8],vals):self.write(a,v,4 if a==0x02003878 else 1)
   if k==5 and t==0x0802392c:self.write(0x02005f1a,1,1);self.write(0x02001691,255,1)
  # Helpers are explicit opaque ABI contracts, not original child execution.
  for j in range(4):self.reg[j]=(0xdead0000+j)&MASK
 def run(self):
  pc=0x0800b580
  while pc!=0x0800b6d0:
   assert self.steps<256 and (0x0800b580<=pc<0x0800b60e or 0x0800b674<=pc<0x0800b694 or 0x0800b6c4<=pc<0x0800b6d0),hex(pc)
   if pc not in DECODE:DECODE[pc]=next(MD.disasm(ROM[pc-0x08000000:pc-0x08000000+4],pc,count=1))
   ins=DECODE[pc];self.visited[pc]=bytes(ins.bytes).hex();self.steps+=1;npc=pc+ins.size;ops=ins.operands;m=ins.mnemonic
   def reg(o):
    name=ins.reg_name(o.reg);assert name.startswith('r') and name[1:].isdigit(),name;return int(name[1:])
   def val(o):
    if o.type==ARM_OP_REG:return self.reg[reg(o)]
    assert o.type==ARM_OP_IMM;return o.imm&MASK
   if m in ('ldr','ldrb','ldrsb','str','strb'):
    o=ops[1];base=ins.reg_name(o.mem.base);a=((pc+4)&~3) if base=='pc' else self.reg[int(base[1:])]
    if o.mem.index:a+=self.reg[int(ins.reg_name(o.mem.index)[1:])]
    a+=o.mem.disp;n=1 if m in ('ldrb','ldrsb','strb') else 4
    if m.startswith('str'):self.write(a,self.reg[reg(ops[0])],n)
    else:
     v=self.read(a,n);self.reg[reg(ops[0])]=((v-256)&MASK) if m=='ldrsb' and v&128 else v
   elif m in ('movs','mov'):self.reg[reg(ops[0])]=val(ops[1]);self.z=self.reg[reg(ops[0])]==0
   elif m in ('adds','subs'):
    a=val(ops[1]) if len(ops)==3 else self.reg[reg(ops[0])];b=val(ops[-1]);v=a+b if m=='adds' else a-b;self.c=v>MASK if m=='adds' else a>=b;self.reg[reg(ops[0])]=v&MASK;self.z=(v&MASK)==0
   elif m in ('lsls','lsrs','asrs'):
    a=val(ops[1]);s=val(ops[2]);v=a<<s if m=='lsls' else a>>s if m=='lsrs' else (a-0x100000000 if a&0x80000000 else a)>>s;self.reg[reg(ops[0])]=v&MASK;self.z=(v&MASK)==0
   elif m=='cmp':a,b=val(ops[0]),val(ops[1]);self.z=a==b;self.c=a>=b
   elif m=='bl':self.external(ops[0].imm)
   elif m in ('b','beq','bne','bls'):
    if m=='b' or m=='beq' and self.z or m=='bne' and not self.z or m=='bls' and (not self.c or self.z):npc=ops[0].imm
   else:raise AssertionError((hex(pc),m,ins.op_str))
   pc=npc
  return [len(self.trace),*(v for row in self.trace for v in row),*self.owners()]
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest();files=[B/'src/normal_exit.c',B/'src/normal_exit.h',B/'tests/normal_exit_children_compare.c',Path(__file__)];before={str(f):sha(f) for f in files};cc=P/'work/compiler/llvm-mingw-20261006/llvm-mingw-20261006-ucrt-x86_64/bin/x86_64-w64-mingw32-clang.exe';exe=W/'normal-exit-children-compare.exe';flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-march=x86-64','-mtune=generic','-fintegrated-cc1','-I',str(B/'src')]
build=subprocess.run([str(cc),*flags,str(files[0]),str(files[2]),'-Wl,--stack,16777216','-o',str(exe)],cwd=P,capture_output=True,text=True);(W/'children-build.log').write_text(build.stdout+build.stderr);assert build.returncode==0,(build.returncode,build.stderr)
cases=list(itertools.product((0,1,128),(0,1,128,255),(0,1,255),range(6)));text=''.join(' '.join(map(str,c))+'\n' for c in cases);run=subprocess.run([str(exe)],input=text,capture_output=True,text=True,timeout=120,cwd=P);(W/'children-native.out.log').write_text(run.stdout);(W/'children-native.err.log').write_text(run.stderr);assert run.returncode==0 and not run.stderr,(run.returncode,run.stderr);rows=[list(map(int,line.split())) for line in run.stdout.splitlines()];assert len(rows)==len(cases);steps=0;visited={};childcounts={}
for case,want in zip(cases,rows):
 p=Probe(case);actual=p.run();assert actual==want,dict(case=case,actual=actual,native=want);steps+=p.steps;visited.update(p.visited)
 for t,*_ in p.trace:childcounts[hex(t)]=childcounts.get(hex(t),0)+1
assert before=={str(f):sha(f) for f in files}
report=dict(status='PASS_ORIGINAL_NORMAL_EXIT_ASYNCHRONOUS_CHILD_BOUNDARIES',cases=len(cases),original_thumb_instructions=steps,original_visited_bytes={hex(a):v for a,v in sorted(visited.items())},ordered_boundary_counts=childcounts,rom_sha256=SHA,native_executable_sha256=sha(exe),source_sha256=before,wait_polls_per_pending_child=2100,limits=['Diagnostic executes only original ordinary caller instructions; audio/credits/results/name/game-over/profile callees are explicit opaque contracts with controlled shared-global mutation','Native asynchronous caller unit, not actual GUI expiry, child rendering, profile saving, earned progression, or whole game proof','Generic native370-module viewer rebuild and existing Quit/Retry regressions are still required before integration'])
(W/'children-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report['status'],report['cases'],'cases',steps,'actual original caller instructions; all child call/final owner states match',flush=True)

