"""Active original brake differential from real configuration states plus controlled inputs."""
original=open('outputs/crazy-taxi-native/tools/compare_bound_brake_rom.py').read();exec(original.split('steps=actions=0')[0])
keys=list(fields);inputs=['delta','settings','flag_1697','flag_153c','action2','action3','sound_enabled','freeze']
def expr(key):
 if key.startswith('gears_'):return 'v.gears.'+key[6:]
 if key.startswith('gear_ratio_'):return 'v.gears.ratios['+key.split('_')[-1]+']'
 if key.startswith('curve_'):return 'v.curve['+key.split('_')[-1]+']'
 return 'v.'+key
assign=''.join(expr(key)+'=values[%d];'%i for i,key in enumerate(keys));n=len(keys)
code='#include "../src/braking.h"\n#include <stdio.h>\nint main(void){int values[%d];while(scanf("%%d",&values[0])==1){for(unsigned i=1;i<%d;i++)if(scanf("%%d",&values[i])!=1)return 2;TaxiAcceleration v={0};'% (n+8,n+8)+assign
code+='TaxiBrakeInput in={'+','.join('.'+key+'=values[%d]'%(n+i) for i,key in enumerate(inputs[:-1]))+'};uint8_t freeze=values[%d];TaxiBrakeEffects e={0};int rc=taxi_brake_update(&v,&in,&freeze,&e);printf("%%d",rc);int out[]={'%(n+7)+','.join(expr(key) for key in keys)+',freeze,e.rpm.count,e.rpm.invalid_mode,e.sound_requested,e.sound_index};for(unsigned i=0;i<sizeof out/sizeof out[0];i++)printf(" %d",out[i]);printf("\\n");}return 0;}\n'
(root/'tests/active_braking_rom_compare.c').write_text(code)
build=['work/compiler/ziglang/zig.exe','cc','-O2','-Wall','-Wextra','-Werror',str(root/'tests/active_braking_rom_compare.c'),str(root/'src/braking.c'),str(root/'src/gears.c'),'-o',str(root/'bin/active-braking-rom-compare.exe')];subprocess.run(build,check=True)
observed=[dict(zip(schema,row[1:])) for row in rows];cases=[]
for d in observed:
 for throttle in [0,1]:
  c=d.copy();c['input_action3']=1;c['input_action2']=throttle;cases.append(c)
for speed in [-500000,-100266,-1000,-1,0,1,1000,100266,500000]:
 for reverse in [0,32]:
  for request in [0,1]:
   for throttle in [0,1]:
    for settings,ended,gate in [(128,0,0),(128,1,0),(128,1,1)]:
     c=observed[0].copy();c.update(input_gears_speed_544=speed,input_gears_flags_4ac=(c['input_gears_flags_4ac']&~32)|reverse,input_action3=request,input_action2=throttle,input_settings=settings,input_flag_1697=ended,input_flag_153c=gate);cases.append(c)
res=subprocess.run([str(root/'bin/active-braking-rom-compare.exe')],input=''.join(' '.join(str(d['input_'+key]) for key in keys+inputs)+'\n' for d in cases),text=True,capture_output=True,check=True);native=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(native)==len(cases)
rows=[]
for idx,(d,row) in enumerate(zip(cases,native)):
 assert row[0]==0,(idx,row[0]);d=d.copy()
 for key,v in zip(keys,row[1:1+n]):d['output_'+key]=v
 for key,v in zip(['output_freeze','effects_count','effects_invalid_mode','sound_requested','sound_index'],row[1+n:]):d[key]=v
 rows.append([idx]+[d[key] for key in schema])
body=original[original.index('steps=actions=0'):original.index('report=dict')];exec(body)
report=dict(cases=len(cases),brake_pressed_cases=sum(d['input_action3'] for d in cases),thumb_instructions=steps,action_query_calls=actions,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original full1403C+nestedRPM vsfreshnative activebrake variants onrealcapturedconfiguration;71stateoutputs/freeze/effects.248forcedbrake/throttlevariants plus216signedspeed/reverse/gatevariants. Controlledstates notnaturaltrajectory; signed/unsigneddivision, BIOSsqrt, action2/3fixtures,sounddisabled contracts.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/verify_active_braking_rom.py',root/'tests/active_braking_rom_compare.c',root/'src/braking.c',root/'src/gears.c',root/'bin/active-braking-rom-compare.exe',root/'reports/bound-corrected-brake-states.txt']};(root/'reports/active-braking-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
