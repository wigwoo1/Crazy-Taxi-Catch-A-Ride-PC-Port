"""Compare original full14DD8 mode2 to actual native helpers."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
import sys
label=sys.argv[1] if len(sys.argv)>1 else 'camera-distinct-fresh'
body_offset=int(sys.argv[2]) if len(sys.argv)>2 else 0
capture=root/('reports/'+label+'-camera-states.txt')
records=[list(map(int,l.split())) for l in capture.read_text().splitlines()];assert len(records)>0 and all(len(a)==32 for a in records)
cases=[a[:19] for a in records];rows=[a[19:31] for a in records]
p=CameraProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
carry_checks=0;thumb_steps=arm_steps=0;yaw_aliases=0
for index,(case,row) in enumerate(zip(cases,rows)):
 ch,h,cp,bp,off,target,zoom,scale,flags,delta,freeze,ay,az,cam_base,x,y,z,active,mode=case
 if index:
  retained=[signed(p.read(base+0x654)),signed(p.read(base+0x650)),signed(p.arm.mem[0x2002618]),signed(p.arm.mem[0x2002628])]
  assert retained==[ch,cp,off,zoom],dict(index=index,retained=retained,captured=[ch,cp,off,zoom]);carry_checks+=4
 for o,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x64c,mode,2),(0x654,ch,4),(0x650,cp,4),(0x20,h,4),(0x1c,signed((records[index][31]+body_offset)&0xffffffff),4),(0x18,bp,2),(0xe84,scale,4),(0x4ac,flags,1),(0x680,cam_base,4)]:
  if index and o in [0x654,0x650]:continue
  p.write(base+o,v,n)
 for a,v in {0x2005f20:freeze,0x2001708:0,0x20034d4:1,0x20034d8:1,0x2001674:0,0x2001678:ay,0x200167c:az,0x2001694:(active&255)<<16,0x2003b00:delta,0x2002618:off,0x200261c:target,0x2002624:0,0x2002628:zoom,0x3006ac4:0x8000a40,0x3006a90:0x8000718,0x3006ab8:0x80009bc,0x3006a94:0x80009d4,0x3006ab4:0x80009ec}.items():
  if index and a in [0x2002618,0x2002624,0x2002628]:continue
  p.arm.mem[a]=v
 p.steps=0;p.arm.steps=0
 p.reg[:2]=[base+0x498,base];p.reg[14]=0x10020001;p.run(entry=0x8014dd8,stop=0x10020000)
 got=[signed(p.read(base+o)) for o in [0x654,0x650]]+[signed(p.arm.mem[0x2002618]),signed(p.arm.mem[0x2002628])]+[signed(p.read(base+o)) for o in [0x76c,0x770,0x498,0x49c,0x4a0]]+[p.read(base+o,2) if p.read(base+o,2)<32768 else p.read(base+o,2)-65536 for o in [0x4a4,0x4a6,0x4a8]]
 if got[10]!=row[10]:
  assert (got[10]&4095)==row[10],dict(index=index,raw_original_yaw=got[10],native_yaw=row[10]);yaw_aliases+=1;got[10]&=4095
 thumb_steps+=p.steps;arm_steps+=p.arm.steps
 assert got==row,dict(index=index,case=case,original=got,native=row)
report=dict(controlled_body_offset=body_offset,distinct_heading_samples=sum(a[1]!=a[31] for a in records),cases=len(cases),carry_checks=carry_checks,yaw_equivalent_aliases=yaw_aliases,thumb_instructions=thumb_steps,arm_instructions=arm_steps,pass_match=True,scope='Fresh bodyheading1C and movementheading20 supplied independently; optional controlled body offset tests mode2 dependency without changing movement/camera inputs. Original full14DD8 mode2 vs native follow/offset/view, Actual native camera updates compared to full original14DD8. Original actor memory initialized once, camera650/654/2618/2624/2628 and all unmodified actor state retained across the captured calls. External pose/preset/phase inputs and camera outputs captured from freshlybuilt native viewer120frame offscreen run; source bootstrap/fullgame not independently proven. Yaw compared modulo4096 only: original signed halfword may be negative, native wraps0..4095. Other11outputs exact. Division contract, no GBA pixelmatch.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'tools/compare_distinct_headings_camera_rom.py',root/'tools/probe_full_camera_view_rom.py',root/'bin/scene-viewer-distinct-headings-check.exe',root/'src/camera_angle.c',capture,root/'src/scene_viewer.c',root/'src/engine_rotate_owned.c',root/'src/engine_matrix_call.c']}
(root/('reports/'+label+('-body-offset-'+str(body_offset) if body_offset else '')+'-persistent-camera-rom-comparison.json')).write_text(json.dumps(report,indent=2));print('PASS',report)
