"""OriginaltwoFEC8 titlecaller configuration, allfontowners."""
exec(open('outputs/crazy-taxi-native/tools/compare_font_setup_rom.py').read().split('\ncases=')[0])
a=0x80162e6
while a<0x801630c:
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes);a+=i.size
class TitleFontProbe(FontProbe):
 def external(self,t):assert t==0x800fec8;self.run(entry=t,stop=self.reg[14]&~1)
seeds=[0,165,65535];rows=subprocess.run([str(root/'bin/title-font-setup-rom-compare.exe')],input=''.join(str(s)+'\n' for s in seeds),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(seeds);steps=0
for seed,row in zip(seeds,rows):
 p=TitleFontProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2001e14:bytearray((seed+i*37)&255 for i in range(144))};p.scalars={0x2001e10:0x12345678};p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x80162e6,stop=0x801630c)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();assert p.read(0x2001e10)==int(parts[0]) and bytes(p.regions[0x2001e14])==bytes.fromhex(parts[1])
 steps+=p.steps
report={'status':'PASS','cases':len(seeds),'Thumb_instructions':steps,'scope':'Actual162E6..16308 twofont configurations withactualFEC8 throughnextcaller1630C. Exactall144fontdescriptorbytes/cursor/retainedslots2/3/padding/SP/callee-saved; sourcefontidentities083FD3B4/084097B8. Nohelpers/contracts; registerconfiguration only, notfontgraphics/textdraw/fulltitleframe/parenthandoff.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/font_setup.c',root/'src/title_font_setup.c',root/'src/title_font_setup.h',root/'tests/title-font-setup-rom-compare.c',root/'tools/rebuild_title_font_setup.ps1',root/'bin/title-font-setup-rom-compare.exe']}}
(root/'reports/title-font-setup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(seeds),steps)
