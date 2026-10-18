"""Complete original 2DB54 compared to existing runtime trig decoder."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for i in md.disasm(r[0x2db54:0x2dbb6],0x0802db54):allowed_thumb[i.address]=bytes(i.bytes)
saved=allowed_thumb.copy();source=source.replace('self.steps<50000','self.steps<500000');exec(source);allowed_thumb.update(saved)
class TablesProbe(ExitProbe):
 def __init__(self,*args):self.tables=bytearray([0xa5]*32768);super().__init__(*args)
 def read(self,a,size=4):
  if 0x0202c4f0<=a<0x020344f0:return int.from_bytes(self.tables[a-0x0202c4f0:a-0x0202c4f0+size],'little')
  if 0x08000000<=a<0x08800000:return int.from_bytes(r[a-0x08000000:a-0x08000000+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x0202c4f0<=a<0x020344f0:self.tables[a-0x0202c4f0:a-0x0202c4f0+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
 def external(self,target):raise AssertionError(('unexpected helper',hex(target)))
TablesProbe.run=MotionProbe.run
p=TablesProbe((0,0,0,0,0,1,0,0,0,0));p.reg[13]=0x10000400;p.reg[14]=0x10020001
p.run(entry=0x0802db54,stop=0x10020000)
asset=root/'assets/driving.ctr';raw=asset.read_bytes();assert raw[:4]==b'CTR1' and raw[4:8196]==r[0x4a0d08:0x4a2d08] and raw[8196:16388]==r[0x4a2d08:0x4a4d08]
run=subprocess.run([str(root/'bin/render-math-tables-rom-compare.exe'),str(asset)],text=True,capture_output=True,check=True)
rows=[tuple(map(int,line.split())) for line in run.stdout.splitlines()];assert len(rows)==4096
for i,want in enumerate(rows):assert (signed(p.read(0x0202c4f0+4*i)),signed(p.read(0x020304f0+4*i)))==want,(i,want)
report={'status':'PASS','words':8192,'actual_thumb_instructions':p.steps,'scope':'Complete2DB54 no called helpers; all4096 sine and cosine words exact against existing taxi_trig_decode, actual asset halfwords match ROM. Existing runtime loader already calls this decoder; no second shift or new table layer required. Hardware/relocation/wholebootstrap excluded.','rom_sha256':hashlib.sha256(r).hexdigest(),'decoder_sha256':hashlib.sha256((root/'src/impact.c').read_bytes()).hexdigest()}
(root/'reports/render-math-tables-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
