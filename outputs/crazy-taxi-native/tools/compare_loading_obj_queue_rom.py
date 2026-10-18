"""Whole original28288 title4bpp group10 versus native ordinary producer."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
cases=[]
for group,frame in [(2,0),(7,0),(7,1),(7,2)]:
 for flags in [0,3,4,7]:
  for bucket in range(3):
   for x,y,total,count,bank,palette in [(72,120,0,0,3,0),(240,160,0,0,3,0),(72,120,127,127,0xffffffff,2)]:
    cases.append((group,frame,x,y,0x12345678,bucket,3,flags,0x400,64,bank,palette,total,count,165,240,160))
rom='Crazy Taxi - Catch a Ride (USA).gba'
binary=root/'bin/loading-obj-queue-rom-compare.exe'
rows=subprocess.run([str(binary),rom],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 group,frame,x,y,z,bucket,priority,flags,mask,tile,bank,palette,total,count,seed,width,height=case
 p=QueueProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray(struct.pack('<III',x,y,z)),0x20081ec:bytearray(1024),0x20085ec:bytearray(1024),0x20089ec:bytearray(1024),0x2008dec:bytearray(1024)}
 p.scalars={0x2007e2c:99,0x2007e2d:165,0x2007e2e:0,0x2007e2f:total,0x2007e32:count,0x2007e33:count,0x2007e34:count,0x2007e38:0x8070e50,0x2007e3c+group*4:0x6010000+(tile<<5),0x2007f54+group*4:bank,0x2001788:mask,0x2007542:width,0x2007544:height}
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2)
 p.reg[:4]=[frame,group,0x10001000,priority];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for i,value in enumerate([bucket,flags,palette]):p.write(p.reg[13]+i*4,value)
 saved=p.reg[4:12].copy();p.run(entry=0x8028288,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 got=[int(p.reg[0]!=0)]+[p.read(address,1) for address in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]];parts=row.split()
 assert got==list(map(int,parts[:7])),(case,got,parts[:7])
 data=b''.join(p.regions[address] for address in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[7]),(case,'queue mismatch')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'original_Thumb_instructions':steps,'scope':'Whole28288 actualordinary070E50 loadinggroup2 frame0 shape64x32 andgroup7 frames0/1/2 shape32x32. Exact4096queue/depthbytes/counters/dimensions for threebuckets/flip-semi flags/priority3/objectmask400/groupbase/explicitpalette andoffscreen/count127. No deviceOAM/raster/fullloading/frameaudio proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [binary,root/'src/title_obj_queue.c',root/'tests/loading_obj_queue_rom_compare.c',root/'src/startup_obj_queue.c',Path(__file__)]}}
(root/'reports/loading-obj-queue-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'title4bpp requests',steps,'Thumb')
