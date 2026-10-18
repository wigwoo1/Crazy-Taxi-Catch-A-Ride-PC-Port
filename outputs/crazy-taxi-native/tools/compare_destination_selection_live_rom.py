"""Original DF9C destination selection against actual native marker/material state."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
saved_allowed=allowed_thumb.copy();source=source.replace("'bne':not z,","'bne':not z,'blo':not c,");exec(source);allowed_thumb.update(saved_allowed);IdleProbe.run=MotionProbe.run
cfg=json.loads((root/'reports/destination-selection-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class DestinationProbe(IdleProbe):
 def read(self,a,n=4):
  if a==0x3006810:return self.arm.mem[a]
  return super().read(a,n)
 def external(self,target):
  if target==0x8033f2c:
   assert self.reg[:3]==[0x202b8ec,0,512]
   self.order.append('clear')
   for i in range(512):self.write(0x202b8ec+i,0,1)
  elif target==0x8031860:
   assert self.reg[:4]==[64,64,self.record,self.record+12]
   assert self.read(0x2001768)==self.destination and self.read(0x200176c)==self.record
   assert [self.read(0x202b8ec+i,1) for i in range(512)]==self.prior_cache
   self.order.append('panel')
  else:raise AssertionError(('uncovered destination callback',hex(target)))
rows=[list(map(int,l.split())) for l in (root/'reports/current-destination-selection-states.txt').read_text().splitlines()]
world=0x87556cc;table=struct.unpack_from('<I',r,world-0x8000000+0x38)[0];steps=0;selected=cleared=0
for row in rows:
 assert len(row)==1088
 dest,mode=row[:2];pre=row[2:521];post=row[521:1040];record=row[1040:]
 assert len(pre)==len(post)==519 and len(record)==48
 p=DestinationProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.order=[];p.destination=dest;p.record=table+dest*48 if dest!=-1 else 0;p.prior_cache=pre[7:]
 if dest!=-1:assert list(r[p.record-0x8000000:p.record-0x8000000+48])==record
 p.arm.mem[0x3006810]=world;p.write(0x2001eb0,mode,1);p.write(0x2001768,pre[0]);p.write(0x200176c,table+pre[0]*48 if pre[1] else 0)
 for i,v in enumerate(pre[7:]):p.write(0x202b8ec+i,v,1)
 p.reg[0]=dest&MASK;p.run(entry=0x800df9c,stop=0x10020000)
 assert signed(p.read(0x2001768))==post[0]==dest and p.read(0x200176c)==p.record
 assert post[1]==int(dest!=-1) and post[2]==pre[2]
 ids=[int.from_bytes(bytes(record[40+i*2:42+i*2]),'little') for i in range(4)] if dest!=-1 else [65535]*4
 assert post[3:7]==ids and [p.read(0x202b8ec+i,1) for i in range(512)]==post[7:]
 assert p.order==(['panel','clear'] if dest!=-1 and mode&224!=128 else ['clear'])
 selected+=dest!=-1;cleared+=dest==-1;steps+=p.steps
assert rows and (root/'reports/current-destination-traffic-frames.txt').read_bytes()==(root/'reports/current-full-glyph-traffic-frames.txt').read_bytes()
report=dict(cases=len(rows),selected=selected,cleared=cleared,thumb_instructions=steps,match=True,traffic_frames_byte_identical=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete originalDF9C branch throughreturn versus actualnative selection state/cache, actualROMcity1 destination records; selection globals/record pointer/indices/512cache and panel-before-clear ordering match. Preview renderer31860 request boundary; memset33F2C explicitfillcontract; panelpixels/bootstrap/othercities unverified.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_destination_selection_live_rom.py','reports/destination-selection-cfg.json','reports/current-destination-selection-states.txt','src/material_blink.c','bin/scene-viewer.exe']}
(root/'reports/destination-selection-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
