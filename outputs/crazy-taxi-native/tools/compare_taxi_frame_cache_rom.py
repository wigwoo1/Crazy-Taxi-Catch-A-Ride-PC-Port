"""Bounded original15910..15954 cache gate plus actual27FB8 request comparison."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x15910:0x15956],0x8015910):allowed_thumb[i.address]=bytes(i.bytes)
class CacheProbe(ReplaceProbe):
 def external(self,t):
  assert t==0x8027fb8,hex(t)
  self.run(entry=t,stop=self.reg[14]&~1)
cases=[(previous,frame,driver,immediate,count) for driver in range(4) for frame in range(64) for previous in [frame,0xffffffff] for immediate in [0,1] for count in [0,63,64]]
# Equal full-word keys bypass all requests, including otherwise invalid inputs.
cases += [(0x10000,0x10000,255,1,0),(0xffffffff,0xffffffff,255,0,64)]
rows=subprocess.run([str(root/'bin/taxi-frame-cache-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines()
assert len(rows)==len(cases)
steps=0
for case,row in zip(cases,rows):
 previous,frame,driver,immediate,count=case
 p=CacheProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007e3c:bytearray(struct.pack('<11I',0,0x6014000,*([0]*9))),0x20091ec:bytearray([165]*768),0x4000000:bytearray(256)};p.scalars={0x2007e38:0x8070e50,0x20094ec:count,0x30053e0:previous}
 p.reg[13]=0x10000400
 for address,value in [(0x100004f4,0x10000800),(0x100004e8,driver),(0x100004e4,immediate),(0x10000800,frame),(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(address,value)
 p.run(entry=0x8015910,stop=0x8015956)
 parts=row.split();expected=[int(frame!=previous),p.read(0x30053e0),p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)]
 assert list(map(int,parts[:6]))==expected,(case,parts[:6],expected)
 queue=''.join('%08x'%v for v in struct.unpack('<192I',p.regions[0x20091ec]))
 assert parts[6]==queue,(case,'queue')
 assert p.reg[13]==0x10000400
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Original15910..15954 fullword cache equality, two driver bank requests through actual27FB8 and cache publication; native exact cache/DMA/768 queue bytes. All4 drivers64 frames, equal/change, immediate/deferred, queue0/63/64, equal invalid fullword keys. Destination6014000 explicit fixture. Request-only source DMA, not full1556C, palette, rendering, initialization or facing proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/taxi_frame_cache_owned.c',root/'src/taxi_frame_copy_owned.c',root/'tests/taxi-frame-cache-rom-compare.c',root/'bin/taxi-frame-cache-rom-compare.exe']}}
(root/'reports/taxi-frame-cache-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',len(cases),steps)
