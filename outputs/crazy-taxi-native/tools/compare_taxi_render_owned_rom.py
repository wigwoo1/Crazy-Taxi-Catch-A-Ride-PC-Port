"""Ordinary actual assets then15910..15A10 through actual copy/affine/palette."""
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
cases=[(previous,frame,driver,immediate,override,109,0,0,256,count) for driver in range(4) for frame in [0,63] for previous in [frame,0xffffffff] for immediate in [0,1] for override in [0,1] for count in [0,63,64]]
cases += [(0xffffffff,17,2,1,0,scale,roll,sine,cosine,0) for scale in [0,256,512,65535] for roll,sine,cosine in [(1024,-256,0),(2048,0,-256),(65535,1,256)]]
capture=root/'reports/taxi-render-owned-native.bin'
subprocess.run([str(root/'bin/taxi-render-owned-rom-compare.exe'),str(capture)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();stride=20+768+32768+2560+512+7+4096+384;assert len(data)==stride*len(cases),(len(data),stride,len(cases));steps=0
for index,case in enumerate(cases):
 previous,frame,driver,immediate,override,scale,roll,sine,cosine,count=case
 q=RenderProbe((0,0,0,0,0,1,0,0,0,0));q.regions={a:bytearray(b) for a,b in initial_regions.items()};q.scalars=initial_scalars.copy();q.scalars.update({0x30053e0:0,0x3006940:0,0x20304f0+((-roll)&4095)*4:0,0x202c4f0+((-roll)&4095)*4:0})
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
 expected=struct.pack('<5I',q.read(0x30053e0),q.read(0x20094ec,2),q.read(0x40000d4),q.read(0x40000d8),q.read(0x40000dc))
 expected+=bytes(q.read(0x20091ec+i,1) for i in range(768))
 expected+=bytes(q.regions[0x6000000][0x14000:0x1c000])+bytes(q.regions[0x2000000][0x1790:0x1790+q.read(0x2001780,2)*20])+bytes(2560-q.read(0x2001780,2)*20)+bytes(q.regions[0x5000000][512:1024])
 expected+=bytes(q.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d,0x2007e31])
 expected+=bytes(q.regions[0x2000000][0x81ec:0x91ec])+bytes(q.regions[0x2000000][0x806c:0x81ec])
 actual=data[index*stride:(index+1)*stride];assert actual==expected,(case,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b));steps+=q.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'asset_Thumb_instructions':asset_steps,'ARM_instructions':division_namespace['steps'],'scope':'Actual ordinary27994 asset initialization then15910..15A10 full normal taxi graphics tail via actual27FB8/28658/28288/palette, exact cache/DMA/768copies/32768objectbytes/active palette records (unused host slots zero)/512OBJpalette/4096queue-depth/384matrices. Controlled projected half positions/scale/trig and loaded queue templates; source DMA logical byte copies. Not upstream1556C projection/camera/facing/fullframe.','hashes':{str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [Path(__file__),root/'src/taxi_render_owned.c',root/'tests/taxi-render-owned-rom-compare.c',root/'bin/taxi-render-owned-rom-compare.exe',capture]}}
(root/'reports/taxi-render-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps,division_namespace['steps'])
