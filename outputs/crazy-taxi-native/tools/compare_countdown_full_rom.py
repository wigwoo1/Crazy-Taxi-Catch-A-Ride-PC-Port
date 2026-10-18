"""Original BA04 opening phase on actual native countdown inputs, offline only."""
import pathlib,json,re,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for line in (root/'reports/start-sequence-reachable-source.txt').read_text().splitlines():
 m=re.match(r'([0-9a-f]{8}):',line)
 if m:
  address=int(m[1],16);i=next(ns['md'].disasm(ns['r'][address-0x8000000:address-0x8000000+4],address,count=1));ns['allowed_thumb'][address]=bytes(i.bytes)
class ReturnBoundary(Exception):pass
class StartProbe(ns['InitProbe']):
 def external(self,target):
  if target==0x800c524:raise ReturnBoundary
  if target==0x8027fb8:
   self.requests.append(dict(kind='art',args=list(self.reg[:4])));self.events|=8;self.reg[0]=0;return
  if target==0x80267c8:
   table=0x7ec658;keys={int.from_bytes(ns['r'][table+off:table+off+4],'little'):bit for off,bit in [(0xcc,1),(0xd0,2),(0xd4,4)]}
   assert self.reg[0] in keys,('unknown countdown sound',hex(self.reg[0]))
   self.requests.append(dict(kind='sound',source=hex(self.reg[0]),event=keys[self.reg[0]]));self.events|=keys[self.reg[0]];return
  raise AssertionError(('unqualified countdown dependency',hex(target)))
for begin,end in [(0xbf64,0xbf70),(0xc524,0xc532)]:
 for i in ns['md'].disasm(ns['r'][begin:end],0x8000000+begin):ns['allowed_thumb'][i.address]=bytes(i.bytes)
schema=json.loads((root/'reports/ordinary-countdown-capture-schema.json').read_text())['fields'];rows=[dict(zip(schema,map(int,l.split()))) for l in (root/'reports/startup-countdown-states.txt').read_text().splitlines()]
mapping={'phase':(0x20016b4,2),'hold':(0x20016b6,2),'offset':(0x20016b8,2),'scale':(0x20016ba,2),'rotation':(0x2001670,2),'axis_x':(0x2001674,4),'axis_y':(0x2001678,4),'axis_z':(0x200167c,4),'active':(0x2001696,1)}
p=StartProbe((0,0,0,0,0,1,0,0,0,0));p.scalars.update({0x200177b:0,0x2001ea8:0,0x2003b00:1,0x30053c0:1,0x2007542:240,0x2007602:0,0x200388d:0,0x40000d4:0,0x40000d8:0,0x40000dc:0})
for k,(a,n) in mapping.items():p.write(a,rows[0]['input_'+k],n)
p.regions[0x2001790]=bytearray(256)
p.write(0x2001eb0,0,1);cases=0;instructions=0;requests=[]
for d in rows:
 for k,(a,n) in mapping.items():assert p.read(a,n)==d['input_'+k]&((1<<(8*n))-1),(d['tick'],'persistent before',k)
 p.events=0;p.requests=[];p.scalars[0x2007602]=d['audio'];p.scalars[0x200177b]=d['fade'];p.scalars[0x2001ea8]=d['exiting'];p.scalars[0x2003b00]=d['delta'];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.steps=0
 try:p.run(entry=0x800ba04,stop=0x10020000)
 except ReturnBoundary:pass
 for k,(a,n) in mapping.items():assert p.read(a,n)==d['output_'+k]&((1<<(8*n))-1),(d['tick'],'source after',k,p.read(a,n),d['output_'+k])
 if d['input_active'] and not d['output_active']:p.events|=16
 assert p.events==d['events'],(d['tick'],'events',p.events,d['events'])
 requests.extend(dict(tick=d['tick'],**x) for x in p.requests);cases+=1;instructions+=p.steps
report=dict(status='PASS',cases=cases,requests=requests,thumb_instructions=instructions,scope='Original BA04 ordinary opening/Ready/Set/Go phase states across all348native updates. All9fields andeventmasks exact, actor-global state retained. C524 unwind boundary and allocator/audio requests explicit contracts, artwork selector0 fixture, DMA registers retained but hardware transfer not executed; no framebuffer/fullbootstrap proof.',rom_sha256=hashlib.sha256(ns['r']).hexdigest())
report['hashes']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/startup-countdown-states.txt',root/'bin/scene-viewer-countdown-capture-check.exe',root/'src/game_start.c']}
(root/'reports/startup-countdown-full-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)

