import sys
"""Package-backed native graphics vs ordinary actual assets then15910..15A10 through actual copy/affine/palette."""
exec(open('outputs/crazy-taxi-native/tools/compare_ordinary_assets_rom.py').read().split('\nnative=')[0])
asset_steps=p.steps;initial_regions={a:bytes(b) for a,b in p.regions.items()};initial_scalars=p.scalars.copy()
bases=[p.read(0x2007e3c+i*4) for i in range(11)];palette_bases=[p.read(0x2007f54+i*4) for i in range(11)]
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_scaled_rom.py').read().split('\nassets=')[0])
affine_allowed=allowed_thumb.copy()
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
allowed_thumb.update(affine_allowed)
for start,end in [(0x15910,0x159d2),(0x159f0,0x15a12)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
saved_allowed=allowed_thumb.copy()
exec(source.replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')"))
allowed_thumb.update(saved_allowed)
class RenderProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x8027fb8,0x8028658,0x8028288]:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8032454:
   assert self.reg[2]==0x80029fc
   self.reg[0]=division_namespace['original'](self.reg[0],self.reg[1])
  else:raise AssertionError(('helper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc and v&0x80000000:
   source=self.read(0x40000d4)-0x8000000;dest=self.read(0x40000d8);length=(v&65535)*4
   assert 0<=source<=len(r)-length
   for start,data in self.regions.items():
    if start<=dest and dest+length<=start+len(data):data[dest-start:dest-start+length]=r[source:source+length];break
   else:raise AssertionError(('DMA range',hex(dest),length))
render_init_write=RenderProbe.__mro__[1].write
saved_render_allowed=allowed_thumb.copy()
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_setup_rom.py').read().split('\ncases=')[0])
allowed_thumb.update(saved_render_allowed)
combined_allowed=allowed_thumb.copy()
source=source.replace("'b':True,'bne'", "'bhs':c,'blo':not c,'b':True,'bne'").replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')")
exec(source);allowed_thumb.update(combined_allowed)
class CombinedProbe(RenderProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x80280c4,0x8028e88]:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8032454 and self.reg[2]!=0x80029fc:
   src,dst=self.reg[:2];assert (src,dst) in [(0x20081ec,0x7000000),(0x2009508,0x20081ec)]
   data=bytes(self.read(src+i,1) for i in range(1024))
   for i,value in enumerate(data):self.write(dst+i,value,1)
  else:super().external(t)
 def write(self,a,v,size=4):
  render_init_write(self,a,v,size)
  if a==0x40000dc and v&0x80000000:
   src,dst=self.read(0x40000d4),self.read(0x40000d8);length=(v&65535)*4
   data=bytes(self.read(src+i,1) for i in range(length))
   for i,value in enumerate(data):self.write(dst+i,value,1)
projection_namespace={}
exec(open('outputs/crazy-taxi-native/tools/compare_effect_projection_random_rom.py').read().split('\ncases=')[0],projection_namespace)
class NestedProjection(projection_namespace['TypeProbe']):
 def external(self,target):
  if target==0x8028658:
   graphics.reg=self.reg.copy();graphics.projection=self
   graphics.run(entry=target,stop=self.reg[14]&~1)
   self.reg=graphics.reg.copy();graphics.projection=None
  elif target==0x801e72c:
   expected=[(0,65535),(-5,5),(4,8)][self.rng_calls%3];assert tuple(map(projection_namespace['signed'],self.reg[:2]))==expected
   self.rng_calls+=1;self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
class SharedGraphics(CombinedProbe):
 projection=None
 def read(self,a,n=4):
  if self.projection is not None and 0x10008000<=a<0x10010000:return self.projection.read(a,n)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.projection is not None and 0x10008000<=a<0x10010000:self.projection.write(a,v,n)
  else:super().write(a,v,n)
live_capture=Path(sys.argv[1]) if len(sys.argv)>1 else root/'reports/live-effect-source-inputs.json';live=json.loads(live_capture.read_text())
output_name=live_capture.stem+'-publish' if len(sys.argv)>1 else 'live-effect-publish'
record_fixture=root/('reports/'+output_name+'-records.bin');record_fixture.write_bytes(b''.join(bytes(record) for record in live['records']))
cx,cy,cz,cp,cyaw,cr=live['camera']
cases=[(0xffffffff,frame,driver,1,0,109,0,0,256,0,0,0,live['disabled'],0,cx,cy,cz,cp,cyaw,cr,live['phase'],live['random_state']) for frame in (0,63) for driver in (0,3)]
capture=root/('reports/'+output_name+'-native.bin')
subprocess.run([str(root/'bin/live-effect-publish-rom-compare.exe'),str(capture),str(record_fixture)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();stride=24+768+32768+2560+512+7+4096+384+1024+4;assert len(data)==stride*len(cases),(len(data),stride,len(cases));rng_call_counts=[];published_counts=[];steps=projection_steps=projection_arm_steps=0
for index,case in enumerate(cases):
 previous,frame,driver,immediate,override,scale,roll,sine,cosine,count,effect_x,effect_y,effect_palette,effect_z,camera_x,camera_y,camera_z,camera_pitch,camera_yaw,camera_roll,phase,initial_rng=case
 q=SharedGraphics((0,0,0,0,0,1,0,0,0,0));q.regions={a:bytearray(b) for a,b in initial_regions.items()};q.scalars=initial_scalars.copy();q.scalars[0x3004c40]=0x10030001;q.scalars.update({0x30053e0:0,0x3006940:0,0x20304f0+((-roll)&4095)*4:0,0x202c4f0+((-roll)&4095)*4:0})
 for start,length in [(0x200806c,384),(0x20081ec,4096)]:
  for a in range(start,start+length,4):q.write(a,0)
 for b in range(3):
  for i in range(128):
   for j,v in enumerate([0x2a1,0xf1,0,0] if b==0 else [0x20a1,0xf1,0x200,0]):q.write(0x20081ec+b*1024+i*8+j*2,v,2)
 for a in range(0x20091ec,0x20094ec,4):q.write(a,0xa5a5a5a5)
 for a,v,n in [(0x20094ec,count,2),(0x30053e0,previous,4),(0x2001708,override,1),(0x2007e31,0,1),(0x2007e2c,0,1),(0x2007e2d,0,1),(0x2007e2e,0,1),(0x2007e2f,0,1),(0x2007e32,0,1),(0x2007e33,0,1),(0x2007e34,0,1),(0x2001788,0,2),(0x2007542,240,2),(0x2007544,160,2),(0x3006940,0x80029fc,4),(0x20304f0+((-roll)&4095)*4,cosine,4),(0x202c4f0+((-roll)&4095)*4,sine,4)]:q.write(a,v,n)
 q.reg[13]=0x10000400;q.reg[7]=0x10001000;q.reg[8]=0;q.reg[9]=scale
 for a,v,n in [(0x1000101a,roll,2),(0x100004f4,0x10000810,4),(0x10000810,frame,4),(0x100004fc,0x10000800,4),(0x100004e4,immediate,4),(0x100004e8,driver,4),(0x40000d4,0x11111111,4),(0x40000d8,0x22222222,4),(0x40000dc,0x33333333,4)]:q.write(a,v,n)
 for a,pos in [(0x10000420,[106,102,3537]),(0x10000800,[133,102,3537])]:
  for j,v in enumerate(pos):q.write(a+j*4,v)
 q.run(entry=0x8015910,stop=0x8015a12)
 graphics_dma=[q.read(a) for a in (0x40000d4,0x40000d8,0x40000dc)]
 graphics=q;random_state=initial_rng
 projected=NestedProjection((0,0,0,0,0,1,0,0,0,0));projected.requests=[];projected.rng_calls=0
 for off,value,n in [(0x498,camera_x,4),(0x49c,camera_y,4),(0x4a0,camera_z,4),(0x4a4,camera_pitch,2),(0x4a6,camera_yaw,2),(0x4a8,camera_roll,2),(0x544,phase,4)]:projected.write(0x2002634+off,value,n)
 projected.arm.mem.update({0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x2001708:0,0x2005f20:effect_palette,0x2003b04:random_state})
 for off,value in [(0x18,live['center'][0]),(0x1c,live['center'][1]),(0x28,live['focal'])]:projected.arm.mem[0x3006810+off]=value
 for i,record in enumerate(live['records']):
  for j in range(0,72,4):projected.write(0x2005c60+i*72+j,int.from_bytes(bytes(record[j:j+4]),'little'))
 projected.run(entry=0x8020a20,stop=0x10020000);projection_steps+=projected.steps;projection_arm_steps+=projected.arm.steps;random_state=projected.arm.mem[0x2003b04]
 assert random_state==live['post_random_state'];rng_call_counts.append(projected.rng_calls)
 for i in range(1024):q.write(0x2009508+i,q.read(0x20081ec+i,1),1)
 for i in range(384):q.write(0x2009908+i,0,1)
 q.reg[13]=0x1000f000;q.reg[14]=0x10020001;q.write(0x3004c40,0x10030001)
 q.regions[0x7000000]=bytearray(1024)
 q.run(entry=0x8027cd0,stop=0x10020000);published_counts.append(q.read(0x2007e30,1))
 expected=struct.pack('<6I',q.read(0x30053e0),q.read(0x20094ec,2),graphics_dma[0],graphics_dma[1],graphics_dma[2],q.read(0x2007e30,1))
 expected+=bytes(q.read(0x20091ec+i,1) for i in range(768))
 expected+=bytes(q.regions[0x6000000][0x14000:0x1c000])+bytes(q.regions[0x2000000][0x1790:0x1790+q.read(0x2001780,2)*20])+bytes(2560-q.read(0x2001780,2)*20)+bytes(q.regions[0x5000000][512:1024])
 expected+=bytes(q.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d,0x2007e31])
 expected+=bytes(q.regions[0x2000000][0x81ec:0x91ec])+bytes(q.regions[0x2000000][0x806c:0x81ec])+bytes(q.regions[0x7000000])+struct.pack('<I',random_state)
 actual=data[index*stride:(index+1)*stride];assert actual==expected,(case,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b));steps+=q.steps
report={'status':'PASS','cases':len(cases),'published_object_counts':published_counts,'RNG_calls':rng_call_counts,'capture_ticks':live['ticks'],'projected_requests':len(live['requests']),'Thumb_instructions':steps,'projection_Thumb_instructions':projection_steps,'projection_ARM_instructions':projection_arm_steps,'asset_Thumb_instructions':asset_steps,'ARM_instructions':division_namespace['steps'],'scope':'Exact captured native simulation boundary camera/phase/all9 pool records/preRNG replay through original20A20 nested28658 then27CD0 publication, native world-owner pipeline byteexact. Four controlled preceding taxi driver0/3 frame0/63 cases; taxi positions/scale controlled, capture not original full-game state. Exact allocatedgraphics/palette/queues/matrices/output/reset/finalRNG; original RNG consumption count recorded. Original ARM camera and source producers; division contracts. Not original gameplay/fullframe or GPU pixel proof.','hashes':{str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [Path(__file__),live_capture,record_fixture,root/'src/taxi_render_owned.c',root/'src/effect_render_owned.c',root/'src/effect_sprite.c',root/'src/impact.c',root/'src/random.c',root/'tools/compare_effect_projection_random_rom.py',root/'src/ordinary_graphics_owned.c',root/'src/startup_obj_transport.c',root/'src/taxi_graphics_package.c',root/'assets/taxis/graphics.ctg',root/'tests/live-effect-publish-rom-compare.c',root/'bin/live-effect-publish-rom-compare.exe',capture]}}
(root/('reports/'+output_name+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps,division_namespace['steps'])
