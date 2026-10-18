"""WholeF7DC/FBAC/28E3C/28288 loading text source versus native queue."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_font_oam_rom.py').read().split("\nfont=json.loads")[0])
cases=[(x&0xffffffff,y&0xffffffff,id,total,w,h) for id in [148,149] for x,y in [(120,71),(-1,-1),(239,159)] for total in [0,128] for w,h in [(240,160),(160,100)]]
rows=subprocess.run([str(root/'bin/loading-text-owned-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (x,y,id,total,w,h),row in zip(cases,rows):
 p=OamProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v,n in [(0x2001788,0x400,2),(0x2007e38,0x8070e50,4),(0x2001e18,0,4),(0x2001e1c,32,4),(0x2001e20,0x8415bbc,4),(0x2001e24,1,4),(0x2007542,w,2),(0x2007544,h,2),(0x2007f54,3,4),(0x2007e3c,0x6010000,4),(0x2007e2e,0,1),(0x2007e2f,total,1),(0x2007e2c,99,1),(0x2007e2d,165,1)]:p.write(a,v,n)
 for i in range(3):p.write(0x2007e32+i,0,1)
 for a in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]:
  for i in range(0,1024,4):p.write(a+i,0xa5a5a5a5)
 text=0x849dfe0+struct.unpack_from('<I',r,0x49dfe4+id*4)[0];p.reg[:4]=[0,x,y,text];p.reg[13]=0x10018000;p.reg[14]=0x10020001
 for i,v in enumerate([0,0,4,0,256]):p.write(p.reg[13]+i*4,v)
 p.run(entry=0x800f7dc,stop=0x10020000);parts=row.split();actual=[p.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]]
 assert actual==list(map(int,parts[:6])),((x,y,id,total,w,h),actual,parts[:6])
 data=b''.join(bytes(p.read(a+i,1) for i in range(1024)) for a in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[6]),(x,y,id,total,w,h,'queue');steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,queue_bytes_compared=len(cases)*4096,rom_sha256=hashlib.sha256(r).hexdigest(),scope='WholeactualF7DC+FBAC+28E3C+28288 actualLoading.../PleaseWait versusnative loading_text_owned+obj_queue_owned. Loading font0/flags4/scale256/prioritybucketpalette0, actualROM fontdescriptor/metrics/kerning, unsignedcenter shift/signedQ8truncate, space widthsideeffect, clipped/negative-total retainedwidth, fullpartialviewport. Groupbase6010000/palettebase3 fixtures; glyph raster/publication/wholeloading integration excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_text_owned.c','src/loading_text_owned.h','src/obj_queue_owned.c','src/text_layout.c','tests/loading_text_owned_rom_compare.c','tools/compare_loading_text_owned_rom.py']})
(root/'reports/loading-text-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
