"""Baseline Windows viewer build using the project's canonical module list."""
from pathlib import Path
import argparse,concurrent.futures,hashlib,json,os,re,shlex,subprocess,threading
B=Path(__file__).absolute().parent.parent;P=B.parent.parent
ap=argparse.ArgumentParser();ap.add_argument('--compiler',required=True);ap.add_argument('--output',default='scene-viewer.exe');args=ap.parse_args()
assert Path(args.output).name==args.output and args.output.endswith('.exe')
compiler=Path(args.compiler);cache=B/'work/native-viewer-baseline-objects';cache.mkdir(parents=True,exist_ok=True)
os.environ['TEMP']=os.environ['TMP']=str(cache)
text=(B/'build.ps1').read_text(encoding='utf-8-sig');match=re.search(r'\$sceneViewerSourceNames\s*=\s*@\((.*?)\)',text,re.S);assert match
names=re.findall(r"'([A-Za-z0-9_]+\.c)'",match.group(1));assert len(names)==len(set(n.lower() for n in names)) and 'scene_viewer.c' in names
available={x.name for x in (B/'src').iterdir()};assert all(n in available for n in names)
flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-I',(B/'src').as_posix(),'-march=x86-64','-mtune=generic','-fintegrated-cc1']
sha=lambda data:hashlib.sha256(data).hexdigest()
frontend=compiler.parent/'clang-23.exe'
signature=sha(compiler.read_bytes())+sha(frontend.read_bytes())
key=sha(json.dumps([signature,flags]).encode());lock=threading.Lock();memo={}
def digest(path):
 path=Path(path);name=str(path)
 with lock:
  if name not in memo:memo[name]=sha(path.read_bytes())
  return memo[name]
def deps(path):
 raw=path.read_text(encoding='utf-8',errors='strict');body=raw.split(': ',1)[1];body=re.sub(r'\\\r?\n',' ',body)
 result=[]
 for value in shlex.split(body,posix=True):
  f=Path(value)
  if not f.is_absolute():f=B/f
  normal=f.as_posix().lower()
  assert normal.startswith('./') or normal.startswith('./'),f
  result.append(f)
 assert result
 return result
completed=0

def build(name):
 global completed
 source=B/'src'/name;obj=cache/(source.stem+'.o');dep=cache/(source.stem+'.d');manifest=cache/(source.stem+'.json');log=cache/(source.stem+'.log')
 old=json.loads(manifest.read_text()) if manifest.exists() else None
 hit=bool(old and obj.exists() and old['key']==key and sha(obj.read_bytes())==old['object_sha256'] and all(Path(f).exists() and digest(f)==v for f,v in old['dependencies'].items()))
 if not hit:
  initial=sha(source.read_bytes());temp=cache/(source.stem+'.building.o')
  command=[str(compiler),*flags,'-MMD','-MF',dep.as_posix(),'-c',source.as_posix(),'-o',temp.as_posix()]
  with log.open('wb') as out:
   code=subprocess.run(command,cwd=B,stdout=out,stderr=subprocess.STDOUT).returncode
  if code:raise RuntimeError(f'Compile failed {name}; see {log}')
  assert sha(source.read_bytes())==initial,'Source changed while compiling'
  dependencies={str(f):digest(f) for f in deps(dep)}
  temp.replace(obj);manifest.write_text(json.dumps(dict(key=key,dependencies=dependencies,object_sha256=sha(obj.read_bytes())),indent=2))
 with lock:
  completed+=1;print(f'{"CACHED" if hit else "COMPILED"} {completed}/{len(names)} {name}',flush=True)
 return obj
pool=concurrent.futures.ThreadPoolExecutor(max_workers=2)
futures={pool.submit(build,n):n for n in names};built={}
try:
 for future in concurrent.futures.as_completed(futures):built[futures[future]]=future.result()
except BaseException:
 for future in futures:future.cancel()
 pool.shutdown(wait=True,cancel_futures=True)
 raise
else:pool.shutdown(wait=True)
objects=[built[n] for n in names]
output=B/'bin'/args.output;output.parent.mkdir(exist_ok=True)
temp=cache/(args.output+'.linking.exe');rsp=cache/(args.output+'.link.rsp')
link=[*[o.as_posix() for o in objects],'-lopengl32','-lgdi32','-luser32','-lwinmm','-lm','-Wl,--stack,16777216','-o',temp.as_posix()]
rsp.write_text('\n'.join('"'+s.replace('"','\\"')+'"' for s in link)+'\n')
with (cache/(args.output+'.link.log')).open('wb') as out:
 code=subprocess.run([str(compiler),'@'+str(rsp)],cwd=B,stdout=out,stderr=subprocess.STDOUT).returncode
if code:raise RuntimeError('Link failed; see '+str(cache/(args.output+'.link.log')))
if output.exists():
 backup=cache/(args.output+'.before-'+sha(output.read_bytes())[:12]+'.exe')
 if not backup.exists():backup.write_bytes(output.read_bytes())
temp.replace(output)
report=dict(status='PASS_CANONICAL_BASELINE_VIEWER_BUILD',canonical_sources=len(names),compiler_signature=signature,compile_flags=flags,stack_reserve=16777216,stack_commit=4096,output=str(output),output_sha256=sha(output.read_bytes()),limits=['Compilation/link only; CPU instruction audit and native runtime tests still required.'])
(B/'reports'/('native-viewer-build-'+args.output+'.json')).write_text(json.dumps(report,indent=2))
print(json.dumps(report),flush=True)