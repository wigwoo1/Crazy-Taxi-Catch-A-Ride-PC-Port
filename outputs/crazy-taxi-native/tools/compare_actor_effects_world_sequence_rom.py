"""Original complete effect-tail sequence vsnative120successiveframes; terraincontract."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_callbacks_rom.py').read().split('\nclock=')[0])
for name in ['effect-smoke','effect-boost']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
saved_effect_allowed=dict(allowed_thumb)
exec(open('outputs/crazy-taxi-native/tools/compare_effect_height_rom.py').read().split('\nschema=')[0])
allowed_thumb.update(saved_effect_allowed)
class ActorProbe(PoolProbe):
 def external(self,target):
  if target==0x8029b44:
   prior=self.arm.mem[0x3006810];self.arm.mem[0x3006810]=address;self.height_calls+=1;self.run(entry=target,stop=self.reg[14]&~1);self.arm.mem[0x3006810]=prior
  elif target in [0x802df0c,0x8032454]:GroundProbe.external(self,target)
  elif target==0x8015d98:assert self.reg[0]==2;self.reg[0]=1
  else:super().external(target)
p=ActorProbe((0,0,0,0,0,1,0,0,0,0));base=p.base;p.events=[];p.height_calls=0;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x30053c0]=1;p.arm.mem[0x2005c5c]=0;p.arm.mem[0x3006810]=0x10002000;p.arm.mem[0x1000201c]=0x10003000
for a in range(0x2005c60,0x2005f18,4):p.arm.mem[a]=0
for off in [0x3c0,0x3c4,0x3c8,0x3d0]:p.write(base+off,0)
for j in range(6):
 for k,v in enumerate([21039,3705,45212]):p.write(base+0x3d8+16*j+4*k,v)
 for k,v in enumerate([-256 if j==2 else 256 if j==3 else 0,256,-2048 if j==4 else -3072]):p.write(base+0x438+16*j+4*k,v)
res=subprocess.run([str(root/'bin/actor-effects-world-sequence-rom-compare.exe')],text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==120
for tick,row in enumerate(rows):
 stage=tick//20;p.arm.mem[0x10003014]=6 if stage==2 else 0;p.arm.mem[0x2001534]=int(stage>=4);p.arm.mem[0x20025d4]=int(stage==0);p.arm.mem[0x2002630]=45 if stage==3 else 0
 for off,v,n in [(0x28,0,1),(0xe60,0,2),(0x544,100000,4),(0xe84,5324 if stage==1 else 4096,4),(0xe6a,1 if stage==3 else 0,1)]:p.write(base+off,v,n)
 for entry in [0x8020d2c,0x8021078,0x8021220,0x80214f8,0x80208d4]:p.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 slots=[(p.read(0x2005ee8+g*24+j*4)-0x2005c60)//72 if p.read(0x2005ee8+g*24+j*4) else -1 for g in range(2) for j in range(6)];records=[p.read(0x2005c60+j,1) for j in range(648)]
 for j in range(9):
  ptr=p.read(0x2005c60+72*j)
  if base+0x3d8<=ptr<=base+0x428:records[j*72:j*72+4]=list(((ptr-base-0x3d8)//16).to_bytes(4,'little'))
 got=[p.read(0x2005c5c)]+[signed(p.read(base+o)) for o in [0x3c0,0x3c4,0x3c8,0x3d0]]+slots+records;assert got==row,dict(tick=tick,first_difference=next((i,a,b) for i,(a,b) in enumerate(zip(got,row)) if a!=b))
report=dict(frames=120,height_calls=p.height_calls,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original4emitters+208D4+4actualcallbacks sequential120frames vsrealnativeactor_effects_step allring/timers/slots/648bytes; body/materialfixtures,Actualoriginal29B44 BoundROMterrain/nativeworld_asset_height, fixedrampbody/materialfixtures/action2truecontract; notfullgameframe.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/actor_effects.c',root/'src/effect_emit.c',root/'src/effect_pool.c',root/'src/effect_update.c',root/'src/world_asset.c',root/'src/ground.c',root/'tests/actor_effects_world_sequence_rom_compare.c',root/'tools/compare_actor_effects_world_sequence_rom.py',root/'bin/actor-effects-world-sequence-rom-compare.exe']}
(root/'reports/actor-effects-world-sequence-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
