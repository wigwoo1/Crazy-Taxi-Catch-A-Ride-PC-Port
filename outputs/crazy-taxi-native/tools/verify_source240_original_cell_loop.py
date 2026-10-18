from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_source240_cell_visibility.py').read_text().split('\nresults=[]')[0])
src=src.replace('pc not in (0x08002fb0,0x08002fb4)','pc!=0x08002fd8').replace('assert 0x2ec0<=off<0x2fb4,hex(pc)','assert 0x2eb0<=off<0x2fd8,hex(pc)')
src=src.replace("   elif m in ('b','bl'):","""   elif m=='bl' and self.val(p[0])==0x080031ec:
    ptr=self.reg[0];count=self.read(ptr+24);refs=[self.read(ptr+28+i*2,2) for i in range(count)]
    self.cells.append({'x':self.read(self.reg[13]+8),'z':self.read(self.reg[13]+20),'pointer':ptr,'references':refs})
    self.reg[14]=pc+4
   elif m in ('b','bl'):""")
exec(src);arm.detail=True;BASE=0x08000700
class LoopProbe(CellProbe):
 run=MarkerRasterProbe.run
 def __init__(self):
  super().__init__({'x':j['bounds'][0],'z':j['bounds'][1]});self.cells=[];self.entry=0x08002eb0
  minx,minz,maxx,maxz=j['bounds'];scene=0x087556cc;table=struct.unpack_from('<I',rom,scene-0x08000000+96)[0]
  assert struct.unpack_from('<HH',rom,scene-0x08000000+60)==(nx,nz)
  self.reg[12]=scene;self.reg[4]=minx;self.reg[5]=maxx;self.reg[6]=minz;self.reg[7]=maxz;self.reg[8]=(nx-(maxx-minx+1))*4;self.reg[9]=table+4*(minz*nx+minx)
q=LoopProbe();q.run();expected=j['cells'];assert len(q.cells)==len(expected)
for a,b in zip(q.cells,expected):assert all(a[k]==b[k] for k in ['x','z','references']),(a,b)
refs=[r for c in q.cells for r in c['references']];assert len(refs)==425
report={'status':'PASS','candidate_cells':(j['bounds'][2]-j['bounds'][0]+1)*(j['bounds'][3]-j['bounds'][1]+1),'visited_nonempty_cells':len(q.cells),'raw_reference_visits':len(refs),'original_arm_instructions':q.steps,'cells':q.cells,'scope':'Actualoriginal2EB0..2FD4 cellpointer/rowadvance/nullfilter/visibility instructions onrealROMdescriptor/table/cellrecords actualbounds/currentcamera. Each31EC boundary intercepted to recordcellptr/refs, callee notexecutedhere; producer on resulting425refs separatelyqualified. Bounds2D18 and preloop setup separatelyqualified. Not continuousfullentry/cache/cycles/cadenceproof.'};(root/'reports/source240-original-cell-loop-verification.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k!='cells'})
