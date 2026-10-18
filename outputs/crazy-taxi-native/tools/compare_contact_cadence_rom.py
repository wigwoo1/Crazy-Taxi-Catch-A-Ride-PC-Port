import sys
sys.path.insert(0,'work/python-packages')
exec(open('outputs/crazy-taxi-native/tools/verify_world_fill_rom.py').read().split('root=pathlib.Path')[0])
root=pathlib.Path('outputs/crazy-taxi-native')
cfg=json.loads((root/'reports/challenge-bound-decay-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
cases=[(91040,flag,calls) for flag in (0,16) for calls in range(1,7)]
cp=subprocess.run([str(root/'bin/contact-cadence-probe.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True)
native=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(native)==len(cases)
steps=0
for (speed,flags,calls),expected in zip(cases,native):
 p=MotionProbe((0,0,0,0,0,1),0)
 for off,val,size in [(0x544,speed,4),(0x40,123456,4),(0x44,-76543,4),(0x2c,99,2),(0x28,flags,1)]:p.write(p.base+off,val,size)
 for i in range(calls):
  p.reg[0]=p.base;p.reg[14]=0x10020001;p.run(entry=0x8015b54,stop=0x10020000)
 got=[signed(p.read(p.base+o)) for o in (0x544,0x40,0x44)]+[p.read(p.base+0x2c,2)]
 assert got==expected,dict(case=(speed,flags,calls),original=got,native=expected);steps+=p.steps
report=dict(status='PASS_ORIGINAL_CONTACT_DECAY_CADENCE',cases=len(cases),thumb_instructions=steps,series=[dict(flags=f,calls=n,outputs=v) for (_,f,n),v in zip(cases,native)],scope='Repeated original15B54 calls vs native production contact-decay routine on explicit isolated scalar fixtures, all outputs exact. Demonstrates update-count dependence, not elapsed-clock equivalence, a captured trajectory or causal explanation of earlier delivery failures.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/contact-cadence-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
