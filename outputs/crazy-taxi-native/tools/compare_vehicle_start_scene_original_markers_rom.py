"""Actual extracted city spawn/native ground feeds whole original startup."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_start_packed_rom.py').read().split('\ncases=')[0])
wn={}
exec((root/"tools/compare_live_ordinary_world_rom.py").read_text().split("import sys")[0],wn)
ground_calls=0;ground_steps=0;marker_comparisons=[]
class SceneProbe(StartupProbe):
 def external(self,t):
  if t==0x08033f80:
   def string_at(address):
    value=bytearray()
    for i in range(256):
     c=self.read(address+i,1)
     if not c:return bytes(value)
     value.append(c)
    raise AssertionError(('unbounded marker string',hex(address)))
   left=string_at(self.reg[0]);right=string_at(self.reg[1]);result=(left>right)-(left<right)
   marker_comparisons.append(dict(city=hex(self.city_address),left=left.decode('ascii'),right=right.decode('ascii'),result=result))
   self.reg[0]=result&MASK
  elif t==0x0802aba0:
   x,z,height,heading,red,green,blue,found=self.point
   assert self.read(self.reg[0])==x&MASK and self.read(self.reg[0]+4)==0x80000000 and self.read(self.reg[0]+8)==z&MASK
   global ground_calls,ground_steps
   g=wn['WorldProbe']((0,0,0,0,0,0,0,1));point=g.base+0x900;hit=g.base+0x940
   for off,value in [(0,x),(4,0x80000000),(8,z)]:g.write(point+off,value)
   g.arm.mem[0x3006810]=self.city_address;g.reg[:2]=[point,hit];g.reg[14]=0x10020001;g.run(entry=0x802aba0,stop=0x10020000)
   assert g.reg[0]==found,('original ground hit',self.city_address,g.reg[0],found)
   if found:
    got=[g.read(hit+4),g.read(hit+21,1),g.read(hit+22,1),g.read(hit+23,1)]
    assert got==[height&MASK,red,green,blue],('source ground spawn',got,self.point)
    for i in range(24):self.write(self.reg[1]+i,g.read(hit+i,1),1)
   self.reg[0]=g.reg[0];ground_calls+=1;ground_steps+=g.steps
  else:super().external(t)
steps=0;total=0;points=[]
for city in range(2):
 cases=[(city,seed,settings,demo) for seed in [0,165,255] for settings in [0,128] for demo in [0,1]]
 rows=subprocess.run([str(root/'bin/vehicle-start-scene-rom-compare.exe'),str(root/'assets/driving.ctr'),str(root/f'assets/city{city+1}.ctw')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases)
 for case,row in zip(cases,rows):
  city,seed,settings,demo=case;parts=row.split();point=list(map(int,parts[:8]));x,z,height,heading,red,green,blue,found=point;p=SceneProbe((0,0,0,0,0,1,0,0,0,0));p.point=point;p.city_address=[0x087556cc,0x08755734][city];a=0x02002634;p.regions[a]=bytearray((seed+i*37)&255 for i in range(0xea0));p.regions[0x020034d4]=bytearray(8)
  p.scalars.update({0x03006810:p.city_address,0x030053e0:12345,0x02002630:12345});p.write(0x020034d4,1);p.write(0x020034d8,1);p.write(0x02001eb0,settings,1);p.write(0x02001708,demo,1)
  state=seed or 0x4652414e;state=(state<<1)&MASK
  if state&0x80000000:state^=0x1d872b41
  state=((state&0x7fffffff)^0x06000000)&0x7fffffff;p.factor=20+state%41
  p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x08014a58,stop=0x10020000)
  assert bytes(p.regions[a])==bytes.fromhex(parts[8]),case;assert [p.scalars[0x02002630],signed(p.scalars[0x030053e0]),state]==list(map(int,parts[9:12])),case;
  offsets=[(0,4,1),(4,4,1),(8,4,1),(0x1c,4,1),(0x20,4,1),(0x544,4,1),(0x36c,4,1),(0x374,1,0),(0x375,1,0),(0x376,1,0),(0xe60,2,0),(0x754,4,1),(0x758,4,1),(0x730,2,1),(0x732,2,1),(0x378,4,0),(0x37c,4,0),(0x714,4,0),(0x72c,4,0),(0x720,4,1),(0xe84,4,1)]
  values=[]
  for off,n,is_signed in offsets:
   v=p.read(a+off,n)
   if is_signed and v&(1<<(n*8-1)):v-=1<<(n*8)
   values.append(v)
  assert values==list(map(int,parts[12:33])),('typed world projection',case,values,parts[12:33]);steps+=p.steps;total+=1
  # This independent source-offset list checks canonical driver fields.
  assert parts[33]=='D',parts
  driver_offsets=[(0x4ac,1,0),(0x4b4,4,1),(0x4dc,1,1),(0x4dd,1,1),(0x4de,2,1),(0x530,2,1),(0x532,2,1),(0x538,2,1),(0x544,4,1),(0x568,4,1),(0x5b8,4,1),(0x5cc,4,1),(0x75c,4,1),(0x540,4,1),(0x550,4,1),(0x5bc,4,1),(0x5d0,4,1),(0x57a,2,1),(0x57c,2,1),(0x564,4,1),(0x7c8,4,1),(0x570,4,1),(0x574,4,1),(0x358,1,0),(0x28,1,0),(0x744,4,1),(0x7b0,4,1),(0x7b8,4,1)]
  dv=[]
  for off,n,sgn in driver_offsets:
   v=p.read(a+off,n)
   if sgn and v&(1<<(n*8-1)):v-=1<<(n*8)
   dv.append(v)
  dv.extend([p.scalars[0x02002630]]*3);assert dv==list(map(int,parts[34:])),('driver fields',case,dv,parts[34:])
 points.append({'city':city+1,'native_ground_spawn':point})
report={'status':'PASS','cases':total,'actual_thumb_instructions':steps,'marker_comparisons':marker_comparisons,'original_ground_calls':ground_calls,'original_ground_thumb_instructions':ground_steps,'points':points,'scope':'Native real city CTW1 ground and CTR1 marker/template binding feeds whole14A58 comparison, all3744bytes/reset/sprite/RNG owners. Original2ABA0 ground query executed using actual ROM city descriptor/geometry and containment callee-save/arithmetic contracts; actual ROM descriptor marker table and marker names read by original14A58; bounded unsigned string comparison/RNG/division remain explicit contracts; 21 canonical world and31 driver/context fields compared directly to source; not full scene/bootstrap/continuous caller or GBA framebuffer proof.','rom_sha256':hashlib.sha256(r).hexdigest()}
report['hashes']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'tools/compare_live_ordinary_world_rom.py',root/'tools/compare_vehicle_start_packed_rom.py',root/'bin/vehicle-start-scene-rom-compare.exe',root/'assets/driving.ctr',root/'assets/city1.ctw',root/'assets/city2.ctw',root/'src/vehicle_start_scene.c',root/'src/vehicle_start_packed.c']}
(root/'reports/vehicle-start-scene-original-markers-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original ROM marker/ground initialization:',total,'cases;',len(marker_comparisons),'actual string comparisons;',ground_calls,'original ground queries')
