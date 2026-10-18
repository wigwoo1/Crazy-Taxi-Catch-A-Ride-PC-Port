"""Recorded taxi pose drives original/native frame-copy/affine/palette pipeline."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
pose=json.loads((root/'reports/corrected-camera-city1-source-inputs.json').read_text())
assert pose['alpha']==0 and pose['centers']==[[106,102],[133,102]] and pose['depth']==3537
boot={};exec((root/'tools/audit_graphics_bootstrap_cold_source.py').read_text().split("report={'status'")[0],boot)
roll=pose['vehicle'][4]&65535;angle=(-roll)&4095
sine=boot['p'].read(0x0202c4f0+angle*4);cosine=boot['p'].read(0x020304f0+angle*4)
sine=sine-0x100000000 if sine&0x80000000 else sine;cosine=cosine-0x100000000 if cosine&0x80000000 else cosine
case=(0xffffffff,pose['frame'],0,0,0,pose['scale'],roll,sine,cosine,0)
source=(root/'tools/compare_taxi_package_render_rom.py').read_text();start=source.index('cases=[');end=source.index("capture=root/",start);source=source[:start]+'cases=[recorded_case]\n'+source[end:]
source=source.replace('taxi-package-render-native.bin','taxi-recorded-pose-render-native.bin').replace('taxi-package-render-rom-comparison.json','taxi-recorded-pose-render-rom-comparison.json')
source=source.replace('Controlled projected half positions/scale/trig and loaded queue templates','Recorded corrected-camera-city1 projectedhalf positions/depth/scale/bodyroll and actualbootstraptrig; loaded queue templates')
ns={'recorded_case':case,'__file__':__file__};exec(source,ns)
report={'status':'PASS','pose_source':'corrected-camera-city1-source-inputs.json','camera':pose['camera'],'vehicle':pose['vehicle'],'driver_fixture':0,'frame':pose['frame'],'body_roll':roll,'rotation_table_index':angle,'actual_original_sine':sine,'actual_original_cosine':cosine,'scale':pose['scale'],'centers':pose['centers'],'depth':pose['depth'],'scope':'Recorded realnativepose graphics tail15910 framecache/copy/affinequeue/palette originalvsnativepackage allbytes exact, originalbootstraptrig used. Driver0fixture, upstreamprojection separatelyqualified; no scenerycompositor/OAMpixel/fullframe/cycles/cadence claim.'}
(root/'reports/taxi-recorded-pose-graphics-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
