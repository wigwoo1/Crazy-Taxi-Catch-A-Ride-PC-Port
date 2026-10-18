"""Actual original contextual selection using every valid customer type."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_context_voice_rom.py').read().split('\ncases=[]')[0])
cases=list(itertools.product(range(-1,8),range(-1,8),[0,17,18,19,20,21,22]))
cp=subprocess.run([str(root/'bin/game-audio-customer-class-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
assert list(r[0x4a5544:0x4a554c])==[1,0,0,1,0,0,1,1]
for (cur,alt,sel),want in zip(cases,rows):
 p=ContextProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.busy=0;p.start=0x2009040;p.random=0
 for a,v,n in [(0x2002604,0,4),(0x2002608,0,4),(0x20025d6,sel,1),(0x2002610,37,1),(0x20025d7,37,1),(0x20025d8,255,1),(0x2001696,0,1),(0x2001697,0,1),(0x2005f20,0,1),(0x2001688,0 if cur==-1 else 0x2009100,4),(0x200168c,0 if alt==-1 else 0x2009120,4),(0x2009101,cur,1),(0x2009121,alt,1)]:p.write(a,v,n)
 p.run(entry=0x801123c,stop=0x801144e)
 def s8(a):
  v=p.read(a,1);return v if v<128 else v-256
 actual=[p.read(0x2002604),s8(0x20025d6),s8(0x2002610),s8(0x20025d7),p.read(0x20025d8,1),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,((cur,alt,sel),actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='All eight customer types and absent pointers in both original1688/168C roles, selection17/18/21/22 boundaries and actual1123C..1144C ordered start request. Invalid types are native transactional guards, not claimed original behavior. Native scene maps current to trip.active_customer and alternate to pickup.dialogue_index (selected_index deliberately differs). No live host or audible gameplay claim.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_context_voice.c','src/game_audio_context_voice.h','tests/game_audio_customer_class_rom_compare.c','tools/compare_game_audio_customer_class_rom.py']})
(root/'reports/game-audio-customer-class-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
