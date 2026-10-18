"""Complete11804 including whole1E7F0 copy and11540 reset; division contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/vehicle-template-initialize-cfg.json').read_text())['instructions']})
class TemplateProbe(InitProbe):
 def external(self,t):
  if t==0x08032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(t)
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/vehicle-reset-cfg.json').read_text())['instructions']})
class FullTemplateProbe(TemplateProbe):
 def external(self,t):
  if t==0x08011540:self.run(entry=t,stop=self.reg[14]&~1)
  else:super().external(t)
cases=[(offset,seed,settings) for offset in [0x4a554c,0x4a5794] for seed in [0,165,255] for settings in range(256)]
rom='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(root/'bin/vehicle-template-initialize-rom-compare.exe'),rom],input=''.join(f'{o} {s} {m}\n' for o,s,m in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 offset,seed,settings=case;p=FullTemplateProbe((0,0,0,0,0,1,0,0,0,0));p.regions[0x02002634]=bytearray((seed+i*37)&255 for i in range(0xea0))
 p.write(0x02001eb0,settings,1);p.scalars[0x02002630]=12345;p.reg[0]=0x02002634;p.reg[1]=0x08000000+offset;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x08011804,stop=0x10020000)
 parts=row.split();assert bytes(p.regions[0x02002634])==bytes.fromhex(parts[0]),case;assert p.scalars[0x02002630]==int(parts[1]),case;steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete11804 INCLUDING whole original1E7F0 copy, all3744actorbytes, both source templates and nonzero retained fields. All derived configuration writes included; division32488 explicit contract, complete11540 reset and state2630 included; no full14A58/live startup equivalence.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/vehicle-template-initialize-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
