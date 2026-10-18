"""Original pool traversal vsnative; callback/emit side effects explicit diagnostic contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_surface_effect_rom.py').read().split('\nclock=')[0])
cfg=json.loads((root/'reports/effect-pool-update-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class PoolProbe(SurfaceProbe):
 def external(self,target):
  if target==0x8032450:
   assert self.reg[1]==0x1111;self.events.append((self.reg[0]-0x2005c60)//72)
  elif target in [0x8021220,0x80214f8]:
   self.events.append(102 if target==0x8021220 else 103)
   if self.mode:self.write(0x2005c60+8*72+0x34,1,1);self.write(0x2005c60+8*72+0x3a,10,2)
  else:super().external(target)
cases=[(life,delta,pattern,mode) for life in [0,1,2,32767,32768,65535] for delta in [0,1,4,65536] for pattern in [0,1,21,63] for mode in [0,1]]
res=subprocess.run([str(root/'bin/effect-pool-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 life,delta,pattern,mode=case;probe=PoolProbe((0,0,0,0,0,1,0,0,0,0));probe.events=[];probe.mode=mode;probe.arm.mem[0x30053c0]=delta
 for a in range(0x2005c60,0x2005ee8,4):probe.arm.mem[a]=0x5a5a5a5a
 for i in range(9):
  b=0x2005c60+i*72;probe.write(b+0x34,i%5+1,1);probe.write(b+0x3a,life,2);probe.write(b+0x3c,0x1111)
 for g in range(2):
  for i in range(6):probe.arm.mem[0x2005ee8+g*24+i*4]=0x2005c60+i*72 if pattern&(1<<i) else 0
 probe.run(entry=0x80208d4,stop=0x10020000);mapped=[(probe.read(0x2005ee8+g*24+i*4)-0x2005c60)//72 if probe.read(0x2005ee8+g*24+i*4) else -1 for g in range(2) for i in range(6)]
 got=[len(probe.events)]+probe.events+mapped+[probe.read(0x2005c60+i,1) for i in range(648)];assert got==row,dict(case=case,events=probe.events,native_events=row[:1+row[0]]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal208D4 traversal, expiry andslots vsnative. Callback logging/noop andemitfutureindex8 mutation explicitmatchedcontracts; no originaleffectcallbacks/surfaceemitters/render/fullframe execution.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_pool.c',root/'tests/effect_pool_rom_compare.c',root/'tools/compare_effect_pool_rom.py',root/'bin/effect-pool-rom-compare.exe',root/'reports/effect-pool-update-cfg.json']}
(root/'reports/effect-pool-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
