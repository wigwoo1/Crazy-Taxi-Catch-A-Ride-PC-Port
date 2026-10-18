from pathlib import Path
import sys,json,subprocess
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_source240_cell_visibility.py').read_text().split('\nresults=[]')[0])
src=src.replace('pc not in (0x08002fb0,0x08002fb4)','pc!=0x08002f84');exec(src);arm.detail=True;BASE=0x08000700;CellProbe.run=MarkerRasterProbe.run
base=[sx,sz,ox,oz,j['camera'][0],j['camera'][2],sine[(-j['yaw'])&4095],cosine[(-j['yaw'])&4095],sine[(-j['pitch'])&4095],cosine[(-j['pitch'])&4095],160,256,38400];cases=[base+[c['x'],c['z']] for c in j['cells']];actual=len(cases)
for angle in [0,1,1024,2048,4095]:
 for x,z in [(0,0),(131,84),(255,255),(0xffffffff,1)]:
  for horizontal in [0,1,160,-160]:
   a=base.copy();a[6:10]=[sine[angle],cosine[angle],sine[(angle+142)&4095],cosine[(angle+142)&4095]];a[10]=horizontal;cases.append(a+[x,z])
for origin in [-2147483648,2147483647]:
 a=base.copy();a[2]=origin;a[3]=-origin if origin!=-2147483648 else origin;cases.append(a+[0xffffffff,0xffffffff])
cp=subprocess.run([str(root/'bin/world-cell-visibility-probe.exe')],input=''.join(' '.join(map(str,a))+'\n' for a in cases),capture_output=True,text=True,check=True);rows=cp.stdout.splitlines();assert len(rows)==len(cases);steps=0;nonzero=0
for a,row in zip(cases,rows):
 q=CellProbe({'x':a[13],'z':a[14]});q.reg[1]=(a[2]-a[4])&MASK;q.reg[2]=(a[3]-a[5])&MASK
 for off,v in [(0x40,a[0]),(0x44,a[1])]:q.write(0x12000000+off,v)
 for off,v in zip([0x34,0x40,0x30,0x3c],a[6:10]):q.write(0x14000000+off,v)
 for off,v in zip([0x38,0x2c,0x30],a[10:13]):q.write(0x03006810+off,v)
 for i,v in enumerate([0,0,a[0],a[0]]):q.write(0x030068e0+i*4,v)
 for i,v in enumerate([0,a[1],0,a[1]]):q.write(0x030068f0+i*4,v)
 q.run();flags=q.reg[5];common=(flags&63)&((flags>>6)&63)&((flags>>12)&63)&((flags>>18)&63)
 assert list(map(int,row.split()))==[int(common==0),flags,common],(a,row,flags)
 assert flags&63==0;steps+=q.steps;nonzero+=flags!=0
report={'status':'PASS','actual_cells':actual,'boundary_fixtures':len(cases)-actual,'cases':len(cases),'original_arm_instructions':steps,'nonzero_packed_flags':nonzero,'scope':'Native exact2EC0..2F80 retained-coordinate/packedflags body vsoriginalARM for90actualcells plus82rotated/scalar/wrappedboundaryfixtures. Trailing6-bit shift makeslowestmaskzero beforeoriginal2F84..2FA8 intersection; thereforecellguardacceptsallvalidinputs. Originalcornerowner separatelyexecuted. Module not yet linkedlive; no traversal/cycles/cadenceproof.'};(root/'reports/world-cell-visibility-native-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
