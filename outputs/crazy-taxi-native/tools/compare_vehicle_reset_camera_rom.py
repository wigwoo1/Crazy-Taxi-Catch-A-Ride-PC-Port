"""Original full reset camera outputs compared to native for all settings."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/vehicle-reset-cfg.json').read_text())['instructions']})
class ResetProbe(CameraProbe):
 def write(self,a,v,n=4):
  if a==0x2002630 and n==4:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
cases=[(s,h) for s in range(256) for h in [0,3073*4096,-128*4096,4095*4096+2048]]
res=subprocess.run([str(root/'bin/vehicle-reset-camera-compare.exe')],input=''.join(f'{s} {h}\n' for s,h in cases),capture_output=True,text=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
for (settings,heading),row in zip(cases,rows):
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634
 for a in range(base,base+0xea0,4):p.arm.mem[a]=0
 p.write(base+0x20,heading);p.arm.mem[0x2001eb0]=settings;p.arm.mem[0x2002630]=99;p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=0x8011540,stop=0x10020000)
 got=[signed(p.read(base+0x654)),p.read(base+0x64c,2),signed(p.read(base+0x650)),signed(p.arm.mem[0x2002630])]
 assert got==row,dict(settings=settings,heading=heading,original=got,native=row)
report=dict(cases=len(cases),pass_match=True,mode=2,heading_rule='settings&E0==80: camera heading equals actor20; all other settings: ((ASR12(actor20)+2048)&4095)<<12',scope='Complete original11540 reset camera outputs vs native for256settings4headings. No original placement/bootstrap/fullframe proof.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'tools/compare_vehicle_reset_camera_rom.py',root/'src/vehicle_reset.c',root/'reports/vehicle-reset-cfg.json',root/'tests/vehicle_reset_camera_compare.c',root/'bin/vehicle-reset-camera-compare.exe']}
(root/'reports/vehicle-reset-camera-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
