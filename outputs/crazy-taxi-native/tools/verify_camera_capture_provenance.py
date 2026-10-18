import hashlib,json,pathlib,subprocess,sys
root=pathlib.Path('outputs/crazy-taxi-native');binary=root/'bin/scene-viewer-camera-provenance-check.exe';reports=root/'reports'
label='camera-heading-chain-120'
args=['--drive','--recovered-camera','--taxi-owned-graphics','--ticks','120','--snapshot',str(reports/(label+'.bmp')),'--orientation-state-trace',str(reports/(label+'.txt')),'--camera-state-trace',str(reports/(label+'-camera.txt')),'--taxi-source-capture',str(reports/(label+'.json'))]
r=subprocess.run([str(binary),*args],capture_output=True,text=True,creationflags=subprocess.CREATE_NO_WINDOW)
(reports/(label+'.log')).write_text(r.stdout);(reports/(label+'.err')).write_text(r.stderr);assert r.returncode==0,r.stderr
checks=[]
for name,extra,expected in [('missing-camera',['--drive'],'requires driving with recovered camera'),('orbit',['--drive','--recovered-camera','--capture-orbit','10'],'requires driving with recovered camera'),('no-taxi',['--recovered-camera'],'Snapshot did not draw a source taxi')]:
 path=reports/('camera-capture-reject-'+name+'.json');assert not path.exists(),path
 args=extra+['--ticks','0','--snapshot',str(reports/('camera-capture-reject-'+name+'.bmp')),'--taxi-source-capture',str(path)]
 r=subprocess.run([str(binary),*args],capture_output=True,text=True,creationflags=subprocess.CREATE_NO_WINDOW)
 assert r.returncode!=0 and expected in r.stderr and not path.exists(),(name,r.returncode,r.stderr,path.exists())
 checks.append(dict(case=name,exit=r.returncode,error=r.stderr.strip(),capture_created=False))
subprocess.run([sys.executable,str(root/'tools/compare_live_heading_camera_chain_rom.py'),label,str(binary)],check=True)
report=dict(status='PASS',rejections=checks,valid_capture=label,scope='Separate native diagnostic binary; installed playable binary unchanged. Capture configuration and actual draw guards, then120 original routine pairs and retained heading/camera links. Full GBA camera framing still unproved.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [pathlib.Path(__file__),binary,root/'src/scene_viewer.c']})
(reports/'camera-capture-provenance-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS valid native4K capture and all3 invalid camera/taxi capture rejections')
