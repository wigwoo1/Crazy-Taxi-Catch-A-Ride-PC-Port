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
  raise AssertionError(('unqualified countdown dependency',hex(target)))
schema=json.loads((root/'reports/ordinary-countdown-capture-schema.json').read_text())['fields'];rows=[dict(zip(schema,map(int,l.split()))) for l in (root/'reports/startup-countdown-states.txt').read_text().splitlines()]
mapping={'phase':(0x20016b4,2),'hold':(0x20016b6,2),'offset':(0x20016b8,2),'scale':(0x20016ba,2),'rotation':(0x2001670,2),'axis_x':(0x2001674,4),'axis_y':(0x2001678,4),'axis_z':(0x200167c,4),'active':(0x2001696,1)}
p=StartProbe((0,0,0,0,0,1,0,0,0,0));p.scalars.update({0x200177b:0,0x2001ea8:0,0x2003b00:1,0x30053c0:1,0x2007542:240})
for k,(a,n) in mapping.items():p.write(a,rows[0]['input_'+k],n)
p.write(0x2001eb0,0,1);cases=0;instructions=0
for d in rows:
 if d['input_phase']!=-1:break
 for k,(a,n) in mapping.items():assert p.read(a,n)==d['input_'+k]&((1<<(8*n))-1),(d['tick'],'persistent before',k)
 p.scalars[0x200177b]=d['fade'];p.scalars[0x2001ea8]=d['exiting'];p.scalars[0x2003b00]=d['delta'];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.steps=0
 try:p.run(entry=0x800ba04,stop=0x10020000)
 except ReturnBoundary:pass
 for k,(a,n) in mapping.items():assert p.read(a,n)==d['output_'+k]&((1<<(8*n))-1),(d['tick'],'source after',k,p.read(a,n),d['output_'+k])
 assert d['events']==0;cases+=1;instructions+=p.steps
report=dict(status='PASS',cases=cases,thumb_instructions=instructions,scope='Original BA04 opening phase through phase0, persistent actual native axes/rotation/phase and all9countdown fields including fade gates. Stops at original C524 unwind boundary; remaining Ready/Set/Go/art/audio phases not compared.',rom_sha256=hashlib.sha256(ns['r']).hexdigest())
(root/'reports/startup-countdown-opening-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
