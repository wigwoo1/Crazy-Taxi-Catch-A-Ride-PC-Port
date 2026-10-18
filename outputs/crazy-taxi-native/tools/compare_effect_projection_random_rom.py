"""Original complete20A20 all coveredtypebranches; RNG explicit orderedcontract."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_projection_rom.py').read().split('\nd=json.loads')[0])
source=source.replace("['muls','ands','orrs']","['muls','ands','orrs','eors']").replace("a|b if m=='orrs' else a&b","a|b if m=='orrs' else (a^b if m=='eors' else a&b)")
exec(source);VehicleProbe.run=MotionProbe.run;allowed.append((0x9bc,0xa40))
for name in ['effect-projection','random-range']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TypeProbe(ProjectionProbe):
 def write(self,a,v,n=4):
  if a==0x2003b04:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x801e72c:
   expected=[(0,65535),(-5,5),(4,8)][self.rng_calls];assert tuple(map(signed,self.reg[:2]))==expected
   self.rng_calls+=1;self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032488:
   a,b=map(signed,self.reg[:2]);self.reg[0]=trunc(a,b)&MASK
  elif target==0x8032520:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK
  else:super().external(target)
cases=[(100,200,300,*angles,*point,type,frame,phase,disabled) for angles in [(0,0,0),(-323,3073,0),(512,1024,-256)] for point in [(90,210,900),(90,210,492),(400,-100,4000)] for type in [1,2,3,4,5] for frame,phase,disabled in [(0,0,0),(3,113800,0),(255,-113800,1)]]
for angles in [(-323,3073,0),(512,1024,-256)]:
 for point in [(5100,200,300),(-4900,200,300),(100,5200,300),(100,-4800,300),(100,200,5300),(100,200,-4700)]:
  for type in [1,2,3,4,5]:cases.append((100,200,300,*angles,*point,type,3,113800,0))
cases=[c+(seed,) for c in cases for seed in [0,1,0x40000000,0xffffffff]]
res=subprocess.run([str(root/'bin/effect-projection-random-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=armsteps=emitting=0
for case,row in zip(cases,rows):
 x,y,z,pitch,yaw,roll,px,py,pz,type,frame,phase,disabled,seed=case;p=TypeProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.rng_calls=0;p.arm.mem[0x2003b04]=seed
 for off,val,n in [(0x498,x,4),(0x49c,y,4),(0x4a0,z,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x544,phase,4)]:p.write(0x2002634+off,val,n)
 p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x2001708]=0;p.arm.mem[0x2005f20]=disabled
 for off,val in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+off]=val
 for a in range(0x2005c60,0x2005ee8,4):p.write(a,0)
 for off,val,n in [(4,px,4),(8,py,4),(12,pz,4),(52,type,1),(53,frame,1)]:p.write(0x2005c60+off,val,n)
 p.run(entry=0x8020a20,stop=0x10020000);got=[len(p.requests),p.rng_calls,p.read(0x2003b04)]+(p.requests[0] if p.requests else []);assert got==row,dict(case=case,original=got,native=row);steps+=p.steps;armsteps+=p.arm.steps;emitting+=bool(p.requests)
report=dict(cases=len(cases),emitting=emitting,thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete20A20 originalARMcamera/transform andcapture28658requests vsrealnativeeffect_sprite alltypes1..5, framewrap, threecamera rotations, signedphase, depthgate. Original1E72C nested, native realrandom_range, finalstate andorderedcalls matched; signeddivision/remainder contracts, notfullframe RNGownership.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_effect_projection_random_rom.py',root/'tests/effect_projection_random_rom_compare.c',root/'bin/effect-projection-random-rom-compare.exe',root/'src/effect_sprite.c',root/'src/impact.c',root/'src/random.c',root/'reports/random-range-cfg.json']}
(root/'reports/effect-projection-random-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
