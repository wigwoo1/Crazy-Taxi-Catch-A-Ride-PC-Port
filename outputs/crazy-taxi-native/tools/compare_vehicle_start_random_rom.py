"""Original14D00..14D28 random scaling slice; RNG/division explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x14d00:0x14d2a],0x08014d00):allowed_thumb[i.address]=bytes(i.bytes)
class VehicleProbe(InitProbe):
 def external(self,t):
  if t==0x0801e72c:
   assert self.reg[0]==20 and self.reg[1]==60;self.reg[0]=self.factor
  elif t==0x08032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:raise AssertionError(hex(t))
cases=[(base,seed) for base in [-32768,-301,-1,0,1,301,32767] for seed in range(256)]
rows=subprocess.run([str(root/'bin/vehicle-start-random-test.exe')],input=''.join(f'{b} {s}\n' for b,s in cases),text=True,capture_output=True,check=True).stdout.splitlines();steps=0
for case,row in zip(cases,rows):
 base,seed=case;native=list(map(int,row.split()));p=VehicleProbe((0,0,0,0,0,1,0,0,0,0))
 state=seed or 0x4652414e;state=(state<<1)&MASK
 if state&0x80000000:state^=0x1d872b41
 state=((state&0x7fffffff)^0x06000000)&0x7fffffff
 p.factor=20+state%41;p.owners=None;p.regions[0x02002634]=bytearray(0x740);p.write(0x02002634+0x532,base,2)
 p.reg[6]=0x02002634;p.reg[13]=0x10000400;p.write(0x10000480,0x02002634+0x6ec)
 p.run(entry=0x08014d00,stop=0x08014d2a)
 def signedhalf(v):return v-65536 if v&32768 else v
 got=[signedhalf(p.read(0x02002634+0x6ec,2)),signedhalf(p.read(0x02002634+0x6ee,2)),p.read(0x02002634+0x6f0),state]
 assert got==native,(case,got,native);steps+=p.steps
assert len(rows)==len(cases)
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Original14D00..14D28 slice; literal base532 and output6EC/6EE/6F0, signed scaling. RNG1E72C and division32488 explicit contracts; no whole14A58/host startup equivalence.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/vehicle-start-random-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
