"""Diagnostic-only execution of actual ARM callback, never linked into game."""
from pathlib import Path
import sys,hashlib,json,random,subprocess
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_ARM
root=Path('outputs/crazy-taxi-native')
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_ARM)
code={i.address:(i.mnemonic,i.op_str) for i in md.disasm(rom[0x29fc:0x2ce8],0x080029fc)}
MASK=0xffffffff
steps=0
seen=set()
def original(a,b):
 global steps
 regs={f'r{i}':0 for i in range(16)}
 regs.update(r0=a,r1=b,lr=0x12345678)
 N=Z=C=False
 def value(s):
  parts=s.split(', ')
  v=int(parts[0][1:],0) if parts[0].startswith('#') else regs[parts[0]]
  if len(parts)>1:
   kind,amount=parts[1].split(' ');n=int(amount[1:],0)
   if kind=='lsr': v=v>>n
   elif kind=='lsl':v=(v<<n)&MASK
   elif kind=='asr':v=((v if v<0x80000000 else v-0x100000000)>>n)&MASK
   else:raise AssertionError(kind)
  return v
 pc=0x080029fc
 for _ in range(256):
  mnemonic,args=code[pc];seen.add(mnemonic);steps+=1
  cond=mnemonic[-2:]
  enabled=True
  if cond in ('mi','eq','ne','hi','hs'):
   enabled={'mi':N,'eq':Z,'ne':not Z,'hi':C and not Z,'hs':C}[cond]
   mnemonic=mnemonic[:-2]
  nxt=pc+4
  if enabled:
   parts=args.split(', ',2)
   if mnemonic=='bx':assert args=='lr';return regs['r0']
   elif mnemonic=='b':nxt=int(args[1:],0)
   elif mnemonic=='cmp':
    x=value(parts[0]);y=value(', '.join(parts[1:]));d=(x-y)&MASK
    N=bool(d&0x80000000);Z=d==0;C=x>=y
   elif mnemonic=='tst':Z=(value(parts[0])&value(', '.join(parts[1:])))==0
   elif mnemonic in ('ands','eor','rsb','mov','adc','sub'):
    if mnemonic=='mov':v=value(', '.join(parts[1:]))
    else:
     x=value(parts[1]);y=value(parts[2])
     if mnemonic=='ands':v=x&y;N=bool(v&0x80000000);Z=v==0
     elif mnemonic=='eor':v=x^y
     elif mnemonic=='rsb':v=y-x
     elif mnemonic=='adc':v=x+y+int(C)
     elif mnemonic=='sub':v=x-y
    regs[parts[0]]=v&MASK
   else:raise AssertionError((hex(pc),mnemonic,args))
  pc=nxt
 raise AssertionError('instruction limit')
values=[0,1,2,3,191,192,193,255,256,38400,0x3fffffff,0x40000000,0x7fffffff,0x80000000,0x80000001,0xfffffffe,0xffffffff]
cases=[(a,b) for a in values for b in values]
rng=random.Random(0x29fc)
cases += [(rng.getrandbits(32),rng.getrandbits(32)) for _ in range(4096)]
cases += [(rng.getrandbits(32),192) for _ in range(1024)]
expected=[original(a,b) for a,b in cases]
run=subprocess.run([str(root/'bin/engine-projection-call-rom-compare.exe')],input=''.join(f'{a:08x} {b:08x}\n' for a,b in cases),capture_output=True,text=True,check=True)
actual=[int(x,16) for x in run.stdout.splitlines()]
assert len(actual)==len(expected)
for case,e,n in zip(cases,expected,actual):assert e==n,(case,hex(e),hex(n))
# Original dispatcher BX r2 and bootstrap's loaded callback literal.
assert rom[0x32454:0x32456]==bytes.fromhex('1047')
report={'status':'PASS','cases':len(cases),'actual_arm_instructions':steps,'routine':'080029FC..08002CE4','dispatcher':'08032454 BX r2','opcodes':sorted(seen),'scope':'Original global03006940 native identity routing to complete ARM callback-equivalent r0 output; actual supplied ROM diagnostic execution, no opaque helpers. Register side effects, relocation/bootstrap and frame caller integration excluded. Native runtime contains no ARM interpreter.','rom_sha256':hashlib.sha256(rom).hexdigest(),'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/projection_divide.c','src/engine_projection_call.c','src/engine_projection_call.h','src/engine_callback_identity.c','tests/engine_projection_call_rom_compare.c','bin/engine-projection-call-rom-compare.exe','tools/compare_engine_projection_call_rom.py']}}
(root/'reports/engine-projection-call-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(f'PASS {len(cases)} cases, {steps} actual ARM instructions')
