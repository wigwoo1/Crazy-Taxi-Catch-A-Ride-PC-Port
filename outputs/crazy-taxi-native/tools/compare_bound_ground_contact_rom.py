"""Original ground selection with explicit containment/division contracts, diagnostic."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
containment_source=open('outputs/crazy-taxi-native/tools/compare_bound_containment_rom.py').read().split('root=pathlib.Path')[0]
containment_source=containment_source.replace('def containment(case):','def containment(case,entry_v=False):').replace('n=z=v=False;steps=0','n=z=False;v=entry_v;steps=0')
exec(containment_source)
entry_v_counts={False:0,True:0}
md.detail=True
source=source.replace('self.steps<1000','self.steps<50000');exec(source)
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-ground-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']};address=int(json.loads((root/'reports/bound-ground-query-candidates.json').read_text())['source_world'],16)
class GroundProbe(MotionProbe):
 def external(self,target):
  if target==0x8032454:
   assert self.reg[2]==0x80029fc;self.reg[0]=(trunc(signed(self.reg[0]),signed(self.reg[1])) if self.reg[1] else signed(self.reg[0]))&MASK
  elif target==0x802df0c:
   case=tuple(signed(self.read(ptr+offset)) for ptr in self.reg[:4] for offset in [0,8]);entry_v_counts[self.flags[3]]+=1;self.reg[0]=containment(case,self.flags[3])[0]
  elif target==0x802a978:
   assert self.read(self.reg[13]+4)!=0;self.run(entry=target,stop=self.reg[14]&~1)
  else:raise AssertionError(('unqualifiedgroundcall',hex(target)))
contactcfg=json.loads((root/'reports/challenge-bound-contact-interpolation-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in contactcfg['instructions']})
schema=json.loads((root/'reports/bound-launch-world-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-states.txt').read_text().splitlines()];steps=0
queryinputs=[]
for row in rows:
 d=dict(zip(schema,row[1:]));queryinputs.append((d['query_x'],d['query_y'],d['query_z']))
res=subprocess.run(['outputs/crazy-taxi-native/bin/ground-contact-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in queryinputs),text=True,capture_output=True,check=True);native=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(native)==len(rows)
for row,nativerow in zip(rows,native):
 d=dict(zip(schema,row[1:]));probe=GroundProbe((0,0,0,0,0,1),0);point=probe.base;normal=point+0x100;material=point+0x200;contact=point+0x300;probe.write(contact,0xdeadbeef)
 for off,value in [(0,d['query_x']),(4,d['query_y']),(8,d['query_z'])]:probe.write(point+off,value)
 probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006940]=0x80029fc;probe.reg[:4]=[point,normal,material,contact];probe.reg[14]=0x10020001;probe.run(entry=0x8029664,stop=0x10020000)
 got=[signed(probe.reg[0])]+[signed(probe.read(normal+i)) for i in [0,4,8]]+[probe.read(material)]
 assert nativerow[0]==1;expected=nativerow[1:6];got.append(probe.read(contact));expected.append(nativerow[7] if nativerow[6] else 0xdeadbeef);assert got==expected,dict(tick=row[0],original=got,native=expected);steps+=probe.steps
report=dict(cases=len(rows),thumb_instructions=steps,containment_entry_v_counts=entry_v_counts,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalground29664 readsoriginalROMdescriptor/grid/geometry, comparesheight/normal/material for161actualqueries. Originalcontainment consumes recovered callerVflag; signeddivisioncontract; originalcontactinterpolation2A978executed; nonnulloutput andcontactvaluecompared. NotfullGBAframe.')
(root/'reports/bound-ground-contact-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
