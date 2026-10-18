import sys
"""Full28658 effect group8 square16 4bpp with resolved trig tables and actual ARM029FC."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_scaled_rom.py').read().split('\nassets=')[0])
effect_group=int(sys.argv[1]) if len(sys.argv)>1 else 8
assert effect_group in (8,9,10)
effect_frames={8:4,9:3,10:9}[effect_group]

def matrix(sine,cosine,scale):
 scale=(scale&65535);scale=scale-65536 if scale&32768 else scale
 return [(trunc(signed((v<<8)&0xffffffff),scale) if scale else signed((v<<8)&0xffffffff))&65535 for v in [cosine,sine,-sine,cosine]]
cases=[(f,106,102,3537,b,0,0,0,600,0,0,165,240,160,scale,state,angle,sine,cosine) for f in range(effect_frames) for b in [0,1,2] for scale in [0,109,256,512,65535] for state in [0,2,3,4] for angle,sine,cosine in [(0,0,256),(1024,256,0),(2048,0,-256),(65535,-1,256)]]
for x,y in [(-32,102),(-31,102),(272,102),(271,102),(106,-32),(106,-31),(106,192),(106,191)]:
 for angle,sine,cosine in [(0,0,256),(1024,256,0)]:
  cases.append((0,x&0xffffffff,y&0xffffffff,3537,2,255,7,0x400,600,127,127,65535,240,160,109,0,angle,sine,cosine))
for total in [128,255]:cases.append((1,133,102,3537,2,0,0,0,600,total,0,165,240,160,109,0,0,0,256))
rows=subprocess.run([str(root/'bin/effect-affine-queue-rom-compare.exe'),str(effect_group)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 frame,x,y,z,bucket,priority,flags,mask,tile,total,count,seed,width,height,scale,state,angle,sine,cosine=case;p=QueueProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray(struct.pack('<III',x,y,z)),0x20081ec:bytearray(1024),0x20085ec:bytearray(1024),0x20089ec:bytearray(1024),0x2008dec:bytearray(1024),0x200806c:bytearray(384)}
 p.scalars={0x2007e31:0,0x3006940:0x80029fc,0x20304f0+(angle&4095)*4:cosine,0x202c4f0+(angle&4095)*4:sine,0x2007e2c:99,0x2007e2d:165,0x2007e2e:0,0x2007e2f:total,0x2007e32:count,0x2007e33:count,0x2007e34:count,0x2007e38:0x8070e50,(0x2007e3c+effect_group*4):(0x6010000+(tile<<5))&0xffffffff,(0x2007f54+effect_group*4):4,0x2001788:mask,0x2007542:width,0x2007544:height}
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2)
 for i in range(32):
  p.write(0x200806c+i*12,1 if state>=3 else state)
  for j in range(4):p.write(0x200806c+i*12+4+j*2,seed+i*37+j*53,2)
 if state>=3:
  if state==3:p.write(0x200806c,0)
  for j,v in enumerate(matrix(sine,cosine,scale)):p.write(0x200806c+31*12+4+j*2,v,2)
 p.reg[:4]=[frame,effect_group,0x10001000,priority];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for i,value in enumerate([bucket,flags,0,scale,angle]):p.write(p.reg[13]+i*4,value)
 saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x8028658,stop=0x10020000)
 assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)]
 parts=row.split();got=[int(p.reg[0]!=0)]+[p.read(address,1) for address in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]]
 assert got==list(map(int,parts[:7])),(case,got,parts[:7]);data=b''.join(p.regions[address] for address in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[7]),(case,'queue data mismatch')
 if p.reg[0]:assert p.reg[0]==[0x20081ec,0x20085ec,0x20089ec][bucket]+count*8
 assert p.read(0x2007e31,1)==int(parts[8]);assert p.regions[0x200806c]==bytes.fromhex(parts[9]);steps+=p.steps

report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'ARM_instructions':division_namespace['steps'],'effect_group':effect_group,'effect_frames':effect_frames,'scope':f'Actual whole28658 plus ARM029FC, normal070E50group{effect_group} all{effect_frames}frames 4bpp square16 and group10 frame4 tall16x32, explicit resolved trig table values for four angles; exact all4096 queue/depth bytes/counters/dimensions and384 matrix bytes. Both clipped/emitting, signed scale0/-1,256 delegation, matrixempty/full/reuse-before-free, negative-total gate. Palettebank4/groupbase tile600 fixture; not upstream20A20/live shared queue or full renderer proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/taxi_affine_queue.c',root/'src/taxi_affine_queue.h',root/'src/startup_obj_queue.c',root/'src/title_obj_queue.c',root/'src/startup_obj.c',root/'src/obj_order.c',root/'tests/effect-affine-queue-rom-compare.c',root/'bin/effect-affine-queue-rom-compare.exe']}}
(root/f'reports/effect-affine-queue-group{effect_group}-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps,division_namespace['steps'])
