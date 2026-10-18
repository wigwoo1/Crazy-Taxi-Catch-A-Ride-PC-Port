"""Original single-voice mixer caller and actual ARM PCM/loop callback bodies."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_continuous_rom.py').read().split('\ncases=')[0])
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 address=int(item['address'],16)
 if 0x8026b14<=address<0x8026c96:allowed_thumb[address]=bytes.fromhex(item['bytes'])
arm_namespace={}
arm_source=open('outputs/crazy-taxi-native/tools/compare_audio_repeat_arm.py').read().split('\ncases=')[0].replace('pc=0x800046c','pc=self.entry')
exec(arm_source,arm_namespace)
ArmLoop=arm_namespace['ArmLoop'];pcm_code={i.address:i for i in Cs(CS_ARCH_ARM,CS_MODE_ARM).disasm(r[0x25c:0x320],0x800025c)};loop_code=arm_namespace['code']
class MixProbe(ContinuousProbe):
 def external(self,target):
  if target==0x803245c:
   callback=self.reg[4];assert callback in [0x800025c,0x800046c],hex(callback)
   pointer,phase,step=self.reg[1:4];stack=self.reg[13]
   if callback==0x800046c:length,loop,count,volume=[self.read(stack+k*4) for k in range(4)]
   else:count,volume=[self.read(stack+k*4) for k in range(2)];length=loop=0
   parent=self
   class BoundArm(ArmLoop):
    def read(self,address,size):
     if 0x3005ac0<=address<0x3005ac0+704:return parent.read(address,size)
     return super().read(address,size)
    def write(self,address,value,size):
     if 0x3005ac0<=address<0x3005ac0+704:parent.write(address,value,size)
     else:super().write(address,value,size)
   arm=BoundArm(pointer,length,count,phase,step,volume);arm.entry=callback;arm.reg[0]=0x3005ac0
   values=[length,loop,count,volume] if callback==0x800046c else [count,volume]
   for k,value in enumerate(values):arm.write(arm.reg[13]+k*4,value,4)
   arm_namespace['code']=loop_code if callback==0x800046c else pcm_code
   self.reg[0]=arm.run();self.arm_steps+=arm.steps;self.callbacks+=1
  else:super().external(target)
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 address=int(item['address'],16)
 if 0x8026a5e<=address<0x8026caa:allowed_thumb[address]=bytes.fromhex(item['bytes'])
cases=[(sample,count,mask) for sample in [0,7,16,30] for count in [5,352] for mask in range(64)]
result=subprocess.run([str(root/'bin/music-buffer-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True);assert result.returncode==0,result.stderr;rows=result.stdout.splitlines();assert len(rows)==len(cases);steps=arm_steps=callbacks=0
for case,line in zip(cases,rows):
 sample,count,mask=case;p=MixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007618:bytearray([165]*128),0x3005ac0:bytearray(704)};p.scalars={0x2007570:0,0x2007818:0x3005ac0,0x200781c:count,0x20075f4:64,0x2001697:0,0x2001ea8:0,0x20025da:256,0x3004c48:0x800025c,0x3004c4c:0x800046c,division_owner:0x80029fc,0x2007600:mask&1,0x20075f6:(mask>>1)&1,0x2007602:128 if mask&4 else 0,0x20075f7:(1 if mask&32 else 128) if mask&8 else 0,0x2007560:255 if mask&16 else 0,0x200760a:4};p.arm_steps=p.callbacks=0
 for k in range(4):
  base=0x2007618+k*32;p.write(base+28,0x8421fc0+samples[(sample+k)%len(samples)]);p.write(base+12,0);p.write(base+16,128+k*64);p.write(base+20,64,2);p.write(base+22,64,2)
 for j in range(count):p.write(0x3005ac0+j*2,j*257-30000,2)
 p.reg[6]=0x2007600;p.reg[13]=0x10000400;p.run(entry=0x8026a5e,stop=0x8026caa)
 parts=line.split()
 for k in range(4):
  base=0x2007618+k*32;assert bytes(p.regions[0x2007618][k*32:k*32+28])==bytes.fromhex(parts[k*2]),(case,k,'voice');pointer=p.read(base+28);assert (pointer-0x8421fc0 if pointer else 0xffffffff)==int(parts[k*2+1]),(case,k,'sample')
 actual=[p.read(0x3005ac0+j*2,2) for j in range(count)];actual=[v if v<32768 else v-65536 for v in actual];assert actual==list(map(int,parts[8:])),(case,'samples');steps+=p.steps;arm_steps+=p.arm_steps;callbacks+=p.callbacks
report={'status':'PASS','cases':len(cases),'actual_Thumb_instructions':steps,'nested_ARM_instructions':arm_steps,'callbacks':callbacks,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual26A5E..26CAA clear/gate/full4voice loop vs native music_buffer;64 combinations musicenabled/playing/effectenabled/effectactive/initialized7560 incl differing bitmasks, counts5/352,4 mixed original sample startingindices. ALLvoice nonpointerbytes/sampleidentities/outputsamples. ActualnestedPCM/loop/divider callbacks. Source outputtable pointer and single count supplied, RAMcallback addresses normalized; SFX/outputconversion/IRQ/live viewer binding excluded.'}
(root/'reports/music-buffer-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
