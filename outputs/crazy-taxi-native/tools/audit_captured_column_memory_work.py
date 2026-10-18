from pathlib import Path
from collections import Counter
import struct,json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');s=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1434-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1304<=off<0x1434 or 0x29fc<=off<0x2ce8,hex(pc)').replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z').replace('(PACK,bytearray(512))','(PACK,bytearray(4096))')
s=s.replace("elif m in ('b','bl'):","elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):");s=s.replace('w=self.read(pc);cond=w>>28','w=self.fetch(pc);cond=w>>28');scope={};exec(s,scope);OriginalProbe=scope['MarkerRasterProbe'];BASE=scope['BASE'];DESC=scope['DESC'];FB=scope['FB']
totals=Counter();per_batch=[]
class Probe(OriginalProbe):
 def __init__(self,*args):self.track=False;self.accesses=Counter();super().__init__(*args)
 def region(self,a):
  if BASE<=a<BASE+0x5000:return 'iwram_code_literal'
  if FB<=a<FB+38400:return 'vram_framebuffer'
  if 0x12000000<=a<0x12001000:return 'ewram_descriptors'
  if 0x13000000<=a<0x13002000:return 'iwram_stack'
  if 0x08000000<=a<0x08800000:return 'rom_texture'
  raise AssertionError(hex(a))
 def fetch(self,a):self.accesses['fetch:iwram:4']+=1;return OriginalProbe.read(self,a,4)
 def read(self,a,n=4):
  if self.track:self.accesses['read:'+self.region(a)+':'+str(n)]+=1
  return OriginalProbe.read(self,a,n)
 def write(self,a,v,n=4):
  if self.track:self.accesses['write:'+self.region(a)+':'+str(n)]+=1
  return OriginalProbe.write(self,a,v,n)
 def run(self):self.track=True;return super().run()
trace=root/'reports/original-core-reference/columnOuterLeft.cpu-timing-trace.json';cases=[]
for row in json.loads(trace.read_text())['records']:
 if 'column_context' not in row:continue
 c=row['column_context'];first,end=c['outer_range'];mx,my,sx,sy,tex=struct.unpack('<5I',bytes.fromhex(c['stack_parameters']));d=list(struct.unpack('<'+'I'*((end-first)*8),bytes.fromhex(c['outer_descriptors'])));cases.append(([first,end-first]+c['bounds']+[tex-0x8000000,mx,my,sx,sy],d,row['tick']))
binary=root/'bin/pair-column-batch-probe.exe';rompath=Path('Crazy Taxi - Catch a Ride (USA).gba');cp=subprocess.run([str(binary),str(rompath)],input=''.join(' '.join(map(str,a+d))+'\n' for a,d,t in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38400;steps=0
for index,(a,d,tick) in enumerate(cases):
 first,count,top,bottom,offset,mx,my,sx,sy=a;p=Probe(0x8000000,(0,0,2,2),False);p.entry=BASE+0x1304-0x700;p.reg[10]=DESC;p.reg[13]-=256;sp=p.reg[13]
 for j,v in enumerate(d):p.write(DESC+j*4,v)
 p.write(BASE+0xc50-0x700,top);p.write(BASE+0xc58-0x700,bottom)
 for j,v in enumerate([mx,my,sx,sy,0x8000000+offset]):p.write(sp+j*4,v)
 p.write(sp+0x2c,FB+first*2);p.write(sp+0x74,count);original=p.run();assert original==cp.stdout[index*38400:(index+1)*38400],(index,tick);steps+=p.steps;totals.update(p.accesses);per_batch.append(dict(tick=tick,columns=count,arm_instructions=p.steps,accesses=dict(p.accesses)))
assert totals['fetch:iwram:4']==steps
report=dict(status='PASS_ORIGINAL_COLUMN_DYNAMIC_MEMORY_WORK',batches=len(cases),columns=sum(a[1] for a,d,t in cases),framebuffer_bytes_verified=len(cases)*38400,original_arm_instructions=steps,accesses=dict(totals),per_batch=per_batch,waitcnt_observed='4014',primary_timing_reference='https://raw.githubusercontent.com/mgba-emu/mgba/master/src/gba/memory.c',scope='Offline relocated original prepared batches with conditioned stack/descriptors and synthetic initial framebuffer, exact native pixel comparison retained. Instruction fetch and logical data transfer counts, NOT bus cycles: sequential classification, ARM internal cycles, video contention, IRQ/audio and true function-entry state still missing. No CPU interpreter or bus simulator in native runtime.')
(root/'reports/original-column-memory-work.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'original batches',steps,'instructions',dict(totals),'native pixels exact; no cycle estimate')
