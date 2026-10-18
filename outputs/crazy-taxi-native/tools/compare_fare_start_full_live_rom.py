"""Complete original C540 mode1 including pending formatter vs actual pickup display handoffs."""
exec(open('outputs/crazy-taxi-native/tools/compare_pending_fare_glyph_live_rom.py').read().split('\nrows=')[0])
money=[list(map(int,l.split())) for l in (root/'reports/current-passenger-money-states.txt').read_text().splitlines()]
glyphs=[list(map(int,l.split())) for l in (root/'reports/current-full-cash-glyph-states.txt').read_text().splitlines()]
fmap=[(0x2001698,4),(0x200169c,4),(0x20016a0,4),(0x20016a4,4),(0x2001693,1),(0x2001691,1)]
steps=0;joined=[]
for m in money:
 matches=[(i,g) for i,g in enumerate(glyphs) if g[:6]==m[8:14] and g[16:25]!=g[35:]]
 assert len(matches)==1,(m[:2],len(matches))
 tick,g=matches[0];p=GlyphProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0
 for (a,n),v in zip(fmap,m[2:8]):p.write(a,v,n)
 for i,v in enumerate(m[58:]):p.write(0x2005f2c+m[0]*44+i,v,1)
 for i,v in enumerate(g[16:25]):p.write(0x2005374+i*2,v,2)
 p.reg[0]=1;p.reg[1]=m[0];p.run(entry=0x800c540,stop=0x10020000)
 got=[signed(p.read(a,n)) if n==4 else p.read(a,n) for a,n in fmap]
 assert got==m[8:14] and [p.read(0x2005374+i*2,2) for i in range(9)]==g[35:]
 steps+=p.steps;joined.append(dict(customer=m[0],glyph_update=tick,route_length=m[1]))
assert len(money)==2 and (root/'reports/current-full-glyph-traffic-frames.txt').read_bytes()==(root/'reports/current-glyph-traffic-frames.txt').read_bytes()
report=dict(cases=len(money),glyph_capture_updates=len(glyphs),joined=joined,thumb_instructions=steps,match=True,traffic_frames_byte_identical=True,scope='Complete original C540 mode1 throughreturn, nested actual10830formatter, sixmoneyfields+ninependingtargets exact against actual boarding handoffs uniquely joined to changed glyph records bysixfarefields. Captures from deterministic identical traffic sequence; explicit sprintf/strlen/division/remainder contracts. Totalformat/settlement/DF9C/fullgame unverified.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_fare_start_full_live_rom.py','reports/current-passenger-money-states.txt','reports/current-full-cash-glyph-states.txt','reports/fare-formatter-cfg.json','src/fare.c','bin/scene-viewer.exe']}
(root/'reports/fare-start-full-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
