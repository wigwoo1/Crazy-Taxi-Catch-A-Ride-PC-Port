"""Capture original21C5C Yes exit fade arguments; explicit callee boundary."""
exec(open('outputs/crazy-taxi-native/tools/compare_pause_menu_rom.py').read().split('\ncases=')[0])
rows=[]
for selection in (1,2):
 p=PauseProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[]
 for i in cfg:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for off,v in [(0,1),(1,selection),(2,selection),(3,0)]:p.write(0x2005f20+off,v,1)
 p.write(0x2003500,1,1) # Original Confirm pressed byte.
 p.reg[14]=0x10020001;p.run(entry=0x8021c5c,stop=0x10020000)
 assert p.requests==[[0x800e5a8,0,32,0x201,0x201]],p.requests
 assert p.read(0x2001ea8,1)==selection
 assert p.read(0x2005f1a,1)==int(selection==2)
 assert p.read(0x2005f20,1)==1
 rows.append({'selection':selection,'fade_arguments':p.requests[0][1:],'outer':selection,'exit_flag':int(selection==2),'frozen':1,'thumb_instructions':p.steps})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'equal':True,'observations':rows,'source_sha256':hashlib.sha256((root/'tools/audit_pause_exit_fade_rom.py').read_bytes()).hexdigest(),'scope':'Actual full21C5C instructions for frozen Yes confirmation Restart and Quit. Original E5A8 captured boundary; mode0 endpoint32 BG/OBJ masks0201, owner1/2, frozen retained. Does not execute fade callee, device input, child or full host.'}
(root/'reports/pause-exit-fade-rom-audit.json').write_text(json.dumps(report,indent=2));print('PASS',report)
