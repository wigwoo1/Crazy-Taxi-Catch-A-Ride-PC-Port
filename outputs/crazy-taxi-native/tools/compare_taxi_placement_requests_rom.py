"""Normal taxi15956..1599A original caller request contract;28658 not executed."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x15956:0x1599e],0x8015956):allowed_thumb[i.address]=bytes(i.bytes)
class PlacementProbe(ReplaceProbe):
 def external(self,t):
  assert t==0x8028658,hex(t)
  self.calls += [self.reg[0],self.reg[1]]+[self.read(self.reg[2]+i*4) for i in range(3)]+[self.reg[3]]+[self.read(self.reg[13]+i*4) for i in range(3)]+[signed(self.read(self.reg[13]+i*4)) for i in [3,4]]
cases=[(scale,roll) for scale in [0,1,109,256,512,32767,32768,65535,65536,0xffffffff] for roll in [0,1,4095,32767,32768,65535]]
rows=subprocess.run([str(root/'bin/taxi-placement-requests-rom-compare.exe')],input=''.join('%u %u\n'%c for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 scale,roll=case;p=PlacementProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[13]=0x10000400;p.reg[7]=0x10001000;p.reg[8]=0;p.reg[9]=scale;p.write(0x1000101a,roll,2);p.write(0x100004fc,0x10000800)
 for address,coords in [(0x10000420,[106,102,3537]),(0x10000800,[133,102,3537])]:
  for i,v in enumerate(coords):p.write(address+i*4,v)
 p.run(entry=0x8015956,stop=0x801599e)
 assert list(map(int,row.split()))==p.calls,(case,row,p.calls)
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Normal mode15956..1599A exact two28658 request arguments with projected position fixtures, bucket2/flags0/palette0, signed16 scale and negated16 roll. 28658 is captured helper contract, not executed; no affine queue/placement/render/facing proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/taxi_placement_requests.c',root/'src/taxi_placement_requests.h',root/'tests/taxi-placement-requests-rom-compare.c',root/'bin/taxi-placement-requests-rom-compare.exe']}}
(root/'reports/taxi-placement-requests-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
