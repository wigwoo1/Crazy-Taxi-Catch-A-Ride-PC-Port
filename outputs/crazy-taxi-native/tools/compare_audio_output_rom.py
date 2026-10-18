"""Complete output stage and actual ARM001D8 signed conversion callback."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 a=int(item['address'],16)
 if 0x8026e0e<=a<0x8026ede:allowed_thumb[a]=bytes.fromhex(item['bytes'])
arm_md=Cs(CS_ARCH_ARM,CS_MODE_ARM);code={i.address:i for i in arm_md.disasm(r[0x1d8:0x25c],0x80001d8)}
def conversion(p):
 reg=p.reg[:];reg[14]=0x10020000;pc=0x80001d8;steps=0;z=n=v=False
 while pc!=0x10020000:
  assert pc in code,hex(pc);i=code[pc];assert r[pc-0x8000000:pc-0x8000000+4]==bytes(i.bytes);steps+=1;assert steps<10000;nextpc=pc+4;m=i.mnemonic;o=i.op_str
  if m=='stmdb':reg[13]-=4;p.write(reg[13],reg[3])
  elif m=='ldm':reg[3]=p.read(reg[13]);reg[13]+=4
  elif m=='bx':nextpc=reg[14]
  elif m=='ldrsh':reg[3]=signed(p.read(reg[0],2)<<16)>>16;reg[0]+=2
  elif m=='asr':reg[3]=signed(reg[3])>>6
  elif m in ['cmp','cmn']:
   a=reg[3]&MASK;b=127;value=(a-b if m=='cmp' else a+b)&MASK;z=value==0;n=bool(value&0x80000000);v=bool(((a^b)&(a^value))&0x80000000) if m=='cmp' else bool((~(a^b)&(a^value))&0x80000000)
  elif m=='movgt':
   if not z and n==v:reg[3]=127
  elif m=='mvnlt':
   if n!=v:reg[3]=(~126)&MASK
  elif m=='strb':p.write(reg[1],reg[3],1);reg[1]+=1
  elif m=='subs':
   a=reg[2];reg[2]=(a-4)&MASK;z=reg[2]==0;n=bool(reg[2]&0x80000000);v=bool(((a^4)&(a^reg[2]))&0x80000000)
  elif m=='bgt':
   if not z and n==v:nextpc=int(o[1:],0)
  else:raise AssertionError((hex(pc),m,o))
  pc=nextpc
 p.reg[:3]=reg[:3];return steps
class OutputProbe(InitProbe):
 def external(self,target):
  assert target==0x8032458 and self.reg[3]==0x80001d8,hex(target);self.arm_steps+=conversion(self)
cases=[(mask,bank,count,seed) for mask in range(32) for bank in [0,1] for count in [176,264,352] for seed in [0,32768]]
rows=subprocess.run([str(root/'bin/audio-output-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=arm_steps=0
for case,line in zip(cases,rows):
 mask,bank,count,seed=case;p=OutputProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x3005ac0:bytearray(704),0x10011000:bytearray([165]*352),0x10012000:bytearray([165]*352)};p.scalars={0x2007560:255 if mask&1 else 0,0x2007600:128 if mask&2 else 0,0x20075f6:1 if mask&4 else 0,0x2007602:128 if mask&8 else 0,0x20075f7:1 if mask&16 else 0,0x2007570:bank,0x2007818:0x10011000,0x200781c:count,0x2007820:0x10012000,0x2007824:count,0x3004c50:0x80001d8};p.arm_steps=0
 for j in range(352):p.write(0x3005ac0+j*2,seed+j*257,2)
 p.reg[13]=0x10000400;p.write(p.reg[13]+16,0x10011000 if not bank else 0x10012000);p.run(entry=0x8026e0e,stop=0x8026ede)
 parts=line.split();assert p.read(0x2007570)==int(parts[0]),(case,'bank');assert bytes(p.regions[0x10011000])==bytes.fromhex(parts[1]),(case,'bank0');assert bytes(p.regions[0x10012000])==bytes.fromhex(parts[2]),(case,'bank1');steps+=p.steps;arm_steps+=p.arm_steps
report={'status':'PASS','cases':len(cases),'actual_Thumb_instructions':steps,'actual_ARM_instructions':arm_steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE26E0E..26EDE final output stage and actualARM001D8 converter. ALL32gatecombinations incl differing effects bitmasks,2banks,3actualcounts176/264/352,2wrapped signedinputseeds. ALL704outputbytes retained/written andbankflip exact. Valid outputtable native-owned buffers supplied, codecallback normalizedROMidentity. Preceding music/SFX/actualIRQ entryreturn/bootstrap/hostaudio timing excluded.'}
(root/'reports/audio-output-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
