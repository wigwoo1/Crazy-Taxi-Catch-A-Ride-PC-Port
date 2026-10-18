"""Original slot4 vehicle loop ownership, explicit audio callee boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for a,b in [(0x10f10,0x10f6a),(0x10f94,0x10fb8)]:
 for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[a:b],0x8000000+a):allowed_thumb[ins.address]=bytes(ins.bytes)
class VehicleLoopProbe(UpdateProbe):
 def external(self,target):
  h=self.reg[0]&0xffffffff;source=struct.unpack_from('<I',r,0x7ec658+58*4)[0]
  if target==0x80267c8:
   assert h==source and self.reg[1:3]==[1,1];self.calls.append([1,58,257]);self.reg[0]=self.start;return
  if target==0x80277a4:
   assert self.reg[1]==source;self.calls.append([6,h,58]);self.reg[0]=self.busy;return
  assert target in [0x8027610,0x80276cc],hex(target)
  self.calls.append([4 if target==0x8027610 else 2,h,0 if target==0x8027610 else signed(self.reg[1])])
cases=[]
for handle,state,flags,gates,busy,start in itertools.product([0,0x2009000],[0,1,-2147483648],[0,8,255],range(8),[0,255],[0,0x2009020]):
 cases.append((handle,state,flags,gates&1,(gates>>1)&1,(gates>>2)&1,busy,start))
cp=subprocess.run([str(root/'bin/game-audio-vehicle-loop-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 h,state,flags,freeze,end,fade,busy,start=c;p=VehicleLoopProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.busy=busy;p.start=start
 for a,v,n in [(0x20025f8,h,4),(0x2002634+0x564,state,4),(0x2002634+0x28,flags,1),(0x2005f20,freeze,1),(0x2001697,end,1),(0x200177b,fade,1)]:p.write(a,v,n)
 p.run(entry=0x8010f10,stop=0x8010fb8)
 actual=[p.read(0x20025f8),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original10F10..10FB6 slot4: vehicle564/airborne bit/pause/end/fade gates, start58 protected+volume16, busy-conditioned stop and stale handle retention. Audio callees captured; actual sound mixing/full controller excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_vehicle_loop.c','src/game_audio_vehicle_loop.h','tests/game_audio_vehicle_loop_rom_compare.c','tools/compare_game_audio_vehicle_loop_rom.py']})
(root/'reports/game-audio-vehicle-loop-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
