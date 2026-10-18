"""Original effect callback with original terrain query nested vsnative real terrain."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_height_rom.py').read().split('\nschema=')[0])
cfg=json.loads((root/'reports/effect-update-21958-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class UpdateWorldProbe(GroundProbe):
 def external(self,target):
  if target==0x8029b44:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
schema=json.loads((root/'reports/bound-route-world-full-state-schema.json').read_text())['fields'];poses=[]
for l in (root/'reports/bound-route-world-full-states.txt').read_text().splitlines():
 a=list(map(int,l.split()));d=dict(zip(schema,a[1:]));poses.append(tuple(d[k] for k in ['output_x','output_y','output_z']))
poses+=[(-1000000,0,-1000000),(1000000,0,1000000)]
clock=int.from_bytes(r[0x4a59dc:0x4a59e0],'little');cases=[(0x8021959,delta,timer,frame,size,clock,*pose) for pose in poses for delta,timer,frame,size in [(1,0,0,16384),(4,1,3,32760)]]
res=subprocess.run([str(root/'bin/effect-update-world-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 tag,delta,timer,frame,size,clock,x,y,z=case;probe=UpdateWorldProbe((0,0,0,0,0,1),0);base=probe.base;probe.arm.mem[0x30053c0]=delta;probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006940]=0x80029fc
 for a in range(base,base+72,4):probe.arm.mem[a]=0x5a5a5a5a
 for off,val in [(0,base+0x100),(0x14,0xfffff000),(0x18,8192),(0x1c,4095),(0x24,3072),(0x28,0xfffffc00),(0x2c,8192),(0x40,timer),(0x100,x),(0x104,y),(0x108,z)]:probe.write(base+off,val)
 probe.write(base+0x35,frame,1);probe.write(base+0x36,size,2);probe.reg[0]=base;probe.reg[14]=0x10020001;probe.run(entry=0x8021958,stop=0x10020000)
 got=[probe.read(base+i,1) for i in range(72)];got[:4]=[2,0,0,0];assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal21958 nestedoriginal29B44 terrain lookup usingBoundROMgeometry vsnativeeffect_update_tag+world_asset_height all72recordbytes on161actualpostposes and2outsidepoints x2boundaryvariants. Attachmentpointer normalized, boundedcontainment/division/globalinitializercontracts; notfullframe/render.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_update.c',root/'src/world_asset.c',root/'src/ground.c',root/'tests/effect_update_world_rom_compare.c',root/'tools/compare_effect_update_world_rom.py',root/'bin/effect-update-world-rom-compare.exe',root/'reports/effect-height-cfg.json',root/'reports/effect-update-21958-cfg.json',root/'reports/bound-route-world-full-states.txt']}
(root/'reports/effect-update-world-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
