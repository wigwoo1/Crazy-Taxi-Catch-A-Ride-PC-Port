"""Original complete smoke emitter vs native gated and emitting pool fixtures."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
source=source.replace('self.steps<1000','self.steps<50000');exec(source);VehicleProbe.run=MotionProbe.run
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/effect-boost-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
cfg=json.loads((root/'reports/challenge-bound-acceleration-action-query-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class SmokeProbe(VehicleProbe):
 def external(self,target):
  if target==0x8015d98:assert self.reg[0]==2;self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
 def write(self,a,v,n=4):
  if 0x2005c5c<=a<0x2005ee8:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
clock=int.from_bytes(r[0x4a59dc:0x4a59e0],'little',signed=True)
cases=[(flags,mat,alt,action,0,ring,scale,timer,delta,0,0,clock) for flags,mat,alt,action,scale in [(0,0,0,1,5324),(8,0,0,1,5324),(0,2,0,1,5324),(0,0,1,1,5324),(0,0,0,0,5324),(0,0,0,1,4096),(0,4,0,1,4097)] for ring in [0,7,8] for timer in [0,1,2,-1] for delta in [0,1,4]]
res=subprocess.run([str(root/'bin/boost-effect-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0;emitting=0
for case,row in zip(cases,rows):
 flags,mat,alt,phase,voice,ring,scale,timer,delta,speed,state,clock=case;probe=SmokeProbe((0,0,0,0,0,1,0,0,0,0));base=probe.base
 for off,v,n in [(0x28,flags,1),(0xe6a,phase,1),(0xe84,scale,4),(0x3d0,timer,4),(0x544,speed,4),(0xe60,0,2)]:probe.write(base+off,v,n)
 for off,x in [(0x458,-256),(0x468,256)]:
  for j,v in enumerate([x,256,-3072]):probe.write(base+off+j*4,v)
 probe.arm.mem[0x2002630]=state;probe.arm.mem[0x20025d4]=voice;probe.arm.mem[0x2001534]=alt;probe.arm.mem[0x30053c0]=delta;probe.arm.mem[0x3006810]=0x10002000;probe.arm.mem[0x1000201c]=0x10003000;probe.arm.mem[0x10003014]=mat;probe.arm.mem[0x2005c5c]=ring
 for a in range(0x2005c60,0x2005ee8,4):probe.arm.mem[a]=0x5a5a5a5a
 probe.arm.mem[0x2001708]=1;probe.arm.mem[0x30053d4]=phase<<2
 probe.reg[0]=base;probe.run(entry=0x8021078,stop=0x10020000)
 got=[probe.read(0x2005c60+i,1) for i in range(648)]
 for i in range(9):
  if probe.read(0x2005c60+i*72+0x34,1)==5:
   ptr=probe.read(0x2005c60+i*72);assert ptr in [base+0x3f8,base+0x408];tag=2 if ptr==base+0x3f8 else 3
   got[i*72:i*72+4]=list(tag.to_bytes(4,'little'))
 assert got==row[3:] and probe.read(0x2005c5c)==row[1] and signed(probe.read(base+0x3d0))==row[2],dict(case=case,original_ring=probe.read(0x2005c5c),native_ring=row[1]);steps+=probe.steps;emitting+=row[0]>0
report=dict(cases=len(cases),emitting_cases=emitting,thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal21078 plusoriginal15D98 overridequery vsnative boost emission gates/timer/ring/all648recordbytes, originalattachmentpointers normalizedtonativetags. Explicitmaterialdescriptor/global/divisionfixtures; notrender/effectupdate/fullframe.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_emit.c',root/'tests/boost_effect_rom_compare.c',root/'tools/compare_boost_effect_rom.py',root/'bin/boost-effect-rom-compare.exe',root/'reports/effect-boost-cfg.json']}
(root/'reports/boost-effect-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
