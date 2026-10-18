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
cases=[(sample,count,phase,step,volume,fade) for sample,offset in enumerate(samples) for count in [5,352] for phase in ([0,(int.from_bytes(module[offset:offset+4],'little')-1)*256+128] if int.from_bytes(module[offset:offset+4],'little') else [0]) for step in [128,511] for volume,fade in [(64,256),(255,64),(0,256),(64,-1)]]
result=subprocess.run([str(root/'bin/music-voice-mix-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True);assert result.returncode==0,result.stderr;rows=result.stdout.splitlines();assert len(rows)==len(cases);steps=arm_steps=callbacks=0
for case,line in zip(cases,rows):
 sample,count,phase,step,volume,fade=case;offset=samples[sample];p=MixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007618:bytearray([165]*32),0x3005ac0:bytearray(704)};p.scalars={0x2007570:0,0x200781c:count,0x20075f4:volume,0x2001697:int(fade!=256),0x2001ea8:0,0x20025da:fade&65535,0x3004c48:0x800025c,0x3004c4c:0x800046c,division_owner:0x80029fc};p.arm_steps=p.callbacks=0
 p.write(0x2007634,0x8421fc0+offset);p.write(0x2007624,phase);p.write(0x2007628,step);p.write(0x200762c,64,2);p.write(0x200762e,64,2)
 for j in range(count):p.write(0x3005ac0+j*2,j*257-30000,2)
 p.reg[10]=0x2007618;p.reg[13]=0x10000400;p.run(entry=0x8026b14,stop=0x8026c96)
 parts=line.split();assert bytes(p.regions[0x2007618][:28])==bytes.fromhex(parts[0]),(case,'voice');pointer=p.read(0x2007634);assert (pointer-0x8421fc0 if pointer else 0xffffffff)==int(parts[1]),(case,'sample')
 actual=[p.read(0x3005ac0+j*2,2) for j in range(count)];actual=[v if v<32768 else v-65536 for v in actual];assert actual==list(map(int,parts[2:])),(case,'samples',actual[:5],parts[2:7]);steps+=p.steps;arm_steps+=p.arm_steps;callbacks+=p.callbacks
report={'status':'PASS','cases':len(cases),'actual_Thumb_instructions':steps,'actual_nested_ARM_instructions':arm_steps,'callbacks':callbacks,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Original26B14..26C96 singlevoice caller, actualARM025C PCM/046C loop callbacks andARM029FC short-tail divide. All31 original samples, counts5/352, first/finalphase,steps128/511, ordinary/faded/zero/signednegativefade volume. Exact28voicebytes/samplepointer andALLoutput samples, retained initialbuffer. Installedcallback addresses normalized to ROM identity; actualRAMcopy/bootstrap notexecuted. Outerchannel loop, outputconversion/SFX/fullIRQ/presentation/livehostbinding excluded.'}
(root/'reports/music-voice-mix-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
