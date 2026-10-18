"""Whole original stop/pause/resume wrappers versus native playing-byte ownership."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
for start,end in [(0x27730,0x27738),(0x2773c,0x2774e),(0x27754,0x27766)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
rows=[list(map(int,x.split())) for x in subprocess.run([str(root/'bin/music-playback-rom-compare.exe')],capture_output=True,text=True,check=True).stdout.splitlines()]
assert len(rows)==768;steps=0
for command,before,want in rows:
 p=DispatchProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x20075f6,before,1);p.reg[14]=0x10020001
 p.run(entry=[0x8027730,0x802773c,0x8027754][command],stop=0x10020000)
 assert p.read(0x20075f6,1)==want,(command,before,want);steps+=p.steps
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':768,'thumb_instructions':steps,'equal':True,'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/music_playback.c','src/music_playback.h','tests/music_playback_rom_compare.c','tools/compare_music_playback_rom.py']},'scope':'Wholeoriginal27730 stop/2773C pause/27754 resume throughreturn for all256 playing-byte values. Exact sharedbyte75F6 ownership; no waveform/device/music tick/full pause caller proof.'}
(root/'reports/music-playback-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
