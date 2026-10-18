from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');s=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1038-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x0efc<=off<0x1038 or 0x29f8<=off<0x2ce8,hex(pc)').replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z').replace('(PACK,bytearray(512))','(PACK,bytearray(8192))')
s=s.replace("elif m in ('b','bl'):", "elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):")
s=s.replace("elif m in ('b','bl'):","elif m=='ldm':\n    address=self.val(i.op_str.split(', ')[0]);regs=[self.ri(v) for v in i.op_str.split('{')[1].split('}')[0].split(', ')];values=[self.read(address+j*4) for j in range(len(regs))]\n    for register,value in zip(regs,values):self.reg[register]=value\n   elif m in ('b','bl'):")
scope={};exec(s,scope);BaseProbe=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB'];DESC=scope['DESC']
class Probe(BaseProbe):
 def __init__(self,*args):self.track=False;super().__init__(*args);self.reads=self.writes=0
 def read(self,a,n=4):
  if self.track and 0x08000000<=a<0x08800000 and n==1:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
trace=root/'reports/original-core-reference/perspectiveRowsLeft.cpu-timing-trace.json';cases=[]
for row in json.loads(trace.read_text())['records']:
 if 'perspective_row_context' not in row:continue
 c=row['perspective_row_context'];first,last=c['outer_range'];mx,my,sx,sy,texture=struct.unpack('<5I',bytes.fromhex(c['stack_parameters']));d=struct.unpack('<'+'I'*((last-first)*8),bytes.fromhex(c['row_descriptors']));cases.append(([first,last-first,texture-0x08000000,mx,my,sx,sy]+c['clipped_bounds'],list(d),row['tick']))
captured_count=len(cases)
for left,right in [(-8,0),(-1,1),(239,248),(240,248),(-16,-1),(-20,10),(1,2)]:
 base=cases[0][0];descriptor=[left&0xffffffff,0xfffe0000,0x12345678,0x01000000,right&0xffffffff,0x80008000,0xabcdef00,0x02000000];cases.append(([0,1]+base[2:],[*descriptor],-1))
binary=root/'bin/pair-row-batch-perspective-probe.exe';rom='Crazy Taxi - Catch a Ride (USA).gba';cp=subprocess.run([str(binary),rom],input=''.join(' '.join(map(str,a+d))+'\n' for a,d,t in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408;steps=reads=writes=0
for index,(a,d,tick) in enumerate(cases):
 first,count,offset,mx,my,sx,sy,left,right=a;p=Probe(0x08000000,(0,0,2,2),False);p.entry=BASE+0x0efc-0x700;p.reg[10]=DESC;p.reg[13]-=256;sp=p.reg[13]
 for j,v in enumerate(d):p.write(DESC+j*4,v)
 for j,v in enumerate((mx,my,sx,sy,0x08000000+offset)):p.write(sp+j*4,v)
 p.write(BASE+0x0c4c-0x700,left);p.write(BASE+0x0c54-0x700,right);p.write(sp+0x2c,FB+first*240);p.write(sp+0x38,count);p.track=True;pixels=p.run();expected=pixels+struct.pack('<2I',p.reads,p.writes);assert expected==cp.stdout[index*38408:(index+1)*38408],(index,tick);steps+=p.steps;reads+=p.reads;writes+=p.writes
report=dict(status='PASS',batches=len(cases),captured_batches=captured_count,boundary_fixtures=len(cases)-captured_count,prepared_rows=sum(a[1] for a,d,t in cases),framebuffer_bytes=len(cases)*38400,texture_byte_reads=reads,framebuffer_halfword_writes=writes,original_arm_instructions=steps,scope='Actual original prepared row ranges/realROMtextures/masks, conditioned restartEFC..1038 plus29F8 division, synthetic173initialframebuffer. Native perspective prepared batch and boundary pixel/transfer equality, not true invocation-entry/fullframe/cycle/generalcadence proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (Path(__file__),trace,root/'src/pair_row_raster.c',binary)});(root/'reports/pair-row-batch-perspective-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'original prepared batches',report['prepared_rows'],'rows',report['framebuffer_bytes'],'bytes exact',reads,'texture loads',writes,'halfword stores',steps,'ARM steps')
