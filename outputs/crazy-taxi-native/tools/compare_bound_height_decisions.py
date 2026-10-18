"""Original height decision branches using observed contact data, diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
source=source.replace('while pc!=stop:','while pc not in stop if isinstance(stop,set) else pc!=stop:');exec(source)
root=pathlib.Path('outputs/crazy-taxi-native');allowed_thumb={}
for name in ['world','gravity']:
 cfg=json.loads((root/('reports/challenge-bound-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class HeightProbe(MotionProbe):
 def write(self,a,v,n=4):
  if 0x2001540<=a<0x2001550:
   assert n==4 and a%4==0;self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  else:super().external(target)
schema=json.loads((root/'reports/bound-launch-world-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-states.txt').read_text().splitlines()];steps=0;events=[]
for row in rows:
 tick=row[0];d=dict(zip(schema,row[1:]));probe=HeightProbe((0,0,0,0,0,d['delta']),0);base=probe.base;contact=base+0xf00
 for off,v,n in [(0,d['query_x'],4),(4,d['input_y'],4),(8,d['query_z'],4),(0x10,d['dy'],4),(0x698,d['gravity698'],4),(0x28,(d['flags28']&~16)|(d['output_flags28']&16),1),(0x704,d['response704'],4),(0x70c,d['response70c'],4),(0x730,d['velocity730'],2),(0x732,d['velocity732'],2),(0xe84,5324,4)]:probe.write(base+off,v,n)
 probe.reg[14]=0x10020001;probe.run(entry=0x8015b00,stop=0x10020000)
 probe.write(contact+4,d['ground_height']);probe.reg[3]=base+0x28;probe.reg[5]=base;probe.reg[6]=d['ground_height']&MASK;probe.reg[7]=contact;probe.reg[10]=1
 for a in [0x200153c,0x2001540,0x2001544,0x2001548,0x200154c,0x2001eb0]:probe.arm.mem[a]=0
 probe.arm.mem[0x2001534]=5<<24;probe.arm.mem[0x2001eb0]=128
 probe.run(entry=0x8012124,stop={0x80124ba,0x80124c6})
 got=[signed(probe.read(base+4)),signed(probe.read(base+0x698)),probe.read(base+0x28,1)];expected=[d['output_y'],d['output_gravity698'],d['output_flags28']]
 assert got==expected,dict(tick=tick,original=got,native=expected)
 if d['took_off'] or d['landed']:events.append(dict(tick=tick,y=got[0],gravity=got[1],flags=got[2]))
 steps+=probe.steps
report=dict(cases=len(rows),thumb_instructions=steps,events=events,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalgravity andheightdecision12124..24BA/24C6 on161nativecontactfixtures, comparesY/gravity/flags. Excludesgroundedsprings/alignment/finalpose; contactflags explicitobservedfixture, notindependentgroundquery.')
(root/'reports/bound-height-decision-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
