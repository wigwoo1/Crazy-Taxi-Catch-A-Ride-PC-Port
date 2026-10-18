from pathlib import Path
"""Original complete effect-tail sequence vsnative120successiveframes; terraincontract."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_callbacks_rom.py').read().split('\nclock=')[0])
for name in ['effect-smoke','effect-boost']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
saved_effect_allowed=dict(allowed_thumb)
exec(open('outputs/crazy-taxi-native/tools/compare_effect_height_rom.py').read().split('\nschema=')[0])
allowed_thumb.update(saved_effect_allowed)
cfg=json.loads((root/'reports/body-points-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
address=int(json.loads((root/'reports/bound-ground-query-candidates.json').read_text())['source_world'],16)
class ActorProbe(PoolProbe):
 def external(self,target):
  if target==0x8032460:
   entry=self.reg[5];assert entry==0x8000a40;self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x8032450 and self.reg[1] not in [0x80218ad,0x8021959,0x8021a05,0x8021ab1]:next(c for c in type(self).__mro__ if c.__name__=="VehicleProbe").external(self,target)
  elif target==0x8029b44:
   prior=self.arm.mem[0x3006810];self.arm.mem[0x3006810]=address;self.height_calls+=1;self.run(entry=target,stop=self.reg[14]&~1);self.arm.mem[0x3006810]=prior
  elif target in [0x802df0c,0x8032454]:GroundProbe.external(self,target)
  elif target==0x8015d98:assert self.reg[0]==2;self.reg[0]=self.action2
  else:super().external(target)
p=ActorProbe((0,0,0,0,0,1,0,0,0,0));base=p.base;p.events=[];p.height_calls=0;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x30053c0]=1;p.arm.mem[0x2005c5c]=0;p.arm.mem[0x3006810]=address
for a in range(0x2005c60,0x2005f18,4):p.arm.mem[a]=0
for off in [0x3c0,0x3c4,0x3c8,0x3d0]:p.write(base+off,0)
for j in range(6):
 for k,v in enumerate([21039,3705,45212]):p.write(base+0x3d8+16*j+4*k,v)
 for k,v in enumerate([-256 if j==2 else 256 if j==3 else 0,256,-2048 if j==4 else -3072]):p.write(base+0x438+16*j+4*k,v)
rows=[list(map(int,l.split())) for l in (root/'reports/bound-hud-analog-effects.txt').read_text().splitlines()]
assert len(rows)==len(set(row[0] for row in rows)) and len(rows)>0
for tick,row in enumerate(rows):
 attachment=row[720:756];assert len(attachment)==36
 row=row[:685]
 _,x,y,z,pitch,roll,heading,drift,material,flags,speed,scale,phase,voice,action,state,materialflags,alternate,ring,delta,clock=row[:21]
 p.arm.mem[0x30053c0]=delta
 matptr=int.from_bytes(r[address-0x8000000+0x1c:address-0x8000000+0x20],"little");assert r[matptr-0x8000000+material*28+20]==materialflags
 for off,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x18,pitch,2),(0x1a,roll,2),(0x1c,heading,4),(0x69c,drift,4)]:p.write(base+off,v,n)
 p.steps=0;p.arm.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=0x80145e4,stop=0x10020000)
 got_attachment=[signed(p.read(base+o+16*j+4*k)) for o in (0x3d8,0x438) for j in range(6) for k in range(3)];assert got_attachment==attachment,dict(tick=tick,original=got_attachment,native=attachment)
 p.action2=action;p.arm.mem[0x2001534]=alternate;p.arm.mem[0x20025d4]=voice;p.arm.mem[0x2002630]=state
 for off,v,n in [(0x28,flags,1),(0xe60,material,2),(0x544,speed,4),(0xe84,scale,4),(0xe6a,phase,1)]:p.write(base+off,v,n)
 for entry in [0x8020d2c,0x8021078,0x8021220,0x80214f8,0x80208d4]:p.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 slots=[(p.read(0x2005ee8+g*24+j*4)-0x2005c60)//72 if p.read(0x2005ee8+g*24+j*4) else -1 for g in range(2) for j in range(6)];records=[p.read(0x2005c60+j,1) for j in range(648)]
 for j in range(9):
  ptr=p.read(0x2005c60+72*j)
  if base+0x3d8<=ptr<=base+0x428:records[j*72:j*72+4]=list(((ptr-base-0x3d8)//16).to_bytes(4,'little'))
 got=[p.read(0x2005c5c)]+[signed(p.read(base+o)) for o in [0x3c0,0x3c4,0x3c8,0x3d0]]+slots+records;assert got==[row[18]]+row[21:],dict(tick=tick,first_difference=next((i,a,b) for i,(a,b) in enumerate(zip(got,row)) if a!=b))
report=dict(frames=len(rows),height_calls=p.height_calls,scope='Executed original145E4/four emitters/208D4 on current captured injectedXbox Bound inputs; compared ring,four timers,twelve slots and648 pool bytes continuously. Captured action2 return contract; Original29B44 terrain query executed when requested; height_calls reports coverage. All36 body attachment position/vector words compared each call; no original fullengine/frame/trajectory claim.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'reports/bound-hud-analog-effects.txt',Path(__file__),root/'tests/scene_viewer_bound_pause_exit_probe.c',root/'bin/scene-viewer-bound-command-capture.exe']}
(root/'reports/bound-hud-analog-presentation-capture-effects-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
