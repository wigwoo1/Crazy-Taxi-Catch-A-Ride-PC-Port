"""Original boarding completion caller with explicit destination/formatting boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_trip_start_live_rom.py').read().split('\nrows=')[0])
for name in ['passenger-completion','fare']:
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/f'reports/{name}-cfg.json').read_text())['instructions']})
class CompletionProbe(TripProbe):
 def read(self,a,n=4):
  if a==0x3006810:return self.arm.mem[a]
  return super().read(a,n)
 def external(self,target):
  if target==0x800df9c:self.order.append(('destination',self.reg[0]))
  elif target==0x800cb84:self.order.append(('trip',self.reg[0]));self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x800c540:
   self.order.append(('fare',self.reg[0],self.reg[1]));saved=self.reg[:]
   self.run(entry=target,stop=0x800c7de)
   # Explicit callee-save ABI boundary for the unexecuted formatting/glyph suffix.
   for k in list(range(4,12))+[13,14]:self.reg[k]=saved[k]
  else:super().external(target)
formatter_cfg=json.loads((root/'reports/fare-formatter-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in formatter_cfg['instructions']})
MoneyProbe=TripProbe
exec((root/'tools/compare_pending_fare_glyph_live_rom.py').read_text().split('class GlyphProbe',1)[1].split('\nrows=')[0].join(['class GlyphProbe','']))
class FullCompletionProbe(CompletionProbe,GlyphProbe):
 def external(self,target):
  if target==0x800c540:self.order.append(('fare',self.reg[0],self.reg[1]));self.run(entry=target,stop=self.reg[14]&~1)
  elif target in [0x8010830,0x801e420,0x801e7d4,0x80325f0,0x8032668]:GlyphProbe.external(self,target)
  else:CompletionProbe.external(self,target)
saved_allowed=allowed_thumb.copy();source=source.replace("'bne':not z,","'bne':not z,'blo':not c,");exec(source);allowed_thumb.update(saved_allowed);FullCompletionProbe.run=MotionProbe.run
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/destination-selection-cfg.json').read_text())['instructions']})
class ConnectedCompletionProbe(FullCompletionProbe):
 def external(self,target):
  if target==0x800df9c:
   self.order.append(('destination',self.reg[0]));self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8033f2c:
   assert self.reg[:3]==[0x202b8ec,0,512];self.panel_order.append('clear')
   for i in range(512):self.write(0x202b8ec+i,0,1)
  elif target==0x8031860:
   assert self.reg[:4]==[64,64,self.record,self.record+12]
   assert self.read(0x2001768)==self.destination and self.read(0x200176c)==self.record
   assert [self.read(0x202b8ec+i,1) for i in range(512)]==self.prior_cache
   self.panel_order.append('panel')
  else:super().external(target)
selection_rows=[list(map(int,l.split())) for l in (root/'reports/current-destination-selection-states.txt').read_text().splitlines()]
table=struct.unpack_from('<I',r,0x7556cc+0x38)[0]
glyphs=[list(map(int,l.split())) for l in (root/'reports/current-full-cash-glyph-states.txt').read_text().splitlines()]
money=[list(map(int,l.split())) for l in (root/'reports/current-passenger-money-states.txt').read_text().splitlines()]
trip_rows=[list(map(int,l.split())) for l in (root/'reports/current-shared-voice-fare.txt').read_text().splitlines()]
pickup_rows=[list(map(int,l.split())) for l in (root/'reports/current-shared-voice-pickup.txt').read_text().splitlines() if l.split()[1]=='1']
tmap=[(0x2001688,4),(0x200753c,4),(0x20016ac,4),(0x20016b0,4),(0x20016e0,4),(0x2005310,2),(0x2005312,2),(0x2005314,2),(0x2001694,1),(0x2001695,1),(0x20025d6,1)]
fmap=[(0x2001698,4),(0x200169c,4),(0x20016a0,4),(0x20016a4,4),(0x2001693,1),(0x2001691,1)]
steps=0
assert len(money)==len(trip_rows)==len(pickup_rows)==2
for m,t,k in zip(money,trip_rows,pickup_rows):
 index,length=m[:2];assert index==t[0]==k[14]
 p=ConnectedCompletionProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.order=[];p.dma=[];actor=0x2005f2c+index*44
 for i,v in enumerate(m[14:58]):p.write(actor+i,v,1)
 for (a,n),v in zip(tmap,t[9:20]):p.write(a,0 if a==0x2001688 and v<0 else 0x2005f2c+v*44 if a==0x2001688 else v,n)
 for (a,n),v in zip(fmap,m[2:8]):p.write(a,v,n)
 p.write(0x2001704,k[13]);p.write(0x2007530,actor);p.arm.mem[0x3006810]=0x87556cc
 p.write(0x2001eb0,t[4],1);p.write(0x2001eb3,t[5],1);p.write(0x20016a8,t[2]);p.write(0x2003b04,t[6])
 matches=[g for g in glyphs if g[:6]==m[8:14] and g[16:25]!=g[35:]];assert len(matches)==1;g=matches[0]
 for i,v in enumerate(g[16:25]):p.write(0x2005374+i*2,v,2)
 selections=[d for d in selection_rows if d[0]==k[12]];assert len(selections)==1;d=selections[0];prior=d[2:521];selected_post=d[521:1040]
 p.destination=k[12];p.record=table+k[12]*48;p.prior_cache=prior[7:];p.panel_order=[]
 assert list(r[p.record-0x8000000:p.record-0x8000000+48])==d[1040:]
 p.write(0x2001768,prior[0]);p.write(0x200176c,table+prior[0]*48 if prior[1] else 0)
 for i,v in enumerate(prior[7:]):p.write(0x202b8ec+i,v,1)
 p.run(entry=0x800b098,stop={0x800b10c,0x800b2d8})
 assert p.panel_order==['panel','clear']
 assert p.read(0x2001768)==selected_post[0] and p.read(0x200176c)==p.record
 assert [p.read(0x202b8ec+i,1) for i in range(512)]==selected_post[7:]
 assert [p.read(0x2005374+i*2,2) for i in range(9)]==g[35:]
 assert p.order==[('destination',k[12]),('trip',index),('fare',1,index)],p.order
 assert p.read(0x2001704)==0 and [p.read(actor+i,1) for i in range(44)]==m[58:]
 got=[((p.read(a,n)-0x2005f2c)//44 if a==0x2001688 else signed(p.read(a,n)) if n==4 else p.read(a,n)) for a,n in tmap]
 assert got==t[20:] and p.read(0x2003b04)==t[7] and p.calls==1,(got,t[20:])
 assert [signed(p.read(a,n)) if n==4 else p.read(a,n) for a,n in fmap]==m[8:14]
 steps+=p.steps
report=dict(cases=2,thumb_instructions=steps,match=True,scope='Original B098 completion caller, actualROMcity1 source destination lookup, nested completeCB84/originalRNG and completeC540mode1 with actual10830 formatter. Actual customer44bytes,pending clear,trip11fields,fare6fields,RNG and ordered requests match native captures. Actual DF9C throughreturn joined to real native priorselection/cache fixtures; originalrecordpointer/index/512cache and panel-before-clear order exact. Preview31860 remains requestboundary andmemset is fillcontract. Complete C540mode1 throughreturn with ninependingtargets checked; no explicit callee-save shortcut is used. sprintf/strlen/division/remainder helper contracts remain; no GPU/bootstrap/fullframe claim.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_passenger_completion_connected_rom.py','reports/passenger-completion-cfg.json','reports/current-passenger-money-states.txt','reports/current-shared-voice-fare.txt','reports/current-shared-voice-pickup.txt']}
(root/'reports/passenger-completion-connected-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
