exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for start,end in [(0xdf9c,0xdfbc),(0xdfcc,0xe01c),(0xdfba,0xdfc0)]:
 for i in md.disasm(r[start:end],0x08000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class SelectProbe(InitProbe):
 def __init__(self,city,ref,mode,seed):
  super().__init__((0,0,0,0,0,1,0,0,0,0));scene=[0x087556cc,0x08755734][city];self.regions={0x03006810:bytearray(248),0x02001768:bytearray(8),0x02001eb0:bytearray(512),0x0202b8ec:bytearray((seed+i*37)&255 for i in range(512))};self.write(0x03006810,scene);self.write(0x02001eb0,mode,1);self.calls=[];self.panelcache=bytearray(512);self.record=0 if ref<0 else self.read(scene+56)+ref*48
 def external(self,t):
  if t==0x08031860:
   assert self.reg[:4]==[64,64,self.record,self.record+12];self.calls.append(t);self.panelcache[:]=self.regions[0x0202b8ec]
  elif t==0x08033f2c:
   assert self.reg[:3]==[0x0202b8ec,0,512];self.regions[0x0202b8ec][:]=bytes(512)
  else:raise AssertionError(hex(t))
cases=[]
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctp').read_bytes();dc=struct.unpack_from('<I',data,12)[0]
 for ref in [-1,0,dc-1]:
  for mode in range(256):cases.append([city,ref,mode,(mode*13)&255])
rows=subprocess.run([str(root/'bin/destination-select-rom-compare.exe'),str(root/'assets/city1.ctp'),str(root/'assets/city2.ctp')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 city,ref,mode,seed=case;p=SelectProbe(*case);p.reg[0]=ref&0xffffffff;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x0800df9c,stop=0x10020000);parts=row.split();got=p.read(0x02001768);got=got if got<0x80000000 else got-0x100000000;assert list(map(int,parts[:4]))==[got,int(bool(p.read(0x0200176c))),12345,len(p.calls)],case;assert p.read(0x0200176c)==p.record;assert bytes.fromhex(parts[4])==p.regions[0x0202b8ec];assert bytes.fromhex(parts[5])==p.panelcache;steps+=p.steps
report={'pass_match':True,'cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete DF9C validselection/clear throughreturn, actualcity first/last destinationrecords/all256modebytes and priornonzerocache. Native material_blink_select destination/hasrecord/retainedclock/all512cachebytes and callbackprior512cachebytes EXACT, sourcepreview64x64record/record+12 arguments and preview-before-clear. 31860 previewbody and33F2C512zero fill explicit contracts, invalid fatalguard excluded. No pickupcaller/fullpreview/maincache scheduling proof.'}
(root/'reports/destination-select-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
