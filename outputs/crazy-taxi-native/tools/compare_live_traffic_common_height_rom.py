"""Original traffic common-height query on actual frame actor points."""
exec(open('outputs/crazy-taxi-native/tools/compare_live_full_traffic_postimpact_world_rom.py').read().split('\nrows=[];query_groups=[];pending=[]')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/effect-height-cfg.json').read_text())['instructions']})
f=root/'reports/current-traffic-frame-states.txt';lines=f.read_text().splitlines();p=FollowProbe((0,0,0,0,0,1,0,0,0,0));p.call_counts={};p.arm.mem[0x3006810]=0x87556cc;p.arm.mem[0x3006940]=0x8032489;point=0x10003000;cases=steps=0
for before,after in zip(lines[::2],lines[1::2]):
 pre,post=[list(map(int,l.split()[1:])) for l in [before,after]]
 for i in range(pre[1]):
  a=pre[50+i*18:50+(i+1)*18];b=post[50+i*18:50+(i+1)*18]
  if b[2]>0:continue
  assert abs(b[3]-pre[12])<=23040 and abs(b[4]-pre[14])<=23040
  for j,v in enumerate([b[3],a[12],b[4]]):p.write(point+j*4,v)
  p.steps=0;p.reg[0]=point;p.reg[14]=0x10020001;p.run(entry=0x8029b44,stop=0x10020000)
  assert signed(p.reg[0])==b[12],dict(frame=pre[0],actor=i,point=[b[3],a[12],b[4]],original=signed(p.reg[0]),native=b[12]);cases+=1;steps+=p.steps
report=dict(cases=cases,pass_match=True,thumb_instructions=steps,original_call_counts={hex(k):v for k,v in sorted(p.call_counts.items())},rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original29B44 commonheight with ROMcity1 geometry vs actual native frame finalactorY. QuerypostmovementXZ/prioractorY; allcommon-gate passing actors including spawn/normal/recovery. OriginalARMcontainment arithmetic via boundedcallee-save abstraction, divisioncontract. No complete66B4 continuousactor-memory equivalence.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_live_traffic_common_height_rom.py',f,city_world,root/'src/traffic_presentation.c',root/'reports/effect-height-cfg.json']}
(root/'reports/live-traffic-common-height-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
