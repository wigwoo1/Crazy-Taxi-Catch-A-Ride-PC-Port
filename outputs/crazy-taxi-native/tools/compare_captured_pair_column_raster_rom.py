from pathlib import Path
import struct,json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');rompath=Path('Crazy Taxi - Catch a Ride (USA).gba');rom=rompath.read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
source=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=BASE+0x1414-0x700:');source=source.replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x13e8<=off<0x1414,hex(pc)');scope={};exec(source,scope);Probe=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB']
trace=root/'reports/original-core-reference/cpuWindowsLeft.cpu-timing-trace.json';captured=json.loads(trace.read_text())['records'];cases=[];ticks=[]
for row in captured:
 if not 0x3000cf0<=row['gprs'][15]<0x3000d10:continue
 g=row['gprs'];assert not row['cpsr']&32
 if 0x6000000<=g[12]<0x6009600:dst=g[12]-0x6000000
 elif 0x600a000<=g[12]<0x6013600:dst=g[12]-0x600a000
 else:raise AssertionError(hex(g[12]))
 assert g[14]>=0x8000000 and g[14]+4096<=0x8800000 and 1<=g[11]<=160 and dst+240*(g[11]-1)+1<38400 and not dst&1
 assert (g[2],g[3],g[4],g[7])==(63,4032,14,8)
 cases.append((g[14]-0x8000000,dst,g[11],g[2],g[3],g[4],g[7],g[5],g[6],g[8],g[9]));ticks.append(row['tick'])
assert len(cases)==82;binary=root/'bin/pair-column-raster-probe.exe';cp=subprocess.run([str(binary),str(rompath)],input=(''.join(' '.join(map(str,c))+'\n' for c in cases)).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408;steps=0
for i,c in enumerate(cases):
 offset,dst,count,mx,my,sx,sy,x,y,dx,dy=c;p=Probe(0x8000000,(0,0,2,2),False);p.entry=BASE+0x13e8-0x700
 for reg,value in {2:mx,3:my,4:sx,5:x,6:y,7:sy,8:dx,9:dy,11:count,12:FB+dst,14:0x8000000+offset}.items():p.reg[reg]=value
 original=p.run()+struct.pack('<II',p.reg[5],p.reg[6]);native=cp.stdout[i*38408:(i+1)*38408];assert original==native,(i,ticks[i]);steps+=p.steps
report=dict(status='PASS_NATIVE_CAPTURE_CONDITIONED_COLUMN_RASTER',cases=len(cases),original_capture_ticks=ticks,compared_framebuffer_bytes=len(cases)*38400,original_arm_instructions=steps,scope='Actual savedROM texture pointers/masks/coordinates/steps/remaining-row registers condition native and original13E8 inner-loop entry fixtures. Zero clipping adjustments;82source samples allbound-valid. Initial output is synthetic173-filled buffer, not capturedframebuffer; savedPC may be mid-loop with pipeline semantics, so fixtures explicitly restart at13E8 and are NOTactualroutine-entry or fullframe replay. Byteoutput/finalXY exact using actualreadonlyROMtexturedata. No timing-cost/4KGPU/wholegame claim.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),trace,binary,root/'src/pair_column_raster.c',root/'tests/pair_column_raster_probe.c']})
(root/'reports/pair-column-raster-captured-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS82 captured-register-conditioned columns',report['compared_framebuffer_bytes'],'pixelbytes',steps,'originalARM instructions')
