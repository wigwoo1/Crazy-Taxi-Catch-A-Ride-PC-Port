"""Persistent original66B4 traffic actor/global history on actual native frame inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_live_full_traffic_postimpact_all_rom.py').read().split('\nrows=[];powers=[];query_groups=[];pending=[]')[0])
heading_ns={};exec(open('outputs/crazy-taxi-native/tools/compare_live_traffic_common_heading_rom.py').read().split('\nroot=pathlib.Path')[0],heading_ns)
for name in ['traffic-dispatcher','traffic-spawn','traffic-follow','effect-height','traffic-pitch-transform','traffic-yaw-transform']:
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/f'reports/{name}-cfg.json').read_text())['instructions']})
class FrameProbe(FollowProbe):
 def read(self,a,n=4):
  if 0x3000000<=a<0x3008000:
   assert a&~3 in self.arm.mem,('missing frame global',hex(a));return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x3000000<=a<0x3008000:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x8006908,0x8006bc0,0x8006ce8,0x8029b44,0x8029e84,0x802dbc8,0x802dc20]:
   self.call_counts[target]=self.call_counts.get(target,0)+1;self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8034064:
   self.call_counts[target]=self.call_counts.get(target,0)+1;self.reg[0]=heading_ns['original_heading_approach'](*self.reg[:3])
  elif target==0x803201c:self.graphics_states.append(self.reg[0])
  elif target==0x8031a50:self.submissions+=1
  else:super().external(target)
producer_ns={'__name__':'original_producers'};exec((root/'tools/original_external_shared_producers.py').read_text(),producer_ns)
transitions={t['frame']:t for t in json.loads((root/'reports/external-traffic-rng-sites-audit.json').read_text())['transitions']}
f=root/'reports/current-shared-voice-traffic-frames.txt';lines=f.read_text().splitlines();frames=[tuple(list(map(int,l.split()[1:])) for l in lines[i:i+2]) for i in range(0,len(lines),2)]
p=FrameProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.contacts=0;p.sound_ptr=None;p.call_counts={};p.submissions=0;p.graphics_states=[];base=0x20000f4;player=0x2002634;camera=0x10003000
world=0x87556cc;descriptors=struct.unpack_from('<I',r,world-0x8000000+0x34)[0]
amap=[(0,4,0),(4,4,1),(0x10,4,2),(0x14,4,3),(0x1c,4,4),(0x20,4,5),(0x58,4,7),(0x40,4,8),(0x48,4,9),(0x4c,4,10),(0x54,4,11),(0x18,4,12),(0x28,4,13),(0x2c,4,14),(0x34,4,15),(0x38,4,16),(0x3c,2,17)]
pmap=[(0x4ac,1),(0x544,4),(0x4dc,1)]+[(0x4b8+i*4,4) for i in range(7)]+[(0x4d8,4),(0x5b8,4),(0x5cc,4),(0x75c,4),(0x538,2),(0xe84,4),(0xe6c,4),(0xe70,4),(0xe78,4),(0xe7c,4),(0xe80,4)]
gmap=[(0x20000e4,2),(0x20025d3,1),(0x20025d6,1),(0x2009aa6,1),(0x2009aa8,4),(0x2009aac,4),(0x2003b04,4),(0x20025d5,1),(0x3004c98,4)]
for i in range(frames[0][0][1]):
 actor=base+i*0x5c;a=frames[0][0][50+i*18:50+(i+1)*18]
 for off in range(0,0x5c,4):p.write(actor+off,0)
 for off,n,j in amap:p.write(actor+off,a[j],n)
 desc=descriptors+a[6]*20;points=struct.unpack_from('<I',r,desc-0x8000000+16)[0];p.write(actor+0x24,desc);p.write(actor+0x30,points+a[14]*16)
for addr in range(player,player+0xea0,4):p.write(addr,0)
for i in range(4096):
 p.arm.mem[0x202c4f0+i*4]=(struct.unpack_from('<h',r,0x4a0d08+i*2)[0]>>4)&MASK;p.arm.mem[0x20304f0+i*4]=(struct.unpack_from('<h',r,0x4a2d08+i*2)[0]>>4)&MASK
for addr in range(0x3006760,0x3006810,4):p.arm.mem[addr]=0
for i,v in enumerate([0x8e800,0xa0600,0x6b8cc]):p.write(0x20000e8+i*4,v)
p.arm.mem[0x3006810]=world;p.arm.mem[0x3006814]=camera;p.arm.mem[0x3006940]=0x8032489;p.write(0x2001708,0);p.write(0x2001eb0,0x44<<24);p.write(0x2001534,0)
steps=carry=external_rng=external_combo=0;previous=None
for index,(pre,post) in enumerate(frames):
 for i in range(pre[1]):
  actor=base+i*0x5c;a=pre[50+i*18:50+(i+1)*18]
  for off,n,j in amap:assert p.read(actor+off,n)==a[j]&((1<<(n*8))-1),('pre actor',index,i,hex(off),p.read(actor+off,n),a[j]);carry+=1
  assert p.read(actor+0x24)==descriptors+a[6]*20
 if previous is not None and pre[26]!=previous[26]:
  assert index in transitions
  computed,computed_voice=producer_ns['advance_transition'](transitions[index],p.read(0x2003b04),p.read(0x20025d6,1))
  assert computed==pre[26]
  p.write(0x2003b04,computed)
  p.write(0x20025d6,computed_voice,1)
  external_rng+=1
 for j,((addr,n),v) in enumerate(zip(gmap,pre[20:29])):
  if previous is None:p.write(addr,v,n)
  elif j==3 and v!=previous[20+j]:
   p.write(addr,v,n);external_combo+=1
  else:assert p.read(addr,n)==v&((1<<(n*8))-1),('pre global',index,hex(addr),p.read(addr,n),v)
 for (off,n),v in zip(pmap,pre[29:50]):p.write(player+off,v,n)
 for off,v in [(0,pre[8]),(4,pre[9]),(8,pre[10]),(0x1c,pre[11])]:p.write(player+off,v)
 for off,v in [(0,pre[12]),(4,pre[13]),(8,pre[14]),(12,pre[16]),(16,pre[15])]:p.write(camera+off,v)
 p.write(0x2005f20,pre[4],1);p.write(0x2007602,pre[5],1);p.write(0x2001694,pre[6],1);p.write(0x30053c0,pre[3]);p.steps=0;p.reg[0]=pre[2]&MASK;p.reg[14]=0x10020001
 p.run(entry=0x80066b4,stop=0x10020000)
 for i in range(post[1]):
  actor=base+i*0x5c;a=post[50+i*18:50+(i+1)*18]
  for off,n,j in amap:assert p.read(actor+off,n)==a[j]&((1<<(n*8))-1),('post actor',index,i,hex(off),p.read(actor+off,n),a[j])
  assert p.read(actor+0x24)==descriptors+a[6]*20
 for (addr,n),v in zip(gmap,post[20:29]):assert p.read(addr,n)==v&((1<<(n*8))-1),('post global',index,hex(addr),p.read(addr,n),v)
 for (off,n),v in zip(pmap,post[29:50]):assert p.read(player+off,n)==v&((1<<(n*8))-1),('post power',index,hex(off),p.read(player+off,n),v)
 assert p.graphics_states[-2:]==[4608,0];steps+=p.steps;previous=post
 if index%256==255:print('Verified frames',index+1,flush=True)
report=dict(frames=len(frames),pass_match=True,thumb_instructions=steps,actor_prechecks=carry,external_rng_transitions=external_rng,external_combo_transitions=external_combo,graphics_submission_calls=p.submissions,original_call_counts={hex(k):v for k,v in sorted(p.call_counts.items())},rom_sha256=hashlib.sha256(r).hexdigest(),captured_rng_output_injections=0,captured_voice_output_injections=0,original_external_producers=producer_ns['executed'],scope='Persistent original66B4 actors,sharedRNG andsharedvoice020025D6. External RNG changes computed by actual original idle/pickup/CB84/worldvoice consumers in captured native order with explicit captured consumer context fixtures, no captured RNG orvoiceoutput substitution. Actualcurrentcanonical voice fixture checked beforeeachoriginalconsumer andcomputedoriginalvoice carriedbacktosharedtrafficbyte. Customer/physics/full outerframe scheduling/bootstrap not independently retained; consumer scratch memory separate and reset per event. Externalplayer/camera replay, signeddivision/BIOSroot/containment contracts; graphics/audio boundaries not executed. Not complete independent game trajectory.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_persistent_traffic_shared_rom.py',root/'tools/original_external_shared_producers.py',root/'reports/external-traffic-rng-sites-audit.json',f,city_world,root/'bin/scene-viewer.exe']}
(root/'reports/persistent-traffic-shared-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
