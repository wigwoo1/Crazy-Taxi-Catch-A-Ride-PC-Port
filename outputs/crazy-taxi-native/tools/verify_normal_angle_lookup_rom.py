from pathlib import Path
import json,subprocess,struct
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
r=ns['r'];assert struct.unpack_from('<I',r,0x2dda8)[0]==0x087ec9ec
for ins in ns['md'].disasm(r[0x2dd94:0x2dda6],0x0802dd94):ns['allowed_thumb'][ins.address]=bytes(ins.bytes)
p=ns['InitProbe']((0,0,0,0,0,1,0,0,0,0));values=list(range(-4096,4097));original=[];steps=0
for n in values:
 p.reg[0]=n&0xffffffff;p.reg[14]=0x10020001;p.steps=0;before=p.reg.copy();p.run(entry=0x0802dd94,stop=0x10020000);out=p.reg[0];original.append(out-4294967296 if out&0x80000000 else out);steps+=p.steps;assert p.reg[3:15]==before[3:15]
rom='Crazy Taxi - Catch a Ride (USA).gba';cp=subprocess.run([str(root/'bin/normal-angle-lookup-compare.exe'),rom],input='\n'.join(map(str,values))+'\n',capture_output=True,text=True);assert cp.returncode==0;native=list(map(int,cp.stdout.split()));assert native==original
report=dict(status='NATIVE_ORIGINAL_NORMAL_ANGLE_LOOKUP_PASS',cases=len(values),source_instructions=steps,table_owner='087ec9ec',scope='Original2DD94 nine-instruction signedhalfword lookup all8193 normal values vs existing native normal_angles_decode/table indexing. Saved r3-r14 preserved. Isolated routine, no sharedcaller/liveworld trajectory claim.')
(root/'reports/normal-angle-lookup-original-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
