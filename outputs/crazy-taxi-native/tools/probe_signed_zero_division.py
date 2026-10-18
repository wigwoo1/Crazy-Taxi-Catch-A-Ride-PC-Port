import pathlib,json
root=pathlib.Path('outputs/crazy-taxi-native');scope={}
exec((root/'tools/compare_live_ordinary_driver_drag_rom.py').read_text().split('import sys')[0],scope)
results=[]
for numerator in [0,1,-1,2147483647,-2147483648]:
 p=scope['DriverProbe']((0,0,0,0,0,1,0,0,0,0));p.reg[0]=numerator&0xffffffff;p.reg[1]=0;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8032488,stop=0x10020000)
 assert p.reg[0]==0;results.append(dict(numerator=numerator,quotient=p.reg[0],instructions=p.steps))
report={'status':'PASS','cases':results,'scope':'Actual original8032488 zero branch through2510/251C/return instructions. No substituted quotient. Thumb interpreter movPC return support adapter; runtime remains native.'}
(root/'reports/original-signed-zero-division.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
