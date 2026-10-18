"""Complete11804..11A1C preparation including whole1E7F0 copy;11540 excluded."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/vehicle-template-initialize-cfg.json').read_text())['instructions']})
class TemplateProbe(InitProbe):
 def external(self,t):
  if t==0x08032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(t)
cases=[(offset,seed) for offset in [0x4a554c,0x4a5794] for seed in [0,1,127,165,255,0x12345678]]
rom='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(root/'bin/vehicle-template-load-rom-compare.exe'),rom],input=''.join(f'{o} {s}\n' for o,s in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 offset,seed=case;p=TemplateProbe((0,0,0,0,0,1,0,0,0,0));p.regions[0x02002634]=bytearray((seed+i*37)&255 for i in range(0xea0))
 p.reg[0]=0x02002634;p.reg[1]=0x08000000+offset;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x08011804,stop=0x08011a1e)
 assert bytes(p.regions[0x02002634])==bytes.fromhex(row),case;steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete11804..11A1C preparation INCLUDING whole original1E7F0 copy, all3744actorbytes, both source templates and nonzero retained fields. All derived configuration writes included; division32488 explicit contract,11540 reset excluded; no full14A58/live startup equivalence.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/vehicle-template-load-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
