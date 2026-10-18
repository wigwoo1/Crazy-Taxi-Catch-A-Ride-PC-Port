"""Original156D8..157A4 plus actual2DC78 ordinary half-center rotation."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
for start,end in [(0x156d8,0x157a6),(0x2dc78,0x2dd10)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class HalfProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t==0x8032488:
   assert signed(self.reg[1])!=0
   self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&0xffffffff
  elif t==0x802dc78:self.run(entry=t,stop=self.reg[14]&~1)
  else:raise AssertionError(('helper',hex(t)))
cases=[(x,y,192,distance,base,roll,sine,cosine) for x,y in [(120,116),(0,0),(-200,500),(2147483647,-2147483648)] for distance,base in [(3072*4096,-512),(1536*4096,0),(1536*4096-1,0),(-1536*4096-1,0)] for roll,sine,cosine in [(0,0,256),(1024,-256,0),(2048,0,-256),(65535,1,256)]]
rows=subprocess.run([str(root/'bin/taxi-half-plan-rom-compare.exe')],input=''.join(' '.join(map(str,c[:5]+c[6:]))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 x,y,focal,distance,base,roll,sine,cosine=case;p=HalfProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};angle=(-roll)&4095;p.scalars={0x2001534:0,0x2005c58:0,0x20304f0+angle*4:cosine,0x202c4f0+angle*4:sine}
 p.reg[13]=0x10000400;p.reg[4]=0x10002000;p.reg[7]=0x10001000;p.reg[6]=0x10003000
 for a,v,n in [(0x10002028,focal,4),(0x10001770,distance,4),(0x10001680,base,4),(0x1000101a,roll,2),(0x100004ec,x,4),(0x100004f0,y,4),(0x10003008,3537,4)]:p.write(a,v,n)
 p.run(entry=0x80156d8,stop=0x80157a6)
 expected=[signed(p.reg[9])]+[signed(p.read(a)) for a in [0x10000420,0x10000424,0x1000042c,0x10000430]]
 assert list(map(int,row.split()))==expected,(case,row,expected)
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Original156D8..157A4 normal mode includingactual2DC78 Q8rotation, signed-distance ASR12/base/wrap/256to257 and halfcenters. Signed division32488 explicit scalar contract; resolved trig table fixtures. Projected center supplied, no originalcamera/full1556C/facing proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/taxi_half_plan.c',root/'src/taxi_half_plan.h',root/'tests/taxi-half-plan-rom-compare.c',root/'bin/taxi-half-plan-rom-compare.exe']}}
(root/'reports/taxi-half-plan-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
