"""Original34064 ARM531C heading approach on actual common-frame updates."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
exec(open('outputs/crazy-taxi-native/tools/original_angle_difference_probe.py').read())
assert r[0x34064:0x34068]==bytes.fromhex('7847c046') and int.from_bytes(r[0x34068:0x3406c],'little')==0xeaff44ab
import json
MASK=0xffffffff
signed=lambda v:v-0x100000000 if v&0x80000000 else v
steps=0

def original_heading_approach(current,target,step):
 global steps
 regs=[0]*16;regs[:3]=[current&MASK,target&MASK,step&MASK];regs[13]=0x10010000;regs[14]=0x10020000;mem={};pc=0x531c;comparison=0
 for _ in range(32):
  steps+=1;assert 0x531c<=pc<=0x535c and pc%4==0
  w=int.from_bytes(r[pc:pc+4],'little');condition=w>>28
  if not {14:True,10:comparison>=0,11:comparison<0,12:comparison>0}[condition]:pc+=4;continue
  if w==0xe92d4070:
   regs[13]-=16
   for j,k in enumerate([4,5,6,14]):mem[regs[13]+j*4]=regs[k]
  elif w==0xe8bd4070:
   for j,k in enumerate([4,5,6,14]):regs[k]=mem[regs[13]+j*4]
   regs[13]+=16
  elif w==0xe12fff1e:assert regs[14]==0x10020000;return regs[0]
  elif w&0x0e000000==0x0a000000:
   off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;dest=pc+8+off*4
   if w&(1<<24):
    assert dest==0x52b4;regs[14]=pc+4;regs[0]=original_angle_difference(regs[0],regs[1])
   else:pc=dest;continue
  else:
   assert w&0x0c000000==0
   rn=(w>>16)&15;rd=(w>>12)&15;op=(w>>21)&15
   if w&(1<<25):
    v=w&255;n=((w>>8)&15)*2;v=((v>>n)|(v<<(32-n)))&MASK if n else v
   else:assert w&0xff0==0;v=regs[w&15]
   if op==10:comparison=signed(regs[rn])-signed(v)
   elif op==13:regs[rd]=v
   elif op==3:regs[rd]=(v-regs[rn])&MASK
   elif op==2:regs[rd]=(regs[rn]-v)&MASK
   elif op==4:regs[rd]=(regs[rn]+v)&MASK
   else:raise AssertionError((hex(pc),hex(w)))
  pc+=4
 raise AssertionError('approach instruction bound')
root=pathlib.Path('outputs/crazy-taxi-native');lines=(root/'reports/current-traffic-frame-states.txt').read_text().splitlines();cases=0
for before,after in zip(lines[::2],lines[1::2]):
 pre,post=[list(map(int,l.split()[1:])) for l in [before,after]]
 if pre[4]:continue
 for i in range(pre[1]):
  a=pre[50+i*18:50+(i+1)*18];b=post[50+i*18:50+(i+1)*18]
  if a[2]>0 or b[2]>0 or not(a[0]&1) or not(b[0]&1) or a[7] or b[7]:continue
  got=original_heading_approach(a[5],-b[17],pre[2]<<4)
  assert got==b[5]&MASK,dict(frame=pre[0],actor=i,current=a[5],target=-b[17],original=signed(got),native=b[5]);cases+=1
report=dict(cases=cases,approach_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual original34064 veneer/ARM531C body with nestedoriginalARM52B4 arithmetic vs eligible native pre/post commonheading updates across2048frames. Normal active/timer0 noncollision cases only; nested helper boundedcall abstraction, no complete66B4 continuous execution yet.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_live_traffic_common_heading_rom.py',root/'tools/original_angle_difference_probe.py',root/'reports/current-traffic-frame-states.txt',root/'src/traffic_heading.c']}
(root/'reports/live-traffic-common-heading-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
