"""Original collision effect tail, actual RNG and combo reset against captured contacts."""
exec(open('outputs/crazy-taxi-native/tools/compare_random_range_rom.py').read().split('\ncases=')[0])
for lo,hi in [(0x6afa,0x6b68),(0x2958c,0x2959c)]:
 allowed_thumb.update({i.address:bytes(i.bytes) for i in md.disasm(r[lo:hi],0x8000000+lo)})
class EffectsProbe(RandomProbe):
 def write(self,a,v,n=4):
  if a in [0x20000e4,0x20025d3,0x20025d6,0x2009aa6,0x2009aa8,0x2009aac]:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x801e72c,0x802958c]:
   if target==0x801e72c:self.calls+=1
   self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x80267c8:self.sound_ptr=self.reg[0]
  else:super().external(target)
f=root/'reports/current-traffic-effect-states.txt';rows=[list(map(int,l.split())) for l in f.read_text().splitlines()];steps=0
for index,a in enumerate(rows):
 p=EffectsProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.sound_ptr=None
 for addr,v,n in zip([0x20000e4,0x20025d3,0x20025d6,0x2009aa6,0x2009aa8,0x2009aac],a[:6],[2,1,1,1,4,4]):p.write(addr,v,n)
 p.arm.mem[0x2007600]=(a[6]&255)<<16;p.arm.mem[0x2001694]=a[7]&255;p.arm.mem[0x2003b04]=a[8]
 p.run(entry=0x8006afa,stop=0x8006b68)
 got=[p.read(addr,n) for addr,n in [(0x20000e4,2),(0x20025d3,1),(0x20025d6,1),(0x2009aa6,1),(0x2009aa8,4),(0x2009aac,4)]]+[p.read(0x2003b04),p.calls]
 assert got==a[9:17],dict(index=index,original=got,native=a[9:17])
 expected=None if a[17]<0 else struct.unpack_from('<I',r,0x7ec658+a[17]*4)[0];assert p.sound_ptr==expected;steps+=p.steps
report=dict(contacts=len(rows),pass_match=True,thumb_instructions=steps,scope='Actual original6AFA..6B66 tail+1E72C RNG+2958C comboreset vs captured22effects. Signedremainder contract, audio267C8 submission pointer captured not sound playback/fullframe.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [f,root/'tools/compare_live_traffic_effects_rom.py',root/'src/traffic_effects.c',root/'bin/scene-viewer.exe']}
(root/'reports/live-traffic-effects-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
