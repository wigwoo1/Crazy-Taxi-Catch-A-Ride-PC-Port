"""Original player collision response blocks plus full13610 on captured states."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/challenge-bound-rpm-cfg.json').read_text())['instructions']})
for lo,hi in [(0x6a98,0x6afa),(0x6d7c,0x6dc2)]:allowed_thumb.update({i.address:bytes(i.bytes) for i in md.disasm(r[lo:hi],0x8000000+lo)})
class ResponseProbe(TaxiProbe):
 def external(self,target):
  if target==0x8013610:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x802c880:raise AssertionError('unexpected invalid drive mode')
  else:super().external(target)
f=root/'reports/current-traffic-response-states.txt';rows=[list(map(int,l.split())) for l in f.read_text().splitlines()];assert len(rows)==22
mapping=[(0x4ac,1),(0x544,4),(0x4dc,1)]+[(0x4b8+i*4,4) for i in range(7)]+[(0x4d8,4),(0x5b8,4),(0x5cc,4),(0x75c,4),(0x538,2),(0xe84,4),(0xe6c,4),(0xe70,4),(0xe78,4),(0xe7c,4),(0xe80,4)]
steps=0
for index,a in enumerate(rows):
 mode,delta=a[:2];p=ResponseProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634
 for off in range(0,0xea0,4):p.arm.mem[base+off]=0
 for (off,n),v in zip(mapping,a[2:23]):p.write(base+off,v,n)
 p.arm.mem[0x30053c0]=delta
 if mode==0:p.reg[7]=base;p.reg[8]=base;p.reg[9]=base+0x544;p.reg[3]=0;entry=0x8006a98;stop=0x8006afa
 else:p.reg[5]=base;p.reg[8]=base+0x544;entry=0x8006d7c;stop=0x8006dc2
 p.run(entry=entry,stop=stop)
 got=[]
 for off,n in mapping:
  v=p.read(base+off,n)
  if n==4:v=signed(v)
  elif off==0x4dc and v&128:v-=256
  elif off==0x538 and v&32768:v-=65536
  got.append(v)
 assert got==a[23:],dict(index=index,mode=mode,original=got,native=a[23:]);steps+=p.steps
report=dict(contacts=22,follow=17,postimpact=5,pass_match=True,thumb_instructions=steps,scope='Actual original normal6A98..6AF6/postimpact6D7C..6DBE playerresponse blocks +complete13610 RPM vs21capturedoutputs. Entry live registers explicitly seeded, signeddivisioncontract. Not fulltrafficcaller/playerframe.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [f,root/'tools/compare_live_traffic_response_rom.py',root/'src/traffic_follow.c',root/'src/gears.c',root/'bin/scene-viewer.exe']}
(root/'reports/live-traffic-response-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
