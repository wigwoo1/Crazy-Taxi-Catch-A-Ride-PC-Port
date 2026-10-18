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
exec(open('outputs/crazy-taxi-native/tools/compare_music_voice_mix_rom.py').read().split('\ncases=')[0])
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 address=int(item['address'],16)
 if 0x8026918<=address<0x8026caa:allowed_thumb[address]=bytes.fromhex(item['bytes'])

cases=[(order,600 if order==module[1]-1 else 240,seed,mode) for order,seed,mode in [(63,0,0),(88,7,0),(115,0x7fffffff,0),(34,1,0),(5,0,1),(module[1]-1,7,0)]]
rows=subprocess.run([str(root/'bin/music-frame-mix-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==sum(c[1] for c in cases)
index=0;steps=0;dispatches=0;total_draws=0;arm_mix_steps=mix_callbacks=0
for order,frames,seed,mode in cases:
 p=MixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),pitch_address:bytearray(pitch),0x3005ac0:bytearray(704)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:0,0x20075f6:1,0x2007600:1,0x2003b04:seed,0x20075f9:0});p.draws=0;p.arm_steps=p.callbacks=0;p.scalars.update({0x2007570:0,0x2007818:0x3005d80,0x200781c:352,0x20075f4:64,0x2001697:0,0x2001ea8:0,0x2007560:1,0x2007602:0,0x20075f7:0,0x3004c48:0x800025c,0x3004c4c:0x800046c})
 p.write(0x2007608,1,1);p.reg[0]=order;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8027110,stop=0x10020000);steps+=p.steps
 for frame in range(frames):
  enabled=int(not(100<=frame%193<109));p.write(0x2007600,enabled,1);old_divider=p.read(0x20075f9);dispatch=int(old_divider!=6 and enabled and p.read(0x2007608) and p.read(0x20075f6));dispatches+=dispatch
  p.steps=0;p.reg[13]=0x10000400;p.run(entry=0x8026918,stop=0x8026caa);steps+=p.steps
  parts=rows[index].split();index+=1;case=(order,frame)
  assert [p.read(0x20075f9),dispatch]==list(map(int,parts[:2])),(case,'gate')
  assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(parts[2]),(case,'state',p.regions[0x2007608].hex(),parts[2])
  assert p.read(0x2007614)-0x8421fc0==int(parts[3]),(case,'pattern')
  for k in range(16):
   actual=bytes(p.regions[0x2007618][k*32:k*32+28]);assert actual==bytes.fromhex(parts[4+k*2]),(case,k,'voice',actual.hex(),parts[4+k*2]);pointer=p.read(0x2007618+k*32+28);offset=pointer-0x8421fc0 if pointer else 0xffffffff;assert offset==int(parts[5+k*2]),(case,k,'sample')
  assert [p.read(0x2003b04),p.draws,p.read(0x20075f6)]==list(map(int,parts[36:39])),(case,'RNG')
  pcm=[p.read(0x3005ac0+j*2,2) for j in range(352)];pcm=[v if v<32768 else v-65536 for v in pcm];assert pcm==list(map(int,parts[39:])),(case,'mix')
 total_draws+=p.draws;arm_mix_steps+=p.arm_steps;mix_callbacks+=p.callbacks
report={'status':'PASS','frames':index,'sequences':cases,'dispatches':dispatches,'random_draws':total_draws,'source_Thumb_observations':steps,'actual_ARM_mix_instructions':arm_mix_steps,'mix_callbacks':mix_callbacks,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Consecutive actual26918..26CAA tracker scheduling plus fullmusic clear/gate/4voice mixer, nested actualrows/nonrows/restarts/RNG/divider andARM025CPCM/046Cloop callbacks. Exact12trackerbytes/ALL16voice nonpointerbytes/sampleidentities/sharedRNG/divider/playing and352signed16mixsamples EVERYframe. Periodicmusicdisabled retains original buffer; zeroSFX fixtures. SFX/outputconversion/hardwareIRQ/hostdevice/presentation/bootstrap and realelapsed loading excluded.'}
(root/'reports/music-frame-mix-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
