"""Original2901C comparison against actual native integrated reward requests."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_rom.py').read().split('\nimport itertools')[0])
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-requests.txt').read_text().splitlines()]
steps=created=active=0
for row in rows:
 assert len(row)==1141,len(row)
 kind,trip,mode,sound,whole,fraction=row[:6];pre=row[6:572];post=row[572:1138];slot,sound_index,random_calls=row[1138:]
 p=RewardProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.sound=None
 for a,v,n in [(0x2001694,trip,1),(0x2001eb0,mode,1),(0x2007602,sound,1),(0x2009aa4,pre[0],1),(0x2009aa6,pre[1],1),(0x20025d6,pre[2],1),(0x2009aa8,pre[3],4),(0x2009aac,pre[4],4),(0x2003b04,pre[5],4)]:p.write(a,v,n)
 for i,v in enumerate(pre[6:]):p.write(0x2009abc+i,v,1)
 p.reg[:3]=[kind,whole&MASK,fraction&MASK];p.run(entry=0x802901c,stop=0x10020000)
 got=[p.read(0x2009aa4,1),p.read(0x2009aa6,1),p.read(0x20025d6,1),signed(p.read(0x2009aa8)),signed(p.read(0x2009aac)),p.read(0x2003b04)]+[p.read(0x2009abc+i,1) for i in range(560)]
 assert got==post
 assert p.calls==random_calls
 expected=None if sound_index<0 else struct.unpack_from('<I',r,0x7ec658+sound_index*4)[0];assert p.sound==expected
 steps+=p.steps;created+=slot>=0;active+=trip!=0
report=dict(cases=len(rows),active_requests=active,created=created,thumb_instructions=steps,match=bool(rows),rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original2901C throughreturn/nested1E72C vs actualnative integrated requests: allstate/RNG/560tipbytes/audio request exact per observedentry. No continuoussourceframe/inputtrajectory or tipupdate/render proof. Zero cases means no execution proof.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_reward_live_rom.py','reports/current-reward-requests.txt','reports/reward-capture-trip.log','src/reward_session.c','src/scene_viewer.c','bin/scene-viewer.exe']}
(root/'reports/reward-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print(report)
