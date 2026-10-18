"""Original surface emitters vsnative, paired slot reuse and every record byte."""
exec(open('outputs/crazy-taxi-native/tools/compare_smoke_rom.py').read().split('\nclock=')[0])
for name in ['surface2','surface3']:
 cfg=json.loads((root/('reports/effect-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class SurfaceProbe(SmokeProbe):
 def write(self,a,v,n=4):
  if 0x2005ee8<=a<0x2005f18:
   assert n==4;self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
clock=int.from_bytes(r[0x4a59dc:0x4a59e0],'little',signed=True)
cases=[(typ,flags,mat,alt,ring,pattern,speed,timer,clock) for typ in [2,3] for flags,mat,alt,speed in [(0,6,0,54901),(0,6,0,-54901),(0,6,0,54900),(8,6,0,100000),(0,0,0,100000),(0,6,1,100000)] for ring in [0,7,8] for pattern in [0,1,2,3,21,63] for timer in [0,-1]]
res=subprocess.run([str(root/'bin/surface-effect-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=emitting=0
for case,row in zip(cases,rows):
 typ,flags,mat,alt,ring,pattern,speed,timer,clock=case;probe=SurfaceProbe((0,0,0,0,0,1,0,0,0,0));base=probe.base;timer_off=0x3c4 if typ==2 else 0x3c8;slots=0x2005ee8 if typ==2 else 0x2005f00
 for off,v,n in [(0x28,flags,1),(timer_off,timer,4),(0x544,speed,4),(0xe60,0,2)]:probe.write(base+off,v,n)
 for off,x,z in [(0x458,-256,-3072),(0x468,256,-3072),(0x478,0,-2048)]:
  for j,v in enumerate([x,256,z]):probe.write(base+off+j*4,v)
 probe.arm.mem[0x2001534]=alt;probe.arm.mem[0x3006810]=0x10002000;probe.arm.mem[0x1000201c]=0x10003000;probe.arm.mem[0x10003014]=mat;probe.arm.mem[0x2005c5c]=ring
 for a in range(0x2005c60,0x2005ee8,4):probe.arm.mem[a]=0x5a5a5a5a
 for i in range(6):probe.arm.mem[slots+i*4]=0x2005c60+i*72 if pattern&(1<<i) else 0
 probe.reg[0]=base;probe.run(entry=0x8021220 if typ==2 else 0x80214f8,stop=0x10020000)
 got=[probe.read(0x2005c60+i,1) for i in range(648)]
 for i in range(9):
  if probe.read(0x2005c60+i*72+0x34,1) in [2,3]:
   ptr=probe.read(0x2005c60+i*72);assert ptr in [base+0x3f8,base+0x408,base+0x418];tag=2+(ptr-base-0x3f8)//16;got[i*72:i*72+4]=list(tag.to_bytes(4,'little'))
 mapped=[(probe.read(slots+i*4)-0x2005c60)//72 if probe.read(slots+i*4) else -1 for i in range(6)]
 assert got==row[9:] and mapped==row[3:9] and probe.read(0x2005c5c)==row[1] and signed(probe.read(base+timer_off))==row[2],dict(case=case,original_slots=mapped,native_slots=row[3:9]);steps+=probe.steps;emitting+=row[0]>0
report=dict(cases=len(cases),emitting_cases=emitting,thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalcomplete21220/214F8 vsnative timer/ring/6slotindices/all648recordbytes. Originalpointerstonativeattachmenttags/recordindices normalized; material/globalfixtures, noteffectupdate/render/fullframe.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/effect_emit.c',root/'tests/surface_effect_rom_compare.c',root/'tools/compare_surface_effect_rom.py',root/'bin/surface-effect-rom-compare.exe',root/'reports/effect-surface2-cfg.json',root/'reports/effect-surface3-cfg.json']}
(root/'reports/surface-effect-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
