"""Original command11 complete jump branch, loader/start helper contracts explicit."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_row_restart_rom.py').read().split('orders=')[0])
for item in json.loads((root/'reports/music-row-callback-cfg.json').read_text())['instructions']:
 a=int(item['address'],16)
 if 0x802734c<=a<=0x80273f4:allowed_thumb[a]=bytes.fromhex(item['bytes'])
class JumpProbe(RestartProbe):
 def external(self,t):
  if t==0x8027110:super().external(t);self.write(0x20075f6,1,1)
  else:super().external(t)
cases=[(mode,active,row,choice,param,value) for mode in [0,1,255] for active in [0,1,255] for row in [0,3,255] for choice in range(5) for param in [0,module[1]-1,module[1],255] for value in [0,65535]]
rows=subprocess.run([str(root/'bin/music-jump-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0;restarts=0
for case,line in zip(cases,rows):
 mode,active,row,choice,param,value=case;p=JumpProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16),0x10011000:bytearray([0,0,0,11,param])};p.scalars={0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:active,0x2003887:row,0x2003888:choice,0x20075f6:165};p.calls=[];p.draws=0;p.random=value;p.write(0x2007614,0x8421fc0+0x106)
 p.reg[9]=0x10011000;p.reg[13]=0x10000400;p.run(entry=0x802734c,stop=0x8027490)
 raw,offset,draws,playing=line.split();assert bytes(p.regions[0x2007608][1:12])==bytes.fromhex(raw),(case,p.regions[0x2007608].hex(),raw)
 assert [p.read(0x2007614)-0x8421fc0,p.draws,p.read(0x20075f6)]==list(map(int,[offset,draws,playing])),(case,line,p.scalars)
 assert p.read(0x10000400)==1
 if p.calls:restarts+=1;assert p.calls==([0x801e72c] if not(active and row==3) and not mode else [])+[0x8026ef8,0x8027110]
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'restart_cases':restarts,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual command11 2734C..273F4 branch to7490, soundoverride/mainmode/parameterlimit/conditional RNG choice. Statebytes1..11/patternoffset/playing/jump_seen and ordered source helper requests. Loader/orderstart/RNG explicit contracts; excludes full271E0 multiplechannel continuation/voices/reset/fullaudio IRQ.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_tracker.c',root/'src/music_tracker.h',Path(__file__)]}}
(root/'reports/music-jump-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
