exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
mapping=[(33560209, 1, False), (33560211, 1, False), (33560216, 4, True), (33560220, 4, True), (33560224, 4, True), (33560228, 4, True), (33560232, 4, True), (33560236, 4, True), (33560240, 4, True), (33560288, 4, True), (33560212, 1, False), (33560213, 1, False), (33560210, 1, False), (33560272, 1, False), (33560273, 1, False), (33560274, 2, False), (33560296, 4, True), (33560292, 4, True), (33560300, 4, True), (33560320, 4, True), (33560215, 1, False), (33560276, 1, False), (33560252, 2, True), (33560254, 2, True), (33560256, 2, True), (33560258, 1, False), (33560260, 4, True), (33560264, 4, True)]
cases=[(mode,seconds,seed) for mode in range(0,256,32) for seconds,seed in [(0,1),(90,123456),(255,0x80000000)]]
rows=subprocess.run([str(root/'bin/session-init-fare-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 mode,seconds,seed=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0))
 for a,n in regions:p.regions[a]=bytearray(n)
 for a,n,sgn in mapping:p.write(a,int.from_bytes(bytes([0xa5])*n,'little'),n)
 p.scalars={0x02001ea8:0xa5,0x0200430c:0,0x030067d0:0,0x02007542:240}
 p.write(0x02001534,0xa5,1);p.write(0x02001688,0x02005f2c+3*44);p.write(0x0200753c,77)
 for a in [0x02005310,0x02005312,0x02005314]:p.write(a,0xa5a5,2)
 p.write(0x020016a8,seed);p.write(0x02001eb0,mode,1);p.write(0x02001eb5,seconds,1)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x0800b6ec,stop=0x10020000)
 got=[]
 for a,n,sgn in mapping:
  value=p.read(a,n)
  if sgn and value>=(1<<(n*8-1)):value-=1<<(n*8)
  got.append(value)
 assert p.read(0x02001688)==0
 got += [p.read(0x02001ea8,1),p.read(0x02001534,1),-1,signed(p.read(0x0200753c)),p.read(0x02005310,2),p.read(0x02005312,2),p.read(0x02005314,2)]
 assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'CompleteB6EC executed vs35existingfare/trip/expiry fields, allmodefamilies andretainedclock/initialtriptime/status; native ranking/tip/reward/voice fields retained. Otherlivehost ownerbridges excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/session-init-fare-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
