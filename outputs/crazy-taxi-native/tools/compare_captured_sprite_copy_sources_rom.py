from pathlib import Path
import json,struct,subprocess,hashlib
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_copy_flush_rom.py').read().split('\ncases=')[0])
trace=root/'reports/original-core-reference/serviceScratchLeft.cpu-timing-trace.json';new=json.loads(trace.read_text())['records'];old=json.loads((root/'reports/original-core-reference/serviceCopiesLeft.cpu-timing-trace.json').read_text())['records'];cases=[]
for a,b in zip(new,old):
 c=dict(a);o=dict(c['frame_service_owners']);scratch=bytes.fromhex(o.pop('scratch_5400'));c['frame_service_owners']=o;assert c==b
 if o['sprite_copy_count']:cases.append((scratch,bytes.fromhex(o['sprite_copy_records'])))
assert len(new)==len(old)==1201
assert json.loads((root/'reports/original-core-reference/serviceScratchLeft.packed-driving-trace.json').read_text())==json.loads((root/'reports/original-core-reference/serviceCopiesLeft.packed-driving-trace.json').read_text())
binary=root/'bin/sprite-copy-source-capture-probe.exe';capture=root/'reports/sprite-copy-source-captured-native.bin';inputs=[]
for scratch,records in cases:inputs.append(' '.join(map(str,[len(records)//12]+list(scratch)+list(struct.unpack('<'+'I'*(len(records)//4),records)))))
subprocess.run([str(binary),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],input='\n'.join(inputs)+'\n',text=True,capture_output=True,check=True);data=capture.read_bytes();size=16+768+32768;assert len(data)==len(cases)*size;steps=0;ramcopies=romcopies=0
for index,(scratch,records) in enumerate(cases):
 p=FlushProbe((0,0,0,0,0,1,0,0,0,0));p.transfers=[];queue=records+bytes(768-len(records));p.regions={0x20091ec:bytearray(queue),0x4000000:bytearray(256)};p.scalars={0x20094ec:len(records)//12}
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(a,v)
 p.transfers=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8028ee0,stop=0x10020000);objects=bytearray([165])*32768
 for source,dest,control in p.transfers:
  n=(control&65535)*4;assert n
  if source==0x3005400:assert n<=64;payload=scratch[:n];ramcopies+=1
  else:assert 0x8000000<=source<=0x8800000-n;payload=r[source-0x8000000:source-0x8000000+n];romcopies+=1
  off=dest-0x6010000;assert 0<=off<=32768-n;objects[off:off+n]=payload
 want=struct.pack('<4I',p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc))+queue+objects;assert data[index*size:(index+1)*size]==want,index;steps+=p.steps
report=dict(status='PASS_NATIVE_CAPTURED_RAM_AND_ROM_SPRITE_COPIES',cases=len(cases),ram_copy_observations=ramcopies,rom_copy_observations=romcopies,compared_object_bytes=len(cases)*32768,original_thumb_instructions=steps,scope='Actual sampledpendingqueues andIWRAM64bytebuffers condition original28EE0 request execution plus boundedlogicalDMAcopy contract; allqueue/output/count/DMA exactnative. Sourcebuffers capturedmidframe and outputinitialsynthetic165, not trueflush-entry/fulloriginalOAM/frame proof. Counts are sampledobservations, not uniqueevents or cyclecost. Nativeviewerintegrationunfinished.',hashes={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [Path(__file__),trace,binary,root/'src/sprite_copy_flush_owned.c',capture]});(root/'reports/sprite-copy-source-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'capturedqueues',ramcopies,'RAM',romcopies,'ROM copies;',report['compared_object_bytes'],'objectbytes exact')
