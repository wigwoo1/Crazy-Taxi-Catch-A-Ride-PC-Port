"""Whole166B8 callback with actualscroll5EC8 andrandom1E72C."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("'b':True,'bne'", "'bhs':c,'blo':not c,'b':True,'bne'")
source=source.replace("['muls','ands','orrs']","['muls','ands','orrs','eors']").replace("a|b if m=='orrs' else a&b","a|b if m=='orrs' else (a^b if m=='eors' else a&b)")
source=source.replace('self.steps<50000','self.steps<200000')
exec(source);allowed_thumb.update(saved_allowed)
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl','bvs','bvc'}
for entry,end in [(0x80166b8,0x80167bc),(0x8005ec8,0x800614c)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
for i in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']:allowed_thumb[int(i['address'],16)]=bytes.fromhex(i['bytes'])
class TitleProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x8005ec8,0x801e72c]:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8032520:
   assert self.reg[1]==65536;self.reg[0]%=65536
  elif t==0x80267c8:
   assert self.reg[1:3]==[0,0]
   slots=struct.unpack_from('<2I',r,0x7ec658);assert self.reg[0] in slots
   self.sound=slots.index(self.reg[0])
  elif t==0x8028288:
   frame,group,pos,priority=self.reg[:4];assert group==10 and priority==0
   x,y,z=[signed(self.read(pos+i*4)) for i in range(3)]
   bucket,flags,palette=[self.read(self.reg[13]+i*4) for i in range(3)]
   self.requests.extend([x,y,z,frame,group,priority,bucket,flags,palette])
  else:raise AssertionError(('unexpectedhelper',hex(t)))
cases=[]
for done,sound,seed in [(0,0,0),(0,1,0),(0,1,1),(0,1,0xffffffff),(1,1,0x4652414e),(255,255,12345)]:
 for timer,x,delta,clock,fade,w,h in [(0,0,1,0,1,240,160),(33,69632,1,0,0,240,160),(32,12345,-1,0,0,240,160),(34,69632,1,111,0,240,160),(34,69632,1,112,0,240,160),(34,69632,1,127,0,240,160),(34,69632,1,128,0,240,160),(34,69632,1,0,1,240,160),(34,69632,1,0,0,480,160),(34,69632,1,0,0,512,0),(33,0,60,0,0,240,160),(-2147483648,0,2147483647,0,0,240,160)]:cases.append((done,sound,timer,x,delta,clock,fade,w,h,seed))
rows=subprocess.run([str(root/'bin/title-callback-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 done,sound,timer,x,delta,clock,fade,w,h,seed=case;p=TitleProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(56),0x4000000:bytearray(1024),0x6000000:bytearray(2048)}
 p.scalars={0x200356a:done,0x2007602:sound,0x2003570:timer&MASK,0x2003b00:delta&MASK,0x200755c:clock,0x200177b:fade,0x2007542:w,0x2007544:h,0x2003b04:seed,0x2007558:0,0x2007559:0}
 p.write(0x2000000,0x6000000);p.write(0x2000008,struct.unpack_from('<I',r,0x3415c)[0]);p.write(0x2000010,256);p.write(0x200001c,x&MASK);p.write(0x2000024,64,2);p.write(0x2000026,32,2);p.write(0x2000028,1,2)
 p.sound=-1;p.requests=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x80166b8,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 got=[p.read(0x200356a,1),signed(p.read(0x2003570)),signed(p.read(0x200001c)),p.read(0x2003b04),p.sound,len(p.requests)//9]+p.requests
 assert got==list(map(int,row.split())),(case,got,row)
 if timer<=33:
  tiles=(root/'assets/title/background.tiles').read_bytes();indices=(root/'assets/title/background.indices').read_bytes();scroll=p.read(0x200001c)//256
  assert p.read(0x4000010,2)==(scroll&7)
  for y in range(min(h,160)):
   for xpixel in range(w):
    sx=xpixel+p.read(0x4000010,2);entry=p.read(0x6000000+((y//8)*32+sx//8)*2,2)
    xx,yy=sx%8,y%8
    if entry&1024:xx=7-xx
    if entry&2048:yy=7-yy
    pixel=(tiles[(entry&1023)*32+yy*4+xx//2]>>(4*(xx&1)))&15;pixel+=16*(entry>>12)
    assert pixel==indices[y*512+scroll+xpixel],(case,xpixel,y,'viewport map mismatch')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Whole166B8 callback through167BC withactualwhole5EC8 titleBG0 coefficient256/64x32 map/flags0 andactual1E72C random update. Sounddone/timer/Q8scroll/RNG/sampletable slot/full4promptrequestfields/SP/callee-saved compared; sound on/off/alreadydone, clock111/112/127/128 fadegates, widths240/480/512, height0/160, signed/wrappedtimer/delta. Signedremainder32520 explicit65536 contract, sound267C8 records sourceROMtable identity, producer28288 requestcapture contract. Actualscroll-helper BG0map viewport indices for movingcases width240 resolved withsourceextracted4bpp tiles/flip/palettebank andcompared all240x160 visibleindices; no colors/visiblemonitorpixel proof; no titleassetloading/wholeframe/parenthandoff/audio/displayproof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/title_callback.c',root/'src/title_scroll.c',root/'src/title_prompt.c',root/'src/random.c',root/'tests/title-callback-rom-compare.c',Path(__file__),root/'bin/title-callback-rom-compare.exe']}}
(root/'reports/title-callback-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
