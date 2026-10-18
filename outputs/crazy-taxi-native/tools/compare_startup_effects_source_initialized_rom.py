"""Original complete effect-tail sequence vsnative120successiveframes; terraincontract."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_callbacks_rom.py').read().split('\nclock=')[0])
for name in ['effect-smoke','effect-boost']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
saved_effect_allowed=dict(allowed_thumb)
exec(open('outputs/crazy-taxi-native/tools/compare_effect_height_rom.py').read().split('\nschema=')[0])
allowed_thumb.update(saved_effect_allowed)
cfg=json.loads((root/'reports/body-points-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
address=0x087556cc
cfg=json.loads((root/'reports/challenge-bound-acceleration-action-query-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class ActorProbe(PoolProbe):
 def external(self,target):
  if target==0x8032460:
   entry=self.reg[5];assert entry==0x8000a40;self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x8032450 and self.reg[1] not in [0x80218ad,0x8021959,0x8021a05,0x8021ab1]:next(c for c in type(self).__mro__ if c.__name__=="VehicleProbe").external(self,target)
  elif target==0x8029b44:
   prior=self.arm.mem[0x3006810];self.arm.mem[0x3006810]=address;self.height_calls+=1;self.run(entry=target,stop=self.reg[14]&~1);self.arm.mem[0x3006810]=prior
  elif target in [0x802df0c,0x8032454]:GroundProbe.external(self,target)
  elif target==0x8015d98:
   assert self.reg[0]==2;self.action_calls+=1;self.reg[0]=self.action2 # Explicit scripted action2 contract.
  else:super().external(target)
p=ActorProbe((0,0,0,0,0,1,0,0,0,0));base=p.base;p.events=[];p.height_calls=0;p.action_calls=0
p.arm.mem.update({0x3006940:0x80029fc,0x30053c0:1,0x3006810:address})
# Original source actor initializer provides actor bytes and its effect timers.
inits={};exec((root/'tools/compare_actual_startup_owner_rom.py').read_text().split('report=dict(')[0].replace('startup-owner-state.txt','startup-effects-owner.txt'),inits)
for o,v in enumerate(inits['actual']):p.write(base+o,v,1)
# Original pool clear, with cold zero RAM fixture; do not seed from native output.
resetns={};exec((root/'tools/compare_effect_pool_reset_rom.py').read_text().split('seeds=')[0],resetns)
rp=resetns['ResetProbe']((0,0,0,0,0,1,0,0,0,0));rp.reg[13]=0x10000400;rp.reg[14]=0x10020001;rp.run(entry=0x8021c10,stop=0x10020000)
for o,v in enumerate(rp.pool[:648]):p.write(0x2005c60+o,v,1)
p.arm.mem[0x2005c5c]=rp.ring
sessionns={};exec((root/'tools/compare_countdown_source_initialized_rom.py').read_text().split('for d in rows:')[0],sessionns)
for o in range(0,48,4):p.arm.mem[0x2005ee8+o]=sessionns['p'].read(0x2005ee8+o)
rows=[list(map(int,l.split())) for l in (root/'reports/startup-effects-state.txt').read_text().splitlines()];assert len(rows)==61
for tick,row in enumerate(rows):
 assert len(row)==720 # Scripted throttle and raw buttons are distinct capture inputs.
 for a in range(0x20034e0,0x200350c,4):p.arm.mem[a]=0
 for j in range(10):
  for a,v in [(0x20034e2+j,row[685+3*j]),(0x2003500+j,row[686+3*j]),(0x20034f6+j,row[687+3*j])]:p.arm.mem[a&~3]|=v<<((a&3)*8)
 profile,flags4ac,over,cmd,_=row[715:720];p.arm.mem[0x2001eb0]=profile<<1;p.arm.mem[0x2002ae0]=flags4ac;p.arm.mem[0x2001708]=over;p.arm.mem[0x30053d4]=cmd

 _,x,y,z,pitch,roll,heading,drift,material,flags,speed,scale,phase,voice,action,state,materialflags,alternate,ring,delta,clock=row[:21]
 p.arm.mem[0x30053c0]=delta
 matptr=int.from_bytes(r[address-0x8000000+0x1c:address-0x8000000+0x20],"little");assert r[matptr-0x8000000+material*28+20]==materialflags
 for off,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x18,pitch,2),(0x1a,roll,2),(0x1c,heading,4),(0x69c,drift,4)]:p.write(base+off,v,n)
 p.steps=0;p.arm.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=0x80145e4,stop=0x10020000)
 p.action2=action;p.arm.mem[0x2001534]=alternate;p.arm.mem[0x20025d4]=voice;p.arm.mem[0x2002630]=state
 for off,v,n in [(0x28,flags,1),(0xe60,material,2),(0x544,speed,4),(0xe84,scale,4),(0xe6a,phase,1)]:p.write(base+off,v,n)
 for entry in [0x8020d2c,0x8021078,0x8021220,0x80214f8,0x80208d4]:p.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 slots=[(p.read(0x2005ee8+g*24+j*4)-0x2005c60)//72 if p.read(0x2005ee8+g*24+j*4) else -1 for g in range(2) for j in range(6)];records=[p.read(0x2005c60+j,1) for j in range(648)]
 for j in range(9):
  ptr=p.read(0x2005c60+72*j)
  if base+0x3d8<=ptr<=base+0x428:records[j*72:j*72+4]=list(((ptr-base-0x3d8)//16).to_bytes(4,'little'))
 got=[p.read(0x2005c5c)]+[signed(p.read(base+o)) for o in [0x3c0,0x3c4,0x3c8,0x3d0]]+slots+records;assert got==[row[18]]+row[21:685],dict(tick=tick,first_difference=next((i,a,b) for i,(a,b) in enumerate(zip(got,row)) if a!=b))
report=dict(status='PASS',frames=len(rows),height_calls=p.height_calls,action_contract_calls=p.action_calls,scope='Original14A58 actor/effect timer initialization, original21C10 pool clear and B6EC slot initialization feed61native ordinary tail updates. Original145E4 body, four emitters, pool/callbacks/29B44 city1 terrain compare all ring/timers/slots/648recordbytes continuously. Scripted action2 result contract and cold zero RAM fixture explicit; actor pose/material/control inputs captured; not whole original frame or framebuffer proof.')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/startup-effects-state.txt',root/'reports/startup-effects-owner.txt',root/'bin/scene-viewer-startup-effects-gate-check.exe']}
(root/'reports/startup-effects-source-initialized-rom-comparison.json').write_text(json.dumps(report,indent=2));print(report)

