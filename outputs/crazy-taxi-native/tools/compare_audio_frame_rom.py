"""Whole original26888 entry/return, retained native logical audio composition."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_audio_effects_packed_rom.py').read().split('\npacked_indices=')[0])
output_namespace={}
exec(open('outputs/crazy-taxi-native/tools/compare_audio_output_rom.py').read().split('\ncases=')[0],output_namespace)
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:allowed_thumb[int(item['address'],16)]=bytes.fromhex(item['bytes'])
class WholeAudioProbe(PackedMixProbe):
 def external(self,target):
  if target==0x8032458:
   assert self.reg[3]==0x80001d8
   self.arm_steps+=output_namespace['conversion'](self)
  else:super().external(target)

cases=[(63,120,0,0,0),(5,120,7,1,1),(module[1]-1,600,7,0,1)]
rows=subprocess.run([str(root/'bin/audio-frame-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==sum(c[1] for c in cases)
index=0;steps=0;dispatches=0;total_draws=0;arm_mix_steps=mix_callbacks=0
for order,frames,seed,mode,tail in cases:
 p=WholeAudioProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),pitch_address:bytearray(pitch),0x3005ac0:bytearray(704),0x3005d80:bytearray(704),0x2007574:bytearray(128),0x30054b0:bytearray(r[0x49dd80:0x49dd80+608]),0x4000000:bytearray(1024)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:0,0x20075f6:1,0x2007600:1,0x2003b04:seed,0x20075f9:0});p.draws=0;p.arm_steps=p.callbacks=0;p.scalars.update({0x2007570:0,0x2007818:0x3005d80,0x200781c:352,0x20075f4:64,0x2001697:0,0x2001ea8:0,0x2007560:1,0x2007602:1,0x20075f7:1,0x20075f5:64,0x200756c:0,0x2007564:21024,0x2007820:0x3005d80+352,0x2007824:352,0x3004c50:0x80001d8,0x3004c44:0x8000320,0x3004c48:0x800025c,0x3004c4c:0x800046c})
 for k,sound_index in [(0,41),(2,9)]:
  pointer=struct.unpack_from('<I',r,0x7ec658+sound_index*4)[0];off=pointer-0x8000000;rate,pref,length=struct.unpack_from('<HHI',r,off);base=0x2007574+k*32;phase=(length-1)*256+128 if k==0 and tail else 0
  for offset,value,size in [(0,123 if k==0 else 456,4),(8,10512,2),(20,phase,4),(16,128,4),(6,64 if k==0 else 32,1),(7,165,1),(4,int(k==2),1),(5,int(k==2),1),(12,length,4),(24,pointer+8+pref,4),(28,pointer+8 if pref else 0,4)]:p.write(base+offset,value,size)
 p.write(0x2007608,1,1);p.reg[0]=order;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8027110,stop=0x10020000);steps+=p.steps
 for frame in range(frames):
  enabled=int(not(100<=frame%193<109));p.write(0x2007600,enabled,1);old_divider=p.read(0x20075f9);dispatch=int(old_divider!=6 and enabled and p.read(0x2007608) and p.read(0x20075f6));dispatches+=dispatch
  p.steps=0;p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:11];p.run(entry=0x8026888,stop=0x10020000);assert p.reg[13]==0x10000400 and p.reg[4:11]==saved;steps+=p.steps
  parts=rows[index].split();index+=1;case=(order,frame)
  assert [p.read(0x20075f9),dispatch]==list(map(int,parts[:2])),(case,'gate',p.read(0x20075f9),dispatch,parts[:4])
  assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(parts[2]),(case,'state',p.regions[0x2007608].hex(),parts[2])
  assert p.read(0x2007614)-0x8421fc0==int(parts[3]),(case,'pattern')
  for k in range(16):
   actual=bytes(p.regions[0x2007618][k*32:k*32+28]);assert actual==bytes.fromhex(parts[4+k*2]),(case,k,'voice',actual.hex(),parts[4+k*2]);pointer=p.read(0x2007618+k*32+28);offset=pointer-0x8421fc0 if pointer else 0xffffffff;assert offset==int(parts[5+k*2]),(case,k,'sample')
  assert [p.read(0x2003b04),p.draws,p.read(0x20075f6)]==list(map(int,parts[36:39])),(case,'RNG')
  pcm=[p.read(0x3005ac0+j*2,2) for j in range(352)];pcm=[v if v<32768 else v-65536 for v in pcm];assert pcm==list(map(int,parts[39:391])),(case,'mix')
  assert [p.read(0x2007570),p.read(0x200756c)]==list(map(int,parts[391:393])),(case,'bank/request')
  channels=[]
  for k in range(4):
   b=0x2007574+k*32;channels+=[p.read(b),p.read(b+8,2),p.read(b+20),p.read(b+16),p.read(b+4,1),p.read(b+5,1),p.read(b+6,1),p.read(b+7,1),int(bool(p.read(b+24)))]
  assert channels==list(map(int,parts[393:429])),(case,'effects')
  assert bytes(p.regions[0x3005d80][:352])==bytes.fromhex(parts[429]) and bytes(p.regions[0x3005d80][352:])==bytes.fromhex(parts[430]),(case,'output')

 total_draws+=p.draws;arm_mix_steps+=p.arm_steps;mix_callbacks+=p.callbacks
report={'status':'PASS','frames':index,'sequences':cases,'dispatches':dispatches,'random_draws':total_draws,'source_Thumb_observations':steps,'nested_ARM_audio_instructions':arm_mix_steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE26888 entry/return including original peripheral-prefix instructions under logicalIOmemory fixture, retained tracker/music/SFX/output composition vsnative audio_frame. Packed41ordinary/end +raw9repeat,120/120/600frames,original orders63/5/126. Exacttracker16voices/sharedRNG/SFXrepresentedfields/request/scratch352samples/output704bytes/bankafterEVERYframe; sourceSP/savedregisters restored. Actualnested row/load/start/RNG/divider/PCM/loop/packed/converter bodies. Logical hardware writes not native emulation or realIRQ/timer/presentation/asynchronousloading/hostdevice proof; underlying arithmetic modulo contracts retained.'}
(root/'reports/audio-frame-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
