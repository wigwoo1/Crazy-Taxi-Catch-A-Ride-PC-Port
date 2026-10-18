from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');source=(root/'tools/original_marker_raster_probe.py').read_text()
source=source.replace('while pc!=0xffffffff:','while pc!=BASE+0x1414-0x700:').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x1304<=off<0x1414 or 0x29fc<=off<0x2ce8,hex(pc)').replace('14:True,0:self.z','14:True,10:self.n==self.v,0:self.z')
source=source.replace("elif m in ('b','bl'):","elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):")
scope={};exec(source,scope);Probe=scope['MarkerRasterProbe'];BASE=scope['BASE'];DESC=scope['DESC'];FB=scope['FB'];trace=root/'reports/original-core-reference/columnContextsLeft.cpu-timing-trace.json';cases=[]
for record in json.loads(trace.read_text())['records']:
 if 'column_context' not in record:continue
 ctx=record['column_context'];d=list(struct.unpack('<8I',bytes.fromhex(ctx['descriptor'])));mx,my,sx,sy,tex=struct.unpack('<5I',bytes.fromhex(ctx['stack_parameters']));col=ctx['column_destination']-ctx['framebuffer'];assert 0<=col<=238 and not col&1 and 0x8000000<=tex<=0x8800000-4096
 cases.append(d+ctx['bounds']+[col,tex-0x8000000,mx,my,sx,sy])
# Explicit source do-while zero-row boundary and rejection using the same real texture parameters.
edge=cases[0].copy();edge[0]=0xfffffff6;edge[4]=0xfffffffe;cases.append(edge)
rejected=edge.copy();rejected[4]=0xfffffffd;cases.append(rejected)
binary=root/'bin/pair-column-draw-probe.exe';rompath=Path('Crazy Taxi - Catch a Ride (USA).gba');cp=subprocess.run([str(binary),str(rompath)],input=''.join(' '.join(map(str,c))+'\n' for c in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38400;steps=0
for i,c in enumerate(cases):
 p=Probe(0x8000000,(0,0,2,2),False);p.entry=BASE+0x1304-0x700;p.reg[10]=DESC;p.reg[13]-=256
 for j,v in enumerate(c[:8]):p.write(DESC+j*4,v)
 p.write(BASE+0xc50-0x700,c[8]);p.write(BASE+0xc58-0x700,c[9]);sp=p.reg[13]
 for j,v in enumerate([c[12],c[13],c[14],c[15],0x8000000+c[11]]):p.write(sp+j*4,v)
 p.write(sp+0x2c,FB+c[10]);original=p.run();assert original==cp.stdout[i*38400:(i+1)*38400],i;steps+=p.steps
report=dict(status='PASS_NATIVE_JOINED_COLUMN_ORIGINAL_ARM',captured_cases=len(cases)-2,edge_cases=2,compared_bytes=len(cases)*38400,original_arm_instructions=steps,scope='Saved mid-loop real descriptors/stackparameters/live bounds/page conditioned restart at1304 with synthetic173 initial framebuffer. Native setup plus raster compared to original1304..1410 and29FC division, including explicitzero-row do-while and rejection edge. Not actualroutineentry/fullframe replay, GPUintegration, cyclefidelity or wholegame proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),trace,binary,root/'src/pair_column_draw.c',root/'src/pair_column_setup.c',root/'src/pair_column_raster.c',rompath]})
(root/'reports/pair-column-draw-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'joined columns',report['compared_bytes'],'pixelbytes',steps,'original ARM steps')
