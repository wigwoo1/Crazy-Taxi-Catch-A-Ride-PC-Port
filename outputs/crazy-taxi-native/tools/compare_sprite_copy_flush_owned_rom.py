"""Native CPU execution of original28EE0 logical DMA transfers."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_copy_flush_rom.py').read().split('\ncases=')[0])
cases=[(n,b) for n in [0,1,2,63,64] for b in [4,7,32,1024]]
binary=root/'bin/sprite-copy-flush-owned-rom-compare.exe';capture=root/'reports/sprite-copy-flush-owned-native.bin'
subprocess.run([str(binary),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],input=''.join(f'{n} {b}\n' for n,b in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();size=16+768+32768;assert len(data)==len(cases)*size;steps=0
for ci,(n,b) in enumerate(cases):
 p=FlushProbe((0,0,0,0,0,1,0,0,0,0));p.transfers=[];p.regions={0x20091ec:bytearray(768),0x4000000:bytearray(256)};p.scalars={0x20094ec:n}
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(a,v)
 p.transfers=[]
 for i in range(64):
  for j,v in enumerate([0x80fd5fc+i*1024,0x6014000+i*32,b+i*4]):p.write(0x20091ec+i*12+j*4,v)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8028ee0,stop=0x10020000)
 objects=bytearray([165])*32768
 for source,destination,control in p.transfers:
  words=control&65535;length=(words if words else 65536)*4;start=source-0x8000000;off=destination-0x6014000;objects[off:off+length]=r[start:start+length]
 want=struct.pack('<4I',p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc))+bytes(p.regions[0x20091ec])+objects
 assert data[ci*size:(ci+1)*size]==want,(n,b,'ownedcopy');steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,object_bytes_compared=len(cases)*32768,rom_sha256=hashlib.sha256(r).hexdigest(),capture_sha256=hashlib.sha256(data).hexdigest(),scope='Whole28EE0 request source plus logical DMA32 ROM-to-object copy versus native flush CPUcopies. Counts0/1/2/63/64, sizes4/7/32/1024 plus perrecord offsets, overlapping destinations/order/truncation, all32768objectbytes and768queuebytes/finalDMA/count. Hardware copy contract; no raster/presentation/fullgame proof. Host invalidranges prevalidated, not original source guards.',hashes={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [binary,Path(__file__),root/'src/sprite_copy_flush_owned.c',root/'src/sprite_copy_flush_owned.h',root/'tests/sprite_copy_flush_owned_rom_compare.c']})
(root/'reports/sprite-copy-flush-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
