"""Retained source music state scheduling across consecutive frames; mixer excluded."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_restart_mixed_rom.py').read().split('\nrestart_rows=')[0])
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 address=int(item['address'],16)
 if 0x8026918<=address<0x8026a5e:allowed_thumb[address]=bytes.fromhex(item['bytes'])
for function in json.loads((root/'reports/restart-music-dependency-audit.json').read_text())['functions']:
 if function['entry']=='0x8027730':
  for item in function['instructions']:allowed_thumb[int(item['address'],16)]=bytes.fromhex(item['bytes'])
# Include the independently qualified full between-row callback.
todo=[0x80274bc];seen=set()
while todo:
 address=todo.pop()
 if address in seen:continue
 assert 0x80274bc<=address<=0x8027556,hex(address)
 seen.add(address);i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ContinuousProbe(RestartRowsProbe):
 def external(self,target):
  if target in [0x80271e0,0x80274bc,0x8027730]:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
for entry,limit in [(0x8016174,0x80161ba),(0x8026318,0x8026420)]:
 todo=[entry];seen=set()
 while todo:
  address=todo.pop()
  if address in seen:continue
  assert entry<=address<=limit,hex(address)
  seen.add(address);i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(address+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class LoadingProbe(ContinuousProbe):
 def external(self,target):
  if target==0x8026888:
   # Explicit pre-mixer state boundary; preserve original audio callee saved registers.
   saved=self.reg[4:15];self.reg[13]-=256
   self.run(entry=0x8026918,stop=0x8026a5e)
   self.reg[4:15]=saved
  elif target==0x8016208:
   for k in range(self.waits):
    saved=self.reg[14];self.reg[14]=0x10020001
    self.run(entry=0x8016174,stop=0x10020000);self.reg[14]=saved
   self.reg[0]=0
  elif target in [0x803244c,0x80161f4,0x8022508,0x8015c3c,0x800ee10,0x8027cd0,0x8028ee0]:self.reg[0]=0
  else:super().external(target)
cases=[(order,52,seed,mode) for order,seed,mode in [(63,0,0),(88,7,0),(115,0x7fffffff,0),(34,1,0),(5,0,1),(module[1]-1,7,0)]]
rows=subprocess.run([str(root/'bin/loading-music-service-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==sum(c[1] for c in cases)
index=0;steps=0;event_count=0;total_draws=0
for order,frames,seed,mode in cases:
 p=LoadingProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),pitch_address:bytearray(pitch)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:0,0x20075f6:1,0x2007600:1,0x2003b04:seed,0x20075f9:0});p.draws=0
 addresses=[0x2003520,0x2003524,0x2007541,0x2007548,0x4000000,0x200754c,0x2007550,0x200755c,0x2001e10,0x2007540,0x2001eb0,0x4000006,0x3007ff8,0x2004308]
 values=[0xfffffff0,0xfffffff0,4,255,0xa5a5,0x12345678,0xabcdef01,0xfffffff0,0xfffffffe,255,165,160,0,0]
 p.scalars.update(dict(zip(addresses,values)))
 p.write(0x2007608,1,1);p.reg[0]=order;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8027110,stop=0x10020000);steps+=p.steps;dispatches=0
 for frame in range(frames):
  enabled=int(not(10<=frame%17<13));p.write(0x2007600,enabled,1);p.waits=65 if frame%11==0 else 1+frame%3;event_count+=p.waits
  # Enabled is constant within this controlled wait, and loader/start preserve loaded and playing.
  clock=p.read(0x20075f9)
  for k in range(p.waits):
   if clock!=6 and enabled:dispatches+=1
   clock=0 if clock==6 else (clock+1)&255
  p.write(0x2001ea7,frame%2,1);p.write(0x200352c,0x80161dd if frame%3==0 else 0);p.write(0x2005f24,frame%3,1);p.write(0x2005f20,int(frame%7==0),1)
  p.steps=0;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8026318,stop=0x10020000);steps+=p.steps
  parts=rows[index].split();index+=1;case=(order,frame)
  assert [p.read(0x20075f9),dispatches]==list(map(int,parts[:2])),(case,'gate')
  assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(parts[2]),(case,'state',p.regions[0x2007608].hex(),parts[2])
  assert p.read(0x2007614)-0x8421fc0==int(parts[3]),(case,'pattern')
  for k in range(16):
   actual=bytes(p.regions[0x2007618][k*32:k*32+28]);assert actual==bytes.fromhex(parts[4+k*2]),(case,k,'voice',actual.hex(),parts[4+k*2]);pointer=p.read(0x2007618+k*32+28);offset=pointer-0x8421fc0 if pointer else 0xffffffff;assert offset==int(parts[5+k*2]),(case,k,'sample')
  assert [p.read(0x2003b04),p.draws,p.read(0x20075f6)]==list(map(int,parts[36:39])),(case,'RNG')
  result_addresses=[0x2003520,0x2001eb0,0x3007ff8,0x2007548,0x4000000,0x200754c,0x2007550,0x2003524,0x2003b00,0x200755c,0x2001e10,0x2007540]
  assert [p.read(a) for a in result_addresses]==list(map(int,parts[39:])),(case,'presentation')
 total_draws+=p.draws
report={'status':'PASS','presentations':index,'logical_frame_events':event_count,'random_draws':total_draws,'sequences':cases,'source_instruction_observations':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Retained composition actual26318 presentation caller +16174 frame event +26918..26A5E pre-mixer music state and actual nested callbacks. Controlled wait contract1/2/3/65events,52presentations persequence. Exact tracker16voices/sharedRNG plus counter/settings/ack/buffer bank/display/delta/elapsed/cursor/parity. Scanline160 supplied; optionalcallback/handshake/input/palette/sprites/DMA no-op contracts; IRQ dispatch/mixer/hardware/display pacing/actual passenger loading interleaving excluded. No host audio device used.'}
(root/'reports/loading-music-service-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
