from pathlib import Path
import json,struct,hashlib
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
results=[]
for label in ('second','third'):
 iw=(root/f'reports/bound-{label}-frame-pre-consumer-owner-03000000.bin').read_bytes();ew=(root/f'reports/bound-{label}-frame-pre-consumer-owner-02000000.bin').read_bytes()
 def word(a):
  b,off=(iw,a-0x03000000) if 0x03000000<=a<0x03010000 else (ew,a-0x02000000) if 0x02000000<=a<0x02040000 else (rom,a-0x08000000)
  return struct.unpack_from('<I',b,off)[0]
 block=0x03006810;scene=word(block);view=word(block+4);assert scene==0x030069c0
 descriptor=iw[scene-0x03000000:scene-0x03000000+104];origin=0x087ebd98
 assert descriptor[:100]==rom[origin-0x08000000:origin-0x08000000+100]
 asset=(root/'assets/challenge-worlds/challenge5.ctw').read_bytes();assert asset[:4]==b'CTW1'
 nv,np,nm=struct.unpack_from('<3I',asset,4);meta=json.loads((root/'assets/challenge-worlds/challenge5-metadata.json').read_text())
 assert (nv,np,nm)==(meta['vertex_count'],meta['polygon_count'],meta['material_count'])
 offset=20
 for field,count,stride in [('vertex_address',nv,16),('face_address',np,24)]:
  a=int(meta[field],16)-0x08000000;size=count*stride
  assert asset[offset:offset+size]==rom[a:a+size];offset+=size
 material=word(scene+28)-0x08000000;assert asset[offset:offset+nm*28]==rom[material:material+nm*28]
 viewbytes=ew[view-0x02000000:view-0x02000000+72];corners=iw[0x6810+0x50:0x6810+0xd0]
 trig=ew[0x2c4f0:0x304f0]+ew[0x304f0:0x344f0]
 fixture=viewbytes+corners+trig
 path=root/f'reports/bound-{label}-frame-native-producer-owners.fixture';path.write_bytes(fixture)
 results.append({'frame':label,'world':'challenge5','descriptor_origin':hex(origin),'descriptor_matching_prefix_bytes':100,'unqualified_descriptor_tail_hex':descriptor[100:].hex(),'view_pointer':hex(view),'view_words':list(struct.unpack('<18I',viewbytes)),'native_asset_sha256':hashlib.sha256(asset).hexdigest(),'fixture_bytes':len(fixture),'fixture_sha256':hashlib.sha256(fixture).hexdigest()})
report={'status':'RETAINED_NATIVE_PRODUCER_STATIC_OWNERS_PASS','frames':results,'scope':'Native challenge5 CTW vertices/polygons/materials byte-identical suppliedROM source owners, descriptor first100bytes identity, actualretained view/corners/trig fixture. Descriptor lastword mutation remains unqualified. No native producer execution, camera angle interpretation, cache/cell lifecycle or livegame equivalence claim.'}
(root/'reports/bound-retained-native-producer-owner-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
