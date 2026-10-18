from pathlib import Path
import json,struct,subprocess
exec(Path('outputs/crazy-taxi-native/tools/compare_world_bounds_corners_rom.py').read_text().split('\ncases=[]')[0])
results=[];steps=0
asset=(root/'assets/challenge-worlds/challenge5.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',asset,4);grid_offset=20+nv*16+np*24+nm*28
assert asset[grid_offset:grid_offset+4]==b'CTC1'
nx,nz,sx,sz,ox,oy,oz=struct.unpack_from('<7i',asset,grid_offset+4)
for label in ('second','third'):
 fixture=(root/f'reports/bound-{label}-frame-native-producer-owners.fixture').read_bytes();view=struct.unpack_from('<18i',fixture);corners=struct.unpack_from('<32i',fixture,72)
 retained_sine=struct.unpack_from('<4096i',fixture,200);retained_cosine=struct.unpack_from('<4096i',fixture,200+16384)
 assert tuple(sine)==retained_sine and tuple(cosine)==retained_cosine
 pitch,yaw=view[3:5]
 assert [view[6],view[9],view[7],view[10]]==[sine[pitch&4095],cosine[pitch&4095],sine[yaw&4095],cosine[yaw&4095]]
 case=[view[0],view[2],yaw,pitch,nx,nz,sx,sz,ox,oz]+[corners[i*4+j] for i in range(8) for j in range(3)]
 before=steps;source=original(case)
 cp=subprocess.run([str(root/'bin/world-bounds-corners-rom-compare.exe'),str(root/'assets/driving.ctr')],input=' '.join(map(str,case))+'\n',capture_output=True,text=True)
 assert cp.returncode==0 and source==list(map(int,cp.stdout.split()))
 cells=(source[2]-source[0]+1)*(source[3]-source[1]+1)
 observed=json.loads((root/f'reports/bound-{label}-frame-raster-return-audit.json').read_text())['cells'];assert cells==observed
 results.append({'frame':label,'camera_xz':[view[0],view[2]],'yaw':yaw,'pitch':pitch,'source_native_bounds':source,'inclusive_cells':cells,'actual_original_renderer_cells':observed,'source_arm_instructions':steps-before})
report={'status':'NATIVE_ORIGINAL_RETAINED_WORLD_BOUNDS_PASS','cases':results,'scope':'ActualROM2D18..2E6C instruction prefix versus existingnative bounds_corners with actualBound challenge5 grid, retainedview trig coefficients and inheritedcorners. Signeddivision contract; sourceoriginalrenderer observed cellcount also matches. Doesnotverify fullcell traversal/references/rawpacket generation/cache lifecycle/livegame.'}
(root/'reports/bound-retained-native-world-bounds-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
