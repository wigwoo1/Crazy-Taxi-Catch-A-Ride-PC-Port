from pathlib import Path
import subprocess,json,hashlib
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x1f022:0x1f030],0x0801f022):allowed_thumb[i.address]=bytes(i.bytes)
binary=root/'bin/hud-cash-palette-sources-probe.exe';seeds=list(range(0,256,4));rows=subprocess.run([str(binary)],input=''.join(str(s)+'\n' for s in seeds),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(seeds)
steps=0
for seed,row in zip(seeds,rows):
 data=bytearray((seed+i*31+b*17)&255 for i in range(128) for b in range(20));p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02001790:data};p.scalars={};p.reg[3]=0x02001790;p.run(entry=0x0801f022,stop=0x0801f030);steps+=p.steps;assert bytes(data)==bytes.fromhex(row)
report={'status':'PASS','cases':len(seeds),'registry_bytes_per_case':2560,'original_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Original1F022..1F030 pointer stores only, seeded full registry bytes retained except record9/10 source pointers. Not complete initialization or captured live registration proof.','source_sha256':hashlib.sha256((root/'src/hud_cash_pixels.c').read_bytes()).hexdigest()};(root/'reports/hud-cash-palette-sources-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(seeds),'cash palette pointer switches against original ROM; all2560 registry bytes exact per case')
