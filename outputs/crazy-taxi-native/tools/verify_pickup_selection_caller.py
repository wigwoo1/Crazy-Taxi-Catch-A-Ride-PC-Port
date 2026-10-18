exec(open('outputs/crazy-taxi-native/tools/compare_destination_select_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0xb0c2:0xb0f2],0x0800b0c2):allowed_thumb[i.address]=bytes(i.bytes)
class PickupCaller(SelectProbe):
 def __init__(self,city,index,mode,seed):
  scene=[0x087556cc,0x08755734][city];records=int.from_bytes(r[scene-0x08000000+48:scene-0x08000000+52],'little');dest=r[records-0x08000000+index*32+1];super().__init__(city,dest,mode,seed);self.index=index;self.dest=dest;self.order=[];self.regions[0x10030000]=bytearray(8);self.write(0x10030000,0x02005f2c+index*44);self.write(0x10030004,123);self.reg[3]=0x10030000;self.reg[5]=0x10030004
 def external(self,t):
  if t==0x0800df9c:
   assert self.reg[0]==self.dest;self.order.append(['select',self.reg[0]]);self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x0800cb84:assert self.reg[0]==self.index;self.order.append(['fare',self.reg[0]])
  elif t==0x0800c540:assert self.reg[:2]==[1,self.index];self.order.append(['sound',*self.reg[:2]])
  else:super().external(t)
steps=0;cases=0;cities={}
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctp').read_bytes();count=struct.unpack_from('<I',data,4)[0];cities[name]=count
 for index in range(count):
  for mode in [0,31,128,159,224,255]:
   p=PickupCaller(city,index,mode,165);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x0800b0c2,stop=0x0800b2d8);assert p.read(0x10030004)==0;assert p.order==[['select',p.dest],['fare',index],['sound',1,index]];assert len(p.calls)==int(mode&224!=128);steps+=p.steps;cases+=1
report={'pass_match':True,'cases':cases,'actual_thumb_instructions':steps,'customer_counts':cities,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'ActualB0C2..B0F0 pickup completion caller + complete nestedDF9C on everyactualcity customerindex and6modefamilies. Original44byteactorpointer->index inverse thenactual32bytecustomerrecord destination; selection/preview-before-fare-before-sound andgateclear exact. 31860/33F2C/fareCB84/soundC540 helpercontracts, notcompletepickup/frame or nativeobserver trace equivalence.'}
(root/'reports/pickup-selection-caller-source-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
