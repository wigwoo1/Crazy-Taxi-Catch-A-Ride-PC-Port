exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x2763c:0x276c2],0x802763c):allowed_thumb[i.address]=bytes(i.bytes)
base=struct.unpack_from('<I',r,0x276c4)[0];assert base==0x2007574
cases=list(itertools.product(range(16),[0,10512,65535]))
cp=subprocess.run([str(root/'bin/audio-stop-all-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];steps=0
for (mask,rate),want in zip(cases,rows):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x2007564,rate)
 for j in range(4):
  b=base+j*32
  for off,v,n in [(0,123+j,4),(8,456,2),(0x14,789,4),(0x10,1011,4),(4,1,1),(5,2,1),(6,64,1),(7,7,1),(0x18,0x8100000 if mask&(1<<j) else 0,4),(0xc,13,4),(0x1c,17,4)]:p.write(b+off,v,n)
 p.reg[14]=0x10020001;p.run(entry=0x802763c,stop=0x10020000);actual=[]
 for j in range(4):
  b=base+j*32;actual +=[int(bool(p.read(b+0x18))),p.read(b),p.read(b+8,2),p.read(b+0x14),p.read(b+0x10),p.read(b+4,1),p.read(b+5,1),p.read(b+6,1),p.read(b+7,1)]
 assert actual==want,((mask,rate),actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,scope='Complete2763C represented fourchannel state: active rate reset, retained age/volume, inactive untouched. Not host integration.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/audio-stop-all-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)

