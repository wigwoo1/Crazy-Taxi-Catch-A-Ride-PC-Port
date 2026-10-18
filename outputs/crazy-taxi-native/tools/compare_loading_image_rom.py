"""Whole31EF8 original64x64 loading image, three sequential native draws."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_bitmap_rom.py').read().split('\nbases=')[0])
saved_allowed=allowed_thumb.copy();exec(source.replace('self.steps<50000','self.steps<200000'));allowed_thumb.update(saved_allowed);BitmapProbe.run=MotionProbe.run
native=subprocess.run([str(root/'bin/loading-image-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba'],capture_output=True,check=True).stdout
assert len(native)==3*38400
p=BitmapProbe((0,0,0,0,0,1,0,0,0,0));fb=0x10000000
for i in range(0,38400,4):p.write(fb+i,0x6f6f6f6f)
p.write(0x2007550,fb)
for frame,y in enumerate([0,60,120]):
 p.steps=0;p.reg[13]=0x10018000;p.reg[14]=0x10020001;p.reg[:4]=[0,y,0x400040,0x80eb884];p.write(p.reg[13],1)
 p.run(entry=0x8031ef8,stop=0x10020000)
 actual=bytes(p.read(fb+i,1) for i in range(38400));assert actual==native[frame*38400:(frame+1)*38400],(frame,[(i,a,b) for i,(a,b) in enumerate(zip(actual,native[frame*38400:(frame+1)*38400])) if a!=b][:12])
 print('matched source loading image',frame,'Thumb',p.steps)
(root/'reports/loading-image-native-trajectory.bin').write_bytes(native)
report=dict(equal=True,draws=3,pixels_compared=len(native),rom_sha256=hashlib.sha256(r).hexdigest(),capture_sha256=hashlib.sha256(native).hexdigest(),scope='Three sequential WHOLE31EF8 source calls x0/y0,60,120/64x64/flag1/source080EB884 againstnative loading_image_placement. Every38400frame byte aftereachdraw; initial111 retainedframefixture, zero transparency/overlap/bottom clipping. No palette/monitor/sprite/text/wholeD764 rendering proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_image.c','src/loading_image.h','tests/loading_image_rom_compare.c','tools/compare_loading_image_rom.py']})
(root/'reports/loading-image-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
