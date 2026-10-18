"""WholeE388 sixteen-color registration andlogicalDMA bank effect."""
exec(open('outputs/crazy-taxi-native/tools/verify_title_bg_loader_rom.py').read().split('\nclass TitleBGProbe')[0])
class SmallProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpectedhelper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v==0x84000008
   src,dst=self.read(0x40000d4),self.read(0x40000d8)
   for i in range(8):super().write(dst+4*i,self.read(src+4*i),4)
cases=[(flags,mode,cap,count,offset if mode else 64,64 if mode else offset,seed) for flags in range(8) for mode in [0,1] for cap in [0,32,255] for offset in [0,32,480,481,512] for count,seed in [(0,0),(127,165)]]
cases += [(4,mode,cap,0,0,0,65535) for mode in [255,256,257] for cap in [1,33,256]]
rows=subprocess.run([str(root/'bin/palette-small-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 flags,mode,cap,count,bg,obj,seed=case;p=SmallProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2001770:bytearray(32),0x2001790:bytearray([165]*2560),0x10001000:bytearray(struct.pack('<16H',*((seed+i*131)&65535 for i in range(16)))),0x4000000:bytearray(256),0x5000000:bytearray([37]*1024)};p.scalars={}
 p.write(0x2001770,flags,1);p.write(0x2001780,count,2);p.write(0x2001782,bg,2);p.write(0x2001784,obj,2)
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:InitProbe.write(p,a,v)
 p.reg[:3]=[0x10001000,cap,mode];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x800e388,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();got=[signed(p.reg[0]),p.read(0x2001780,2),p.read(0x2001782,2),p.read(0x2001784,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)]
 assert got==list(map(int,parts[:7])),(case,got,parts[:7])
 assert bytes(p.regions[0x2001790])==bytes.fromhex(parts[7]),(case,'records')
 assert bytes(p.regions[0x5000000])==bytes.fromhex(parts[8]),(case,'palette banks')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'WholeE388 original16color registration versusnative metadata/recordindex/DMArequest andlogicalsmallbankapply; all2560records/1024palettebytesincludinguntouchedpadding/otherrecords/nonselectedbank/SP/callee-saved. All8flags/black07FFsourcecaller transfers, BG/OBJ, caplowbyte/clamp, modebytewrap, cursor0/32/480lastslot, capacity481/512rejection, count0/127. LogicalDMA84000008copycontract; nativeinvalidowners/count>=128/odd destination protection outsidevalidsourcefixture. Nofulltitleloader/frame/displayproof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/palette_registration_small.c',root/'src/palette_registration_small.h',root/'tests/palette-small-rom-compare.c',Path(__file__),root/'bin/palette-small-rom-compare.exe',root/'tools/rebuild_palette_small.ps1']}}
(root/'reports/palette-small-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
