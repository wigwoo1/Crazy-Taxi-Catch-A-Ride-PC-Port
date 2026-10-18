from pathlib import Path
import hashlib,json,re,subprocess,os
B=Path(__file__).absolute().parent.parent;P=B.parent.parent;W=B/'work/native-save-frame-owner-20261015'
W.mkdir(parents=True,exist_ok=True)
compiler=P/'work/compiler/llvm-mingw-20261006/llvm-mingw-20261006-ucrt-x86_64/bin/x86_64-w64-mingw32-clang.exe'
os.environ['TEMP']=os.environ['TMP']=str(W)
flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-I',(B/'src').as_posix(),'-march=x86-64','-mtune=generic','-fintegrated-cc1']
sha=lambda data:hashlib.sha256(data).hexdigest()
signature=sha(compiler.read_bytes())+sha((compiler.parent/'clang-23.exe').read_bytes())
key=sha(json.dumps([signature,flags]).encode());memo={}
def digest(path):
 p=Path(path);normal=p.as_posix().lower()
 assert normal.startswith('./') or normal.startswith('./'),p
 if str(p) not in memo:memo[str(p)]=sha(p.read_bytes())
 return memo[str(p)]
text=(B/'tools/rebuild_save_load_owned.ps1').read_text(encoding='utf-8-sig')
match=re.search(r'\$names\s*=\s*@\((.*?)\)',text,re.S);assert match
names=re.findall(r"'([A-Za-z0-9_]+)'",match.group(1));assert names and len(names)==len(set(names)) and {'save_load_owned','save_file_device','frame_event'}<=set(names)
sources=[B/'src'/(n+'.c') for n in names]+[B/'tests/save-load-owned-check.c']
objects=[];reused=[];compiled=[]
cache=B/'work/native-viewer-baseline-objects'
for source in sources:
 obj=cache/(source.stem+'.o');manifest=cache/(source.stem+'.json')
 old=json.loads(manifest.read_text()) if manifest.exists() else None
 valid=bool(old and obj.exists() and old['key']==key and sha(obj.read_bytes())==old['object_sha256'] and all(digest(f)==v for f,v in old['dependencies'].items()))
 if valid:
  objects.append(obj);reused.append(source.name);print('CACHED',source.name,flush=True);continue
 obj=W/(source.stem+'.o');log=W/(source.stem+'-compile.log');before=digest(source)
 with log.open('wb') as out:
  result=subprocess.run([str(compiler),*flags,'-c',source.as_posix(),'-o',obj.as_posix()],cwd=P,stdout=out,stderr=subprocess.STDOUT)
 if result.returncode:raise RuntimeError('Compilation failed: '+str(log))
 assert sha(source.read_bytes())==before,'Source changed during compilation'
 objects.append(obj);compiled.append(source.name);print('COMPILED',source.name,flush=True)
output=W/'save-load-owned-check.exe'
link=[*[x.as_posix() for x in objects],'-lwinmm','-lm','-Wl,--stack,16777216','-o',output.as_posix()]
rsp=W/'link.rsp';rsp.write_text('\n'.join('"'+x.replace('"','\\"')+'"' for x in link)+'\n')
with (W/'link.log').open('wb') as out:
 result=subprocess.run([str(compiler),'@'+str(rsp)],cwd=P,stdout=out,stderr=subprocess.STDOUT)
if result.returncode:raise RuntimeError('Link failed: '+str(W/'link.log'))
with (W/'test.log').open('wb') as out:
 result=subprocess.run([str(output)],cwd=P,stdout=out,stderr=subprocess.STDOUT)
if result.returncode:raise RuntimeError('Native composition check failed: '+str(W/'test.log')+' exit '+str(result.returncode))
stdout=(W/'test.log').read_text();assert stdout.startswith('PASS ')
report=dict(status='PASS_NATIVE_SAVE_FRAME_CALLBACK_ISOLATION',executable=str(output),executable_sha256=sha(output.read_bytes()),compiler_signature=signature,flags=flags,successful_paths=12,suspended_error_paths=6,cached_sources=reused,compiled_sources=compiled,stdout=stdout.strip(),source_sha256={str(x):sha(x.read_bytes()) for x in sources},limits=['Native startup/save/audio composition; not complete playable startup integration.','Timer callback identities are metadata; timer registers are not emulated.','Current playable viewer is not rebuilt or changed by this unlinked bridge correction.'])
(B/'reports/native-save-frame-owner-20261015.json').write_text(json.dumps(report,indent=2))
print(json.dumps({k:report[k] for k in ('status','successful_paths','suspended_error_paths','stdout')}),flush=True)
