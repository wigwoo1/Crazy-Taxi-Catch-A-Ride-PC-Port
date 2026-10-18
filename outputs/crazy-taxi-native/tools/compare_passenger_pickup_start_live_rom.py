"""Original pickup-start block against actual native beginning transitions."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
cfg=json.loads((root/'reports/passenger-pickup-start-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class PickupProbe(IdleProbe):
 def external(self,target):
  if target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-passenger-pickup-states.txt').read_text().splitlines()]
steps=cases=0
for row in rows:
 assert len(row)==73
 if not row[0]:continue
 began,completed,expected_calls,candidate,x,z,speed,gate,random1,random2,before,after,destination=row[:13];pre=row[13:21];post=row[21:29];customer=row[29:];assert not completed
 p=PickupProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;actor=0x2005f2c+candidate*44;taxi=0x10003000
 for i,v in enumerate(customer):p.write(actor+i,v,1)
 p.write(taxi,x);p.write(taxi+8,z);p.write(0x2001704,pre[0]);p.write(0x2001700,gate);p.write(0x2002b78,speed);p.write(0x2003b04,before)
 for addr,k in [(0x2007530,1),(0x200168c,2)]:p.write(addr,0 if pre[k]<0 else 0x2005f2c+pre[k]*44)
 p.write(0x2007534,pre[4]);p.write(0x2007538,pre[5]);p.write(0x20025d7,pre[6],1);p.write(0x20025d6,pre[7],1)
 p.reg[4]=candidate;p.reg[6]=taxi;p.reg[8]=0x2001704
 p.run(entry=0x800b136,stop={0x800b214,0x800b2d8})
 got=[signed(p.read(0x2001704)),(p.read(0x2007530)-0x2005f2c)//44,(p.read(0x200168c)-0x2005f2c)//44,pre[3],signed(p.read(0x2007534)),signed(p.read(0x2007538)),p.read(0x20025d7,1),p.read(0x20025d6,1)]
 assert got==post and p.read(0x2003b04)==after and p.calls==expected_calls,dict(original=got,native=post,rng=p.read(0x2003b04),expected=after,calls=p.calls)
 assert [p.read(actor+i,1) for i in range(44)]==customer
 cases+=1;steps+=p.steps
assert (root/'reports/current-pickup-traffic-frames.txt').read_bytes()==(root/'reports/current-passenger-sites-traffic-frames.txt').read_bytes()
report=dict(cases=cases,thumb_instructions=steps,match=True,traffic_frames_unchanged=True,scope='OriginalB136 pickup-start branch with actual native candidate/customer/taxi/gates/prior pickup/global RNG fixtures, nested original1E72C. All8pickupfields,44unchangedcustomerbytes,RNG/callcount match. Search34074/mark2360C excluded; signeddivision contract; completion/fare notexecuted.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_passenger_pickup_start_live_rom.py','reports/passenger-pickup-start-cfg.json','reports/current-passenger-pickup-states.txt','src/passenger_scene.c','src/passenger_pickup.c','bin/scene-viewer.exe']}
(root/'reports/passenger-pickup-start-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
