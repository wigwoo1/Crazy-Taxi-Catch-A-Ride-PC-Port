"""Original complete C540 mode0 settlement and total targets vs actual native delivery."""
exec(open('outputs/crazy-taxi-native/tools/compare_pending_fare_glyph_live_rom.py').read().split('\nrows=')[0])
rows=[list(map(int,l.split())) for l in (root/'reports/current-fare-settlement-states.txt').read_text().splitlines()]
glyphs=[list(map(int,l.split())) for l in (root/'reports/current-settlement-glyph-states.txt').read_text().splitlines()]
fmap=[(0x2001698,4),(0x200169c,4),(0x20016a0,4),(0x20016a4,4),(0x2001693,1),(0x2001691,1)]
steps=0;joined=[]
for row in rows:
 assert len(row)==115 and row[0]==1
 matches=[(i,g) for i,g in enumerate(glyphs) if g[:6]==row[9:15] and g[6:16]!=g[25:35]]
 assert len(matches)==1,len(matches)
 tick,g=matches[0];p=GlyphProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0
 for (a,n),v in zip(fmap,row[3:9]):p.write(a,v,n)
 p.write(0x20016d4,row[1],1)
 for i,v in enumerate(row[15:]):p.write(0x20022b0+i*8,0);p.write(0x20022b4+i*8,v)
 for i,v in enumerate(g[6:16]):p.write(0x200534a+i*2,v,2)
 p.reg[0]=0;p.reg[1]=0xffffffff;p.run(entry=0x800c540,stop=0x10020000)
 got=[signed(p.read(a,n)) if n==4 else p.read(a,n) for a,n in fmap]
 assert got==row[9:15] and p.read(0x20016d4,1)==row[2],dict(original=got,native=row[9:15],rank=p.read(0x20016d4,1),expected_rank=row[2])
 assert [p.read(0x200534a+i*2,2) for i in range(10)]==g[25:35]
 steps+=p.steps;joined.append(dict(glyph_update=tick,rank=row[2],whole=row[9],fraction=row[10]))
assert rows and (root/'reports/current-settlement-traffic-frames.txt').read_bytes()==(root/'reports/current-destination-traffic-frames.txt').read_bytes()
report=dict(cases=len(rows),thumb_instructions=steps,match=True,joined=joined,traffic_frames_byte_identical=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete originalC540mode0 throughreturn with actual10830formatter vsactualnative normalizedsettlemententry: sixfarefields,ranking100capturedscores/priorrank/postrank,10totaldigit targets exact. Joinedactualglyphbyuniquechangedtotal+sixfarefields. sprintf/strlen/division/rem helpercontracts; tipcollection/trip-end/caller scheduling/fullgame/pixels unverified.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_fare_settlement_live_rom.py','reports/current-fare-settlement-states.txt','reports/current-settlement-glyph-states.txt','src/fare_session.c','src/fare.c','bin/scene-viewer.exe']}
(root/'reports/fare-settlement-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
