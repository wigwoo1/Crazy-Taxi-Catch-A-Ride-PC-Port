import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');cn={};sn={}
exec((root/'tools/probe_ordinary_first_movement_rom.py').read_text().split('rows=[]')[0],cn)
source=(root/'tools/compare_countdown_source_initialized_rom.py').read_text().split('for d in rows:')[0].replace('startup-countdown-states.txt','startup-transition-countdown.txt')
exec(source,sn);sp=sn['p'];calls=[]
camera=[list(map(int,line.split())) for line in (root/'reports/startup-transition-camera.txt').read_text().splitlines()]
assert len(camera)==408
for tick,row in enumerate(camera):
 if tick<348:
  d=sn['rows'][tick];sp.scalars.update({0x2007602:d['audio'],0x200177b:d['fade'],0x2001ea8:d['exiting'],0x2003b00:d['delta']});sp.events=0;sp.requests=[];sp.reg[13]=0x10000400;sp.reg[14]=0x10020001
  try:sp.run(entry=0x800ba04,stop=0x10020000)
  except sn['ReturnBoundary']:pass
 active=sp.read(0x2001696,1);assert active==row[17]
 p=cn['CallerProbe']((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 p.scalars.update({0x2001700:0,0x2001704:0,0x2003b00:1,0x30053c0:1,0x3004ca0:0x2002634})
 p.write(0x2005f20,row[10],1);p.write(0x2001696,active,1);p.write(0x2001697,0,1);p.write(0x2001708,0,1)
 p.reg[9]=0x2001708;p.reg[7]=0x2002634;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 try:p.run(entry=0x800afd4,stop=0x800b320)
 except cn['FirstCall']:pass
 calls.append(p.calls[-1])
assert calls==['0x80130ac']*347+['0x8011a3c']*61
assert len((root/'reports/startup-transition-gear.txt').read_text().splitlines())==calls.count('0x80130ac')
assert len((root/'reports/startup-transition-driver.txt').read_text().splitlines())==calls.count('0x8011a3c')
report=dict(status='PASS',frames=len(calls),intro_updates=347,normal_updates=61,scope='OriginalB6EC/BA04 feeds originalAFD4 first movement call selection; captured freeze, ordinary demo0/ended0/boarding0/delivery0 fixtures; sourceSBpointer1708; 29264 boundary no-state contract; native trace counts agree with previously source-compared persistent actor chain. Not full original caller or frame proof.')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'tools/probe_ordinary_first_movement_rom.py',root/'reports/startup-transition-countdown.txt',root/'reports/startup-transition-camera.txt',root/'reports/startup-transition-gear.txt',root/'reports/startup-transition-driver.txt']}
(root/'reports/transition-caller-frames-rom.json').write_text(json.dumps(report,indent=2));print(report)
