"""Actualtitlelanguage setup1630C..1638E pluswhole27FB8."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
a=0x801630c
while a<0x8016390:
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes);a+=i.size
class SetupProbe(ReplaceProbe):
 def external(self,t):
  assert t==0x8027fb8;self.run(entry=t,stop=self.reg[14]&~1)
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v in [0x84000040,0x80000010],hex(v)
   src,dst=self.read(0x40000d4),self.read(0x40000d8);self.transfers.extend([src,dst,v])
   unit=4 if v&0x04000000 else 2
   for j in range(v&65535):InitProbe.write(self,dst+j*unit,self.read(src+j*unit,unit),unit)
rompath='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(root/'bin/title-prompt-setup-rom-compare.exe'),rompath],text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==4;steps=0
for language,row in enumerate(rows):
 p=SetupProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007e3c:bytearray(struct.pack('<32I',*([0x60163a0]*32))),0x2001790:bytearray([165]*2560),0x4000000:bytearray(256),0x5000000:bytearray([37]*1024),0x6010000:bytearray([123]*32768)};p.scalars={0x2007e38:0x8034634,0x200388d:language};p.transfers=[]
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x801630c,stop=0x8016390)
 # Fragment uses caller R1/R2 scratch anddoesnotpromise functioncallee-saved.
 assert p.reg[13]==0x10000400
 parts=row.split();assert int(parts[0])==language and p.transfers==list(map(int,parts[1:16]))
 assert bytes(p.regions[0x2001790])==bytes.fromhex(parts[16])
 assert p.regions[0x5000000][0x340:0x360]==r[0x290ad4:0x290af4]
 assert p.regions[0x5000000][:0x340]==bytes([37]*0x340) and p.regions[0x5000000][0x360:]==bytes([37]*(1024-0x360))
 tilesource=struct.unpack_from('<I',r,0x7ebf48+4*language)[0]-0x8000000
 for frame in range(4):
  dst=p.transfers[frame*3+1]-0x6010000
  assert p.regions[0x6010000][dst:dst+256]==r[tilesource+frame*256:tilesource+(frame+1)*256]
 steps+=p.steps
report={'status':'PASS','languages':4,'Thumb_instructions':steps,'scope':'Actual1630C..1638E callerfragment withactualwhole27FB8 nestedforfourtitlegroup10 replacements andpalette32byte upload/registeredrecord10 pointeroverride versusnative title_prompt_setup requests/all2560recordbytes. All4validlanguages/full1024uploadedtilebytes/palettebank10 andretainedother992palettebytes/SP. DMA84000040wordcopy/80000010halfwordcopy logicalcontracts. Destinationbase60163A0 source-established bytitle-sprite-allocation-rom-probe.json; callerresolvedassetmetadata fixtures; upstreamFEC8fontload/whole27994groupallocation andactualqueueproducer/OAM/pixels/handoff notqualified.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/title_prompt_setup.c',root/'src/title_prompt_setup.h',root/'src/sprite_replacement.c',root/'tests/title-prompt-setup-rom-compare.c',root/'tools/rebuild_title_prompt_setup.ps1',root/'bin/title-prompt-setup-rom-compare.exe']}}
(root/'reports/title-prompt-setup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',4,steps)
