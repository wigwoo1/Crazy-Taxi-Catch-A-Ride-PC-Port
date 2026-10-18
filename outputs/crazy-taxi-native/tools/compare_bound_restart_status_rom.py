from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for i in ns['md'].disasm(ns['r'][0x743c:0x7450],0x800743c):ns['allowed_thumb'][i.address]=bytes(i.bytes)
cases=[(i,(i*37)&255,255-i) for i in range(256)]
rows=subprocess.run([str(root/'bin/challenge-restart-status-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 p=ns['InitProbe']((0,0,0,0,0,1,0,0,0,0));p.scalars={0x2001691:case[0]};p.write(0x200153c,case[1],1);p.write(0x200153d,case[2],1);p.run(entry=0x800743c,stop=0x8007450)
 assert [p.read(a,1) for a in [0x2001691,0x200153c,0x200153d]]==list(map(int,row.split()));steps+=p.steps
report={'status':'PASS_NATIVE_BOUND_RESTART_STATUS','cases':len(cases),'thumb_instructions':steps,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'Actual743C..744E direct pre-bootstrap3status stores; noexternalcallee executed, input andremainingbootstrap excluded.'}
(root/'reports/bound-restart-status-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
