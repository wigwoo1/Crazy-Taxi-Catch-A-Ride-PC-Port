"""Original Thumb2C858 material phase owner vs native routine, offline only."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);code={i.address:i for i in md.disasm(rom[0x2c858:0x2c876],0x0802c858)}
assert list(code)==list(range(0x0802c858,0x0802c876,2))
assert struct.unpack_from('<II',rom,0x2c878)==(0x02005f20,0x03006810)
def original(phase,delta,freeze):
 regs={'r0':delta&0xffffffff,'r1':0,'r2':0,'sp':0x10001000,'lr':0x15000001};mem={0x02005f20:freeze,0x03006904:phase};pc=0x0802c858;zero=False;steps=0
 while pc!=0x15000000:
  ins=code[pc];steps+=1;assert steps<=16;op=ins.op_str.split(', ');nxt=pc+2
  def val(s):return int(s[1:],0) if s.startswith('#') else regs[s]
  if ins.mnemonic=='push':regs['sp']-=4;mem[regs['sp']]=regs['lr']
  elif ins.mnemonic=='pop':regs['r0']=mem[regs['sp']];regs['sp']+=4
  elif ins.mnemonic in ['ldr','ldrb']:
   dest=op[0];base=ins.op_str.split('[')[1].split(']')[0].split(', ')
   address=((pc+4)&~3) if base[0]=='pc' else regs[base[0]]
   if len(base)>1:address+=val(base[1])
   regs[dest]=struct.unpack_from('<I',rom,address-0x08000000)[0] if address>=0x08000000 and address<0x08800000 else mem[address]
   if ins.mnemonic=='ldrb':regs[dest]&=255
  elif ins.mnemonic=='str':mem[regs[op[1].strip('[]')]]=regs[op[0]]
  elif ins.mnemonic=='adds':regs[op[0]]=(regs[op[0]]+val(op[1]) if len(op)==2 else val(op[1])+val(op[2]))&0xffffffff
  elif ins.mnemonic in ['lsls','asrs']:
   v=val(op[1]);n=val(op[2]);signed=v-0x100000000 if v&0x80000000 else v
   regs[op[0]]=(v<<n if ins.mnemonic=='lsls' else signed>>n)&0xffffffff
  elif ins.mnemonic=='cmp':zero=val(op[0])==val(op[1])
  elif ins.mnemonic=='bne':
   if not zero:nxt=val(op[0])
  elif ins.mnemonic=='bx':nxt=val(op[0])&~1
  else:raise AssertionError((hex(pc),ins.mnemonic,ins.op_str))
  pc=nxt
 assert regs['sp']==0x10001000;return mem[0x03006904],steps
values=[(0,0),(0,1),(0,60),(0xffffffff,1),(0x7fffffff,1),(0x80000000,-1),(123456,-2147483648),(0xfffffff0,2147483647)]
cases=[(p,d,f) for f in range(256) for p,d in values]
exe=root/'bin/material-phase-probe.exe';cp=subprocess.run([str(exe)],input=''.join(f'{p} {d} {f}\n' for p,d,f in cases),text=True,capture_output=True,check=True);native=list(map(int,cp.stdout.split()));assert len(native)==len(cases);steps=0
for case,got in zip(cases,native):
 expected,n=original(*case);steps+=n;assert got==expected,(case,got,expected)
callers=[]
for site,load,offset in [(0x78f2,0x78ee,0x19c),(0xb470,0xb46c,0x1a0)]:
 i=next(md.disasm(rom[site:site+4],0x8000000+site));assert i.mnemonic=='bl' and i.op_str=='#0x802c858'
 literal=((load+4)&~3)+offset;owner=struct.unpack_from('<I',rom,literal)[0];assert owner==0x02003b00
 callers.append({'site':hex(0x8000000+site),'delta_owner':hex(owner),'literal_address':hex(0x8000000+literal)})
report={'status':'PASS','cases':len(cases),'freeze_byte_values':256,'original_thumb_instructions':steps,'routine_bytes':rom[0x2c858:0x2c876].hex(),'phase_owner':'0x03006904','pause_owner':'0x02005f20','callers':callers,'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'scope':'Original actual Thumb2C858 instruction execution vs native phase advancement, all256freeze bytes, signed extremes/wrap. Both original call sites pass global3B00 before2C840 scene dispatch; bootstrap resets6904. Standalone native routine; not yet linked/integrated into live game, phase cadence/animated scene pixels/fullframe remain unverified.'}
(root/'reports/material-phase-owner-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
