"""Whole four-channel SFX pass: actual raw PCM/loop, protected packed skip."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_voice_mix_rom.py').read().split('\ncases=')[0])
for item in json.loads((root/'reports/frame-audio-service-cfg.json').read_text())['reachable_instructions']:
 a=int(item['address'],16)
 if 0x8026caa<=a<0x8026e0e:allowed_thumb[a]=bytes.fromhex(item['bytes'])
cases=[(mask,count,tail,protect,global_volume) for mask in range(8) for count in [5,352] for tail in [0,1] for protect in [0,1] for global_volume in [64,255]]
result=subprocess.run([str(root/'bin/audio-effects-buffer-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True);assert result.returncode==0,result.stderr;rows=result.stdout.splitlines();assert len(rows)==len(cases);steps=arm_steps=0
for case,line in zip(cases,rows):
 mask,count,tail,protect,global_volume=case;p=MixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007574:bytearray([165]*128),0x3005ac0:bytearray(704)};p.scalars={0x200756c:165,0x2007602:mask&1,0x20075f7:mask&2,0x2007560:mask&4,0x2007570:0,0x200781c:count,0x20075f5:global_volume,0x3004c48:0x800025c,0x3004c4c:0x800046c,division_owner:0x80029fc};p.arm_steps=p.callbacks=0
 for k,index in enumerate([9,10,58,41]):
  pointer=struct.unpack_from('<I',r,0x7ec658+index*4)[0];off=pointer-0x8000000;rate,prefix,length=struct.unpack_from('<HHI',r,off);base=0x2007574+k*32;phase=(length-1)*256+128 if tail else 0
  for offset,value,size in [(0,0xa5a5a5a5+k,4),(8,1234,2),(20,phase,4),(16,128+k*64,4),(6,64+k*7,1),(7,165,1),(4,1 if k==3 else protect,1),(5,255,1),(12,length,4),(24,pointer+8+prefix if k!=1 else 0,4),(28,pointer+8 if prefix else 0,4)]:p.write(base+offset,value,size)
 for j in range(count):p.write(0x3005ac0+j*2,j*257-30000,2)
 p.reg[13]=0x10000400;p.run(entry=0x8026caa,stop={0x8026e0e,0x8026e80})
 got=[p.read(0x200756c)]
 for k in range(4):
  b=0x2007574+k*32;got+=[p.read(b),p.read(b+8,2),p.read(b+20),p.read(b+16),p.read(b+4,1),p.read(b+5,1),p.read(b+6,1),p.read(b+7,1),int(bool(p.read(b+24)))]
 pcm=[p.read(0x3005ac0+j*2,2) for j in range(count)];got+=[v if v<32768 else v-65536 for v in pcm];assert got==list(map(int,line.split())),(case,'channel/mix mismatch');steps+=p.steps;arm_steps+=p.arm_steps
report={'status':'PASS','cases':len(cases),'actual_Thumb_instructions':steps,'actual_ARM_instructions':arm_steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole26CAA..26E0E fourchannel SFX pass versus native effects_buffer. Gate8combinations/counts5/352/first-finalphase/protected-normal/raw9/10/58 andprotectedpacked41skip/global64/255. ActualARM025CPCM/046Cloop/029FCdivide. Exact representedchannel fields,ALLmixedsamples and756Crequest count includingentryactivechannels that stop. Normalpacked0320 callback excluded; outputconversion/wholeIRQ/livebinding excluded.'}
(root/'reports/audio-effects-buffer-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
