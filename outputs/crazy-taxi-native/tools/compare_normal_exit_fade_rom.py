import hashlib, json, os, pathlib, subprocess, sys, time
P=pathlib.Path(r'./');B=P/'outputs/crazy-taxi-native';W=B/'work/native-normal-parent-20261015'
sys.path.insert(0,str(B/"tools"))
from bounded_normal_fade_probe import FadeProbe,lookup,SHA,signed,ROM
os.environ['TEMP']=os.environ['TMP']=str(W)
CC=P/'work/compiler/llvm-mingw-20261006/llvm-mingw-20261006-ucrt-x86_64/bin/x86_64-w64-mingw32-clang.exe'
EXE=W/'normal-exit-fade-compare.exe'
flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-march=x86-64','-mtune=generic','-fintegrated-cc1']
inputs=[B/'src'/f for f in ['palette_fade_registered.c','palette_fade_setup.c','palette_fade.c']]+[B/'tests/normal_exit_fade_rom_compare.c']
print('Building native fade comparison.',flush=True)
t=time.monotonic();run=subprocess.run([str(CC),*flags,'-I',str(B/'src'),*[str(p) for p in inputs],'-Wl,--stack,16777216','-o',str(EXE)],cwd=P,capture_output=True,text=True)
(W/'native-build.log').write_text(run.stdout+run.stderr);assert run.returncode==0,run.stderr
guard=W/'normal-exit-fade-check.exe'
check=subprocess.run([str(CC),*flags,'-I',str(B/'src'),*[str(p) for p in inputs[:3]],str(B/'tests/normal_exit_fade_check.c'),'-Wl,--stack,16777216','-o',str(guard)],cwd=P,capture_output=True,text=True)
(W/'native-guard-build.log').write_text(check.stdout+check.stderr);assert check.returncode==0,check.stderr
check=subprocess.run([str(guard)],cwd=P,capture_output=True,text=True)
(W/'native-guard-test.log').write_text(check.stdout+check.stderr);assert check.returncode==0,(check.returncode,check.stdout,check.stderr)
print(check.stdout.strip(),flush=True)
print('Native build passed.' ,round(time.monotonic()-t,2),'seconds',flush=True)
LUT,gen=lookup();print('Original lookup passed.',flush=True)

def original(case):
 stage,mode,cap,bg,obj,limit,active,domain,colors,offset,seed,progress,fade_active,delta,interval,reload,endpoints=case
 p=FadeProbe()
 for i,b in enumerate(LUT):p.write(0x02001a10+i,b,1)
 p.write(0x02001780,1,2)
 for a,v in [(0x02007542,240),(0x02007544,160),(0x0200177c,256),(0x0200177e,256),(0x04000050,0x1234),(0x04000054,0x2345),(0x0400004c,0x3456)]:p.write(a,v,2)
 p.write(0x02003b00,delta)
 for off,v in [(1,interval),(2,reload)]:p.write(0x02001770+off,v,1)
 for off,v in [(0,colors),(2,offset),(4,limit)]:p.write(0x02001790+off,v,2)
 p.write(0x02001796,active,1);p.write(0x02001797,domain,1);p.write(0x020017a0,0x084a5a34 if seed==65536 else 0x09010000)
 for i in range(256):p.write(0x09010000+i*2,(i*131+seed)&65535,2)
 d,o=p.region(0x05000000,1024);d[o:o+1024]=b'\xa5'*1024
 if stage:
  if (bg|obj)&0x2000:p.write(0x04000050,0xff,2)
  for off,v,size in [(3,mode,1),(4,endpoints,1),(11,fade_active,1),(22,bg,2),(24,obj,2),(26,progress,2),(28,min(cap&255,32),2)]:p.write(0x02001770+off,v,size)
  for off in range(5,11):p.write(0x02001770+off,1,1)
  motion=[-240*4096,160*4096,240*4096,240*256,-160*256,-240*256] if mode else [0,0,0,-240*256,160*256,240*256]
  for i,v in enumerate(motion):p.write(0x020094f0+i*4,v)
  p.run(0x0800ee10,0x0800f5c6)
 else:
  p.reg[:4]=[mode,cap,bg,obj];p.run(0x0800e5a8,0x0800edde)
 read=lambda off,n=1:p.read(0x02001770+off,n)
 pr=read(26,2);pr=pr-65536 if pr&32768 else pr
 state=[read(3),read(28,2),read(22,2),read(24,2),pr,read(11),read(1),read(2),read(4),read(12,2),read(14,2),p.read(0x04000050,2),p.read(0x04000054,2),p.read(0x0400004c,2)]
 state += [signed(p.read(0x020094f0+i*4)) for i in range(6)]+[read(i) for i in range(5,11)]
 d,o=p.region(0x05000000,1024)
 return bytes(d[o:o+1024]),state,p

cases=[]
for mode in (0,1):
 for bg,obj in [(0x201,0x201),(0x201,1),(1,0x201),(1,1),(0x2001,0x201),(0x201,0x2001)]:
  for domain in (0,1):
   for limit,active,colors,offset,seed in [(32,1,16,32,131),(33,1,256,0,511),(0,0,16,480,32769),(0,1,256,0,49151)]:
    cases.append((0,mode,32,bg,obj,limit,active,domain,colors,offset,seed,0,0,1,3,2,0))
for mode in (0,1):
 for bg,obj in [(0x201,0x201),(0x201,1),(1,0x201),(1,1),(0x2001,0x201),(0x201,0x2001)]:
  for domain in (0,1):
   for progress in range(32):
    timing=[(1,1,0,0,0),(1,2,1,2,1),(1,60,3,0,0),(0,1,0,0,0),(1,32,128,255,0),(1,1,255,3,1)][progress%6]
    fa,delta,interval,reload,endpoints=timing
    cases.append((1,mode,32,bg,obj,32,1,domain,16,32,(progress*997+32769)&65535,progress,fa,delta,interval,reload,endpoints))
   for limit,active,colors,offset in [(33,1,256,0),(0,0,16,480),(0,1,256,0)]:
    for progress in (0,15,31):cases.append((1,mode,32,bg,obj,limit,active,domain,colors,offset,131,progress,1,60,1,0,1))
# Actual unmodified ROM palette covers every progress row in both domains.
for mode in (0,1):
 for domain in (0,1):
  cases.append((0,mode,32,0x201,0x201,32,1,domain,256,0,65536,0,1,1,0,0,0))
  for progress in range(32):cases.append((1,mode,32,0x201,0x201,32,1,domain,256,0,65536,progress,1,1,0,0,0))
run=subprocess.run([str(EXE),str(P.parent/'Crazy Taxi - Catch a Ride (USA).gba')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,cwd=P)
(W/'native-comparison.stderr.log').write_text(run.stderr);assert run.returncode==0,run.stderr
rows=run.stdout.splitlines();assert len(rows)==len(cases)
steps=0;visited=dict(gen.visited);dma=0
for k,(case,row) in enumerate(zip(cases,rows)):
 parts=row.split();banks,state,p=original(case)
 assert banks==bytes.fromhex(parts[0]),('banks',k,case,next((j for j,(a,b) in enumerate(zip(banks,bytes.fromhex(parts[0]))) if a!=b),None))
 assert state==list(map(int,parts[1:])),('state',k,case,state,parts[1:])
 steps+=p.steps;visited.update(p.visited);dma+=int(0x040000dc in p.dma)
 if k%100==0:print('Matched',k+1,'/',len(cases),flush=True)
report={'status':'PASS','rom_sha256':SHA,'cases':len(cases),'initialization_cases':sum(c[0]==0 for c in cases),'frame_service_cases':sum(c[0]==1 for c in cases),'lookup_bytes':1024,'lookup_sha256':hashlib.sha256(LUT).hexdigest(),'lookup_instructions':gen.steps,'actual_palette_and_service_thumb_instructions':steps,'instruction_sites':len(visited),'executable_sha256':hashlib.sha256(EXE.read_bytes()).hexdigest(),'guard_executable_sha256':hashlib.sha256(guard.read_bytes()).hexdigest(),'guard_test':check.stdout.strip(),'sources':{str(p.relative_to(B)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs},'actual_rom_palette_cases':sum(c[10]==65536 for c in cases),'retained_initial_reference_error':'work/native-normal-parent-20261015/comparison-attempt1-error.txt','scope':'Original E5A8..EDDC normal0/1 initial palette and EE10..F5C4 service; one explicit16/256-color nonanimated registered record, both domains, mask1/0x201 and mixed global0x2000 bypass; all32progress rows, cap eligibility/inactive records, full1024 bankbytes and26state fields. Original F69C..F730 generates all1024 lookup bytes. Signed division and DMA data copies are explicit diagnostic contracts. Equal scales only. This is not a whole game/emulator/render/parent-flow test.','instructions':{f'{k:08x}':v for k,v in sorted(visited.items())}}
(B/'reports/normal-exit-fade-rom-comparison-20261015.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',len(cases),'cases;',steps,'original instructions;',round(time.monotonic()-t,2),'seconds.',flush=True)
