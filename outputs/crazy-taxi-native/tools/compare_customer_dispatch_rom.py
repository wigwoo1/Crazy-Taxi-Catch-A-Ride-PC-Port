"""Original full normal customer dispatcher state, rendering explicit boundary."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import itertools,subprocess,json
freeze=int("--freeze" in sys.argv)
lines=(root/'reports/normal-customer-dispatch-cfg.txt').read_text().splitlines()
for line in lines:
 address=int(line.split()[0],16);i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
class DispatchProbe(IdleProbe):
 def read(self,a,n=4):
  if 0x3000000<=a<0x3010000:
   assert a&~3 in self.arm.mem,('unmappedIWRAM',hex(a));return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x3000000<=a<0x3010000:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x803201c:self.depth_requests.append(self.reg[0])
  elif target==0x8031a50:self.sprite_requests.append(self.reg[0])
  else:super().external(target)
cases=[]
for flags,kind,stage,x,timer,selection in itertools.product([1,5,65,3],[0,7,8],range(4),[0,12799,13000],[0,5],[(0,0),(1,1)]):
 b=bytearray(44);b[0]=flags;b[1]=kind;b[9]=5;b[10]=stage;b[12]=timer
 animation=[0x87ec47c,0x87ec484,0x87ec4bc,0x87ec4f4][stage]
 b[28:32]=animation.to_bytes(4,'little');b[16:20]=x.to_bytes(4,'little')
 cases.append([17,*selection,6,0,1,0,0,*b])
if "--fade-edges" in sys.argv:
 expanded=[]
 for case in cases:
  for fade in (0,88,89,90,127,255):
   row=case.copy();row[8+11]=fade;expanded.append(row)
 cases=expanded
cp=subprocess.run([str(root/'bin/customer-dispatch-rom-compare.exe')]+(['freeze'] if freeze else []),input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
outputs=[list(map(int,s.split())) for s in cp.stdout.splitlines()];assert len(outputs)==len(cases)
steps=0;request_count=0;rng_calls=0
for index,(case,expected) in enumerate(zip(cases,outputs)):
 seed,selected,pending,c,f,d,x,z=case[:8];b=case[8:];p=DispatchProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.depth_requests=[];p.sprite_requests=[]
 # Map only referenced global blocks, then replace meaningful owner pointers.
 for address,data in allowed_thumb.items():
  if not 0x8022df4<=address<0x80233d8:continue
  h=int.from_bytes(data[:2],'little')
  if h&0xf800==0x4800:
   off=address-0x8000000;at=((off+4)&~3)+(h&255)*4;v=int.from_bytes(r[at:at+4],'little')
   if 0x2000000<=v<0x2040000 or 0x3000000<=v<0x3010000:
    for a in range(v&~3,(v&~3)+32,4):p.write(a,0)
 actor=0x2005f2c
 for j,v in enumerate(b):p.write(actor+j,v,1)
 p.write(0x2003b04,seed);p.write(0x2003b00,1);p.write(0x2005f28,c,1);p.write(0x2005f29,f,1);p.write(0x2005f2a,d,1)
 p.write(0x2002634,x);p.write(0x2002638,0);p.write(0x200263c,z)
 p.write(0x2007530,actor if selected else 0);p.write(0x2001704,pending)
 p.write(0x2005f20,freeze,1)
 p.write(0x2007534,256);p.write(0x2007538,0)
 descriptor=0x2001000;p.write(0x3006810,descriptor);p.write(descriptor+12,1<<16)
 p.reg[14]=0x10020001;p.run(entry=0x8022df4,stop=0x10020000)
 actual=[p.read(0x2003b04),p.read(0x2005f28,1),p.read(0x2005f29,1),p.read(0x2005f2a,1)]+[p.read(actor+j,1) for j in range(44)]
 assert actual==expected,(index,case[:8],b[0],b[10],actual,expected)
 steps+=p.steps;request_count+=len(p.sprite_requests);rng_calls+=p.calls
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'freeze':freeze,'fade_edges':('--fade-edges' in sys.argv),'cases':len(cases),'thumb_instructions':steps,'render_requests_observed':request_count,'equal':True,'original_rng_calls':rng_calls,'scope':'Complete original22DF4 throughreturn versus nativecustomer service all44actorbytes/RNG/phase on singlecustomer normalmode fixtures. NestedoriginalRNG; signedremainder explicitcontract. 31A50/3201C request boundaries only, notprojection/pixels. Originalstartup/globalbootstrap/fullnormalframe notqualified; freeze state specified by report.'}
(root/('reports/customer-frozen-dispatch-rom-comparison.json' if freeze else 'reports/customer-full-dispatch-rom-comparison.json')).write_text(json.dumps(report,indent=2));print('PASS',report)
