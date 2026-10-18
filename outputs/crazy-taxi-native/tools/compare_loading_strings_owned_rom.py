"""Actual wholeD764 + whole28FE4 versus native shared string composition."""
exec(open('outputs/crazy-taxi-native/tools/compare_loading_present_rom.py').read().split('\ncases=')[0])
for start,end in [(0x28fe4,0x29008),(0x29010,0x29016)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class OwnedProbe(LoadingProbe):
 def external(self,target):
  if target==0x8028fe4:self.run(entry=target,stop=self.reg[14]&~1);return
  pointer=self.reg[2] if target==0x8028288 else 0
  super().external(target)
  if self.variant:
   if target==0x802c9f0:self.write(0x2007542,320,2);self.write(0x2007544,180,2)
   if target==0x800f7dc:
    if not self.texts:self.write(0x2009aa0,0);self.write(0x2007542,400,2);self.write(0x2007544,200,2)
    self.texts+=1
   if pointer:
    for i,add in enumerate([5,1,7]):self.write(pointer+4*i,self.read(pointer+4*i)+add)
   if target==0x8031ef8:self.write(0x2001684,255,1)
cases=[(a,b,w,h,animation,v) for a,b in [(0,0),(0x60,0),(0x80,0),(0,1)] for w,h in [(240,160),(0,0),(65535,65535)] for animation in [0,128,255] for v in [0,1]]
cp=subprocess.run([str(root/'bin/loading-strings-owned-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (a,b,w,h,animation,v),want in zip(cases,rows):
 p=OwnedProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.variant=v;p.texts=0;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for addr,value,size in [(0x2007542,w,2),(0x2007544,h,2),(0x2001eb0,a,1),(0x2001708,b,1),(0x2001684,animation,1),(0x2009a88,5,1),(0x2009aa0,0x849dfe0,4)]:p.write(addr,value,size)
 p.run(entry=0x800d764,stop=0x10020000)
 actual=[len(p.calls),*[v for row in p.calls for v in row],p.read(0x2009aa0),p.read(0x2007542,2),p.read(0x2007544,2),p.read(0x2001684,1)]
 assert actual==want,((a,b,w,h,animation,v),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='WholeD764 + actual nestedwhole28FE4 source English/null selection againstnative loading_strings_owned. Exact orderedremainingchildargs/positions/owners; clear dimensions mutate before text, first text changesselectedtable and dimensions, sprites mutate sharedstackposition, image changesanimation before late signedread. Remaininghelpers contracts; no pixel/fade/frame/audio or fullattract proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_present.c','src/loading_strings_owned.c','src/loading_strings_owned.h','src/string_lookup.c','tests/loading_strings_owned_rom_compare.c','tools/compare_loading_strings_owned_rom.py']})
(root/'reports/loading-strings-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
