exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import subprocess,random
for i in md.disasm(r[0x31aee:0x31b00],0x8031aee):allowed_thumb[i.address]=bytes(i.bytes)
rng=random.Random(0x31aee);cases=[0,1,0x80000,0xc0000,0x7fffffff,0x80000000,0xffffffff]+[rng.getrandbits(32) for _ in range(256)]
cp=subprocess.run([str(root/'bin/passenger-marker-dimensions-rom-compare.exe')],input=''.join(str(s)+'\n' for s in cases),text=True,capture_output=True,check=True);outputs=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(outputs)==len(cases);steps=0
for scale,expected in zip(cases,outputs):
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0));desc=0x2005f2c;p.write(desc,64,1);p.write(desc+1,64,1);p.reg[0]=64;p.reg[1]=desc;p.reg[10]=scale;p.run(entry=0x8031aee,stop=0x8031b00)
 actual=[signed(p.reg[3]),signed(p.reg[2])];assert actual==expected,(scale,actual,expected);steps+=p.steps
report={'cases':len(cases),'thumb_instructions':steps,'match':True,'scope':'Original31AEE..31AFE dimensions for64x64 marker descriptors vs native helper used by marker billboard. Wrapped scale product and signedASR exact. Fullcamera/projection/depth/rasterization not compared.'}
(root/'reports/passenger-marker-dimensions-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
