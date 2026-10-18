"""Original alignment and normal-angle lookup vs native; diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-align-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
for i in md.disasm(r[0x2dd94:0x2dda6],0x802dd94):allowed_thumb[i.address]=i.bytes
assert int.from_bytes(r[0x2dda8:0x2ddac],'little')==0x87ec9ec
class AlignProbe(MotionProbe):
 def external(self,target):
  if target==0x802dd94:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
cases=[(p,-p,v,-v,nx,nz,d) for p in [-178,0,272] for v in [-32768,-10,-1,0,1,32767] for nx in [-4096,0,4096] for nz in [-4096,-1088,0,4096] for d in [1,2]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/align-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;p,z,v,w,nx,nz,d=case;probe=AlignProbe((0,0,0,0,0,d),0);b=probe.base;normal=b+0xf00
 for off,value,n in [(0x6f4,p,4),(0x6fc,z,4),(0x730,v,2),(0x732,w,2),(0xf00,nx,4),(0xf04,4000,4),(0xf08,nz,4)]:probe.write(b+off,value,n)
 probe.reg[:4]=[normal,b+0x6f4,b+0x730,b+0x732];probe.reg[14]=0x10020001;probe.run(entry=0x8014968,stop=0x10020000)
 def half(off):
  value=probe.read(b+off,2);return value-65536 if value&32768 else value
 got=[signed(probe.read(b+off)) for off in [0x6f4,0x6fc]]+[half(0x730),half(0x732)];assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal14968alignmentand2DD94lookup directROMnormalangletable vsnative; halfwordwrapping/dampingboundaryfixtures. Syntheticbodyinputs, notfullworld/GBAframe.')
(root/'reports/align-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
