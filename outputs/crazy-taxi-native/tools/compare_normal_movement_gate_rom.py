"""Original normal-frame movement branch gates, not movement execution."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
for a in range(0x800b012,0x800b022,2):allowed_thumb[a]=r[a-0x8000000:a-0x8000000+2]
saved=allowed_thumb.copy();source=source.replace('   pc=nextpc\n','   pc=nextpc\n  self.stopped_pc=pc\n');exec(source);allowed_thumb.update(saved);UpdateProbe.run=MotionProbe.run
values=[0,1,-1,120,-2147483648,2147483647];cases=[(a,b) for a in values for b in values];native=list(map(int,subprocess.run([str(root/'bin/normal-movement-gate-compare.exe')],input=''.join('%d %d\n'%c for c in cases),text=True,capture_output=True,check=True).stdout.split()));assert len(native)==len(cases);steps=0
for (gate,pending),want in zip(cases,native):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x2001700,gate);p.write(0x2001704,pending);p.run(entry=0x800b012,stop={0x800b022,0x800b08c});actual=int(p.stopped_pc==0x800b022)
 assert actual==want,((gate,pending),p.steps,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,match=True,scope='CompleteoriginalB012..B020 gate branches toB022 driver/world orB08C gear-only, signed32zero semantics. Productiongate notyetconnected; body/emitters skipped but208D4poolupdate continues.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/normal-movement-gate-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
