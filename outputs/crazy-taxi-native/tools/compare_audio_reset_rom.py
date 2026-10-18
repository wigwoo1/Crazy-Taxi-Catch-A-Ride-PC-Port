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

for entry,limit in [(0x8026704,0x802679a),(0x8016208,0x8016242),(0x8016174,0x80161ba),(0x80161f4,0x80161fe)]:
 todo=[entry];seen=set()
 while todo:
  address=todo.pop()
  if address in seen:continue
  assert entry<=address<=limit,hex(address);seen.add(address)
  i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(address+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ResetProbe(WholeAudioProbe):
 def external(self,target):
  if target==0x8016208:self.prepared_io=[self.read(a) for a in [0x40000bc,0x40000c0,0x40000c4,0x4000100]]
  if target in (0x8016208,0x8026888,0x80161f4):
   self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x2003528 and not(v&1):self.awaiting=True
  if a==0x40000a0:self.fifo_writes+=1
 def read(self,a,size=4):
  if a==0x2003528 and self.awaiting:
   self.awaiting=False;self.events+=1
   saved=self.reg[:];flags=self.flags
   before=self.read(0x20075f9);self.last_dispatch=int(before!=6 and self.read(0x2007600) and self.read(0x2007608) and self.read(0x20075f6))
   self.reg[13]=0x10000800;self.reg[14]=0x10030001
   self.run(entry=0x8016174,stop=0x10030000)
   self.reg[:]=saved;self.flags=flags
  return super().read(a,size)
cases=[(order,seed,mode,enabled,handshake,counter,keep,setting,divider,bank,0) for order,seed,mode in [(63,0,0),(5,7,1),(126,7,0)] for enabled,handshake,counter,keep,setting in [(1,0xffff,0xffffffff,0,165),(1,1,0x7fffffff,1,255),(0,0xa5a5,123,0,0),(2,1,123,0,0)] for divider,bank in [(0,0),(6,1),(5,0)]]
cases.extend([(126,7,0,enabled,0xffff,0xffffffff,0,165,0,0,182) for enabled in [0,1]])
rows=subprocess.run([str(root/'bin/audio-reset-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases)
index=steps=total_events=total_arm=reset_draws=0
for case in cases:
 order,seed,mode,enabled,handshake,counter,keep,setting,initial_divider,bank,warm=case;tail=0
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),pitch_address:bytearray(pitch),0x3005ac0:bytearray(704),0x3005d80:bytearray(704),0x2007574:bytearray(128),0x30054b0:bytearray(r[0x49dd80:0x49dd80+608]),0x4000000:bytearray(1024)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:0,0x20075f6:1,0x2007600:1,0x2003b04:seed,0x20075f9:0});p.draws=0;p.arm_steps=p.callbacks=0;p.scalars.update({0x2007570:0,0x2007818:0x3005d80,0x200781c:352,0x20075f4:64,0x2001697:0,0x2001ea8:0,0x2007560:1,0x2007602:1,0x20075f7:1,0x20075f5:64,0x200756c:0,0x2007564:21024,0x2007820:0x3005d80+352,0x2007824:352,0x3004c50:0x80001d8,0x3004c44:0x8000320,0x3004c48:0x800025c,0x3004c4c:0x800046c})
 for k,sound_index in [(0,41),(2,9)]:
  pointer=struct.unpack_from('<I',r,0x7ec658+sound_index*4)[0];off=pointer-0x8000000;rate,pref,length=struct.unpack_from('<HHI',r,off);base=0x2007574+k*32;phase=(length-1)*256+128 if k==0 and tail else 0
  for offset,value,size in [(0,123 if k==0 else 456,4),(8,10512,2),(20,phase,4),(16,128,4),(6,64 if k==0 else 32,1),(7,165,1),(4,int(k==2),1),(5,int(k==2),1),(12,length,4),(24,pointer+8+pref,4),(28,pointer+8 if pref else 0,4)]:p.write(base+offset,value,size)
 p.write(0x2007608,1,1);p.reg[0]=order;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8027110,stop=0x10020000);steps+=p.steps

 p.awaiting=False;p.events=p.fifo_writes=p.last_dispatch=0
 p.scalars.update({0x2003528:handshake,0x2003520:counter,0x2004308:0,0x2001ea7:keep,0x2001eb0:setting,0x200352c:0,0x3007ff8:0,0x4000208:enabled,0x4000006:159,0x2007568:352,0x20075f9:initial_divider,0x2007570:bank})
 for j in range(warm):
  p.steps=0
  p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8026888,stop=0x10020000)
  steps+=p.steps
 draws_before_reset=p.draws
 p.steps=0
 p.write(0x4000208,enabled,2);p.write(0x4000006,159,2)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:9];p.run(entry=0x8026704,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:9]==saved
 dispatch=p.last_dispatch
 parts=rows[index].split();index+=1;case=case
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

 extra=[p.read(a) for a in [0x2003520,0x2004308,0x2001eb0,0x3007ff8,0x2003528]]+[12,draws_before_reset]
 assert extra==list(map(int,parts[431:])),(case,'wait/event',extra,parts[431:])
 assert p.events==(5 if enabled==1 else 0) and p.fifo_writes==8,(case,p.events,p.fifo_writes)
 assert p.read(0x4000082,2)==0x300
 assert p.prepared_io==[0x3005d80+352,0x40000a0,0xf6600004,((-int(16777216/21024)&65535)|0xc00000)],(case,"prepared IO",p.prepared_io)
 steps+=p.steps;total_events+=p.events;total_arm+=p.arm_steps;reset_draws+=p.draws-draws_before_reset
report={'status':'PASS','cases':len(cases),'source_Thumb_observations':steps,'nested_ARM_audio_instructions':total_arm,'fresh_native_events':total_events,'RNG_draws_during_reset_wait':reset_draws,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE26704 reset with actual16208 wait, actual16174 event and26888 tracker/mix/output, actual161F4 acknowledgement under controlled asynchronous event injection. Nativeaudio_reset +frame_wait+frame_event+audio_frame exact represented retained state/352samples/two704byte banks/sharedRNG. Platform FIFO/DMA/timer writes observed in offlinefixture; nativeprepare/enable callbacks are explicit platform contracts, not Windows reset/IRQ/livebootstrap proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/audio_reset.c',root/'src/audio_reset.h',root/'src/frame_wait.c',root/'src/frame_event.c',root/'src/audio_frame.c',root/'tests/audio-reset-rom-compare.c',Path(__file__),root/'bin/audio-reset-rom-compare.exe']}}
(root/'reports/audio-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
