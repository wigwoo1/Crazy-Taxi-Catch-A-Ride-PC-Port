exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x26714:0x26738],0x8026714):allowed_thumb[ins.address]=bytes(ins.bytes)
assert struct.unpack_from('<I',r,0x267ac)[0]==0x2007574
cases=[0,1,2147483647,4294967295];cp=subprocess.run([str(root/'bin/audio-channel-init-rom-compare.exe')],input=''.join(str(x)+'\n' for x in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];steps=0
for seed,want in zip(cases,rows):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));base=0x2007574
 for j in range(4):
  b=base+j*32
  for off,v,n in [(0,(seed+j)&0xffffffff,4),(8,456,2),(0x14,789,4),(0x10,1011,4),(4,1,1),(5,2,1),(6,64,1),(7,7,1),(0x18,0x8100000,4)]:p.write(b+off,v,n)
 p.run(entry=0x8026714,stop=0x8026738);actual=[]
 for j in range(4):
  b=base+j*32;actual +=[int(bool(p.read(b+0x18))),p.read(b),p.read(b+8,2),p.read(b+0x14),p.read(b+0x10),p.read(b+4,1),p.read(b+5,1),p.read(b+6,1),p.read(b+7,1)]
 assert actual==want,(seed,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,scope='Original26714..26736 fourchannel initialization only. Retained age/rate/phase/step/volume/last; full audio bootstrap excluded.',rom_sha256=hashlib.sha256(r).hexdigest(),source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/audio_channel.c','src/audio_channel.h','tests/audio_channel_init_rom_compare.c','tools/compare_audio_channel_init_rom.py']})
(root/'reports/audio-channel-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
