"""Actual27FB8 loading sprite resource resolution and logical DMA copy."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
cases=[(kind,index,frame,flag,count) for kind in [0,1] for index in [0,1] for frame in [0,1] for flag in [0,1] for count in [0,64]]
capture=root/'reports/sprite-replacement-owned-native.bin'
subprocess.run([str(root/'bin/sprite-replacement-owned-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();assert len(data)==len(cases)*(16+768+32768);steps=0
for case_number,(kind,index,frame,flag,count) in enumerate(cases):
 resource=0x805a338 if kind else 0x8070e50;p=ReplaceProbe((0,0,0,0,0,1,0,0,0,0))
 p.regions={0x2007e3c:bytearray(struct.pack('<32I',*([0x6010000]*32))),0x20091ec:bytearray([165]*768),0x4000000:bytearray(256)};p.scalars={0x2007e38:resource,0x20094ec:count}
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(a,v)
 p.reg[:4]=[0x80fd5fc,index,frame,2];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(p.reg[13],flag);p.run(entry=0x8027fb8,stop=0x10020000)
 header=[p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)];objects=bytearray([165])*32768
 if header[3]!=0x33333333:
  size=(header[3]&65535)*4;source_offset=header[1]-0x8000000;destination_offset=header[2]-0x6010000
  objects[destination_offset:destination_offset+size]=r[source_offset:source_offset+size]
 expected=struct.pack('<4I',*header)+bytes(p.regions[0x20091ec])+objects;offset=case_number*len(expected)
 assert data[offset:offset+len(expected)]==expected,(kind,index,frame,flag,count);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,object_bytes_compared=len(cases)*32768,rom_sha256=hashlib.sha256(r).hexdigest(),capture_sha256=hashlib.sha256(data).hexdigest(),scope='Wholeactual27FB8 ordinary/attract group2 versusnative ROM metadata resolver+sprite_replace+immediateCPUcopy. Exact request registers/all768deferredbytes/all32768objectbytes. Immediate hardware DMA modeledlogical bytecopy aftersource; originalobjectallocation/framepublication/placement/loadingcaller integration excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/sprite_replacement_owned.c','src/sprite_replacement_owned.h','src/sprite_replacement.c','tests/sprite_replacement_owned_rom_compare.c','tools/compare_sprite_replacement_owned_rom.py']})
(root/'reports/sprite-replacement-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
