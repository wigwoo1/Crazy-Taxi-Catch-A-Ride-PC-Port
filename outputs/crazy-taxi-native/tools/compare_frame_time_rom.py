import pathlib,json,subprocess,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/probe_full_camera_view_rom.py').read_text().split('\np=CameraProbe')[0],ns)
rom=ns['r'];ns['allowed_thumb'].update({0x8000000+o:rom[o:o+2] for o in range(0x263d2,0x2641e,2)})
class TimingProbe(ns['CameraProbe']):
 def write(self,a,v,n=4):
  if (a&~3) in [0x2003520,0x2003524,0x2003b00,0x2005f20,0x200755c,0x2001e10,0x2007540]:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&0xffffffff
  else:super().write(a,v,n)
cases=[]
for previous,counter in [(0,0),(0,1),(0,3),(0,4),(0,5),(0,60),(0,61),(0,999),(12,11),(0xfffffffe,2),(0,0x80000000),(0x80000000,0),(0xffffffff,0xffffffff)]:
 for freeze in (0,1,255):cases.append((previous,counter,0xfffffffe,0xfffffffe,255,freeze))
rows=[]
for before in cases:
 previous,counter,elapsed,animation,page,freeze=before;p=TimingProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:0,0x2005f20:freeze,0x200755c:elapsed,0x2001e10:animation,0x2007540:page}.items():p.arm.mem[a]=v
 p.run(entry=0x80263d2,stop=0x802641e)
 row=[p.read(0x2003524),ns['signed'](p.read(0x2003b00)),p.read(0x200755c),p.read(0x2001e10),p.read(0x2007540,1)];rows.append(row)
text=''.join(' '.join(map(str,x))+'\n' for x in cases);binary=root/'bin/frame-time-probe.exe';result=subprocess.run([str(binary)],input=text,text=True,capture_output=True,check=True);native=[list(map(int,x.split())) for x in result.stdout.splitlines()];assert rows==native,(rows,native)
report={'status':'PASS','cases':len(cases),'source_range':'080263D2..0802641C','counter_owner':'08016174..0801617C increments02003520; IRQ registration still to qualify','formula':'signed32(counter3520-previous3524), clamp1..60; retain current; elapsed755C advances only when5F20=0; animation1E10+=4; page7540=(page+1)&1','records':[{'input':list(a),'output':b} for a,b in zip(cases,rows)],'scope':'Original timing tail instructions vs native arithmetic incl wrap/signed extremes/pause/page overflow. Hardware timing, IRQ attachment, host scheduling and gameplay integration unverified.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/frame_time.c',root/'src/frame_time.h',binary]}}
(root/'reports/frame-time-original-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'source timing cases')
