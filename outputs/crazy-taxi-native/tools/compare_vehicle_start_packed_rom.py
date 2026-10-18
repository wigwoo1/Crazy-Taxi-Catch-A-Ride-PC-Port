"""Whole14A58 single actor; marker/ground/RNG explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_template_initialize_rom.py').read().split('\ncases=')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/normal-vehicle-startup-cfg.json').read_text())['instructions']})
class StartupProbe(FullTemplateProbe):
 def external(self,t):
  if t==0x08033f80:self.reg[0]=0
  elif t==0x0802aba0:
   assert self.read(self.reg[0])==100 and self.read(self.reg[0]+4)==0x80000000 and self.read(self.reg[0]+8)==200
   if self.hit:
    self.write(self.reg[1]+4,1000);self.write(self.reg[1]+21,17,1);self.write(self.reg[1]+22,29,1);self.write(self.reg[1]+23,33,1)
   self.reg[0]=self.hit
  elif t==0x0801e72c:
   assert self.reg[0]==20 and self.reg[1]==60;self.reg[0]=self.factor
  elif t in [0x08011804,0x0801e784]:self.run(entry=t,stop=self.reg[14]&~1)
  else:super().external(t)
cases=[(seed,settings,demo,hit) for seed in [0,165,255] for settings in [0,31,128,255] for demo in [0,1] for hit in [0,1]]
rom='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(root/'bin/vehicle-start-packed-rom-compare.exe'),rom],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,settings,demo,hit=case;p=StartupProbe((0,0,0,0,0,1,0,0,0,0));a=0x02002634;p.regions[a]=bytearray((seed+i*37)&255 for i in range(0xea0));p.regions[0x020034d4]=bytearray(8)
 p.regions[0x10010000]=bytearray(0x80);p.regions[0x10011000]=bytearray(20);p.regions[0x10012000]=bytearray(16)
 p.scalars.update({0x03006810:0x10010000,0x030053e0:12345,0x02002630:12345,0x02001708:demo});p.write(0x020034d4,1);p.write(0x020034d8,1);p.write(0x02001eb0,settings,1);p.write(0x02001708,demo,1)
 p.write(0x10010010,1,2);p.write(0x10010034,0x10011000);p.write(0x10011010,0x10012000);p.write(0x10012004,100);p.write(0x10012008,200);p.write(0x1001200c,3073,2)
 state=seed or 0x4652414e;state=(state<<1)&MASK
 if state&0x80000000:state^=0x1d872b41
 state=((state&0x7fffffff)^0x06000000)&0x7fffffff;p.factor=20+state%41;p.hit=hit
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x08014a58,stop=0x10020000)
 parts=row.split();actual=bytes(p.regions[a]);native=bytes.fromhex(parts[0]);assert actual==native,(case,[(hex(i),x,y) for i,(x,y) in enumerate(zip(actual,native)) if x!=y][:12])
 assert [p.scalars[0x02002630],signed(p.scalars[0x030053e0]),state]==list(map(int,parts[1:])),case;steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Whole14A58 single actor/human1 with marker string lookup and ground/RNG/division explicit contracts; original11804/11540/1E7F0/1E784 executed. All3744actorbytes and reset/sprite/RNG owners. Synthetic hit/miss retains prior position; not full original scene-ground/startup or live typed binding.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/vehicle-start-packed-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
