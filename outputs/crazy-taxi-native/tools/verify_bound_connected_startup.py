from pathlib import Path
import subprocess,json
root=Path('outputs/crazy-taxi-native');reports=root/'reports';stem='bound-integrated-connected-startup'
args=[str(root/'bin/scene-viewer-bound-integrated.exe'),'--startup-game-logic-smoke','--native-elapsed-time','--recovered-game-audio','--recovered-camera','--host-time-trace',str(reports/(stem+'.txt'))]
p=subprocess.run(args,capture_output=True,text=True,timeout=120,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(stem+'.log')).write_text(p.stdout);(reports/(stem+'.err')).write_text(p.stderr)
print('exit',p.returncode,'stderr',p.stderr,flush=True)
assert p.returncode==0 and not p.stderr
q=subprocess.run([__import__('sys').executable,str(root/'tools/verify_connected_host_time.py'),stem,'scene-viewer-bound-integrated.exe'],capture_output=True,text=True)
print(q.stdout,q.stderr);assert q.returncode==0
