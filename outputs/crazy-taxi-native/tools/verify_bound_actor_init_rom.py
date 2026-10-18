from pathlib import Path
import json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native')
source=(root/'tools/probe_bound_actor_init_dependencies.py').read_text()
prefix,body=source.split("p=ns['Probe']",1)
ctx={};exec(prefix,ctx)
body="p=ns['Probe']"+body
body=body.split("report={'status'")[0]
body=body.replace("p.write(0x2001eb0,128,1)","p.write(0x2001eb0,128,1);p.write(0x2001537,5,1)\nfor j in range(44):p.write(33578796+j,(seed+j*37)&255,1)")
rows=[]
for seed in (0,1,85,165,255):
 ctx['seed']=seed;ctx['ground_calls']=[];exec(body,ctx)
 assert ctx['dependency'] is None
 p=ctx['p'];calls=ctx['ground_calls'];assert len(calls)==1
 got=[p.read(33578796+j,1) for j in range(44)]
 result=subprocess.run([str(root/'bin/bound-actor-init-rom-compare.exe')],input=f"{seed} {calls[0]['height']}\n",text=True,capture_output=True,check=True,timeout=10)
 expected=list(map(int,result.stdout.split()));assert got==expected,dict(seed=seed,original=got,native=expected)
 rows.append(dict(seed=seed,caller_instructions=p.steps,ground=calls,actor=got))
report=dict(status='PASS',cases=len(rows),rom_sha256=hashlib.sha256(ctx['rom']).hexdigest(),rows=rows,scope='Actual227AC reaches return on real Bound descriptor/settings128/submode5; all44 actor bytes match compiled native challenge initializer over five prior-record seeds; all66 input shadow bytes retained. Original29B44 executed separately with bounded containment/division contracts, height returned to caller. Modulo/division register contracts and D764 display clear no-state remain explicit. Not shared-memory whole bootstrap, warm live gameplay, hardware, or full game fidelity.')
(root/'reports/bound-actor-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',report['cases'],'original/native44-byte actor cases; input retained')
