"""Bounded original2518A..25392 commit tail plus actualD0B4 score helper.
Only diagnostic; never imported or interpreted by the native game runtime.
Memory copy/DMA and2776C/10114 boundaries are explicit contracts.
"""
from pathlib import Path
import sys,os,json,hashlib,subprocess,itertools,inspect,textwrap,struct
P=Path.cwd();B=P/'outputs/crazy-taxi-native';W=B/'work/native-profile-commit-20261016';sys.path.insert(0,str(B/'tools'));os.environ['TMP']=os.environ['TEMP']=str(W)
import bounded_normal_fade_probe as base
assert hashlib.sha256(base.ROM).hexdigest()==base.SHA
class CommitProbe(base.FadeProbe):
 def __init__(self,case):
  super().__init__();self.maps=[(0x02001000,bytearray(0x6000)),(0x03005000,bytearray(0x1000)),(0x10000000,bytearray(0x2000))];self.reg[13]=0x10000800;self.persist=0;self.copies=0;self.dma_copies=0
  mode,city,choice,current,fresh,exiting,rank,seed,whole,fraction=case
  block=bytearray(((i*17+seed*13)^90)&255 for i in range(512));snap=bytearray(((i*31+seed*7)^165)&255 for i in range(512));runtime=bytes(((i*19+seed*23)^60)&255 for i in range(800));block[0]=mode|(seed&3);block[4]=city;snap[0]=((mode+32)&96)|(seed&3);snap[4]=1-city
  for addr,data in ((0x02001eb0,block),(0x020020b0,snap),(0x020022b0,runtime),(0x030054a4,b'XYZ')):
   a,o=self.region(addr,len(data));a[o:o+len(data)]=data
  for a,v,n in ((0x02001538,current,1),(0x0200153d,0,1),(0x02003895,exiting,1),(0x020016d4,rank,1),(0x02001690,seed%8,1),(0x02001693,seed*5,1),(0x02001698,whole,4),(0x0200169c,fraction,4),(self.reg[13]+0x1e8,fresh,4),(self.reg[13]+0x1e4,choice,4)):self.write(a,v,n)
 def write(self,a,v,n=4):
  if a in (0x040000d4,0x040000d8,0x040000dc):
   assert n==4;self.dma[a]=v&0xffffffff
   if a==0x040000dc:
    assert v==0x80000028 and self.dma[0x040000d4]==0x02002058 and self.dma[0x040000d8]==0x020022b0
    data=bytes(self.read(self.dma[0x040000d4]+i,1) for i in range(80));p,o=self.region(self.dma[0x040000d8],80);p[o:o+80]=data;self.dma_copies+=1
   return
  return super().write(a,v,n)
 def external(self,target,stop):
  self.calls[target]=self.calls.get(target,0)+1
  if target==0x0800d0b4:self.run(target,stop,budget=10000);return
  if target==0x0801e7f0:
   src,dst,n=self.reg[:3];assert n in (4,8,512),(hex(src),hex(dst),n);data=bytes(self.read(src+i,1) for i in range(n));p,o=self.region(dst,n);p[o:o+n]=data;self.copies+=1
  elif target==0x0802776c:pass # Audio boundary, no modeled settings/score effects.
  elif target==0x08010114:assert self.reg[0]==4096;self.persist=1 # Actual SRAM callee excluded.
  else:raise AssertionError(('unmodeled original call',hex(target)))
  for j in range(4):self.reg[j]=0xdead0000+j
 def output(self):
  def raw(a,n):p,o=self.region(a,n);return bytes(p[o:o+n])
  return struct.pack('<I',self.persist)+raw(0x02001eb0,512)+raw(0x020022b0,800)
source=textwrap.dedent(inspect.getsource(base.FadeProbe.run));lines=source.splitlines()
for i,line in enumerate(lines):
 if line.strip().startswith('allowed='):lines[i]=line[:len(line)-len(line.lstrip())]+'allowed={0x0802518a:(0x0802518a,0x08025394),0x0800d0b4:(0x0800d0b4,0x0800d762)}'
i=next(i for i,l in enumerate(lines) if l.strip()=="elif m=='bl':");j=next(j for j in range(i+1,len(lines)) if lines[j].strip().startswith("elif m=='bx':"));pad=lines[i][:len(lines[i])-len(lines[i].lstrip())]
lines[i:j]=[pad+"elif m=='bl':",pad+' target=val(ops[0]);self.reg[14]=npc|1;self.external(target,npc)']
namespace=dict(vars(base));exec('\n'.join(lines)+'\n',namespace);CommitProbe.run=namespace['run'];(W/'executed-probe-run.py').write_text('\n'.join(lines)+'\n')
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest();paths=[B/'src/profile_commit.c',B/'src/profile_commit.h',B/'src/score_insert.c',B/'src/score_insert.h',B/'tests/profile_commit_rom_compare.c',B/'tests/profile_commit_test.c',Path(__file__),B/'tools/bounded_normal_fade_probe.py'];before={str(p):sha(p) for p in paths}
cc=P/'work/compiler/llvm-mingw-20261006/llvm-mingw-20261006-ucrt-x86_64/bin/x86_64-w64-mingw32-clang.exe';flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-march=x86-64','-mtune=generic','-fintegrated-cc1','-I',str(B/'src')]
executables={}
for test in ('profile_commit_test','profile_commit_rom_compare'):
 exe=W/(test.replace('_','-')+'.exe');cmd=[str(cc),*flags,str(B/'src/profile_commit.c'),str(B/'src/score_insert.c'),str(B/'tests'/f'{test}.c'),'-Wl,--stack,16777216','-o',str(exe)]
 with (W/(test+'.build.log')).open('wb') as f:r=subprocess.run(cmd,cwd=P,stdout=f,stderr=subprocess.STDOUT)
 assert not r.returncode,(r.returncode,(W/(test+'.build.log')).read_text());executables[test]=exe
unit=subprocess.run([str(executables['profile_commit_test'])],cwd=P,capture_output=True,timeout=30);(W/'unit.out.log').write_bytes(unit.stdout);(W/'unit.err.log').write_bytes(unit.stderr);assert not unit.returncode and not unit.stderr,(unit.returncode,unit.stderr)
cases=[]
for mode,city,choice,current,exiting,rank in itertools.product((0,32,64,96),(0,1),(0,1,2),(0,1),(1,2),(0,1,10,99)):
 i=len(cases);seed=i%7;fresh=(i//2)%2;whole=(-1,0,9000,131071)[i%4];fraction=(-1,0,99,128,37)[i%5];cases.append((mode,city,choice,current,fresh,exiting,rank,seed,whole,fraction))
args=''.join(' '.join(map(str,c))+'\n' for c in cases).encode();run=subprocess.run([str(executables['profile_commit_rom_compare'])],input=args,cwd=P,capture_output=True,timeout=60);(W/'native-results.bin').write_bytes(run.stdout);(W/'compare.err.log').write_bytes(run.stderr);assert not run.returncode and not run.stderr,(run.returncode,run.stderr);stride=4+512+800;assert len(run.stdout)==stride*len(cases)
steps=0;visited={};calls={};dma_cases=0;profile_modes={};fail=[]
for i,case in enumerate(cases):
 p=CommitProbe(case).run(0x0802518a,0x08025392,budget=10000);want=run.stdout[i*stride:(i+1)*stride];actual=p.output()
 if actual!=want:fail.append(dict(case=case,byte_differences=[(j,a,b) for j,(a,b) in enumerate(zip(actual,want)) if a!=b][:16]));break
 steps+=p.steps;visited.update(p.visited);dma_cases+=bool(p.dma_copies)
 for t,n in p.calls.items():calls[hex(t)]=calls.get(hex(t),0)+n
 profile_modes[str(case[0])]=profile_modes.get(str(case[0]),0)+1
assert not fail,fail
assert before=={str(p):sha(p) for p in paths}
r=dict(status='PASS_ORIGINAL_PROFILE_COMMIT_ALL_ORDINARY_MODES',cases=len(cases),original_thumb_instructions=steps,dma_refresh_cases=dma_cases,modes=profile_modes,original_calls=calls,visited_bytes={hex(a):v for a,v in sorted(visited.items())},rom_sha256=base.SHA,source_sha256=before,executables={n:dict(path=str(p),sha256=sha(p)) for n,p in executables.items()},native_output_sha256=sha(W/'native-results.bin'),limits=['Original2518A..25392 caller and D0B4 profile score routine executed; byte-for-byte all512 settings and800 runtime bytes plus persistence request','E7F0 data copies, DMA80-byte transfer,2776C audio boundary and10114 persistence request are explicit contracts; original SRAM checksum/write and whole GUI/earned progression excluded','Only ordinary mode0/20/40/60, valid city/profile/choice and153d0 supported; challenge/extra owner remains separate'])
(W/'profile-commit-rom-comparison.json').write_text(json.dumps(r,indent=2));print('PASS original profile commit',len(cases),'cases',steps,'actual original instructions, all512+800bytes and persistence request',flush=True)
