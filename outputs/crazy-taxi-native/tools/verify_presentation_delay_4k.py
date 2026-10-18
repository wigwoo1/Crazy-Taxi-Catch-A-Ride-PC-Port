from pathlib import Path
import subprocess,json,hashlib,csv,re
r=Path('outputs/crazy-taxi-native');b=r/'bin/scene-viewer-presentation-delay.exe';cases=[(['--presentation-delay-ms',x],'Presentation delay must be') for x in ['-1','251','nan','abc']]+[(['--presentation-delay-ms','125'],'Presentation delay requires'),(['--presentation-delay-ms','5','--presentation-delay-ms','10'],'Duplicate presentation delay')];tested=[]
for args,message in cases:
 p=subprocess.run([str(b)]+args,capture_output=True,text=True,timeout=5);assert p.returncode!=0 and message in p.stderr,(args,p.returncode,p.stderr);tested.append(args)
stem='presentation-delay-4k-preview';log=(r/f'reports/{stem}.log').read_text();err=(r/f'reports/{stem}.err').read_text();assert not err and 'LIVE4K GPU COMPLETED: 360 frames at3840x2160' in log and 'NATIVE4K TARGET: rendered360 window blits360.' in log
rows=[l.split() for l in (r/f'reports/{stem}.presentation.txt').read_text().splitlines()];assert len(rows)==360
for v in rows:
 now,target=int(v[0]),int(v[1]);assert target==max(0,now-125000000);assert 0<=float(v[4])<=1
report=dict(status='PASS_CONFIGURABLE_PRESENTATION_DELAY_4K',invalid_argument_cases=len(tested),frames=len(rows),delay_ns=125000000,parser_cases=tested,scope='Strict nativeviewer completed360distinctGPU4Kframequeries and360internal4Ktargetrenders/blits, timestamptraceactualtarget equalsmax(now-125ms,0) everyframe andtimestampedstateimmutabilityassertionsretained. Generalphysicsclock unchanged; doesnotdemonstrateoriginal4..7cadenceinliveviewer, actualmonitor4Kscanout, sustained60orvalidatedinputlatency. Nativeaudio previewdeviceoutputdisabled; no audio listeningclaim. Initialseparatelaunchmissingaudio-previewflag failedbeforegameplay andlogs preserved.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [b,r/'src/scene_viewer.c',r/f'reports/{stem}.log',r/f'reports/{stem}.presentation.txt']})
(r/'reports/presentation-delay-4k-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('hashes','parser_cases')},indent=2))
