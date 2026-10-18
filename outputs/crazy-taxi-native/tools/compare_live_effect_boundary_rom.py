from pathlib import Path
"""Offline actual20A20/RNG replay of exact native per-tick boundary capture."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_projection_random_rom.py').read().split('\ncases=')[0])
class LiveProbe(TypeProbe):
 def external(self,target):
  if target==0x801e72c:
   expected=[(0,65535),(-5,5),(4,8)][self.rng_calls%3];assert tuple(map(signed,self.reg[:2]))==expected
   self.rng_calls+=1;self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
d=json.loads((root/'reports/live-effect-source-inputs.json').read_text());p=LiveProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.rng_calls=0
x,y,z,pitch,yaw,roll=d['camera']
for off,val,n in [(0x498,x,4),(0x49c,y,4),(0x4a0,z,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x544,d['phase'],4)]:p.write(0x2002634+off,val,n)
p.arm.mem.update({0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x2001708:0,0x2005f20:d['disabled'],0x2003b04:d['random_state']})
for off,val in [(0x18,d['center'][0]),(0x1c,d['center'][1]),(0x28,d['focal'])]:p.arm.mem[0x3006810+off]=val
for i,record in enumerate(d['records']):
 for j in range(0,72,4):p.write(0x2005c60+i*72+j,int.from_bytes(bytes(record[j:j+4]),'little'))
p.run(entry=0x8020a20,stop=0x10020000)
assert p.requests==d['requests'],(p.requests,d['requests'])
assert p.arm.mem[0x2003b04]==d['post_random_state']
report={'status':'PASS','requests':len(p.requests),'rng_calls':p.rng_calls,'Thumb_instructions':p.steps,'ARM_instructions':p.arm.steps,'scope':'Actual20A20 original ARM transform and original1E72C RNG compare request args and finalRNG to exact native live projection-boundary capture;28658 args captured, not nested drawing/fullframe proof.','hashes':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [Path(__file__),root/'reports/live-effect-source-inputs.json',root/'src/scene_viewer.c']}}
(root/'reports/live-effect-boundary-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(p.requests),p.rng_calls,p.steps,p.arm.steps)
