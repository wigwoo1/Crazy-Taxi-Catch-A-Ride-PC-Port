"""Original full sound initialization compares every represented controller field."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_controller_rom.py').read().split('\nrng=random.Random')[0])
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x11490:0x114f0],0x8011490):allowed_thumb[i.address]=bytes(i.bytes)
seeds=list(range(256))+[65535,65536,2147483647,2147483648,4294967295]
cp=subprocess.run([str(root/'bin/game-audio-controller-reset-rom-compare.exe')],input=''.join(str(c)+'\n' for c in seeds),capture_output=True,text=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(seeds);steps=0
for seed,row in zip(seeds,rows):
 assert len(row)==50;p=UpdateProbe((0,0,0,0,0,1,0,0,0,0))
 for k,v in zip(sf,row[:24]):a,n,_=smap[k];p.write(a,v,n)
 p.write(0x20025e0,row[24]);p.reg[14]=0x10020001;p.run(entry=0x8011490,stop=0x10020000)
 actual=[]
 for k in sf:
  a,n,sgn=smap[k];v=p.read(a,n);v=v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v;actual.append(v)
 actual.append(signed(p.read(0x20025e0)));assert actual==row[25:],(seed,actual,row[25:]);steps+=p.steps
report=dict(cases=len(seeds),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original11490..114EE reset versus all24represented native sound controller fields plus25E0. Proves retained slot5/cooldown2614/reward25D5/last25D8/class2610, exact selectors/amplitude/factor/flags. Does not stop channels or qualify lifecycle callers.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_reset_state.c','src/game_audio_controller.h','tests/game_audio_controller_reset_rom_compare.c','tools/compare_game_audio_controller_reset_rom.py']})
(root/'reports/game-audio-controller-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
