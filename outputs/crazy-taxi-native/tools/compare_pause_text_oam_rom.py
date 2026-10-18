"""Original pause F7DC text/OAM dispatcher, selected font palette verified."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_font_oam_rom.py').read().split('\nfont=')[0])
font=json.loads((root/'assets/hud/font.json').read_text());steps=cases=glyphs=0
inputs=[(text,bank,x,y) for text in ['Resume','Restart','Quit','Yes','No'] for bank in [0,2] for x,y in [(120,70),(120,80),(120,90),(0,0)]]
cp=subprocess.run([str(root/'bin/pause-text-glyph-compare.exe')],input=''.join(f'{bank} {x} {y} {text}\n' for text,bank,x,y in inputs),text=True,capture_output=True,check=True)
rows=[list(map(int,row.split())) for row in cp.stdout.splitlines()];assert len(rows)==len(inputs)
for (text,bank,x,y),values in zip(inputs,rows):
 p=OamProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v,n in [(0x2001788,0,2),(0x2007e38,0x8070e50,4),(0x2001e18,0,4),(0x2001e1c,32,4),(0x2001e20,0x8415bbc,4),(0x2001e24,1,4),(0x2007542,240,2),(0x2007544,160,2),(0x2007f54,0,4),(0x2007e3c,0x6010000,4),(0x2007e2e,0,1),(0x2007e2f,0,1)]:p.write(a,v,n)
 for j in range(3):p.write(0x2007e32+j,0,1)
 for j,v in enumerate(text.encode()+b'\0'):p.write(0x2008000+j,v,1)
 p.reg[:4]=[0,x,y,0x2008000]
 for j,v in enumerate([0,0,4,bank,256]):p.write(p.reg[13]+4*j,v)
 p.run(entry=0x800f7dc,stop=0x10020000)
 count=p.read(0x2007e32,1);actual=[[p.read(0x20081ec+8*j+2*k,2) for k in range(4)] for j in range(count)]
 native=[]
 for j in range(values[0]):
  g,gx,gy=values[1+3*j:4+3*j];m=font['glyphs'][g];assert m['tiles']==[1,1]
  if gx>=240 or gy>=160 or gx<=-8 or gy<=-8:continue
  native.append([gy&255,gx&511,(bank<<12)|m['tile'],0])
 assert actual==native,(text,bank,x,y,actual,native);steps+=p.steps;cases+=1;glyphs+=count
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':cases,'oam_glyph_requests':glyphs,'instructions':steps,'equal':True,'font_palette_banks':{'ordinary':0,'selected':2},'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/text_layout.c','tests/pause_text_glyph_compare.c','tools/compare_pause_text_oam_rom.py','assets/hud/font.json','assets/hud/font-metrics.ctf']},'scope':'ActualoriginalF7DC/FBAC/28E3C/28288 throughreturn pausefont0 alignment4 scale256 and callerstyle0/2 vsactualnative text_layout.c glyphpositions/clipping/tile/palette OAMfields. Controlled group0 registration/VRAMbase/viewport/queue. No original OAM rasterpixels or full hostpause integration. Native usesexisting originalfontRGBA bank0/2; do not invent selected tint.'}
(root/'reports/pause-text-oam-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',cases,'pause text cases',glyphs,'glyphs; ordinarybank0/selectedbank2 OAM exact',steps,'Thumb instructions')
