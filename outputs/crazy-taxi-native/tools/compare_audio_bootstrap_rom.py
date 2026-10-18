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
for entry,limit in [(0x8026488,0x8026598),(0x8027558,0x802757a),(0x8027584,0x8027602),(0x801e784,0x801e7d0)]:
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
class BootstrapProbe(ResetProbe):
 def external(self,target):
  if target in (0x8027558,0x8027584,0x8026704,0x801e784):self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
cases=[(selector,enabled,initialized,count) for selector in [0,1,2] for enabled in [0,1] for initialized,count in [(0,0),(1,352)]]
rows=subprocess.run([str(root/'bin/audio-bootstrap-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases)
steps=total_events=0
for case,line in zip(cases,rows):
 selector,enabled,initialized,old_count=case
 p=BootstrapProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),pitch_address:bytearray(pitch),0x3005ac0:bytearray(704),0x3005d80:bytearray(704),0x2007574:bytearray(128),0x30054b0:bytearray(r[0x49dd80:0x49dd80+608]),0x4000000:bytearray(1024)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:1,0x2003874:0,0x20075f6:1,0x2007600:1,0x2003b04:7,0x20075f9:0});p.draws=0;p.arm_steps=p.callbacks=0;p.scalars.update({0x2007570:0,0x2007818:0x3005d80,0x200781c:352,0x20075f4:64,0x2001697:0,0x2001ea8:0,0x2007560:1,0x2007602:1,0x20075f7:1,0x20075f5:64,0x200756c:0,0x2007564:21024,0x2007820:0x3005d80+352,0x2007824:352,0x3004c50:0x80001d8,0x3004c44:0x8000320,0x3004c48:0x800025c,0x3004c4c:0x800046c})

 p.awaiting=False;p.events=p.fifo_writes=p.last_dispatch=0
 p.regions[0x2007608]=bytearray(16);p.regions[0x2007618]=bytearray(512)
 p.regions[0x3005ac0][:]=bytes([165])*704;p.regions[0x3005d80][:]=bytes([165])*704
 p.scalars.update({0x2003528:0xa5a5,0x2003520:0xffffffff,0x2004308:0,0x2001ea7:0,0x2001eb0:165,0x2001eb1:37,0x2001eb2:53,0x200352c:0,0x3007ff8:0,0x2007568:352,0x20075f9:6,0x2007570:1,0x2007561:initialized,0x2007600:0,0x2007602:0,0x20075f6:0,0x200781c:old_count,0x2007824:old_count})
 p.write(0x4000208,enabled,2);p.write(0x4000006,159,2)
 for offset,value,size in [(0,123,4),(8,10512,2),(20,9876,4),(16,128,4),(6,64,1),(7,165,1),(4,1,1),(24,0xdeadbeef,4)]:p.write(0x2007574+offset,value,size)
 p.reg[0]=selector;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8026488,stop=0x10020000)
 bank_after=p.read(0x2007570);divider_after=p.read(0x20075f9);steps+=p.steps;p.steps=0
 p.reg[0]=10;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8016208,stop=0x10020000)
 got=[p.read(a) for a in [0x2007564,0x2007568,0x2007560,0x2007561,0x2007570,0x20075f9,0x20075f4,0x20075f5,0x20075f6,0x20075f7,0x2007608,0x200760a]]+[bank_after,divider_after]+[p.read(a) for a in [0x2003520,0x2001eb0,0x2003528]]+[1342 if not initialized else 134,p.read(0x2003b04),p.draws]
 # Native phase sequence: stop1, resetprepare3, resetenable4, optionalbind2.
 for k in range(4):
  b=0x2007574+k*32;got+=[p.read(b),p.read(b+8,2),p.read(b+20),p.read(b+16),p.read(b+4,1),p.read(b+5,1),p.read(b+6,1),p.read(b+7,1),int(bool(p.read(b+24)))]
 parts=line.split();assert got==list(map(int,parts[:56])),(case,'fields',got,parts[:56])
 assert bytes(p.regions[0x3005d80]).hex()==parts[56],(case,'output')
 scratch=''.join(format(p.read(0x3005ac0+j*2,2),'04x') for j in range(352));assert scratch==parts[57],(case,'scratch')
 table=[p.read(0x2007818)-0x3005d80,p.read(0x200781c),p.read(0x2007820)-0x3005d80,p.read(0x2007824)];assert table==list(map(int,parts[58:])),(case,'table',table,parts[58:])
 assert p.events==(15 if enabled==1 else 0)
 steps+=p.steps;total_events+=p.events
report={'status':'PASS','cases':len(cases),'source_Thumb_observations':steps,'controlled_events':total_events,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole26488/27584/26704/16208/16174/26888/161F4 andactual1E784 memoryclear plus outer post-bootstrap wait10, cold/warminitialized flag,3validqualities/eventgate0+1. Exactrepresentedflags/retainedchannels/bank/divider/counters/704outputbytes/704scratchbytes/buffertable; unloaded tracker anddisabled domains. Native platform stop/reset/bind contracts; DMAcodecopy observed notexecuted or nativeARM runtime. Not complete5404/systeminitialization/liveWindows binding or realasync preemption proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/audio_bootstrap.c',root/'src/audio_bootstrap.h',root/'tests/audio-bootstrap-rom-compare.c',Path(__file__),root/'bin/audio-bootstrap-rom-compare.exe']}}
(root/'reports/audio-bootstrap-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
