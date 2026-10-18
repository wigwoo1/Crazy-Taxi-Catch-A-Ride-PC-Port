"""Original collision voice branch vs actual native collision captures."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
instructions=list(md.disasm(r[0x2a90a:0x2a92a],0x802a90a));assert sum(i.size for i in instructions)==0x20
allowed_thumb.update({i.address:bytes(i.bytes) for i in instructions})
rows=[list(map(int,l.split())) for l in (root/'reports/current-impact-voice-states.txt').read_text().splitlines()]
active_addr=int.from_bytes(r[0x2a96c:0x2a970],'little');voice_addr=int.from_bytes(r[0x2a970:0x2a974],'little');assert (active_addr,voice_addr)==(0x2001694,0x20025d6)
steps=0;requests=0
for index,row in enumerate(rows):
 active,before,voice_before,after,voice_after,requested=row;assert requested==int(active!=0)
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.write(active_addr,active,1);p.write(voice_addr,voice_before,1);p.write(0x2003b04,before)
 p.run(entry=0x802a90a,stop=0x802a92a)
 assert p.read(0x2003b04)==after and p.read(voice_addr,1)==voice_after and p.calls==requested,dict(case=index,rng=p.read(0x2003b04),expected=after,voice=p.read(voice_addr,1),expected_voice=voice_after,calls=p.calls)
 steps+=p.steps;requests+=requested
assert rows and (root/'reports/current-voice-traffic-frames.txt').read_bytes()==(root/'reports/current-pickup-traffic-frames.txt').read_bytes()
report=dict(collision_cases=len(rows),requests=requests,thumb_instructions=steps,match=True,traffic_frames_unchanged=True,entry=hex(0x802a90a),stop=hex(0x802a92a),instructions=[dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str) for i in instructions],rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual original2A90A signed active gate, nested original1E72C, signed remainder helper contract, voice/global RNG output match actual native collision boundary captures. Collision producer/gate outside this block and audio playback unverified.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_impact_voice_live_rom.py','reports/current-impact-voice-states.txt','reports/current-voice-random.txt','src/impact_voice.h','src/scene_viewer.c','bin/scene-viewer.exe']}
(root/'reports/impact-voice-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
