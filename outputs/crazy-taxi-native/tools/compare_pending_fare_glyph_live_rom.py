"""Original C540 mode2 pending fare target update with original10830 formatter."""
exec(open('outputs/crazy-taxi-native/tools/compare_fare_money_live_rom.py').read().split('\nrows=')[0])
cfg=json.loads((root/'reports/fare-formatter-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class GlyphProbe(MoneyProbe):
 def external(self,target):
  if target==0x8010830:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x801e420:
   assert self.reg[1]==0x80345e4 and r[0x345e4:0x345e7]==b'%d\0'
   data=str(signed(self.reg[2])).encode()+b'\0'
   for i,v in enumerate(data):self.write(self.reg[0]+i,v,1)
   self.reg[0]=len(data)-1
  elif target==0x801e7d4:
   ptr=self.reg[0];n=0
   while self.read(ptr+n,1):n+=1;assert n<32
   self.reg[0]=n
  elif target==0x80325f0:self.reg[0]=self.reg[0]//self.reg[1] if self.reg[1] else 0
  elif target==0x8032668:self.reg[0]=self.reg[0]%self.reg[1] if self.reg[1] else 0
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-cash-glyph-states.txt').read_text().splitlines()]
steps=cases=changes=0
for index,row in enumerate(rows):
 assert len(row)==44
 if index>=64 and row[6:25]==row[25:]:continue
 p=GlyphProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0
 for (a,n),v in zip([(0x2001698,4),(0x200169c,4),(0x20016a0,4),(0x20016a4,4),(0x2001693,1),(0x2001691,1)],row[:6]):p.write(a,v,n)
 for i,v in enumerate(row[16:25]):p.write(0x2005374+i*2,v,2)
 p.reg[0]=2;p.reg[1]=0xffffffff;p.run(entry=0x800c540,stop=0x10020000)
 got=[p.read(0x2005374+i*2,2) for i in range(9)]
 assert got==row[35:],dict(index=index,original=got,native=row[35:])
 steps+=p.steps;cases+=1;changes+=row[16:25]!=row[35:]
assert len(rows)==2048
report=dict(cases=cases,changed_pending_cases=changes,thumb_instructions=steps,match=True,scope='Complete originalC540mode2 throughreturn with actualoriginal10830 pending formatter againstfirst64actualnative target updates plus every changed target update in2048capture. Nine pending target halfwords exact. Explicit sprintf %d/strlen/unsigneddivision/remainder helper contracts; total targets and GPU pixels unverified.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/pending-fare-glyph-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
