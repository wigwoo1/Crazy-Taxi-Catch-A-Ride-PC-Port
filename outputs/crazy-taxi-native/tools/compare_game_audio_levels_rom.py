"""Original gameplay sound prefix10B54..10C56, before handle operations."""
exec(open('outputs/crazy-taxi-native/tools/compare_audio_stop_one_rom.py').read().split('\ncases=')[0])
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x10b54:0x10c58],0x8010b54):allowed_thumb[i.address]=bytes(i.bytes)
def signed(v):
 v&=0xffffffff
 return v if v<0x80000000 else v-0x100000000
class LevelsProbe(UpdateProbe):
 def external(self,target):
  assert target==0x8032488,hex(target)
  a,b=map(signed,self.reg[:2]);assert b
  q=abs(a)//abs(b);q=-q if (a<0)!=(b<0) else q
  self.reg[0]=q&0xffffffff
cases=[]
for speed,factor,ended,outer in itertools.product([-2147483648,-1138,-1000,0,90,99,100,240,249,250,500,509,510,1000,2147483647],[0,63,64,256,32767,-32768],[0,1],[0,1,2,3]):
 cases.append((speed,1000,4096,37,3,1234,factor,0,ended,outer))
cases.extend((1000,1000,-2147483648,cooldown,delta,-32768,256,mute,1,0) for cooldown,delta,mute in itertools.product([0,1,-1,2147483647,-2147483648],[1,60],[0,1,255]))
cp=subprocess.run([str(root/'bin/game-audio-levels-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 speed,maxspeed,scale,cooldown,delta,rpm,factor,mute,ended,outer=case;p=LevelsProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v,n in [(0x2002614,cooldown,4),(0x2003b00,delta,4),(0x2002634+0x544,speed,4),(0x2002634+0x548,maxspeed,4),(0x2002634+0x538,rpm,2),(0x2002634+0xe84,scale,4),(0x20025da,factor,2),(0x2001534,mute,1),(0x2001697,ended,1),(0x2001ea8,outer,1)]:p.write(a,v,n)
 p.run(entry=0x8010b54,stop=0x8010c58)
 f=p.read(0x20025da,2);f=f if f<32768 else f-65536
 actual=[signed(p.read(0x2002614)),signed(p.reg[9]),signed(p.reg[4]),signed(p.reg[5]),signed(p.reg[7]),signed(p.reg[8]),f]
 assert actual==want,(case,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original10B54..10C56 cooldown/speed/volumes/pitch/fade prefix; signed divider explicit mathematical boundary. Handle operations and actual audio excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_levels.c','src/game_audio_levels.h','tests/game_audio_levels_rom_compare.c','tools/compare_game_audio_levels_rom.py']})
(root/'reports/game-audio-levels-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
