"""Source orientation -> live world boundary -> full source camera diagnostics.
Offline instruction probes only; never linked into the native game.
"""
import hashlib,json,pathlib,subprocess,sys
root=pathlib.Path('outputs/crazy-taxi-native')
label=sys.argv[1];binary=pathlib.Path(sys.argv[2]);camera=root/('reports/'+label+'-camera.txt')
subprocess.run([sys.executable,str(root/'tools/compare_live_ordinary_orientation_rom.py'),label,str(binary)],check=True)
subprocess.run([sys.executable,str(root/'tools/compare_live_full_camera_view_rom.py'),label,str(camera),str(binary)],check=True)
o=[list(map(int,l.split())) for l in (root/('reports/'+label+'.txt')).read_text().splitlines()]
c=[list(map(int,l.split())) for l in camera.read_text().splitlines()]
assert o and len(o)==len(c)
for i,(a,b) in enumerate(zip(o,c)):
 assert a[19]==b[1],('world boundary changed heading20',i,a[19],b[1])
 if i:
  for pre,post in [(0,19),(2,20),(4,21),(6,22)]:
   assert b[pre]==c[i-1][post],('camera history discontinuity',i,pre)
settings=json.loads((root/('reports/'+label+'.json')).read_text())
assert settings['recovered_camera'] and settings['owned_graphics']
assert settings['viewport']==[3840,2160]
report=dict(status='PASS',calls=len(o),source_orientation_output_to_camera_input_links=len(o),camera_retained_state_links=(len(c)-1)*4,
 input_flags358=sorted(set(a[11] for a in o)),input_state564=sorted(set(a[7] for a in o)),input_timer2a=sorted(set(a[6] for a in o)),
 body_motion_equal_after=sum(a[18]==a[19] for a in o),capture=settings,
 scope='Actual native ordinary phase0 orientation inputs/outputs match full original12C6C; each produced heading20 matches subsequent camera input across native world boundary; camera inputs/outputs match full original14DD8, with four retained camera states continuous across all calls. Each original routine seeded from native actor/global inputs. Does not prove original full driver/world caller inputs, startup phase lifecycle, original GBA framebuffer, camera-facing intent or display60fps. No camera/sprite flip applied.',
 hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [pathlib.Path(__file__),binary,root/('reports/'+label+'.txt'),camera,root/('reports/'+label+'.json'),root/('reports/'+label+'.bmp')]})
(root/('reports/'+label+'-chain-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n')
print('PASS heading-camera chain:',len(o),'source-qualified routine pairs;',report['camera_retained_state_links'],'retained camera state links')
