"""Original CB84 trip initialization against live native fare handoffs."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
cfg=json.loads((root/'reports/trip-start-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TripProbe(IdleProbe):
 def read(self,a,n=4):
  if 0x40000d4<=a<0x40000e0:return self.arm.mem[a]
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x40000d4<=a<0x40000e0:self.dma.append((a,v));self.arm.mem[a]=v
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-passenger-fare-states.txt').read_text().splitlines()]
mapping=[(0x2001688,4),(0x200753c,4),(0x20016ac,4),(0x20016b0,4),(0x20016e0,4),(0x2005310,2),(0x2005312,2),(0x2005314,2),(0x2001694,1),(0x2001695,1),(0x20025d6,1)]
steps=0
for row in rows:
 assert len(row)==31
 index,length,remaining,category,setting0,setting3,before,after,random=row[:9];pre=row[9:20];post=row[20:]
 p=TripProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.dma=[]
 for (addr,n),v in zip(mapping,pre):p.write(addr,0 if addr==0x2001688 and v<0 else 0x2005f2c+v*44 if addr==0x2001688 else v,n)
 p.write(0x2005f2c+index*44+4,length);p.write(0x2005f2c+index*44+2,category,1)
 p.write(0x2001eb0,setting0,1);p.write(0x2001eb3,setting3,1);p.write(0x20016a8,remaining);p.write(0x2003b04,before)
 p.reg[0]=index;p.run(entry=0x800cb84,stop=0x10020000)
 got=[((p.read(a,n)-0x2005f2c)//44 if a==0x2001688 else signed(p.read(a,n)) if n==4 else p.read(a,n)) for a,n in mapping]
 assert got==post and p.read(0x2003b04)==after and p.calls==1,dict(original=got,native=post,rng=p.read(0x2003b04),expected=after,calls=p.calls)
 if setting0&224!=128:
  assert p.dma==[(0x40000d4,0x8290b1c),(0x40000d8,0x5000320),(0x40000dc,0x80000010),(0x40000d4,0x8290b40),(0x40000d8,0x5000340),(0x40000dc,0x80000010)]
  assert p.read(0x2001854)==0x8290b1c and p.read(0x2001868)==0x8290b40
 steps+=p.steps
assert rows and (root/'reports/current-fare-traffic-frames.txt').read_bytes()==(root/'reports/current-voice-traffic-frames.txt').read_bytes()
report=dict(cases=len(rows),thumb_instructions=steps,match=True,traffic_frames_unchanged=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete originalCB84 throughreturn vsactualnative fare handoffs: all11trip fields,sharedRNG/callcount exact. Customerindex/route/category/settings/remainingclock/priortrip capturedfixtures. Signeddivision contract. OriginalDMA registerrequests and palette source references asserted; DMA transfer/renderedpalette not executed. Native fareC540 money initialization/completioncaller notcompared.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_trip_start_live_rom.py','reports/trip-start-cfg.json','reports/current-passenger-fare-states.txt','src/trip_start.c','src/passenger_scene.c','bin/scene-viewer.exe']}
(root/'reports/trip-start-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
