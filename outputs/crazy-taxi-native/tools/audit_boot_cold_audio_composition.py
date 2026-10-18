"""Cold system boot followed by original audio initialization/wait with controlled frame delivery."""
exec(open('outputs/crazy-taxi-native/tools/audit_boot_cold_copy_install.py').read().split('\nresults=')[0])
b=ColdBootProbe((0,0,0,0,0,1,0,0,0,0));b.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};b.scalars={};b.stores=[];b.published_copy=False;b.calls=[];b.contracts=[];b.dma=[];b.shade_divides=0;b.reg[13]=0x10000400;b.reg[14]=0x10020001
b.run(entry=0x801e2e4,stop=0x10020000);b.reg[14]=0x10020001;b.run(entry=0x8010a54,stop=0x10020000)
cold_steps=b.steps
ns={};exec(open('outputs/crazy-taxi-native/tools/compare_audio_bootstrap_rom.py').read().split('\ncases=')[0],ns)
Base=ns['BootstrapProbe'];audio_allowed=ns['allowed_thumb'];audio_md=ns['md']
for a in [0x80161dc]+list(range(0x8005424,0x800543c,2)):audio_allowed[a]=r[a-0x8000000:a-0x8000000+2]
class PostColdAudio(Base):
 def external(self,target):
  if target==0x803244c:
   assert self.reg[0]==0x80161dd
   self.run(entry=0x80161dc,stop=self.reg[14]&~1);return
  if target==0x8032454:
   assert self.reg[2]==0x3002304
   self.reg[0]=arm['original'](self.reg[0],self.reg[1]);return
  if target==0x8032458:
   actual=self.reg[3];assert actual==self.read(0x3004c50)
   assert actual==next(dst for src,dst,count in self.code_copies if src==0x80001d8)
   self.reg[3]=0x80001d8
   return super().external(target)
  try:return super().external(target)
  except AssertionError as e:raise AssertionError(('audiohelper',hex(target),[hex(v) for v in self.reg[:5]])) from e
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc and v&0x80000000:
   src,dst=self.read(0x40000d4),self.read(0x40000d8);count=(v&65535)*4
   assert 0x8000000<=src<src+count<=0x8800000 and 0x3000000<=dst<dst+count<=0x3008000,(hex(v),hex(src),hex(dst))
   self.regions[0x3000000][dst-0x3000000:dst-0x3000000+count]=r[src-0x8000000:src-0x8000000+count];self.code_copies.append((src,dst,count))
p=PostColdAudio((0,0,0,0,0,1,0,0,0,0));p.regions={a:bytearray(v) for a,v in b.regions.items()};p.scalars={};p.awaiting=False;p.events=p.fifo_writes=p.last_dispatch=0;p.draws=p.arm_steps=p.callbacks=0;p.code_copies=[]
p.reg[0]=2;p.reg[13]=0x10000400;p.reg[14]=0x10020001
p.run(entry=0x8026488,stop=0x10020000)
bootstrap_events=p.events;bootstrap_steps=p.steps
p.reg[0]=10;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8016208,stop=0x10020000)
assert p.read(0x2003899,1)==0
assert p.reg[13]==0x10000400
assert p.events==15 and p.read(0x2003520)==15
assert p.read(0x2007564)==21024 and p.read(0x2007568)==352 and p.read(0x2007560,1)==1 and p.read(0x2007561,1)==1
p.run(entry=0x8005424,stop=0x800543c)
assert p.read(0x2007600,1)==1 and p.read(0x2007602,1)==1 and p.read(0x2001ea6,1)==1
owned=[p.read(a,1) for a in [0x2001ea4,0x2001ea5,0x2007560,0x2007600,0x2007602,0x2001ea6]]
native=subprocess.run([str(root/'bin/startup-boot-rom-compare.exe')],input='0 0 0 0\n',text=True,capture_output=True,check=True).stdout.split()
assert owned==list(map(int,native[:6]))
report={'status':'PASS_COLD_AUDIO_SOURCE_COMPOSITION','cold_boot_Thumb':cold_steps,'audio_bootstrap_Thumb':bootstrap_steps,'audio_total_Thumb':p.steps,'events_during_bootstrap':bootstrap_events,'events_including_outer_wait':p.events,'native_startup_owned_bytes':owned,'parent_flag':p.read(0x2003899,1),'random_seed':p.read(0x2003b04),'random_draws':p.draws,'ARM_audio_instructions':p.arm_steps,'audio_mix_callbacks':p.callbacks,'copied_audio_code':[{'source':f'{src:08X}','destination':f'{dst:08X}','bytes':count} for src,dst,count in p.code_copies],'scope':'Originalcold1E2E4 +10A54 RAM state passedwithoutfixture reseeding towhole26488(selector2) thenactual16208(10), controlled16174events withretained161DDcallback. DMA3rawROM codecopies modeled; copiedcode neverexecuted. InstalledARM029FC division normalizedtooriginalROM quotient; ARM001D8 conversion handler resolvesactualDMA destination butisnotreachedwhilecoldmusic/effectsdomainsdisabled; noaudibleoutputclaim; logicalFIFO/timerhardware, controlledframeeventdelivery notrealinterrupt/preemption/Windows pacing. Actual5404 late5424..543A coldgateenable/mainmode suffix executed; sixnative startup_boot ownerbytes match. Earlier5404 calls manuallycomposed inoriginalorder, notwhole5404 stackoracle. No warmboot/parententry/livehostintegration proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/boot-cold-audio-composition-source.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS coldboot->actualaudio->outerwait',p.events,'controlled events',p.steps,'audio Thumb')
