"""Export original9 challenge descriptor worlds using existing native decoders."""
import sys,json,hashlib,struct,contextlib,io
from pathlib import Path
import extract_scene,extract_world
rom=Path('Crazy Taxi - Catch a Ride (USA).gba')
data=rom.read_bytes();assert hashlib.sha256(data).hexdigest()==extract_scene.EXPECTED
out=Path('outputs/crazy-taxi-native/assets/challenge-worlds');out.mkdir(exist_ok=True)
rows=[]
for mode in range(9):
 address=struct.unpack_from('<I',data,0x4a4f7c+24*mode+16)[0]
 label=f'challenge{mode}';extract_scene.SCENES[label]=address
 sys.argv=['extract_scene',str(rom),'--output',str(out),'--scene',label]
 with contextlib.redirect_stdout(io.StringIO()):extract_scene.main()
 h=data[address-extract_scene.BASE:address-extract_scene.BASE+104]
 count=struct.unpack_from('<H',h,16)[0];paths=struct.unpack_from('<I',h,52)[0]-extract_scene.BASE
 matches=[]
 for index in range(count):
  entry=data[paths+index*20:paths+(index+1)*20]
  if entry[4:16].split(b'\0')[0]==b'GRID_01':
   marker=struct.unpack_from('<I',entry,16)[0]-extract_scene.BASE
   matches.append(data[marker:marker+16])
 assert len(matches)==1 and len(matches[0])==16
 marker_path=out/(label+'-spawn.bin');marker_path.write_bytes(matches[0]);assert marker_path.read_bytes()==matches[0]
 world=extract_world.world(data,label);path=out/(label+'.ctw');path.write_bytes(world);assert path.read_bytes()==world
 meta=json.loads((out/(label+'-metadata.json')).read_text())
 rows.append({'submode':mode,'world_address':hex(address),'vertices':meta['vertex_count'],'polygons':meta['polygon_count'],'ctw_bytes':len(world),'ctw_sha256':hashlib.sha256(world).hexdigest()})
 print('PASS',label,hex(address),meta['vertex_count'],meta['polygon_count'],len(world))
(out/'manifest.json').write_text(json.dumps({'rom_sha256':extract_scene.EXPECTED,'scope':'Original geometry/base textures/ground grid; challenge bootstrap/gameplay not integrated','rows':rows},indent=2))
