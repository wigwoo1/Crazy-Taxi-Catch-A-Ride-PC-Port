"""Original angle helper and render-prefix integer pitch path; FP helper contracts explicit."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_ARM
from capstone.arm import ARM_OP_IMM
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();md=Cs(CS_ARCH_ARM,CS_MODE_ARM);md.detail=True
code={i.address:i for i in md.disasm(rom[0x52b4:0x5330],0x080052b4)}
def source_angle(pitch):
 regs={'r0':0,'r1':pitch&0xffffffff,'r2':0,'r3':0,'lr':0x15000000,'pc':0};pc=0x080052b4;eq=lt=gt=False;steps=0
 while pc!=0x15000000:
  ins=code[pc];regs['pc']=pc+8;ops=ins.operands;mn=ins.mnemonic;nxt=pc+4;steps+=1;assert steps<20
  def signed(v):return v-0x100000000 if v&0x80000000 else v
  def val(op):return op.imm&0xffffffff if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  if mn=='ldr':a=regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp;regs[md.reg_name(ops[0].reg)]=struct.unpack_from('<I',rom,a-0x08000000)[0]
  elif mn=='and':regs[md.reg_name(ops[0].reg)]=val(ops[1])&val(ops[2])
  elif mn=='cmp':a=signed(val(ops[0]));b=signed(val(ops[1]));eq=a==b;lt=a<b;gt=a>b
  elif mn in ['blt','bgt','bge']:
   if {'blt':lt,'bgt':gt,'bge':not lt}[mn]:nxt=ops[0].imm
  elif mn=='bx':nxt=val(ops[0])
  elif mn=='mov':regs[md.reg_name(ops[0].reg)]=val(ops[1])
  elif mn in ['sub','add','rsb']:
   a=val(ops[1]);b=val(ops[2]);regs[md.reg_name(ops[0].reg)]=(a-b if mn=='sub' else a+b if mn=='add' else b-a)&0xffffffff
  else:raise AssertionError((hex(pc),mn))
  pc=nxt
 return signed(regs['r0']),steps
# Exact encoded ROM double limits, IEEE conversions/comparisons are explicit contracts.
hi=struct.unpack('<d',rom[0x2b764:0x2b768]+rom[0x2b760:0x2b764])[0];lo=struct.unpack('<d',rom[0x2b76c:0x2b770]+rom[0x2b768:0x2b76c])[0];assert 142<hi<143 and -143<lo<-142
cases=list(range(4096))+[-2147483648,2147483647,-4097,-4096,-2049,-2048,-1,4096,8191]
exe=root/'bin/background-pitch-probe.exe';cp=subprocess.run([str(exe)],input=''.join(str(p)+'\n' for p in cases),capture_output=True,text=True,check=True);rows=list(map(int,cp.stdout.split()));assert len(rows)==len(cases);steps=0
for pitch,native in zip(cases,rows):
 angle,n=source_angle(pitch);steps+=n
 # Actual2B6BE..2B6C2 LSRS(sign)/ADD/ASRS divides signed value toward zero.
 word=angle&0xffffffff;word=(word+(word>>31))&0xffffffff;signed=word-0x100000000 if word&0x80000000 else word;half=signed>>1
 expected=142 if float(half)>hi else -142 if float(half)<lo else half
 assert native==expected,(pitch,angle,half,native,expected)
report={'status':'PASS','cases':len(cases),'all_4096_pitch_words':True,'original_angle_helper_instructions':steps,'double_limits':[lo,hi],'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'scope':'Actual ARM052B4 angle-difference helper with firstangle0 vs everypitch0..4095 andwrapped signed edges, then exact renderprefix integer divide/clamp limits vs productionnativebackgroundpitch. IEEEint-to-double/comparison semantics explicit contracts; original libgcc33340/33210/332A8 bodies and completeThumb2B6A0 prefix not executed here. No framebuffer/wholeframe/cycles proof.'}
(root/'reports/background-pitch-owner-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
