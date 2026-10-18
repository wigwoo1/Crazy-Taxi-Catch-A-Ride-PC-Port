"""Compare original full14DD8 mode2 to actual native helpers."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
class TimeCameraProbe(CameraProbe):
 def read(self,a,n=4):
  if a==0x30053c0:return self.global_delta&0xffffffff
  return super().read(a,n)
import sys
label=sys.argv[1] if len(sys.argv)>1 else 'live-forward'
capture=pathlib.Path(sys.argv[2]) if len(sys.argv)>2 else root/('reports/'+label+'-camera-states.txt')
binary=pathlib.Path(sys.argv[3]) if len(sys.argv)>3 else root/'bin/scene-viewer.exe'
records=[list(map(int,l.split())) for l in capture.read_text().splitlines()];assert records and all(len(a)==32 for a in records)
cases=[a[:19] for a in records];rows=[a[19:31] for a in records]
for index,(case,row) in enumerate(zip(cases,rows)):
 ch,h,cp,bp,off,target,zoom,scale,flags,delta,freeze,ay,az,cam_base,x,y,z,active,mode=case;p=TimeCameraProbe((0,0,0,0,0,1,0,0,0,0));p.global_delta=records[index][31];base=0x2002634
 for a in range(base,base+0xea0,4):p.arm.mem[a]=0
 for o,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x64c,mode,2),(0x654,ch,4),(0x650,cp,4),(0x20,h,4),(0x1c,h,4),(0x18,bp,2),(0xe84,scale,4),(0x4ac,flags,1),(0x680,cam_base,4)]:p.write(base+o,v,n)
 for a,v in {0x2005f20:freeze,0x2001708:0,0x20034d4:1,0x20034d8:1,0x2001674:0,0x2001678:ay,0x200167c:az,0x2001694:(active&255)<<16,0x2003b00:delta,0x2002618:off,0x200261c:target,0x2002624:0,0x2002628:zoom,0x3006ac4:0x8000a40,0x3006a90:0x8000718,0x3006ab8:0x80009bc,0x3006a94:0x80009d4,0x3006ab4:0x80009ec}.items():p.arm.mem[a]=v
 p.reg[:2]=[base+0x498,base];p.reg[14]=0x10020001;p.run(entry=0x8014dd8,stop=0x10020000)
 got=[signed(p.read(base+o)) for o in [0x654,0x650]]+[signed(p.arm.mem[0x2002618]),signed(p.arm.mem[0x2002628])]+[signed(p.read(base+o)) for o in [0x76c,0x770,0x498,0x49c,0x4a0]]+[p.read(base+o,2) if p.read(base+o,2)<32768 else p.read(base+o,2)-65536 for o in [0x4a4,0x4a6,0x4a8]]
 assert got==row,dict(index=index,case=case,original=got,native=row)
report=dict(cases=len(cases),pass_match=True,scope='Original full14DD8 mode2 vs native follow/offset/view, actual native camera updates with separate captured frame3B00 and global53C0 owners compared to full original14DD8. Each original call seeded from captured inputs; source pose/preset/bootstrap and retained-history not independently proven. Division contract, no GBA pixelmatch.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'tools/compare_live_camera_time_owners_rom.py',root/'tools/probe_full_camera_view_rom.py',root/'tests/camera_view_rom_compare.c',root/'bin/camera-view-rom-compare.exe',root/'src/camera_angle.c',capture,root/'src/scene_viewer.c',binary]}
(root/('reports/'+label+'-full-camera-rom-comparison.json')).write_text(json.dumps(report,indent=2));print('PASS',report)
