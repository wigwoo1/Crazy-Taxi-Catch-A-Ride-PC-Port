import pathlib,struct,json,sys
root=pathlib.Path('outputs/crazy-taxi-native')
stem=sys.argv[1] if len(sys.argv)>1 else 'connected-cold'
rows=[list(map(int,l.split())) for l in (root/f'reports/{stem}-camera.txt').read_text().splitlines()]
assert len(rows)==500 and all(len(r)==32 for r in rows)
cold=(root/'reports/original-core-reference/boot2700.03000000.bin').read_bytes()
assert struct.unpack_from('<i',cold,0x53c0)[0]==rows[0][31]==0
first_nonzero=next(i for i,r in enumerate(rows) if r[31]!=0)
assert all(r[31]==0 for r in rows[:first_nonzero])
original=(root/'reports/original-core-reference/boot4200.ewram').read_bytes()
base=0x2634
actor=list(struct.unpack_from('<iii',original,base))
camera=list(struct.unpack_from('<iii',original,base+0x498))+list(struct.unpack_from('<hhh',original,base+0x4a4))
native_actor=rows[-1][14:17]
native_camera=rows[-1][25:31]
assert native_actor==actor,(native_actor,actor)
assert native_camera==camera,(native_camera,camera)
report={'status':'PASS_COLD_OWNER_AND_IDLE_CAMERA_ENDPOINT','calls':len(rows),'first_nonzero_global_delta_call':first_nonzero+1,'initial_global_delta':0,'source_actor':actor,'source_camera':camera,'native_actor':native_actor,'native_camera':native_camera,'scope':'Original independent boot2700 cold global owner and boot4200 no-input idle actor/camera endpoint match. Native retained zero before first gate copy;500 original camera calls individually compared separately. Does not prove continuous original trajectory/cadence, sprite pixels, per-frame framing, original cold RNG or physical4K60 display.'}
(root/f'reports/{stem}-owner-endpoint.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
