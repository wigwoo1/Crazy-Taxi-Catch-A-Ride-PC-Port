"""Actual titlecallback166B8 directly invoking actual28288, no producer contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_title_callback_rom.py').read().split('\ncases=[]')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')")
exec(source);allowed_thumb.update(saved_allowed)
class RequestsProbe(TitleProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x8028288:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
cases=[(clock,fade) for clock in [0,1,110,111,112,113,126,127,128,239,240,255,256] for fade in [0,1]]
binary=root/'bin/title-requests-rom-compare.exe';rom='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(binary),rom],input=''.join(f'{clock} {fade}\n' for clock,fade in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (clock,fade),row in zip(cases,rows):
 p=RequestsProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x20081ec:bytearray(1024),0x20085ec:bytearray(1024),0x20089ec:bytearray(1024),0x2008dec:bytearray(1024)}
 p.scalars={0x200356a:1,0x2007602:0,0x2003570:34,0x2003b00:1,0x200755c:clock,0x200177b:fade,0x2007542:240,0x2007544:160,0x2003b04:12345,0x2007e2c:99,0x2007e2d:165,0x2007e2e:0,0x2007e2f:0,0x2007e32:0,0x2007e33:0,0x2007e34:0,0x2007e38:0x8034634,0x2007e64:0x60163a0,0x2007f7c:15,0x2001788:0x2001}
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,165+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,165+i*37+j*53,2)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x80166b8,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();got=[p.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]]
 assert got==list(map(int,parts[:6])),((clock,fade),got,parts[:6])
 data=b''.join(p.regions[a] for a in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[6]),((clock,fade),'queued bytes differ')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'original_Thumb_instructions':steps,'scope':'Whole166B8 title callback directly executes actual28288 title4bpp producer, source timer34/sounddone1/width240,height160, originalgroup10base60163A0,palettebase15 andmask2001. No helper contracts reached. All4096queue/depthbytes/counters/dimensions/SP/regs compare native callback+requests bridge. Clock blink boundaries111/112/127/128 acrosscycles andfadegate. Not scrolling/soundon/fullpresentation/OAMpublication/renderer/parent proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [binary,root/'src/title_requests.c',root/'tests/title-requests-rom-compare.c',Path(__file__)]}}
(root/'reports/title-requests-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native callback-to-sprite composition',len(cases),'cases',steps,'Thumb')
