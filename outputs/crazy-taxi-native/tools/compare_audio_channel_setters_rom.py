"""Original whole volume/pitch setters; signed divider explicit boundary."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x276cc:0x276da],0x80276cc):allowed_thumb[ins.address]=bytes(ins.bytes)
for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x276dc:0x276fa],0x80276dc):allowed_thumb[ins.address]=bytes(ins.bytes)
class SetterProbe(UpdateProbe):
 def read(self,a,n=4):
  if a==0x3006940:assert n==4;return 0x80029fc
  return super().read(a,n)
 def external(self,target):
  assert target==0x8032454 and self.reg[2]==0x80029fc
  a,b=map(signed,self.reg[:2]);q=abs(a)//abs(b);q=-q if (a<0)!=(b<0) else q;self.reg[0]=q&0xffffffff
cases=list(itertools.product([0,10512,65535,65536,4294967295],[10512,15768,21024,2147483648,4294967295],[0,64,65,4294967295]))
cp=subprocess.run([str(root/'bin/audio-channel-setters-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];steps=0
for (rate,output,volume),want in zip(cases,rows):
 p=SetterProbe((0,0,0,0,0,1,0,0,0,0));base=0x2009000;p.write(base,123);p.write(base+0x14,456);p.write(0x2007564,output)
 for entry,arg in [(0x80276cc,volume),(0x80276dc,rate)]:p.reg[0]=base;p.reg[1]=arg;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 actual=[p.read(base+8,2),p.read(base+0x10),p.read(base+6,1),p.read(base),p.read(base+0x14)];assert actual==want,((rate,output,volume),actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,scope='Complete276CC/276DC rate16/step/unsigned capvolume and untouchedage/phase; signed divider explicit boundary, no liveplayback.',rom_sha256=hashlib.sha256(r).hexdigest(),source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/audio_channel.c','src/audio_channel.h','tests/audio_channel_setters_rom_compare.c','tools/compare_audio_channel_setters_rom.py']})
(root/'reports/audio-channel-setters-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
