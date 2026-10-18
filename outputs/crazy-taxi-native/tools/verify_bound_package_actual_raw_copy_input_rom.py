from pathlib import Path
import json,hashlib
actual_copy_calls=[]
root=Path('outputs/crazy-taxi-native');base=(root/'tools/compare_ordinary_assets_rom.py').read_text();insert=base.index('\np=TitleAssetProbe')+1
extra="""
class InputObservedAssets(TitleAssetProbe):
 def __init__(self,*args):self.observing=False;super().__init__(*args)
 def external(self,t):
  if t!=0x803406c:return super().external(t)
  assert r[0x3406c:0x34074]==bytes.fromhex("7847c0463a31ffea")
  src,dst,count=self.reg[:3];assert count%4==0
  reg=self.reg.copy();sp=reg[13];saved=reg[4:12].copy();rom=r;pc=0x8000560;z=False;steps=0;stores=[]
  before=bytes(self.read(src+j,1) for j in range(count))
  def read(a):return self.read(a,4)
  def write(a,v):
   assert dst<=a<dst+count or sp-32<=a<sp,hex(a)
   assert not(a<0x200351e and a+4>0x20034dc)
   self.write(a,v,4);stores.append(a)
  while pc!=reg[14]:
   assert 0x8000560<=pc<=0x80005f0 and pc%4==0,hex(pc)
   steps+=1;assert steps<20000
   w=read(pc);nxt=pc+4;cond=w>>28
   assert cond in (0,1,14)
   if (cond==0 and not z) or (cond==1 and z):pc=nxt;continue
   if w==0xe92d0ff0:
    reg[13]-=32
    for j in range(8):write(reg[13]+j*4,reg[j+4])
   elif w==0xe8bd0ff0:
    reg[4:12]=[read(reg[13]+j*4) for j in range(8)];reg[13]+=32
   elif w in (0xe8b00ff0,0xe8a10ff0):
    if w==0xe8b00ff0:reg[4:12]=[read(reg[0]+j*4) for j in range(8)];reg[0]+=32
    else:
     for j in range(8):write(reg[1]+j*4,reg[j+4])
     reg[1]+=32
   elif w==0xe4904004:reg[4]=read(reg[0]);reg[0]+=4
   elif w==0xe4814004:write(reg[1],reg[4]);reg[1]+=4
   elif w==0xe12fff1e:nxt=reg[14]
   elif w&0x0f000000==0x0a000000:
    off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;nxt=pc+8+off*4
   elif w in (0xe1b03422,0xe1b032a3,0xe1b03123):
    n={0xe1b03422:8,0xe1b032a3:5,0xe1b03123:2}[w];reg[3]=(reg[2] if w==0xe1b03422 else reg[3])>>n;z=reg[3]==0
   elif w in (0xe20230e0,0xe212301c):reg[3]=reg[2]&(0xe0 if w==0xe20230e0 else 0x1c);z=reg[3]==0 if w==0xe212301c else z
   elif w==0xe2533001:reg[3]=(reg[3]-1)&0xffffffff;z=reg[3]==0
   else:raise AssertionError((hex(pc),hex(w)))
   pc=nxt
  assert reg[0]==src+count and reg[1]==dst+count and reg[3]==0 and reg[13]==sp and reg[4:12]==saved
  assert bytes(self.read(dst+j,1) for j in range(count))==before
  assert sum(dst<=a<dst+count for a in stores)==count//4
  self.reg[:]=reg;actual_copy_calls.append(dict(source=hex(src),destination=hex(dst),bytes=count,instructions=steps,stack=hex(sp-32)))
 def write(self,a,v,size=4):
  if self.observing:assert not(a<0x200351e and a+size>0x20034dc),('package input write',hex(a),size)
  super().write(a,v,size)
TitleAssetProbe=InputObservedAssets
"""
code=base[:insert]+extra+base[insert:]
needle='p.write(0x2007541,4,1);'
code=code.replace(needle,"p.regions[0x2000000][0x34dc:0x351e]=bytes((input_seed+i*13)&255 for i in range(66));before_input=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.observing=True;"+needle)
code=code.replace("native=(root/", "assert before_input==bytes(p.regions[0x2000000][0x34dc:0x351e]);assert all(not(int(t['destination'],16)<0x200351e and int(t['destination'],16)+t['bytes']>0x20034dc) for t in p.transfers)\nnative=(root/")
results=[]
for seed in [0,165,255]:
 source=code.replace('ordinary-assets-rom-comparison.json',f'bound-package-actual-copy-input-{seed}-rom.json');env={'actual_copy_calls':actual_copy_calls,'input_seed':seed,'__file__':str(Path(__file__).resolve())};exec(compile(source,'bound-package-actual-copy-input-qualification','exec'),env)
 results.append({'seed':seed,'instructions':env['p'].steps,'group_count':len(env['p'].images),'DMAtransfers':env['p'].transfers})
report={'status':'PASS_BOUND_PACKAGE_ACTUAL_ARM_UPLOAD_INPUT_RETAINED','actual_copy_calls':actual_copy_calls,'cases':results,'scope':'Whole27994/E154/E388/7190/73C0 onactualBound8070E50package plusFEC8fontsetup;66inputbytes retained/nooverlappingCPUstores orlogicalDMAtransfer ranges. Existingnativeassetbuffer comparison unchanged. ActualoriginalARM0560copybodyexecutedateachactualpackagecallwithboundedstore/stack/callee-savechecks; DMAlogicalbytecopies,28BD0objectsetup return0 stillexplicit; notjoinedobjectsetup/completebootstrapinputownership.'}
(root/'reports/bound-package-actual-copy-input-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS actual ARM packageinputretention3cases')
