"""Trace original renderer asset-bank selection for all driver/view pairs."""
exec(open('outputs/crazy-taxi-native/tools/compare_taxi_renderer_original_angle_rom.py').read().split('\ncases=')[0])
class LoadProbe(TaxiProbe):
 def external(self,target):
  if target==0x8027fb8:self.loads.append(self.reg[:4]+[self.read(self.reg[13])]);self.reg[0]=0
  else:super().external(target)
results=[]
for driver in range(4):
 left,right,palette=struct.unpack_from('<III',r,0x4a5514+driver*12)
 for frame in range(64):
  p=LoadProbe((0,0,0,0,0,1,0,0,0,0));p.loads=[];p.requests=[];base=0x2002634
  for a in range(base,base+0xea0,4):p.arm.mem[a]=0
  for o,v,n in [(0x49c,468,4),(0x4a0,-1024,4),(0x1c,-frame*64*4096,4),(0x770,1024*4096,4)]:p.write(base+o,v,n)
  for a,v in {0x2001708:0,0x30053e0:0xffffffff,0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x3006810:0x10002000,0x3006828:120,0x300682c:80,0x3006838:192}.items():p.arm.mem[a]=v
  p.reg[:3]=[base,0,driver];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
  assert p.read(base+0xe64)==frame
  assert p.loads==[[left,frame,0,1,0],[right,frame,1,1,0]],dict(driver=driver,frame=frame,loads=p.loads)
  results.append(dict(driver=driver,frame=frame,left=hex(left+frame*2048),right=hex(right+frame*2048)))
report=dict(cases=256,pass_match=True,loads=results,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete1556C original bank/frame/half arguments for4drivers64views, allocator intercepted.2048 stride from aligned27FB8 descriptor8x8 and color16 path; full allocator DMA/device not executed. No visual front/rear semantic inference.')
(root/'reports/taxi-original-frame-load-audit.json').write_text(json.dumps(report,indent=2));print('PASS original256bank/frame/halfselections')
