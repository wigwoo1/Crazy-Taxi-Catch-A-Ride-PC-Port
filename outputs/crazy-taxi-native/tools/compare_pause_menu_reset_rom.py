from pathlib import Path
import json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for i in ns['md'].disasm(ns['r'][0x2277c:0x22796],0x802277c):ns['allowed_thumb'][i.address]=bytes(i.bytes)
rows=subprocess.run([str(root/'bin/pause-menu-reset-rom-compare.exe')],input=''.join(f'{s}\n' for s in range(256)),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==256
mapping=[(0x2005f20,1),(0x2005f21,1),(0x2005f22,1),(0x2005f23,1),(0x2001ea8,1),(0x2005f1a,1),(0x2005f24,1),(0x2005f19,1),(0x2005f1c,2),(0x2005f1e,2)];steps=0
for seed,row in enumerate(rows):
 p=ns['InitProbe']((0,0,0,0,0,1,0,0,0,0));del p.regions[0x2005f20];p.regions[0x2005f10]=bytearray((seed+i*37)&255 for i in range(64));before=bytes(p.regions[0x2005f10])
 initial=[(seed+i)&255 for i in range(8)]+[seed*257,-seed*257]
 for (a,n),v in zip(mapping,initial):p.write(a,v,n)
 before=bytes(p.regions[0x2005f10]);p.reg[14]=0x10020001;p.run(entry=0x802277c,stop=0x10020000)
 got=[]
 for a,n in mapping:
  v=p.read(a,n);got.append(v-(1<<(8*n)) if n==2 and v&(1<<(8*n-1)) else v)
 assert got==list(map(int,row.split())),(seed,got,row)
 expected=bytearray(before)
 for a,v in [(0x2005f20,0),(0x2005f21,0),(0x2005f22,0),(0x2005f23,1),(0x2005f1a,0)]:expected[a-0x2005f10]=v
 assert bytes(p.regions[0x2005f10])==bytes(expected),seed;steps+=p.steps
report={'status':'PASS_ORIGINAL_PAUSE_RESET','cases':256,'thumb_instructions':steps,'scope':'Complete original2277C five-byte reset and all59 neighbor bytes retained across256 nonzero prestates; all10native menu fields exact including retained outer/palette/view/zoom. No live pause/retry/hardware proof.'}
(root/'reports/pause-menu-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
