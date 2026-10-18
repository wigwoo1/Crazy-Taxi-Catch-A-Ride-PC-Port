"""Original pool traversal vsnative; callback/emit side effects explicit diagnostic contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_surface_effect_rom.py').read().split('\nclock=')[0])
cfg=json.loads((root/'reports/effect-pool-update-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
for name in ['218ac','21958','21a04','21ab0']:
 cfg=json.loads((root/('reports/effect-update-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class PoolProbe(SurfaceProbe):
 def external(self,target):
  if target==0x8032450:
   assert self.reg[1] in [0x80218ad,0x8021959,0x8021a05,0x8021ab1];self.events.append((self.reg[0]-0x2005c60)//72);self.run(entry=self.reg[1]&~1,stop=self.reg[14]&~1)
  elif target in [0x8021220,0x80214f8]:
   self.events.append(102 if target==0x8021220 else 103)
   self.reg[0]=self.base;self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8029b44:self.reg[0]=1234
  else:super().external(target)
clock=int.from_bytes(r[0x4a59dc:0x4a59e0],'little',signed=True)
cases=[(life,delta,pattern,ring,clock) for life in [0,1,2,32768] for delta in [1,4] for pattern in [0,1,21,63] for ring in [0,7,8]]
res=subprocess.run([str(root/'bin/effect-pool-callbacks-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 life,delta,pattern,ring,clock=case;probe=PoolProbe((0,0,0,0,0,1,0,0,0,0));probe.events=[];probe.arm.mem[0x30053c0]=delta
 probe.arm.mem[0x2005c5c]=ring;probe.arm.mem[0x2001534]=0;probe.arm.mem[0x3006810]=0x10002000;probe.arm.mem[0x1000201c]=0x10003000;probe.arm.mem[0x10003014]=6
 for off,v,n in [(0x28,0,1),(0xe60,0,2),(0x544,100000,4),(0x3c4,0,4),(0x3c8,0,4)]:probe.write(probe.base+off,v,n)
 for off,x,z in [(0x458,-256,-3072),(0x468,256,-3072),(0x478,0,-2048)]:
  for j,v in enumerate([x,256,z]):probe.write(probe.base+off+j*4,v)
 for off in [0x3f8,0x408,0x418]:
  for j,v in enumerate([20,100,300]):probe.write(probe.base+off+j*4,v)
 for a in range(0x2005c60,0x2005ee8,4):probe.arm.mem[a]=0x5a5a5a5a
 for i in range(9):
  b=0x2005c60+i*72;probe.write(b+0x34,i%5+1,1);probe.write(b+0x3a,life,2);probe.write(b,probe.base+0x3f8);probe.write(b+0x3c,[0x80218ad,0x8021959,0x8021a05,0x8021ab1][i%4]);probe.write(b+0x40,0)
 for g in range(2):
  for i in range(6):probe.arm.mem[0x2005ee8+g*24+i*4]=0x2005c60+i*72 if pattern&(1<<i) else 0
 probe.run(entry=0x80208d4,stop=0x10020000);mapped=[(probe.read(0x2005ee8+g*24+i*4)-0x2005c60)//72 if probe.read(0x2005ee8+g*24+i*4) else -1 for g in range(2) for i in range(6)]
 records=[probe.read(0x2005c60+i,1) for i in range(648)]
 for i in range(9):
  ptr=probe.read(0x2005c60+i*72)
  if ptr in [probe.base+0x3f8,probe.base+0x408,probe.base+0x418]:records[i*72:i*72+4]=list((2+(ptr-probe.base-0x3f8)//16).to_bytes(4,'little'))
 got=[probe.read(0x2005c5c),signed(probe.read(probe.base+0x3c4)),signed(probe.read(probe.base+0x3c8)),len(probe.events)]+probe.events+mapped+records;assert got==row,dict(case=case,events=probe.events,native_events=row[:1+row[0]]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal208D4 traversal, expiry andslots vsnative. Nestedoriginal21220/214F8 surfaceemitters; actualoriginal218AC/21958/21A04/21AB0 callbacks nested; height29B44return1234 explicitcontract; no render/fullframe execution.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_pool.c',root/'tests/effect_pool_callbacks_rom_compare.c',root/'tools/compare_effect_pool_callbacks_rom.py',root/'bin/effect-pool-callbacks-rom-compare.exe',root/'reports/effect-pool-update-cfg.json']}
(root/'reports/effect-pool-callbacks-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
