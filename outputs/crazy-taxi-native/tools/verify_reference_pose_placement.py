from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');env={}
exec((root/'tools/compare_taxi_renderer_original_angle_rom.py').read_text().split('\ncases=')[0],env)
rows=[]
for label in ['fullsharedleft','fullsharedright']:
 ram=(root/f'reports/original-core-reference/{label}.ewram').read_bytes();p=env['TaxiProbe']((0,0,0,0,0,1,0,0,0,0));p.requests=[];base=0x2002634
 for n,v in enumerate(ram[0x2634:0x2634+0xea0]):p.write(base+n,v,1)
 p.arm.mem.update({0x2001708:0,0x30053e0:0,0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x3006810:0x10002000})
 for off,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+off]=v
 p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
 original=dict(frame=p.read(base+0xe64),centers=[q[:2] for q in p.requests],depth=p.requests[0][2],scale=p.requests[0][3]);native=json.loads((root/f'reports/reference-{label}-render.json').read_text());assert original=={k:native[k] for k in original},(label,original,native)
 assert native['viewport']==[3840,2160];assert not (root/f'reports/reference-{label}-4k.err').read_text();rows.append(dict(label=label,original=original,viewport=native['viewport'],source_ram_sha256=hashlib.sha256(ram).hexdigest()))
report=dict(status='PASS_ORIGINAL_MOVING_POSE_NATIVE_4K_TAXI_PLACEMENT',cases=rows,binary_sha256=hashlib.sha256((root/'bin/scene-viewer-reference-pose.exe').read_bytes()).hexdigest(),scope='Offline original1556C invoked from complete original VBlank actor snapshot; native4K production taxi draw selects identical frame, quantized two-half centres, depth and scale. Original interpreter uses explicit allocator/division/ground-shadow boundary contracts. Native reference-pose mode imports presentation only, no gameplay steps. Full GPU pixel/original traffic/HUD/raster fidelity and sustained4K60 not proven.')
(root/'reports/reference-pose-placement-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',rows)
