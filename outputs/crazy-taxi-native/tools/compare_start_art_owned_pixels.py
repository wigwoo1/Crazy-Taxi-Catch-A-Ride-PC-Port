"""Native published startup owners compared with independent original ROM assets."""
from pathlib import Path
import itertools,subprocess,json,struct,hashlib
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
cases=list(itertools.product(range(3),range(5)))
cp=subprocess.run([str(root/'bin/start-art-owned-compare.exe')],input=''.join(f'{p} {l}\n' for p,l in cases),text=True,capture_output=True,check=True)
rows=cp.stdout.splitlines();assert len(rows)==15
bank=(root/'assets/start-phase/frames.rgba').read_bytes();base=struct.unpack_from('<I',(root/'reports/ordinary-native-assets.bin').read_bytes(),7*8)[0]
for (phase,lang),row in zip(cases,rows):
 pixels,objects,palette,pointer=map(bytes.fromhex,row.split());index=phase*5+lang
 art=struct.unpack_from('<I',rom,0x7ec210+index*4)[0];pal=struct.unpack_from('<I',rom,0x7ec24c+index*4)[0]
 expected=bytearray([165])*32768;offset=base-0x6014000;assert offset>=0
 expected[offset:offset+1536]=rom[art-0x8000000:art-0x8000000+1536]
 assert objects==expected,(phase,lang,'objects')
 assert palette==rom[pal-0x8000000:pal-0x8000000+32]
 assert pointer==struct.pack('<I',pal)
 assert pixels==bank[index*12288:(index+1)*12288],(phase,lang,'pixels')
report=dict(status='PASS_NATIVE_START_ART_OWNED_SOURCE_PIXELS',cases=15,frames=45,rgba_bytes=184320,object_bytes_per_case=32768,scope='Native three deferred copies retain objectbytes until flush, publish exact1536originaltilebytes andretainallotherobjectbytes, exact32palettebytes/registrypointer, decoded45frames equal independentlyextractedROM RGBA. Original27FB8/28EE0 notexecutedhere; source requestsequence separatelyqualified. FinalOAMshader/presentation/fullgameexcluded.')
(root/'reports/start-art-owned-source-pixels.json').write_text(json.dumps(report,indent=2));print('PASS',report)
