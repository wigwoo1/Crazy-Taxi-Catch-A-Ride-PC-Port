"""Persistent original customer dispatcher, four actors and shared RNG."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
cp=subprocess.run([str(root/'bin/customer-dispatch-persistent-rom-compare.exe'),'requests'],text=True,capture_output=True,check=True)
rows=[list(map(int,s.split())) for s in cp.stdout.splitlines()];assert len(rows)==512
class EntryProbe(DispatchProbe):
 def external(self,target):
  if target==0x8031a50:
   a=self.reg[5]
   descriptor=self.reg[0];pos=self.reg[1]
   body=descriptor==a+32
   if body:
    assert self.read(descriptor)==0x06054020,hex(self.read(descriptor))
    body_flips[self.read(self.reg[13])]+=1;scale_values.add(self.reg[2])
   self.ordered_requests.append([self.read(descriptor+4) if body else descriptor,signed(self.read(pos)),signed(self.read(pos+4)),signed(self.read(pos+8)),self.reg[2],self.read(self.reg[13]) if body else self.reg[3],self.reg[3] if body else self.read(self.reg[13])])
   assert 0x2005f2c<=a<0x2005f2c+176 and (a-0x2005f2c)%44==0,hex(a)
   self.entry_mask[(a-0x2005f2c)//44]=1
  super().external(target)
p=EntryProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.depth_requests=[];p.sprite_requests=[]
for address,data in allowed_thumb.items():
 if not 0x8022df4<=address<0x80233d8:continue
 h=int.from_bytes(data[:2],'little')
 if h&0xf800==0x4800:
  off=address-0x8000000;at=((off+4)&~3)+(h&255)*4;v=int.from_bytes(r[at:at+4],'little')
  if 0x2000000<=v<0x2040000 or 0x3000000<=v<0x3010000:
   for a in range(v&~3,(v&~3)+32,4):p.write(a,0)
actor=0x2005f2c
for j,(kind,flag,x) in enumerate(zip([0,1,7,0],[1,65,3,1],[0,500,13000,12799])):
 b=bytearray(44);b[0]=flag;b[1]=kind;b[9]=5;b[32:36]=bytes([32,64,5,6]);b[10]=3 if j==3 else 0;b[12]=5 if j==3 else 0
 b[16:20]=x.to_bytes(4,'little');b[28:32]=(0x87ec4f4 if j==3 else 0x87ec47c).to_bytes(4,'little')
 for n,v in enumerate(b):p.write(actor+j*44+n,v,1)
p.write(0x2003b04,17);p.write(0x2003b00,1);p.write(0x2005f2a,1,1)
p.write(0x2007530,0);p.write(0x2001704,0);p.write(0x2007534,256);p.write(0x2007538,0)
descriptor=0x2001000;p.write(0x3006810,descriptor);p.write(descriptor+12,4<<16)
runtime=(root/'assets/driving.ctr').read_bytes()
for base,offset in [(0x202c4f0,4),(0x20304f0,8196)]:
 for i,v in enumerate(struct.unpack_from('<4096h',runtime,offset)):p.write(base+4*i,v>>4)
steps=0;body_flips=[0,0];scale_values=set()
for tick,row in enumerate(rows):
 assert len(row)>=187 and row[0]==tick
 p.write(0x2002650,(((tick//32)*257)&4095)<<12);p.write(0x200752c,[0,0x80000,0xc0000][(tick//64)%3]);p.write(0x2002634,row[1]);p.write(0x2002638,0);p.write(0x200263c,0)
 p.entry_mask=[0]*4;p.ordered_requests=[];p.steps=0;p.reg[14]=0x10020001;p.run(entry=0x8022df4,stop=0x10020000)
 actual=[p.read(0x2003b04),p.read(0x2005f28,1),p.read(0x2005f29,1),p.read(0x2005f2a,1)]+[p.read(actor+j,1) for j in range(176)]
 assert actual==row[2:182],(tick,[(j,a,b) for j,(a,b) in enumerate(zip(actual,row[2:182])) if a!=b][:8])
 assert p.entry_mask==row[182:186],(tick,p.entry_mask,row[182:186])
 assert [len(p.ordered_requests)]+[v for q in p.ordered_requests for v in q]==row[186:],(tick,p.ordered_requests,row[186:])
 steps+=p.steps
assert all(body_flips) and len(scale_values)==3,(body_flips,scale_values)
report={'body_facing_counts':body_flips,'scale_values':sorted(scale_values),'heading_phases':[((j*257)&4095) for j in range(16)],'rom_sha256':hashlib.sha256(r).hexdigest(),'frames':len(rows),'actors':4,'compared_actor_bytes':len(rows)*176,'original_thumb_instructions':steps,'original_rng_calls':p.calls,'render_requests_observed':len(p.sprite_requests),'equal':True,'render_entry_masks_equal':True,'ordered_render_requests_equal':True,'captured_output_state_injections':0,'scope':'Original complete22DF4 throughreturn retainsall4actor records/sharedRNG/phase across512frames. Normalmode unpaused delta1; prescribedtaxiX/16heading phases/three source scales perframe, fixturebootstrap. Ordered31A50 caller requests compared descriptor/XYZ/scale/flip/flags; body descriptor normalized to its bitmap pointer, normal32x64 header06054020 asserted at every original body call. Native actual qualified projection provides scale/flip with controlled camera and actual ROM-derived Q8 sine/cosine; original projection and3201C remain boundaries, signedremainder helpercontract. Nofullgame/traffic/customersearch/pause/currentcityseed/physicalcontroller proof.'}
report['source_sha256']={name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/passenger_scene.c','src/passenger_scene.h','src/passenger_idle.c','src/passenger_render.c','src/scene_viewer.c','tests/customer_dispatch_persistent_rom_compare.c','tools/compare_customer_dispatch_persistent_rom.py','tools/compare_customer_dispatch_rom.py','assets/driving.ctr']}
(root/'reports/customer-persistent-dispatch-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
