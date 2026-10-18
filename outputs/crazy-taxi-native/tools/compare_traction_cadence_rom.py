import sys
sys.path.insert(0,"work/python-packages")
"""Bounded original traction/vector helper vs native, BIOS sqrt explicit contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_motion_steering.py').read().split('\ncases=[')[0])
import math
source=source.replace("'bgt':not z and n==v}","'bgt':not z and n==v,'blt':n!=v}").replace("'ldrsh','str'","'ldrsh','ldrsb','str'").replace("else (1 if m in ['ldrb','strb'] else 4)","else (1 if m in ['ldrb','ldrsb','strb'] else 4)").replace("if m=='ldrsh' and value&32768:value-=65536","if m=='ldrsh' and value&32768:value-=65536\n     if m=='ldrsb' and value&128:value-=256")
exec(source);allowed_thumb={}
for filename in ['challenge-bound-traction-cfg.json','challenge-bound-vector-angle-cfg.json']:
 cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/'+filename).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TractionProbe(MotionProbe):
 def write(self,a,v,n=4):
  if a in [0x20025d4,0x200262c]:
   assert n==1;self.arm.mem[a&~3]=(self.arm.read(a&~3)&~255)|(v&255)
  else:super().write(a,v,n)
 def __init__(self,case):
  super().__init__((0,0,0,0,0,1),0);self.reg[14]=0x10020001;self.reward=[0,0,0,0]
  fx,fz,cx,cz,speed,phase,phaseb,flags,gate,latch=case
  for off,v,n in [(0x53c,fx,4),(0x540,fz,4),(0x570,cx,4),(0x574,cz,4),(0x544,speed,4),(0x564,99,4),(0xe6a,phase,1),(0xe6b,phaseb,1),(0x28,flags,1)]:self.write(self.base+off,v,n)
  for a in [0x2001694,0x2001eb0,0x20025d4,0x200262c,0x2002630]:self.arm.mem[a]=0
  self.arm.mem[0x2001694]=gate<<16;self.arm.mem[0x2001eb0]=128;self.arm.mem[0x200262c]=latch
  ctr=pathlib.Path('outputs/crazy-taxi-native/assets/driving.ctr').read_bytes()
  for i in range(4096):
   self.arm.mem[0x202c4f0+i*4]=(int.from_bytes(ctr[4+i*2:6+i*2],'little',signed=True)>>4)&MASK
   self.arm.mem[0x20304f0+i*4]=(int.from_bytes(ctr[8196+i*2:8198+i*2],'little',signed=True)>>4)&MASK
 def external(self,target):
  if target==0x802e00c:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  elif target==0x802901c:self.reward=[1]+self.reg[:3]
  else:super().external(target)
 def outputs(self):return [signed(self.read(self.base+0x564)),signed(self.read(self.base+0x544)),self.read(0x20025d4,1),self.read(0x200262c,1)]+self.reward
root=pathlib.Path('outputs/crazy-taxi-native')
results=[];steps=0
for initial in [45520,91040,136561]:
 for flags in [0,8]:
  speed=initial;values=[]
  for call in range(6):
   case=(0,0,8000,10000,speed,0,0,flags,0,0)
   cp=subprocess.run([str(root/'bin/traction-rom-compare.exe')],input=' '.join(map(str,case))+'\n',text=True,capture_output=True,check=True);row=list(map(int,cp.stdout.split()));assert row[0]==0
   probe=TractionProbe(case);probe.run(entry=0x8014394,stop=0x10020000);got=probe.outputs();assert got==row[1:];steps+=probe.steps;speed=row[2];values.append(speed)
  if flags:assert values[0]>values[3]>values[5]
  else:assert values==[initial]*6
  results.append({'initial_speed':initial,'flags':flags,'native_and_original_speed_after_each_call':values,'one_vs_four_call_difference':values[0]-values[3]})
reference=[]
from collections import Counter
for side in ['left','right']:
 records=json.loads((root/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];updates=[b for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];reference.append({'side':side,'updates':len(updates),'observed_frame_delta_counts':dict(Counter(x['frame_delta'] for x in updates))})
report={'status':'PASS_ORIGINAL_PER_UPDATE_DAMPING_CADENCE_DEPENDENCY','rom_sha256':hashlib.sha256(r).hexdigest(),'thumb_instructions':steps,'results':results,'reference_update_deltas':reference,'scope':'Original14394 traction with explicit force0/capacity/settings128/phase0/flags fixtures, original vector-angle plus sqrt/division helper contracts, native probe exact every call. Only speed is fed between isolated calls; other fixture inputs are fixed and do not claim full vehicle trajectory. Flags8 multiplies speed4075/4096 per call without frame delta, so splitting a source interval across more calls is not numerically equivalent for this branch. Does not establish that flags8 is active at failed bend, that cadence caused delivery failure, or a correct whole-game60fps retiming strategy.'}
(root/'reports/traction-cadence-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
