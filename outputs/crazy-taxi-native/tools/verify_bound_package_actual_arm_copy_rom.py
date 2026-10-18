from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
base=(root/'tools/compare_ordinary_assets_rom.py').read_text()
arm_source=(root/'tools/verify_bound_raw_copy_arm_rom.py').read_text()
arm_body=arm_source.split(' src,dst,sp=')[1].split("report=dict(status=")[0]
arm_body=' src,dst,sp='+arm_body
arm_body=arm_body.replace(' src,dst,sp=0x8071000,0x6010000,0x3007f00',' src,dst,sp=copy_source,copy_destination,copy_stack')
arm_body='\n'.join(line[1:] if line.startswith(' ') else line for line in arm_body.splitlines())
arm_body=arm_body.replace("reg[4:12]=[0x12340000+i for i in range(8)]","reg[4:12]=copy_saved.copy()")
prefix,body=base.split('\np=TitleAssetProbe',1)
ns={'__file__':str(Path(__file__).resolve())};exec(prefix,ns)
Parent=ns['TitleAssetProbe'];copies=[]
class ActualCopyAssets(Parent):
 def external(self,t):
  if t!=0x803406c:return super().external(t)
  src,dst,count=self.reg[:3];sp=self.reg[13]
  assert count%4==0 and src%4==0 and dst%4==0
  assert not(dst<0x200351e and dst+count>0x20034dc)
  assert not(sp-32<0x200351e and sp>0x20034dc)
  env=dict(rom=ns['r'],struct=ns['struct'],count=count,copy_source=src,copy_destination=dst,copy_stack=sp,copy_saved=self.reg[4:12].copy(),rows=[])
  exec(arm_body,env)
  for offset in range(0,count,4):self.write(dst+offset,env['mem'][dst+offset])
  self.reg[:4]=env['reg'][:4]
  copies.append(dict(source=hex(src),destination=hex(dst),bytes=count,diagnostic_stack=hex(sp),arm_instructions=env['steps']))
ns['TitleAssetProbe']=ActualCopyAssets
body='p=TitleAssetProbe'+body
body=body.split("report={'status'")[0]
exec(body,ns)
report=dict(status='PASS',copies=copies,thumb_instructions=ns['p'].steps,native_asset_bytes=len(ns['native']),scope='Actual Bound8070E50 package and font callers invoke exact original ARM0560 raw-copy body at every3406C request. Returned registers and bytes used by caller;17168 native asset bytes remain exact. Destination and32-byte diagnostic stack disjoint from retained-input owner. Caller stack synthetic10000400 as prior CPU probe; no original gameplay stack claim. HardwareDMA logical,28BD0 return contract remain; no complete bootstrap/OAM/frame proof.',rom_sha256=hashlib.sha256(ns['r']).hexdigest())
(root/'reports/bound-package-actual-arm-copy-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(copies),'actual package ARM copies;',len(ns['native']),'native asset bytes')
