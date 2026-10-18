"""Complete original2944C request projection vsnative, pixels excluded."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/tip-hud-cfg.json').read_text())['instructions']})
class HudProbe(UpdateProbe):
 def external(self,target):
  if target==0x8031ef8:
   assert self.reg[2]==0x100010 and self.read(self.reg[13])==1
   self.images.append([signed(self.reg[0]),signed(self.reg[1]),self.reg[3]])
  elif target==0x8032520:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b)&MASK
  elif target==0x801e420:
   assert self.reg[1]==0x84a0cfc and r[0x4a0cfc:0x4a0d04]==b'$%d.%d%d'
   data=('$%d.%d%d'%(signed(self.reg[2]),signed(self.reg[3]),signed(self.read(self.reg[13])))).encode()+b'\0'
   for i,v in enumerate(data):self.write(self.reg[0]+i,v,1)
   self.reg[0]=len(data)-1
  elif target==0x800f7dc:
   sp=self.reg[13];assert self.reg[0]==0 and [self.read(sp+i) for i in [0,4,8,16]]==[0,0,36,256]
   ptr=self.reg[3];data=[]
   while self.read(ptr,1):data.append(self.read(ptr,1));ptr+=1;assert len(data)<32
   self.texts.append([signed(self.reg[1]),signed(self.reg[2]),self.read(sp+12),bytes(data).decode()])
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-updates.txt').read_text().splitlines()]
cases=[(frame,pause,row[16:576]) for row in rows for frame in [0,256,4352,32768] for pause in [0,1]]
res=subprocess.run([str(root/'bin/tip-hud-rom-compare.exe')],input=''.join(' '.join(map(str,[f,p]+tips))+'\n' for f,p,tips in cases),text=True,capture_output=True,check=True)
native=[line.split() for line in res.stdout.splitlines()];assert len(native)==len(cases);steps=requests=0
for case,output in zip(cases,native):
 frame,pause,tips=case;p=HudProbe((0,0,0,0,0,1,0,0,0,0));p.images=[];p.texts=[]
 p.write(0x2005332,frame,2);p.write(0x2005f20,pause,1)
 for i,v in enumerate(tips):p.write(0x2009abc+i,v,1)
 p.run(entry=0x802944c,stop=0x10020000)
 count=int(output[0]);assert count==len(p.images)==len(p.texts)
 for i,(image,txt) in enumerate(zip(p.images,p.texts)):
  fields=output[1+i*7:8+i*7];want=list(map(int,fields[:6]))+[fields[6]];assert image+txt==want,(case[:2],image+txt,want)
 steps+=p.steps;requests+=count
report=dict(cases=len(cases),image_text_pairs=requests,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal2944C vsnativeHUDrequest projection using296actualactive buffers with4controlledframe5332 values/2pause states. ImageXY/address andtextXY/color/string/requestflags exact. Original31EF8/F7DC are request boundaries; sprintf/signedmath contracts; frame5332 ownership and GPU/pixels unverified.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_tip_hud_rom.py','src/tip_hud.c','src/tip_hud.h','tests/tip_hud_rom_compare.c','bin/tip-hud-rom-compare.exe','reports/tip-hud-cfg.json','reports/current-reward-updates.txt']}
(root/'reports/tip-hud-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
