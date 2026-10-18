"""Original auxiliary sound owners with explicit callee boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for a,b in [(0x10e1c,0x10e9a),(0x10ebc,0x10f10)]:
 for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[a:b],0x8000000+a):allowed_thumb[ins.address]=bytes(ins.bytes)
class AuxProbe(UpdateProbe):
 def external(self,target):
  h=self.reg[0]&0xffffffff
  if target==0x8015d98:
   assert h==3;self.calls.append([5,0,3]);self.reg[0]=self.action;return
  if target==0x80267c8:
   ix=next(i for i in [56,58] if struct.unpack_from('<I',r,0x7ec658+i*4)[0]==h)
   assert self.reg[1:3]==([1,1] if ix==58 else [0,0])
   self.calls.append([1,ix,257 if ix==58 else 0]);self.reg[0]=self.starts[ix];return
  if target==0x80277a4:
   assert self.reg[1]==struct.unpack_from('<I',r,0x7ec658+56*4)[0]
   self.calls.append([6,h,56]);self.reg[0]=self.busy;return
  assert target in [0x8027610,0x80276cc],hex(target)
  self.calls.append([4 if target==0x8027610 else 2,h,0 if target==0x8027610 else signed(self.reg[1])])
cases=[]
for speed,loop,one,pending,action,over,gates,busy,failed in itertools.product([-2147483648,-65,-64,-1,0,1,64,65],[0,0x2009000],[0,0x2009020],[0,255],[0,255],[0,1],[0,1,2,4,7],[0,255],[0,1]):
 cases.append((loop,one,pending,speed,action,over,gates&1,(gates>>1)&1,(gates>>2)&1,busy,0 if failed else 0x2009040,0 if failed else 0x2009060))
cp=subprocess.run([str(root/'bin/game-audio-aux-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 loop,one,pending,speed,action,over,intro,end,frozen,busy,ls,os=c;p=AuxProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.action=action;p.busy=busy;p.starts={58:ls,56:os};p.reg[9]=speed&0xffffffff
 for a,v,n in [(0x20025f0,loop,4),(0x20025f4,one,4),(0x20025d2,pending,1),(0x2002620,over,1),(0x2001696,intro,1),(0x2001697,end,1),(0x2005f20,frozen,1)]:p.write(a,v,n)
 p.run(entry=0x8010e1c,stop=0x8010f10)
 actual=[p.read(0x20025f0),p.read(0x20025f4),p.read(0x20025d2,1),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original10E1C..10F0E slots2/3, pending flag and ordered action/start/volume/stop/busy requests. Callees explicit boundaries; full sound controller and actual mixing excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_aux.c','src/game_audio_aux.h','tests/game_audio_aux_rom_compare.c','tools/compare_game_audio_aux_rom.py']})
(root/'reports/game-audio-aux-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
