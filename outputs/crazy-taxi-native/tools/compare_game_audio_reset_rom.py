"""Original11490 handle reset; other audio settings remain outside native API."""
exec(open('outputs/crazy-taxi-native/tools/compare_audio_stop_one_rom.py').read().split('\ncases=')[0])
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x11490:0x114f0],0x8011490):allowed_thumb[i.address]=bytes(i.bytes)
cases=[0,1,2,65535,65536,0x7fffffff,0x80000000,0xffffffff]
cp=subprocess.run([str(root/'bin/game-audio-reset-rom-compare.exe')],input=''.join(str(c)+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for seed,want in zip(cases,rows):
 p=ExitProbe((0,0,0,0,0,1,0,0,0,0))
 for a in range(0x20025d0,0x2002610,4):p.write(a,0xa5a5a5a5)
 for i in range(10):p.write(0x20025e8+i*4,(seed*(i+1)+0x12340000+i)&0xffffffff)
 p.reg[14]=0x10020001;p.run(entry=0x8011490,stop=0x10020000)
 actual=[p.read(0x20025e8+i*4) for i in range(10)];assert actual==want,(seed,actual,want);steps+=p.steps
 # Full routine observations are recorded, but not falsely exposed by handle API.
 assert p.read(0x20025da,2)==256 and p.read(0x20025d6,1)==255 and p.read(0x20025d7,1)==255
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original whole11490 executed; native comparison only ten handle slots, preserving slot5, other audio reset fields observed but not implemented by this API',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_cleanup.c','tests/game_audio_reset_rom_compare.c','tools/compare_game_audio_reset_rom.py']})
(root/'reports/game-audio-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
