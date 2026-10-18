"""Full original postimpact traffic caller vs captured collision entries."""
exec(open('outputs/crazy-taxi-native/tools/compare_random_range_rom.py').read().split('\ncases=')[0])
import math
heading_rows=[list(map(int,l.split())) for l in (root/'reports/current-traffic-heading-states.txt').read_text().splitlines()]
for px,pz,h,i,s,c in heading_rows:
 assert i==((h&MASK)>>12)&4095
 assert s==struct.unpack_from('<h',r,0x4a0d08+i*2)[0]>>4 and c==struct.unpack_from('<h',r,0x4a2d08+i*2)[0]>>4
sys.path.insert(0,str(root/'tools'))
from extract_world import world as extract_world_bundle
city_world=root/'assets/city1.ctw'
assert city_world.read_bytes()==extract_world_bundle(r,'city1'),'Loaded city1 terrain asset differs from original ROM geometry bundle'
containment_ns={}
containment_source=open('outputs/crazy-taxi-native/tools/compare_bound_containment_rom.py').read().split('root=pathlib.Path')[0]
containment_source=containment_source.replace('def containment(case):','def containment(case,entry_v=False):').replace('n=z=v=False;steps=0','n=z=False;v=entry_v;steps=0')
exec(containment_source,containment_ns)
containment=containment_ns['containment']
for name in ['traffic-postimpact','challenge-bound-obstacle','challenge-bound-movement','traffic-collision','traffic-path-advance','traffic-path-initialize','challenge-bound-rpm']:
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/f'reports/{name}-cfg.json').read_text())['instructions']})
allowed_thumb.update({i.address:bytes(i.bytes) for i in md.disasm(r[0x2958c:0x2959c],0x802958c)})
class FollowProbe(RandomProbe):
 def read(self,a,n=4):
  if 0x10003000<=a<0x10010000 or 0x2000000<=a<0x2040000 or 0x3006760<=a<0x3006810 or a==0x3006940 or a==0x30053c0 or a==0x30053f0 or a==0x3006810:
   assert a&~3 in self.arm.mem,('unmapped fixture',hex(a));return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x10003000<=a<0x10010000 or 0x2000000<=a<0x2040000 or 0x3006760<=a<0x3006810 or a==0x30053f0 or a==0x3006810:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  self.call_counts[target]=self.call_counts.get(target,0)+1
  if target in [0x8007050,0x802e264,0x802e190,0x8013610,0x801e72c,0x802958c,0x802a27c]:
   if target==0x801e72c:self.calls+=1
   if target==0x8007050:self.contacts+=1
   self.run(entry=target,stop=self.reg[14]&~1)
  elif target in [0x8032454,0x803247c,0x8032488]:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  elif target==0x8032520:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK
  elif target==0x8032038:self.reg[0]=math.isqrt(self.reg[0]&MASK)
  elif target==0x80267c8:self.sound_ptr=self.reg[0]
  elif target==0x8029e84:
   got=[signed(self.read(self.reg[0]+off)) for off in [0,4,8]]
   assert self.query_index<len(self.query_rows),('extra query',got)
   q=self.query_rows[self.query_index];assert got==q[:3],('query position',got,q[:3])
   assert q[3]==1,('uncovered terrain kind',q[3])
   self.run(entry=target,stop=self.reg[14]&~1)
   assert signed(self.reg[0])==q[4],('original terrain height',signed(self.reg[0]),q[4])
   self.query_index+=1
  elif target==0x802df0c:
   case=tuple(signed(self.read(ptr+off)) for ptr in self.reg[:4] for off in [0,8]);self.reg[0]=containment(case,self.flags[3])[0]
  else:raise AssertionError(('unexpected call',hex(target)))
rows=[];query_groups=[];pending=[]
for line in (root/'reports/current-traffic-impact-with-queries.txt').read_text().splitlines():
 if line.startswith('Q '):pending.append(list(map(int,line.split()[1:])))
 else:rows.append(list(map(int,line.split())));query_groups.append(pending);pending=[]
assert not pending
powers=[list(map(int,l.split())) for l in (root/'reports/current-traffic-impact-query-power.txt').read_text().splitlines()];powers=[a for a in powers if a[0]==1]
assert len(rows)==len(powers)==len(query_groups)==5
mapping=[(0x4ac,1),(0x544,4),(0x4dc,1)]+[(0x4b8+i*4,4) for i in range(7)]+[(0x4d8,4),(0x5b8,4),(0x5cc,4),(0x75c,4),(0x538,2),(0xe84,4),(0xe6c,4),(0xe70,4),(0xe78,4),(0xe7c,4),(0xe80,4)]
gmap=[(0x20000e4,2),(0x20025d3,1),(0x20025d6,1),(0x2009aa6,1),(0x2009aa8,4),(0x2009aac,4),(0x2003b04,4)]
amap=[(0,4,8),(0x14,4,11),(0x1c,4,12),(0x40,4,16),(0x48,4,17),(0x58,4,15),(0x20,4,13),(0x4c,4,18),(0x54,4,19),(0x18,4,20)]
steps=0;call_counts={};query_count=0
for index,(a,b) in enumerate(zip(rows,powers)):
 p=FollowProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.contacts=0;p.sound_ptr=None;p.query_rows=query_groups[index];p.query_index=0;p.call_counts={};actor=0x10003000;state=actor+0x24;desc=0x10003100;road=0x10004000;base=0x2002634
 for addr in range(base,base+0xea0,4):p.arm.mem[addr]=0
 for addr in range(actor,actor+0x60,4):p.arm.mem[addr]=0
 for off,n,j in amap:p.write(actor+off,a[j],n)
 p.write(desc,a[61]);p.write(desc+16,road)
 for j,v in enumerate(a[62:]):p.write(road+j*4,v)
 for off,v in [(0,desc),(4,a[21]),(8,a[22]),(12,road+a[22]*16),(16,a[23]),(20,a[24]),(24,a[25])]:p.write(state+off,v)
 for (off,n),v in zip(mapping,b[2:23]):p.write(base+off,v,n)
 for (addr,n),v in zip(gmap,a[26:33]):p.write(addr,v,n)
 p.write(base,a[0]);p.write(base+8,a[1]);actual_headings={h[2] for h in heading_rows if h[:2]==a[:2] and h[4:]==a[2:4]};assert len(actual_headings)==1
 heading=actual_headings.pop();p.write(base+0x1c,heading)
 for i in range(4096):
  p.arm.mem[0x202c4f0+i*4]=(struct.unpack_from('<h',r,0x4a0d08+i*2)[0]>>4)&MASK
  p.arm.mem[0x20304f0+i*4]=(struct.unpack_from('<h',r,0x4a2d08+i*2)[0]>>4)&MASK
 for addr,v in [(0x30053c0,a[5]),(0x3006940,0x8032489),(0x2001708,0),(0x2007600,a[6]<<16),(0x2001694,a[7])]:p.arm.mem[addr]=v&MASK
 for addr in range(0x3006760,0x3006810,4):p.arm.mem[addr]=0
 p.arm.mem[0x3006810]=0x87556cc
 p.arm.mem[0x2001eb0]=128 if a[-1] else 0;p.arm.mem[0x2001534]=5<<24
 p.reg[:2]=[actor,a[4]&MASK];p.reg[14]=0x10020001;p.run(entry=0x8006ce8,stop=0x10020000)
 got=[]
 for off,n in mapping:
  v=p.read(base+off,n)
  if n==4:v=signed(v)
  elif off==0x4dc and v&128:v-=256
  elif off==0x538 and v&32768:v-=65536
  got.append(v)
 assert got==b[23:],dict(index=index,power=got,native=b[23:])
 for off,n,j in amap:assert p.read(actor+off,n)==(a[j+25]&((1<<(n*8))-1)),dict(index=index,actor_offset=hex(off),original=p.read(actor+off,n),native=a[j+25])
 path=[signed(p.read(state+4)),p.read(state+8),signed(p.read(state+16)),signed(p.read(state+20)),p.read(state+24,2)]
 assert path==a[46:51],dict(index=index,path=path,native=a[46:51])
 for (addr,n),v in zip(gmap,a[51:58]):assert p.read(addr,n)==v&((1<<(n*8))-1),(index,hex(addr),p.read(addr,n),v)
 assert p.contacts==a[58] and p.calls==a[59],(index,p.contacts,p.calls,a[58:60])
 ptr=None if a[60]<0 else struct.unpack_from('<I',r,0x7ec658+a[60]*4)[0];assert p.sound_ptr==ptr
 assert p.query_index==len(p.query_rows)
 for target,count in p.call_counts.items():call_counts[target]=call_counts.get(target,0)+count
 query_count+=p.query_index
 steps+=p.steps
report=dict(cases=len(rows),city1_asset_exact_rom_bundle=True,obstacle_queries=query_count,original_call_counts={hex(k):v for k,v in sorted(call_counts.items())},pass_match=True,thumb_instructions=steps,scope='Complete original6CE8 postimpact caller incl original7050collision/RPM/RNG/comboreset and2A27Cmovement vs5actualcontacts. Original29E84 terrain selection executes using ROMcity1 descriptor087556CC; querycoordinates and returnedheight matchactualcapture. Original ARM51CC containment arithmetic invoked via bounded callee-save abstraction retaining entryV. Relocated actor/path pointers; actual captured playerheading with complete4096entry ROMderived Q8sine/cosine tables, settings/submode preserveY fixture, signed division/remainder and BIOSsqrt arithmetic contracts, sound submission pointer only. No full frame/bootstrap/trajectory/rejoin/audio proof; city1 descriptor explicitly selected from native startup selection, not original bootstrap.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_live_full_traffic_postimpact_heading_rom.py',root/'reports/current-traffic-impact-with-queries.txt',root/'reports/current-traffic-impact-query-power.txt',root/'bin/scene-viewer.exe',root/'reports/current-traffic-heading-states.txt',city_world,root/'tools/extract_world.py',root/'reports/challenge-bound-obstacle-cfg.json']}
(root/'reports/live-full-traffic-postimpact-heading-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
