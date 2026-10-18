"""Bounded original14DD8 camera probe with explicit initialized globals."""
exec(open('outputs/crazy-taxi-native/tools/compare_taxi_renderer_original_angle_rom.py').read().split('\ncases=')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/camera-view-cfg.json').read_text())['instructions']})
class CameraProbe(TaxiProbe):
 def write(self,a,v,n=4):
  if a in [0x2002618,0x2002624,0x2002628] and n==4:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x803244c,0x8032450,0x803245c]:
   entry=self.reg[0 if target==0x803244c else 4 if target==0x803245c else 1];assert entry in [0x8000718,0x80009bc,0x80009d4,0x80009ec,0x8000a40]
   self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  else:super().external(target)
p=CameraProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
for off,v,n in [(0x64c,2,2),(0x654,3073*4096,4),(0x20,3073*4096,4),(0x1c,3073*4096,4),(0xe84,4096,4)]:p.write(base+off,v,n)
for a,v in {0x2005f20:0,0x2001708:0,0x20034d4:1,0x20034d8:1,0x2001674:0,0x2001678:4096,0x200167c:4096,0x2001694:0,0x2001696:0,0x2003b00:1,0x2002618:0,0x200261c:0,0x2002624:0,0x2002628:0,0x3006ac4:0x8000a40,0x3006a90:0x8000718,0x3006ab8:0x80009bc,0x3006a94:0x80009d4,0x3006ab4:0x80009ec}.items():p.arm.mem[a]=v
p.reg[:2]=[base+0x498,base];p.reg[14]=0x10020001;p.run(entry=0x8014dd8,stop=0x10020000)
print('COMPLETE',p.steps,p.arm.steps,[signed(p.read(base+o)) for o in [0x498,0x49c,0x4a0,0x650,0x654,0x76c,0x770]])

report=dict(complete=True,thumb_instructions=p.steps,arm_instructions=p.arm.steps,camera=[signed(p.read(base+o)) for o in [0x498,0x49c,0x4a0]],angles=[p.read(base+o,2) for o in [0x4a4,0x4a6,0x4a8]],offsets=[signed(p.read(base+o)) for o in [0x76c,0x770]],scope='Complete original14DD8 mode2 identity-follow controlled fixture, original ARMmatrix/copy/vectortransform/angle. Globals explicitly seeded; division contract. No native comparison or livehistory/source bootstrap proof.',rom_sha256=hashlib.sha256(r).hexdigest())
assert report['camera']==[-1024,468,0] and report['angles']==[0,3073,0] and report['offsets']==[468*4096,1024*4096],report
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'tools/probe_full_camera_view_rom.py',root/'reports/camera-view-cfg.json']}
(root/'reports/full-camera-view-first-probe.json').write_text(json.dumps(report,indent=2))
