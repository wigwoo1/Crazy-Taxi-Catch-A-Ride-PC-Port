"""Original29264 complete updater against actualnative active tip updates."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-updates.txt').read_text().splitlines()]
steps=expired=0
for row in rows:
 assert len(row)==1152,len(row)
 delta,sound=row[:2];pre=row[2:576];post=row[576:1150];expiry,sounds=row[1150:]
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.expired=p.sounds=0
 for a,v,n in [(0x2003b00,delta,4),(0x2007602,sound,1),(0x2009aa4,pre[0],1),(0x2009aa5,pre[1],1),(0x2009aa7,pre[2],1),(0x20016a0,pre[3],4),(0x20016a4,pre[4],4)]:p.write(a,v,n)
 for i,v in enumerate(pre[5:14]):p.write(0x2005374+2*i,v,2)
 for i,v in enumerate(pre[14:]):p.write(0x2009abc+i,v,1)
 p.run(entry=0x8029264,stop=0x10020000)
 got=[p.read(0x2009aa4,1),p.read(0x2009aa5,1),p.read(0x2009aa7,1),signed(p.read(0x20016a0)),signed(p.read(0x20016a4))]+[p.read(0x2005374+2*i,2) for i in range(9)]+[p.read(0x2009abc+i,1) for i in range(560)]
 assert got==post,dict(mismatch=[(i,a,b) for i,(a,b) in enumerate(zip(got,post)) if a!=b][:5])
 assert p.expired==expiry and p.sounds==sounds
 steps+=p.steps;expired+=expiry
assert rows and expired
report=dict(cases=len(rows),tip_expirations=expired,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original29264 nestedcompleteC540mode2/10830 vsactualnative active update entries: gate/clock/count/pendingfare/9glyph targets/560tipbytes/audio request count exact. Explicit sprintf/strlen/division contracts, per-entry fixtures; continuoussourceframe/inputtrajectory/graphics unverified.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_reward_update_live_rom.py','reports/current-reward-updates.txt','reports/reward-update-live-trip.log','src/reward_session.c','src/reward_update.c','src/fare.c','src/scene_viewer.c','bin/scene-viewer.exe']}
assert (root/'reports/reward-update-run-requests.txt').read_bytes()==(root/'reports/current-reward-requests.txt').read_bytes();report['request_capture_byte_identical']=True
(root/'reports/reward-update-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
