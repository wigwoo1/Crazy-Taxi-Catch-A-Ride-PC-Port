"""Original14CB6..14D84 single actor suffix, all packed bytes."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_template_initialize_rom.py').read().split('\ncases=')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/normal-vehicle-startup-cfg.json').read_text())['instructions']})
class FinalizeProbe(FullTemplateProbe):
 def external(self,t):
  if t==0x0801e72c:
   assert self.reg[0]==20 and self.reg[1]==60;self.reg[0]=self.factor
  else:super().external(t)
cases=[(seed,base) for seed in [0,1,165,255,0x7fffffff,0xffffffff] for base in [-32768,-1,0,1,8500,32767]]
rows=subprocess.run([str(root/'bin/vehicle-start-finalize-rom-compare.exe')],input=''.join(f'{s} {b}\n' for s,b in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,base=case;p=FinalizeProbe((0,0,0,0,0,1,0,0,0,0));actor=0x02002634;p.regions[actor]=bytearray((seed+i*37)&255 for i in range(0xea0));p.regions[0x020034d4]=bytearray((1).to_bytes(4,'little')+bytes(4))
 p.write(actor+0x532,base,2);state=seed or 0x4652414e;state=(state<<1)&MASK
 if state&0x80000000:state^=0x1d872b41
 state=((state&0x7fffffff)^0x06000000)&0x7fffffff;p.factor=20+state%41
 p.reg[6]=actor;p.reg[8]=0;p.reg[9]=actor+0x390;p.reg[10]=actor+0x764;p.reg[7]=actor+0x358;p.reg[13]=0x10000400
 for off,value in [(0x8c,actor+0x654),(0x84,actor+0x4ac),(0x80,actor+0x6ec),(0x88,actor+0x18),(0x6c,0)]:p.write(p.reg[13]+off,value)
 p.run(entry=0x08014cb6,stop=0x08014d86);parts=row.split();actual=bytes(p.regions[actor]);native=bytes.fromhex(parts[0])
 assert actual==native,(case,[(hex(i),a,b) for i,(a,b) in enumerate(zip(actual,native)) if a!=b][:10]);assert state==int(parts[1]),case;steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Original14CB6..14D84 single actor suffix all3744bytes, RNG1E72C/division32488 explicit contracts; caller prefix/whole14A58 and live host excluded. Camera offsets overwrites verified.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/vehicle-start-finalize-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
