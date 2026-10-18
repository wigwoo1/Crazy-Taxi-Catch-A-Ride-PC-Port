from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');base=(root/'tools/compare_ordinary_assets_rom.py').read_text();insert=base.index('\np=TitleAssetProbe')+1
extra="""
class InputObservedAssets(TitleAssetProbe):
 def __init__(self,*args):self.observing=False;super().__init__(*args)
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
 source=code.replace('ordinary-assets-rom-comparison.json',f'bound-package-input-{seed}-rom.json');env={'input_seed':seed,'__file__':str(Path(__file__).resolve())};exec(compile(source,'bound-package-input-qualification','exec'),env)
 results.append({'seed':seed,'instructions':env['p'].steps,'group_count':len(env['p'].images),'DMAtransfers':env['p'].transfers})
report={'status':'PASS_BOUND_PACKAGE_LOGICAL_UPLOAD_INPUT_RETAINED','cases':results,'scope':'Whole27994/E154/E388/7190/73C0 onactualBound8070E50package plusFEC8fontsetup;66inputbytes retained/nooverlappingCPUstores orlogicalDMAtransfer ranges. Existingnativeassetbuffer comparison unchanged. ARMrawcopy andDMA logicalbytecopy contracts,28BD0objectsetup return0 stillexplicit; notactualARM/objectsetup/completebootstrapinputownership.'}
(root/'reports/bound-package-input-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS packageinputretention3cases')
