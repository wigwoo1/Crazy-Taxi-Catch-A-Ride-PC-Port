exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for x in json.loads((root/'reports/game-audio-live-speed-divisor-audit.json').read_text())['instructions']:allowed_thumb[int(x['address'],16)]=bytes.fromhex(x['bytes'])
cases=list(itertools.product([0,150000,-1],[0,4,8,12],[65536,131072,-65536],[256,512],[-32768,6000],[12000,24000]))
cp=subprocess.run([str(root/'bin/vehicle-speed-divisor-rom-compare.exe')],input=''.join(' '.join(map(str,c+(c[-1]+1000,)))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=list(map(int,cp.stdout.split()));steps=0
for c,want in zip(cases,rows):
 initial,flags,ratio,final,rpm,wheel=c;p=LevelsProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634;p.reg[6]=base;p.reg[10]=base+0x4ac;p.reg[8]=base+0x5b8;p.reg[9]=base+0x5cc
 for off,v,n in [(0x548,initial,4),(0x4ac,flags,1),(0x4b4,1,4),(0x4b8,ratio,4),(0x4d8,final,4),(0x532,rpm,2),(0x5b8,wheel,4),(0x5cc,wheel+1000,4)]:p.write(base+off,v,n)
 p.run(entry=0x8011908,stop=0x801199a);actual=signed(p.read(base+0x548));assert actual==want,(c,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,scope='Actual11908..11998 speed divisor initialization, valid gear_count1; retains nonzero, mode0/1 wheel selection, signed/wrapped arithmetic. Divider explicit boundary; full bootstrap excluded.',rom_sha256=hashlib.sha256(r).hexdigest(),source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/vehicle_config.c','src/vehicle_config.h','src/simulation_start.c','src/drive_world.h','tools/compare_vehicle_speed_divisor_rom.py','tests/vehicle_speed_divisor_rom_compare.c']})
(root/'reports/vehicle-speed-divisor-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
