"""Original driver inline steering input vsnative."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-driver-loop-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
cases=[(analog,old,flags,freeze,phase,timer) for analog in [0,-1,1,-7,7,-8,8,-191,191,-192,192,-193,193,-4095,4095,-4096,4096,-32768,32767] for old in [-1536,-32,0,32,1536] for flags in [0,1,2,3,0x96,0xff] for freeze in [0,1] for phase in [0,1] for timer in [0,45]]
res=subprocess.run([str(root/'bin/steering-input-current-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0;mismatches=[]
for case,row in zip(cases,rows):
 analog,old,flags,freeze,phase,timer=case;probe=MotionProbe((0,0,0,0,0,1),0);cmd=probe.base+0xf00;probe.reg[13]-=128;probe.reg[4]=cmd;probe.reg[5]=probe.base+0x4ac;probe.reg[10]=0x30053c0;probe.write(probe.reg[13]+0x20,0);probe.write(probe.reg[13]+0x14,probe.base+0xe6a)
 for off,v,n in [(0x578,old,2),(0x57a,64,2),(0xe6a,phase,1),(0xe6b,8,1)]:probe.write(probe.base+off,v,n)
 probe.write(cmd+4,flags,1);probe.write(cmd+12,analog,2);probe.arm.mem[0x2002620]=freeze;probe.arm.mem[0x2002630]=timer;probe.run(entry=0x8011b64,stop=0x8011d50);v=probe.read(probe.base+0x578,2);got=[v-65536 if v&32768 else v,probe.read(probe.base+0xe6a,1),probe.read(probe.base+0xe6b,1),probe.read(cmd+4,1)];steps+=probe.steps
 if got!=row:mismatches.append(dict(case=case,original=got,native=row))
report=dict(cases=len(cases),thumb_instructions=steps,mismatch_count=len(mismatches),first_mismatches=mismatches[:12],rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original11B64..11D4E onexplicitouterstackfixture vsnative steering_input; analog/deadzone/digital/freeze/phase cases; notfullouterdriver.')
(root/'reports/bound-command-fixed-steering-rom-comparison.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2));assert not mismatches
