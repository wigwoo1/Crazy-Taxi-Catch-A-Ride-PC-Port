"""Execute original combined10B54..11468; helpers remain explicit boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
all_allowed=allowed_thumb.copy()
for name in ['engine','events','aux','vehicle_loop','reward_event','turn_loop','voice','context_voice','amplitude']:
 text=open('outputs/crazy-taxi-native/tools/compare_game_audio_'+name+'_rom.py').read()
 exec(text.split('\ncases=')[0]);all_allowed.update(allowed_thumb)
allowed_thumb.update(all_allowed)
import random
schema=json.loads((root/'reports/game-audio-controller-schema.json').read_text());sf=schema['state'];inf=schema['input']
smap={**{f'handles[{j}]':(0x20025e8+j*4,4,False) for j in range(10)},'cooldown':(0x2002614,4,True),'amplitude':(0x20025dc,4,True),'hold_timer':(0x20025e4,4,True),'factor':(0x20025da,2,True),**{k:(0x20025d0+j,1,False) for j,k in enumerate(['flag0','flag1','pending2','pending3','enabled4','reward_cooldown'])},'last':(0x20025d8,1,False),'context_selection':(0x20025d6,1,True),'driver_selection':(0x20025d7,1,True),'classification':(0x2002610,1,True)}
imap={'speed':(0x2002b78,4),'max_speed':(0x2002b7c,4),'scale':(0x20034b8,4),'delta':(0x2003b00,4),'vehicle564':(0x2002b98,4),'rpm':(0x2002b6c,2),'angle578':(0x2002bac,2),'gate00e4':(0x20000e4,2),'muted':(0x2001534,1),'ended':(0x2001697,1),'outer':(0x2001ea8,1),'intro':(0x2001696,1),'frozen':(0x2005f20,1),'fade':(0x200177b,1),'effects':(0x2007602,1),'override2620':(0x2002620,1),'flags28':(0x200265c,1),'settings':(0x2001eb0,1),'challenge_driver':(0x2001539,1),'driver':(0x2001690,1)}
class ControllerProbe(LevelsProbe):
 def external(self,target):
  h=self.reg[0]&0xffffffff;pc=self.reg[14]
  if target==0x8032488:return super().external(target)
  if target==0x8032520:
   a,b=map(signed,self.reg[:2]);q=abs(a)//abs(b);q=-q if (a<0)!=(b<0) else q;self.reg[0]=(a-q*b)&0xffffffff;return
  if target==0x8015d98:
   assert h in [2,3];self.calls.append([5,0,h]);self.reg[0]=self.inputs['action'+str(h)];return
  if target==0x801e72c:
   assert self.reg[:2]==[0,65535];self.calls.append([8,0,65535]);self.reg[0]=self.inputs['random'];return
  if target==0x802901c:
   assert h==1;self.calls.append([7,self.reg[1],signed(self.reg[2])]);return
  if target in [0x8027610,0x80276cc,0x80276dc]:
   op={0x8027610:4,0x80276cc:2,0x80276dc:3}[target];self.calls.append([op,h,0 if op==4 else signed(self.reg[1])]);return
  assert target in [0x80267c8,0x80277a4],hex(target)
  pointer=h if target==0x80267c8 else self.reg[1]
  indices=[j for j in range(203) if struct.unpack_from('<I',r,0x7ec658+j*4)[0]==pointer];assert len(indices)==1,(hex(pointer),indices);ix=indices[0]
  slot=0 if pc<0x8010cbd else 1 if pc<0x8010d5c else -1 if pc<0x8010e1c else 2 if pc<0x8010ebc else 3 if pc<0x8010f10 else 4 if pc<0x8010fb8 else 5 if pc<0x8011074 else (5 if ix==44 else 6) if pc<0x801114e else 8 if pc<0x801123c else 7
  if target==0x80267c8:
   flags=self.reg[1]|self.reg[2]<<8
   self.calls.append([9 if slot==-1 else 1,ix,flags]);self.reg[0]=0 if slot==-1 else self.inputs[f'starts[{slot}]'];return
  assert slot>=0;self.calls.append([6,h,ix]);self.reg[0]=self.inputs[f'busy[{slot}]']
rng=random.Random(0x10b54);cases=[]
for z in range(512):
 s={k:0 for k in sf};s.update(factor=rng.choice([64,256,-32768]),amplitude=rng.choice([0,16384,16000]),cooldown=rng.choice([0,1,60]),hold_timer=rng.choice([0,60]),context_selection=rng.choice([-1,0,6,18,22]),driver_selection=rng.choice([-1,0,8,17]),classification=rng.choice([-1,0,1]),last=rng.randrange(24))
 for j in range(10):s[f'handles[{j}]']=0 if rng.randrange(2) else 0x2009000+j*32
 for k in ['flag0','flag1','pending2','pending3','enabled4','reward_cooldown']:s[k]=rng.choice([0,1,255])
 i={k:0 for k in inf};i.update(speed=rng.choice([-75000,-1138,0,1138,68280,102421,136561]),max_speed=150000,scale=4096,delta=rng.choice([1,60]),vehicle564=rng.randrange(2),rpm=1234,angle578=rng.choice([-769,0,769]),gate00e4=rng.randrange(2),effects=1,current_class=rng.choice([-1,0,1]),alternate_class=rng.choice([-1,0,1]),random=rng.randrange(65536),driver=rng.randrange(4),challenge_driver=rng.randrange(4),settings=rng.choice([0,128]))
 for k in ['muted','ended','intro','frozen','fade','action2','action3','override2620']:i[k]=rng.randrange(2) if z>=128 else 0
 i['outer']=rng.randrange(4);i['flags28']=rng.choice([0,8])
 for j in range(9):i[f'starts[{j}]']=0 if rng.randrange(4)==0 else 0x2009200+j*32;i[f'busy[{j}]']=rng.choice([0,255])
 cases.append((s,i))
def source_frame(s,i):
 p=ControllerProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.inputs=i
 for k,(a,n,sgn) in smap.items():p.write(a,s[k],n)
 for k,(a,n) in imap.items():p.write(a,i[k],n)
 for k,a,base in [('current_class',0x2001688,0x2009500),('alternate_class',0x200168c,0x2009520)]:p.write(a,0 if i[k]==-1 else base);p.write(base+1,1 if i[k]==0 else 0,1)
 p.run(entry=0x8010b54,stop=0x801146a)
 values=[]
 for k in sf:
  a,n,sgn=smap[k];v=p.read(a,n);v=v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v;values.append(v)
 actual=values+[len(p.calls)]+[v for call in p.calls for v in call]
 return actual,p.steps
# Carry original retained state across consecutive frames; native must match each.
base_s,base_i=cases[0];chain_s=base_s.copy();chain_s.update(context_selection=0,driver_selection=0,classification=0,last=0,amplitude=0,factor=256)
for j in range(10):chain_s[f'handles[{j}]']=0
for frame in range(240):
 i=base_i.copy();i.update(speed=frame*1138 if frame<120 else (240-frame)*1138,intro=0,ended=0,frozen=1 if 60<=frame<90 else 0,fade=0,outer=0,muted=0,effects=1,action2=frame%2,action3=frame%3==0,override2620=0,flags28=0,vehicle564=frame%5!=0,angle578=769 if frame%4==0 else 0,delta=1,gate00e4=0,random=frame*257%65536,current_class=0,alternate_class=1)
 for j in range(9):i[f'starts[{j}]']=0x2009200+j*32;i[f'busy[{j}]']=255 if frame%7 else 0
 if frame%23==0:chain_s['pending3']=1;chain_s['pending2']=1;chain_s['enabled4']=1;chain_s['flag0']=1;chain_s['flag1']=1
 if frame%31==0:chain_s['context_selection']=frame%24
 cases.append((chain_s.copy(),i));actual,_=source_frame(chain_s,i);chain_s=dict(zip(sf,actual[:len(sf)]))
cp=subprocess.run([str(root/'bin/game-audio-controller-rom-compare.exe')],input=''.join(' '.join(str(int(s[k])) for k in sf)+' '+' '.join(str(int(i[k])) for k in inf)+'\n' for s,i in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for num,((s,i),want) in enumerate(zip(cases,rows)):
 p=ControllerProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.inputs=i
 for k,(a,n,sgn) in smap.items():p.write(a,s[k],n)
 for k,(a,n) in imap.items():p.write(a,i[k],n)
 for k,a,base in [('current_class',0x2001688,0x2009500),('alternate_class',0x200168c,0x2009520)]:p.write(a,0 if i[k]==-1 else base);p.write(base+1,1 if i[k]==0 else 0,1)
 p.run(entry=0x8010b54,stop=0x801146a)
 values=[]
 for k in sf:
  a,n,sgn=smap[k];v=p.read(a,n);v=v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v;values.append(v)
 actual=values+[len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(num,s,i,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Combined original10B54..11468 state and ordered requests against native composition. Audio/action/RNG/reward/division helpers explicit boundaries; 512 independent plus240 consecutive retained-state frames; no audible/live channel claim.',source_sha256={str(f.relative_to(root)):hashlib.sha256(f.read_bytes()).hexdigest() for f in list((root/'src').glob('game_audio_*.c'))+[root/'tools/compare_game_audio_controller_rom.py',root/'tests/game_audio_controller_rom_compare.c']})
(root/'reports/game-audio-controller-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
