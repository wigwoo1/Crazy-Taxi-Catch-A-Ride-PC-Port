"""Original C540 money-start scalar branch on actual native completed pickups."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
cfg=json.loads((root/'reports/fare-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class MoneyProbe(IdleProbe):
 def external(self,target):
  assert target==0x8032488,target
  self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
rows=[list(map(int,l.split())) for l in (root/'reports/current-passenger-money-states.txt').read_text().splitlines()]
mapping=[(0x2001698,4),(0x200169c,4),(0x20016a0,4),(0x20016a4,4),(0x2001693,1),(0x2001691,1)]
steps=0
for row in rows:
 assert len(row)==102
 index,length=row[:2];pre=row[2:8];post=row[8:14];customer_before=row[14:58];customer_after=row[58:]
 assert int.from_bytes(bytes(customer_before[4:8]),'little',signed=True)==length
 assert customer_after[0]==((customer_before[0]&254)|2) and customer_before[0]&32
 assert customer_before[1:]==customer_after[1:]
 p=MoneyProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0
 for (a,n),v in zip(mapping,pre):p.write(a,v,n)
 for i,v in enumerate(customer_after):p.write(0x2005f2c+index*44+i,v,1)
 p.reg[0]=1;p.reg[1]=index;p.run(entry=0x800c540,stop=0x800c7de)
 got=[signed(p.read(a,n)) if n==4 else p.read(a,n) for a,n in mapping]
 assert got==post,dict(index=index,route=length,original=got,native=post)
 steps+=p.steps
assert rows and (root/'reports/current-money-traffic-frames.txt').read_bytes()==(root/'reports/current-shared-voice-traffic-frames.txt').read_bytes()
assert (root/'reports/current-money-random.txt').read_bytes()==(root/'reports/current-shared-voice-random.txt').read_bytes()
report=dict(cases=len(rows),thumb_instructions=steps,match=True,traffic_frames_byte_identical=True,random_trace_byte_identical=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original C540 mode1 from actual entry through C7DE before formatting: all6fare fields exact against actual completed native pickup handoffs. Actual44byte before/aftercustomer capture retained, onlysourcecompletion flags changed. Signeddivision32488contract; formatting10830/glyphupdates excluded, fullB098completion caller notyetexecuted.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_fare_money_live_rom.py','reports/fare-cfg.json','reports/current-passenger-money-states.txt','src/fare.c','src/passenger_scene.c','bin/scene-viewer.exe']}
(root/'reports/fare-money-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
