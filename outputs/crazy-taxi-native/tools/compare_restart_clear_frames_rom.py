"""Complete original2C558 including both framebuffer fills."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_index_rom.py').read().split('\npalettes=')[0])
saved=allowed_thumb.copy();source=source.replace('self.steps<50000','self.steps<500000');exec(source);allowed_thumb.update(saved);IndexProbe.run=MotionProbe.run
for a in range(0x802c632,0x802c686,2):allowed_thumb[a]=r[a-0x8000000:a-0x8000000+2]
buffers_slot=int.from_bytes(r[0x2c658:0x2c65c],'little');width_slot=int.from_bytes(r[0x2c65c:0x2c660],'little');height_slot=int.from_bytes(r[0x2c660:0x2c664],'little')
class FullProbe(IndexProbe):
 def __init__(self,*args):
  self.first=bytearray([165])*38400;self.second=bytearray([165])*38400
  super().__init__(*args)
 def read(self,a,size=4):
  if a==mode_slot:return self.mode
  if a==context_slot:return 0x2009000 if self.present else 0
  if a==buffers_slot:return 0x21000000
  if a==buffers_slot+4:return 0x21010000
  if a==width_slot:return self.width
  if a==height_slot:return self.height
  for base,frame in [(0x21000000,self.first),(0x21010000,self.second)]:
   if base<=a and a+size<=base+38400:return int.from_bytes(frame[a-base:a-base+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  for base,frame in [(0x21000000,self.first),(0x21010000,self.second)]:
   if base<=a and a+size<=base+38400:
    frame[a-base:a-base+size]=(value&((1<<(8*size))-1)).to_bytes(size,'little');return
  super().write(a,value,size)
palettes=[[0]*256,[((i*131)^((i%5)*977))&65535 for i in range(256)],[int.from_bytes(r[0x4a5a34+2*i:0x4a5a36+2*i],'little') for i in range(256)]]
configs=[(4,1,240,160,0,0,0),(4,1,7,5,31,0,0),(4,1,0,160,15,15,15),(3,1,240,160,0,0,0),(4,0,240,160,0,0,0),(4,1,1,1,0,31,31)]
cases=[(c,p) for p in palettes for c in configs]
cp=subprocess.run([str(root/'bin/restart-clear-frames-rom-compare.exe')],input=''.join(' '.join(map(str,[*c,*p]))+'\n' for c,p in cases),text=True,capture_output=True,check=True)
rows=cp.stdout.splitlines();assert len(rows)==len(cases);steps=0
for (c,palette),row in zip(cases,rows):
 want,a,b=row.split();p=FullProbe((0,0,0,0,0,1,0,0,0,0));p.mode,p.present,p.width,p.height=c[:4];p.selected=123;p.first=bytearray([165])*38400;p.second=bytearray([165])*38400;p.reg[:3]=c[4:];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(0x2009018,0x2009100)
 for j,v in enumerate(palette):p.write(0x2009100+2*j,v,2)
 p.run(entry=0x802c558,stop=0x10020000)
 assert p.selected==int(want) and p.first==bytes.fromhex(a) and p.second==bytes.fromhex(b),c
 steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),buffer_pointer_owner=hex(buffers_slot),width_owner=hex(width_slot),height_owner=hex(height_slot),scope='Complete original2C558 through return with no opaque callees. Actualmode/null gates, allpalette entries/nearestindex, globalfillword and BOTH38400-byte buffers matched native adapter. Fullviewport/odd35pixels/zero/onepixel fixtures; threepalettes; safe dimensions. Explicit ownerpointer fixtures; noGPU/composedRestart integration.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/restart_render_context.c','src/restart_render_context.h','src/shade_table.c','tests/restart_clear_frames_rom_compare.c','tools/compare_restart_clear_frames_rom.py']})
(root/'reports/restart-clear-frames-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
