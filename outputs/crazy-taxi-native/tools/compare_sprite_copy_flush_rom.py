"""Actual whole28EE0 deferred sprite DMA request lifecycle, valid producer domain."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x28ee0:0x28f20],0x8028ee0):allowed_thumb[i.address]=bytes(i.bytes)
class FlushProbe(ReplaceProbe):
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:self.transfers.append([self.read(0x40000d4),self.read(0x40000d8),self.read(0x40000dc)])
cases=[(n,b) for n in [0,1,2,63,64] for b in [0,1,3,4,32,1024,0xffffffff]]
binary=root/'bin/sprite-copy-flush-rom-compare.exe'
rows=subprocess.run([str(binary)],input=''.join(f'{n} {b}\n' for n,b in cases),text=True,capture_output=True,check=True).stdout.splitlines()
assert len(rows)==len(cases);steps=0
for (n,b),row in zip(cases,rows):
 p=FlushProbe((0,0,0,0,0,1,0,0,0,0));p.transfers=[];p.regions={0x20091ec:bytearray(768),0x4000000:bytearray(256)};p.scalars={0x20094ec:n}
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(a,v)
 p.transfers=[]
 for i in range(64):
  for j,v in enumerate([0x80fd5fc+i*1024,0x6014000+i*32,(b+i*4)&0xffffffff]):p.write(0x20091ec+i*12+j*4,v)
 before=bytes(p.regions[0x20091ec]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8028ee0,stop=0x10020000)
 left,right=row.split('|');values=list(map(int,left.split()));assert values==[n]+[v for t in p.transfers for v in t],(n,b,'transfers')
 assert list(map(int,right.split()))==[p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)],(n,b,'final')
 assert bytes(p.regions[0x20091ec])==before and p.reg[13]==0x10000400 and p.reg[4:12]==saved;steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Whole28EE0 source versus native sprite_copy_flush, valid0..64producer count; ordered DMA source/destination/control, byte-to-word truncation and32bit wrapping, final count reset/retained queue/DMA/SP/registers. Transfer callback contract only; no actual tile memorycopy or frame raster proof. Native invalid-domain/failure behavior host validation only.',hashes={str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [binary,Path(__file__),root/'src/sprite_copy_flush.c',root/'src/sprite_copy_flush.h',root/'tests/sprite_copy_flush_rom_compare.c']})
(root/'reports/sprite-copy-flush-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
