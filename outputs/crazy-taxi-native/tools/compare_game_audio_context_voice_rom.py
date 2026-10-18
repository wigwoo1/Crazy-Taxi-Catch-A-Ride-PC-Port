"""Original contextual voice block, audio and RNG explicit boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for ins in json.loads((root/'reports/context-voice-cfg.json').read_text())['instructions']:
 allowed_thumb[int(ins['address'],16)]=bytes.fromhex(ins['bytes'])
class ContextProbe(UpdateProbe):
 def external(self,target):
  if target==0x801e72c:
   assert self.reg[:2]==[0,65535];self.calls.append([8,0,65535]);self.reg[0]=self.random&0xffffffff;return
  if target==0x8032520:
   a,b=map(signed,self.reg[:2]);q=abs(a)//abs(b);q=-q if (a<0)!=(b<0) else q;self.reg[0]=(a-q*b)&0xffffffff;return
  source=self.reg[0] if target==0x80267c8 else self.reg[1]
  matches=[x for x in range(203) if struct.unpack_from('<I',r,0x7ec658+x*4)[0]==source]
  assert len(matches)==1,(hex(target),hex(source),matches)
  index=matches[0]
  if target==0x80267c8:
   assert self.reg[1:3]==[0,0];self.calls.append([1,index,0]);self.reg[0]=self.start;return
  assert target==0x80277a4,hex(target)
  self.calls.append([6,self.reg[0]&0xffffffff,index]);self.reg[0]=self.busy
cases=[]
for h,dh,sel,cls,last,gates,busy,start in itertools.product([0,0x2009000],[0,0x2009020],[-1,0,6,18,21,22],[-1,0,1],[0,2,4,6,9,12,18,255],range(8),[0,255],[0,0x2009040]):
 cases.append((h,dh,sel,cls,37,last,gates&1,(gates>>1)&1,(gates>>2)&1,busy,0,1,start,65535))
# Missing customers, classification lookup variants, and RNG boundary residues.
for cur,alt,sel,random in itertools.product([-1,0,1],[-1,0,1],[0,18,21],[0,1,2,65534]):
 cases.append((0,0,sel,1,37,9,0,0,0,0,cur,alt,0x2009040,random))
for last,random in itertools.product(range(24),[0,1,2,65534]):cases.append((0x2009000,0,0,1,37,last,0,0,0,0,0,1,0,random))
cp=subprocess.run([str(root/'bin/game-audio-context-voice-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
assert list(r[0x4a5544:0x4a554c])==[1,0,0,1,0,0,1,1]
for c,want in zip(cases,rows):
 h,dh,sel,cls,follow,last,intro,end,freeze,busy,cur,alt,start,random=c;p=ContextProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.busy=busy;p.start=start;p.random=random
 for a,v,n in [(0x2002604,h,4),(0x2002608,dh,4),(0x20025d6,sel,1),(0x2002610,cls,1),(0x20025d7,follow,1),(0x20025d8,last,1),(0x2001696,intro,1),(0x2001697,end,1),(0x2005f20,freeze,1),(0x2001688,0 if cur==-1 else 0x2009100,4),(0x200168c,0 if alt==-1 else 0x2009120,4),(0x2009101,1 if cur==0 else 0,1),(0x2009121,1 if alt==0 else 0,1)]:p.write(a,v,n)
 p.run(entry=0x801123c,stop=0x801144e)
 def s8(a):
  v=p.read(a,1);return v if v<128 else v-256
 actual=[p.read(0x2002604),s8(0x20025d6),s8(0x2002610),s8(0x20025d7),p.read(0x20025d8,1),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);assert p.read(0x2002608)==dh;steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original1123C..1144C contextual voice owner, source customer classification table, follow-up selection and ordered requests. Sound/RNG/modulo helper boundaries explicit; no audible/full-controller claim.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_context_voice.c','src/game_audio_context_voice.h','tests/game_audio_context_voice_rom_compare.c','tools/compare_game_audio_context_voice_rom.py']})
(root/'reports/game-audio-context-voice-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
