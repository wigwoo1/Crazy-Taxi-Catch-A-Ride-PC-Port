from pathlib import Path
import random,struct,subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');s=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1960-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1934<=off<0x1960,hex(pc)')
scope={};exec(s,scope);BaseProbe=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB'];MASK=0xffffffff;TEX=0x14000000
class Probe(BaseProbe):
 def __init__(self,*args):self.track=False;super().__init__(*args);self.reads=self.writes=0
 def read(self,a,n=4):
  if self.track and TEX<=a<TEX+4096:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
rng=random.Random(1934);cases=[(seed,pairs,rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32)) for seed in (0,1,7,173) for pairs in (1,2,3,7,31,64,119,120)]
binary=root/'bin/pair-row-raster-probe.exe';out=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases).encode(),capture_output=True,check=True).stdout;assert len(out)==len(cases)*38416;steps=0
for index,(seed,pairs,x,y,dx,dy) in enumerate(cases):
 tex=bytearray(((i*37+seed)&255) if (i+seed)%7 else 0 for i in range(4096));p=Probe(0x08000000,(0,0,2,2),False);p.blocks.append((TEX,tex));p.entry=BASE+0x1934-0x700;p.reg[2:10]=[63,4032,14,x,y,8,dx,dy];p.reg[11]=pairs;p.reg[12]=FB;p.reg[14]=TEX;p.track=True;pixels=p.run();tail=struct.pack('<4I',p.reg[5],p.reg[6],p.reads,p.writes);assert pixels+tail==out[index*38416:(index+1)*38416];steps+=p.steps
report=dict(status='PASS',cases=len(cases),compared_framebuffer_bytes=len(cases)*38400,original_arm_instructions=steps,scope='Original1934..195C inner row loop exact native pixels, wrapped end UV and logical texture/read halfword write counts. Synthetic varied texture/UV inputs, not actual prepared row/fullframe/cycle proof. Offline interpreter only.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (Path(__file__),root/'src/pair_row_raster.c',binary)});(root/'reports/pair-row-raster-original-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'original row fixtures, pixels/UV/transfer counts exact',steps,'ARM steps')
