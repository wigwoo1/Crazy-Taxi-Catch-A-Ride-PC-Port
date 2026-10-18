"""Original effect callbacks versusnative records; ground height boundary explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_smoke_rom.py').read().split('\nclock=')[0])
for name in ['218ac','21958','21a04','21ab0']:
 cfg=json.loads((root/('reports/effect-update-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class UpdateProbe(SmokeProbe):
 def external(self,target):
  if target==0x8029b44:self.reg[0]=1234 if self.hit else 0x80000000
  else:super().external(target)
clock=int.from_bytes(r[0x4a59dc:0x4a59e0],'little')
cases=[(tag,delta,timer,frame,size,hit,clock) for tag in [0x80218ad,0x8021959,0x8021a05,0x8021ab1] for delta,timer in [(1,0),(4,1),(0,-1),(65536,2)] for frame,size in [(0,16384),(3,24575),(255,32760)] for hit in [0,1]]
res=subprocess.run([str(root/'bin/effect-update-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 tag,delta,timer,frame,size,hit,clock=case;probe=UpdateProbe((0,0,0,0,0,1,0,0,0,0));base=probe.base;probe.hit=hit;probe.arm.mem[0x30053c0]=delta
 for a in range(base,base+72,4):probe.arm.mem[a]=0x5a5a5a5a
 for off,val in [(0,base+0x100),(0x14,0xfffff000),(0x18,8192),(0x1c,4095),(0x24,3072),(0x28,0xfffffc00),(0x2c,8192),(0x40,timer)]:probe.write(base+off,val)
 for off,v in [(0x100,20),(0x104,100),(0x108,300)]:probe.write(base+off,v)
 probe.write(base+0x35,frame,1);probe.write(base+0x36,size,2);probe.reg[0]=base;probe.run(entry=tag&~1,stop=0x10020000)
 got=[probe.read(base+i,1) for i in range(72)];got[:4]=[2,0,0,0];assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalcomplete218AC/21958/21A04/21AB0 vsnativeall72recordbytes, position/velocity/frame/size/timer. Originalattachmentpointer normalizedtag2; original29B44 heightreturn explicitcontract1234orINTMIN, no renderer/fullframe.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_update.c',root/'tests/effect_update_rom_compare.c',root/'tools/compare_effect_update_rom.py',root/'bin/effect-update-rom-compare.exe']}
(root/'reports/effect-update-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
