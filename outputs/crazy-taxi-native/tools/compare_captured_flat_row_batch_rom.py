from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');s=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1f88-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1f14<=off<0x1f88,hex(pc)').replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z').replace('(PACK,bytearray(512))','(PACK,bytearray(8192))')
old="rd,addr=i.op_str.split(', ',1);terms=addr[1:-1].split(', ');a=(self.val(terms[0])+ (self.val(terms[1]) if len(terms)>1 else 0))&MASK"
new="rd,addr=i.op_str.split(', ',1);post=addr.split('], ');terms=post[0].strip('[]').split(', ');a=(self.val(terms[0])+ (self.val(terms[1]) if len(terms)>1 else 0))&MASK"
assert old in s;s=s.replace(old,new).replace('else:self.write(a,self.val(rd),n)','else:self.write(a,self.val(rd),n)\n    if len(post)>1:self.reg[self.ri(terms[0])]=(a+self.val(post[1]))&MASK')
s=s.replace("elif m in ('b','bl'):","elif m in ('ror','asrs'):\n    rd=self.ri(p[0]);a=self.val(p[1]);shift=self.val(p[2]);x=((a>>shift)|(a<<(32-shift)))&MASK if m=='ror' else (signed(a)>>shift)&MASK\n    self.reg[rd]=x\n    if m=='asrs':self.z=x==0;self.n=bool(x>>31)\n   elif m in ('b','bl'):")
scope={};exec(s,scope);BaseProbe=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB'];DESC=scope['DESC']
class Probe(BaseProbe):
 def __init__(self,*args):self.track=False;super().__init__(*args);self.half=self.word=0
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:
   assert n in (2,4)
   if n==2:self.half+=1
   else:self.word+=1
  return super().write(a,v,n)
trace=root/'reports/original-core-reference/flatRowsLeftBounds.cpu-timing-trace.json';cases=[]
for row in json.loads(trace.read_text())['records']:
 if 'flat_row_context' not in row:continue
 c=row['flat_row_context'];first,last=c['outer_range'];d=struct.unpack('<'+'I'*((last-first)*8),bytes.fromhex(c['row_descriptors']));cases.append(([first,last-first]+c['clipped_bounds']+[c['pattern']&65535],list(d),row['tick']))
captured=len(cases)
for first in (0,1):
 for left,right in [(-8,0),(0,1),(2,4),(2,8),(4,8),(1,6),(239,248),(240,248),(-8,12)]:
  for pattern in (0,0x1234):cases.append(([first,1,0,240,pattern],[left&0xffffffff,0,0,0,right&0xffffffff,0,0,0],-1))
binary=root/'bin/flat-row-batch-probe.exe';cp=subprocess.run([str(binary)],input=''.join(' '.join(map(str,a+d))+'\n' for a,d,t in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408;steps=half=word=0
for index,(a,d,tick) in enumerate(cases):
 first,count,left,right,pattern=a;p=Probe(0x08000000,(0,0,2,2),False);p.entry=BASE+0x1f14-0x700
 for j,v in enumerate(d):p.write(DESC+j*4,v)
 repeated=pattern|(pattern<<16)
 if first&1:repeated=((repeated>>8)|(repeated<<24))&0xffffffff
 p.reg[10]=DESC;p.reg[11]=FB+first*240;p.reg[9]=count;p.reg[8]=repeated;p.reg[2]=left;p.reg[3]=right;p.track=True
 pixels=p.run();expected=pixels+struct.pack('<2I',p.half,p.word);assert expected==cp.stdout[index*38408:(index+1)*38408],(index,tick,p.half,p.word);steps+=p.steps;half+=p.half;word+=p.word
report=dict(status='PASS',captured_batches=captured,boundary_fixtures=len(cases)-captured,batches=len(cases),prepared_rows=sum(a[1] for a,d,t in cases),framebuffer_bytes=len(cases)*38400,framebuffer_halfword_writes=half,framebuffer_word_writes=word,original_arm_instructions=steps,scope='Original1F14..1F88 actual captured prepared batches plus boundary/alignment/parity/zero pattern fixtures. Synthetic173initialfb/conditionedmidloop restart; native framebuffer and original16/32store counts exact, not entry/fullframe/cycle/cadence proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (Path(__file__),trace,root/'src/flat_row_raster.c',binary)})
(root/'reports/flat-row-batch-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
