from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');trace=json.loads((root/'reports/original-core-reference/columnOuterLeft.cpu-timing-trace.json').read_text())['records'];observed=json.loads((root/'reports/original-column-memory-work.json').read_text())['per_batch'];cases=[]
for row in trace:
 if 'column_context' not in row:continue
 c=row['column_context'];first,last=c['outer_range'];mx,my,sx,sy,texture=struct.unpack('<5I',bytes.fromhex(c['stack_parameters']));d=struct.unpack('<'+'I'*((last-first)*8),bytes.fromhex(c['outer_descriptors']));cases.append(([first,last-first]+c['bounds']+[texture-0x08000000,mx,my,sx,sy]+list(d),row['tick']))
text=''.join(' '.join(map(str,a))+'\n' for a,t in cases).encode();rom='Crazy Taxi - Catch a Ride (USA).gba'
old=subprocess.run([str(root/'bin/pair-column-batch-probe.exe'),rom],input=text,capture_output=True,check=True).stdout
new=subprocess.run([str(root/'bin/pair-column-work-probe.exe'),rom],input=text,capture_output=True,check=True).stdout
assert len(cases)==len(observed)==115 and len(old)==115*38400 and len(new)==115*38408
reads=writes=0
for index,((a,tick),source) in enumerate(zip(cases,observed)):
 assert source['tick']==tick
 r,w=struct.unpack_from('<2I',new,index*38408);assert r==source['accesses'].get('read:rom_texture:1',0) and w==source['accesses'].get('write:vram_framebuffer:2',0)
 assert new[index*38408+8:(index+1)*38408]==old[index*38400:(index+1)*38400];reads+=r;writes+=w
report=dict(status='PASS',batches=115,texture_byte_reads=reads,framebuffer_halfword_writes=writes,framebuffer_bytes_compared=len(old),scope='Native logical transfer counters exact original dynamic access observations for captured prepared batches. Pixel outputs exact prior ROM-verified native batch binary. No CPU cycle estimate, true invocation-entry/fullframe or general scheduler proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (root/'src/pair_column_raster.c',root/'src/pair_column_draw.c',root/'bin/pair-column-work-probe.exe',root/'bin/pair-column-batch-probe.exe')});(root/'reports/native-column-work-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',115,'native batches:',reads,'texture loads,',writes,'halfword stores; all4416000 framebuffer bytes retained')
