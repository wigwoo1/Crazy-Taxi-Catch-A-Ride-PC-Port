from pathlib import Path
import json,struct,subprocess
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns);search={};exec((root/'tools/passenger_search_original_arm.py').read_text(),search)
cases=[]
for count in (0,1,4):
 for taxi in ((0,0),(1024,1024),(12800,12800),(-4000,3000)):
  p=ns['InitProbe']((0,0,0,0,0,1,0,0,0,0));scene=0x02010000;spawn=0x02011000;customer=0x02005f2c;out=0x02001680
  p.regions[0x03006810]=bytearray(struct.pack('<I',scene));p.regions[out]=bytearray(4);p.regions[scene]=bytearray(64);p.regions[spawn]=bytearray(128);p.regions[customer]=bytearray(176);p.write(scene+14,count,2);p.write(scene+48,spawn,4)
  xy=[(0,0),(2000,2000),(4000,-3000),(-5000,7000)]
  for i in range(count):
   p.write(customer+i*44,1 if i<3 else 65,1);p.write(customer+i*44+1,0 if i!=2 else 8,1);p.write(customer+i*44+3,i,1);p.write(spawn+i*32+4,xy[i][0],4);p.write(spawn+i*32+12,xy[i][1],4)
  p.reg[:3]=[taxi[0]&0xffffffff,taxi[1]&0xffffffff,out];p.reg[14]=0x10020001
  result=search['execute_search'](p,ns['r']);blob=struct.pack('<I2i',count,*taxi)+bytes.fromhex(result['before'])+b''.join(struct.pack('<2i',*pair) for pair in result['spawns'])
  cp=subprocess.run([str(root/'bin/passenger-search-owner-compare.exe')],input=blob,capture_output=True);expected=struct.pack('<2i',result['index'],result['distance'])+bytes.fromhex(result['after']);assert cp.returncode==0 and cp.stdout==expected,(count,taxi,cp.returncode,cp.stderr,result)
  cases.append(dict(count=count,taxi=taxi,index=result['index'],distance=result['distance'],source_instructions=result['instructions'],compared_bytes=len(expected)))
report=dict(status='NATIVE_ORIGINAL_PASSENGER_SEARCH_OWNER_PASS',cases=cases,scope='Bounded original ROM050AC instruction execution vs existing native passenger_search for isolated explicit scene/customer/spawn fixtures. Full44byte records and candidate/distance exact; not sharedframe or actual eligibility provenance.')
(root/'reports/passenger-search-owner-original-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
