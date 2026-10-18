from pathlib import Path
import argparse,re,subprocess,json,hashlib
ap=argparse.ArgumentParser();ap.add_argument('--compiler',required=True);ap.add_argument('--list-only',action='store_true');a=ap.parse_args()
root=Path(__file__).resolve().parent;base=root/'outputs/crazy-taxi-native'
manifest=(base/'build.ps1').read_text(encoding='utf-8-sig');block=re.search(r'\$sceneViewerSourceNames\s*=\s*@\((.*?)\)',manifest,re.S);assert block
names=re.findall(r"'([A-Za-z0-9_]+\.c)'",block[1]);assert len(names)==len(set(names)) and all((base/'src'/n).is_file() for n in names)
if a.list_only:print(json.dumps(dict(sources=len(names),all_present=True)));raise SystemExit(0)
out=base/'bin';out.mkdir(exist_ok=True);obj=base/'work/public-build';obj.mkdir(parents=True,exist_ok=True)
flags=['-std=c11','-Wall','-Wextra','-Werror','-O2','-march=x86-64','-mtune=generic','-I',str(base/'src')]
objects=[]
for n in names:
 target=obj/(Path(n).stem+'.o');subprocess.run([a.compiler,*flags,'-c',str(base/'src'/n),'-o',str(target)],check=True);objects.append(target)
exe=out/'scene-viewer.exe';rsp=obj/'link.rsp';rsp.write_text('\n'.join('"'+str(x).replace('\\','/')+'"' for x in [*objects,'-lopengl32','-lgdi32','-luser32','-lwinmm','-lm','-Wl,--stack,16777216','-o',exe]))
subprocess.run([a.compiler,'@'+str(rsp)],check=True);print('Built',exe,'SHA256',hashlib.sha256(exe.read_bytes()).hexdigest())
