"""Original1144E..11468 amplitude ramp, no callee substitution."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x1144e:0x1146a],0x801144e):allowed_thumb[ins.address]=bytes(ins.bytes)
cases=list(itertools.product([0,16383,16384,16385,-1,-2147483648,2147483647],[0,1,60,-2147483648,2147483647]))
cp=subprocess.run([str(root/'bin/game-audio-amplitude-rom-compare.exe')],input=''.join('%d %d\n'%c for c in cases),text=True,capture_output=True,check=True)
rows=list(map(int,cp.stdout.split()));assert len(rows)==len(cases);steps=0
for (a,d),want in zip(cases,rows):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x20025dc,a);p.write(0x2003b00,d);p.run(entry=0x801144e,stop=0x801146a)
 assert signed(p.read(0x20025dc))==want,((a,d),want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original exact1144E..11468 amplitude25DC wrapped rise/cap16384, no helper boundary, not actual audio waveform',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_voice.c','tests/game_audio_amplitude_rom_compare.c','tools/compare_game_audio_amplitude_rom.py']})
(root/'reports/game-audio-amplitude-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
