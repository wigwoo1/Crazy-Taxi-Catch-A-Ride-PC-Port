from pathlib import Path
import struct,json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');s=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1434-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1304<=off<0x1434 or 0x29fc<=off<0x2ce8,hex(pc)').replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z').replace('(PACK,bytearray(512))','(PACK,bytearray(4096))')
s=s.replace("elif m in ('b','bl'):","elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):");scope={};exec(s,scope);Probe=scope['MarkerRasterProbe'];BASE=scope['BASE'];DESC=scope['DESC'];FB=scope['FB']
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
 p.write(sp+0x2c,FB+first*2);p.write(sp+0x74,count);original=p.run();assert original==cp.stdout[index*38400:(index+1)*38400],(index,tick);steps+=p.steps
report=dict(status='PASS_NATIVE_COMPLETE_COLUMN_BATCH_ORIGINAL_ARM',batches=len(cases),columns=sum(a[1] for a,d,t in cases),compared_bytes=len(cases)*38400,original_arm_instructions=steps,scope='Complete captured descriptor batches/livebounds/texture conditioned restart original outer1304..1430 plus29FC, synthetic173 initial framebuffer. Native combined setup/raster/batch exact pixels; capturedmidloop not trueinvocation-entry/fullframe/GPU/cyclefidelity proof. No CPU interpreter in native runtime.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),trace,binary,root/'src/pair_column_draw.c',root/'src/pair_column_setup.c',root/'src/pair_column_raster.c',rompath]});(root/'reports/pair-column-batch-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'complete batches',report['columns'],'columns',report['compared_bytes'],'pixelbytes',steps,'original ARM steps')
