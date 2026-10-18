from pathlib import Path
import json,subprocess,random,hashlib
root=Path('outputs/crazy-taxi-native');source=(root/'tools/original_marker_raster_probe.py').read_text()
source=source.replace('while pc!=0xffffffff:','while pc not in (BASE+0x13c8-0x700,BASE+0x1414-0x700):')
source=source.replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1304<=off<0x13c8 or 0x29fc<=off<0x2ce8,hex(pc)')
source=source.replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z')
source=source.replace("elif m in ('b','bl'):","elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):")
scope={};exec(source,scope);Probe=scope['MarkerRasterProbe'];BASE=scope['BASE'];DESC=scope['DESC'];rng=random.Random(1304);cases=[]
for i in range(96):
 start=[-50,0,20,159,160,180][i%6];end=[-10,0,21,80,160,220][(i//6)%6]
 d=[start&0xffffffff,rng.getrandbits(32),rng.getrandbits(32),[0,64,4096,0xffffffc0][i%4],(end-2)&0xffffffff,rng.getrandbits(32),rng.getrandbits(32),[64,0,8192,0xfffff000][i%4]]
 cases.append(d+[0,160])
binary=root/'bin/pair-column-setup-probe.exe';cp=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True);lines=cp.stdout.splitlines();assert len(lines)==len(cases);steps=0;accepted=0
for index,c in enumerate(cases):
 p=Probe(0x8000000,(0,0,2,2),False);p.entry=BASE+0x1304-0x700;p.reg[10]=DESC
 for j,v in enumerate(c[:8]):p.write(DESC+j*4,v)
 p.write(BASE+0xc50-0x700,c[8]);p.write(BASE+0xc58-0x700,c[9]);p.run();steps+=p.steps
 actual=list(map(int,lines[index].split()));start=c[0] if c[0]<0x80000000 else c[0]-0x100000000;end=((c[4]+2)&0xffffffff);end=end if end<0x80000000 else end-0x100000000
 if end-start<=0 or start>=160 or end<0:expected=[0]*7
 else:expected=[1,p.reg[4],p.reg[11],p.reg[5],p.reg[6],p.reg[8],p.reg[9]];accepted+=1
 assert actual==expected,(index,c,actual,expected)
report=dict(status='PASS_NATIVE_COLUMN_SETUP_ORIGINAL_ARM',cases=len(cases),accepted=accepted,rejected=len(cases)-accepted,original_arm_instructions=steps,scope='Synthetic descriptor entry conditions compare native perspective division/interpolation and clipping to actual original1304..13C4 plus29FC division, offline only. Includes negative/top/bottom/empty/offscreen spans, wrapped products, zero and signed depth denominators. No captured fullcaller entry, pixel integration, cycle fidelity or wholegame claim.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),binary,root/'src/pair_column_setup.c',root/'src/projection_divide.c']})
(root/'reports/pair-column-setup-original-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'original column setups',accepted,'accepted',steps,'original ARM steps')
