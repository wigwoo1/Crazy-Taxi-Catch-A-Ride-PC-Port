"""Original effect sequence alternating owner emission and held-frame pool-only traversal."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_callbacks_rom.py').read().split('\nclock=')[0])
for name in ['effect-smoke','effect-boost']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class ActorProbe(PoolProbe):
 def external(self,target):
  if target==0x8015d98:assert self.reg[0]==2;self.reg[0]=1
  else:super().external(target)
class HeldActorProbe(ActorProbe):
 def external(self,target):
  if target==0x8032450:
   tag=hex(self.reg[1]);self.callback_counts[tag]=self.callback_counts.get(tag,0)+1
  return super().external(target)
p=HeldActorProbe((0,0,0,0,0,1,0,0,0,0));p.callback_counts={};base=p.base;p.events=[];p.arm.mem[0x30053c0]=1;p.arm.mem[0x2005c5c]=0;p.arm.mem[0x3006810]=0x10002000;p.arm.mem[0x1000201c]=0x10003000
for a in range(0x2005c60,0x2005f18,4):p.arm.mem[a]=0
for off in [0x3c0,0x3c4,0x3c8,0x3d0]:p.write(base+off,0)
for j in range(6):
 for k,v in enumerate([20,100,300]):p.write(base+0x3d8+16*j+4*k,v)
 for k,v in enumerate([-256 if j==2 else 256 if j==3 else 0,256,-2048 if j==4 else -3072]):p.write(base+0x438+16*j+4*k,v)
res=subprocess.run([str(root/'bin/actor-effects-held-sequence-rom-compare.exe')],text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==360
steps=held=held_active=held_reemissions=0
for tick,row in enumerate(rows):
 stage=tick//20;p.arm.mem[0x10003014]=6 if stage==2 else 0;p.arm.mem[0x2001534]=int(stage>=4);p.arm.mem[0x20025d4]=int(stage==0);p.arm.mem[0x2002630]=45 if stage==3 else 0
 for off,v,n in [(0x28,0,1),(0xe60,0,2),(0x544,100000,4),(0xe84,5324 if 10<=tick<20 else 4096,4),(0xe6a,1 if stage==3 else 0,1)]:p.write(base+off,v,n)
 is_held=bool(stage==1 or stage>=3);held+=is_held
 if is_held:held_active+=any(p.read(0x2005c60+i*72+0x34,1) for i in range(9))
 p.events=[]
 for entry in ([0x80208d4] if is_held else [0x8020d2c,0x8021078,0x8021220,0x80214f8,0x80208d4]):
  p.steps=0;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000);steps+=p.steps
 if is_held:held_reemissions+=sum(e in [102,103] for e in p.events)
 slots=[(p.read(0x2005ee8+g*24+j*4)-0x2005c60)//72 if p.read(0x2005ee8+g*24+j*4) else -1 for g in range(2) for j in range(6)];records=[p.read(0x2005c60+j,1) for j in range(648)]
 for j in range(9):
  ptr=p.read(0x2005c60+72*j)
  if base+0x3d8<=ptr<=base+0x428:records[j*72:j*72+4]=list(((ptr-base-0x3d8)//16).to_bytes(4,'little'))
 got=[p.read(0x2005c5c)]+[signed(p.read(base+o)) for o in [0x3c0,0x3c4,0x3c8,0x3d0]]+slots+records;assert got==row,dict(tick=tick,first_difference=next((i,a,b) for i,(a,b) in enumerate(zip(got,row)) if a!=b))
assert held_active>0 and held_reemissions>0 and set(p.callback_counts)=={'0x80218ad','0x8021959','0x8021a05','0x8021ab1'}
report=dict(frames=360,held_frames=held,held_with_active_records=held_active,held_expiry_emitter_calls=held_reemissions,callback_counts=p.callback_counts,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='360successiveoriginal emitter/pool frames: emissions0..19/40..59, allotherframesheldpool-only vsnativeactor_effects_step/advance. Complete208D4+actualnestedcallbacks (counts listed)+expiry21220/214F8, allring/timers/slots/648recordbytes exact. Storedpriorbody/material/action2true/height1234 fixtures explicit; notproductionterrain/fullframe.')
report['sha256']={f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/actor_effects.c','src/effect_pool.c','src/effect_update.c','tests/actor_effects_held_sequence_rom_compare.c','tools/compare_actor_effects_held_sequence_rom.py','bin/actor-effects-held-sequence-rom-compare.exe']}
(root/'reports/actor-effects-held-sequence-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',{k:v for k,v in report.items() if k!='sha256'})
