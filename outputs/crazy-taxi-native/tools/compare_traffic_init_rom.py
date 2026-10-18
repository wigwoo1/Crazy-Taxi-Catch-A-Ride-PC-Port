"""Complete original6580 traffic startup; all actor/sprite bytes and parameters."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_reset_rom.py').read().split('\nseeds=')[0])
todo=[0x08006580];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x08006580<=a<=0x080066ae,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class TrafficProbe(ExitProbe):
 run=MotionProbe.run
 def __init__(self,*args):self.owners={0x020000e8:bytearray(12),0x020000f4:bytearray(4600),0x020012ec:bytearray(576),0x02001eb0:bytearray(4)};super().__init__(*args)
 def read(self,a,size=4):
  for base,data in self.owners.items():
   if base<=a and a+size<=base+len(data):return int.from_bytes(data[a-base:a-base+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  for base,data in self.owners.items():
   if base<=a and a+size<=base+len(data):data[a-base:a-base+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  super().write(a,v,size)
 def external(self,t):raise AssertionError(hex(t))
cases=[(setting,seed,mask) for setting in range(256) for seed,mask in [(0,0),(165,7),(255,setting&7)]]
rows=subprocess.run([str(root/'bin/traffic-init-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 setting,seed,mask=case;p=TrafficProbe((0,0,0,0,0,1,0,0,0,0))
 for base in [0x020000f4,0x020012ec]:p.owners[base]=bytearray((seed+i*37)&255 for i in range(len(p.owners[base])))
 for i in range(3):p.write(0x020000e8+i*4,0 if mask&(1<<i) else seed+i+1)
 p.write(0x02001eb3,setting,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x08006580,stop=0x10020000)
 parts=row.split();assert int(parts[0])==r[0x4a54b0+(setting>>4)],case
 assert bytes(p.owners[0x020000f4])==bytes.fromhex(parts[1]),('actors',case)
 assert bytes(p.owners[0x020012ec])==bytes.fromhex(parts[2]),('sprites',case)
 assert [p.read(0x020000e8+i*4) for i in range(3)]==list(map(int,parts[3:6])),('parameters',case)
 assert [p.read(0x02000104+i*92) for i in range(int(parts[0]))]==list(map(int,parts[6:])),('native actor countdown bridge',case)
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete6580 with original density table, all256settings, all4600actorbytes/576spritebytes/3parameters, retained non-countdown actor data and sprite trailing bytes. Native typed actor countdown/parameters match source, all other typed object bytes retained. No full traffic simulation/render or packed-to-typed mapping of unrelated fields claim.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/traffic-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
