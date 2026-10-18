"""Replay the one saved 080126DC call in the bounded ROM probe; no frame-loop restart."""
import hashlib,json,os,pathlib,sys
HERE=pathlib.Path(__file__).resolve().parents[1]
WORK=HERE.parent.parent
os.chdir(WORK)
source=(HERE/'tools/compare_vehicle_rom.py').read_text().split('\ncases=[')[0]
ns={}
exec(source,ns)
VehicleProbe=ns['VehicleProbe']
MASK=ns['MASK']
SIZES={'0x2002634':0xe88,'0x30053d0':16,'0x30053c0':4,'0x2001534':0x980,'0x2001694':4,'0x2001eb0':4,'0x20025d4':4,'0x200262c':8,'0x202c4f0':16384,'0x20304f0':16384}
ADDR={k:int(k,16) for k in SIZES}
def load_regions(probe,states):
 for key,size in SIZES.items():
  blob=states[key];assert len(blob)==size,(key,len(blob),size)
  base=ADDR[key]
  for offset in range(0,size,4):
   chunk=blob[offset:offset+4]
   probe.arm.mem[base+offset]=int.from_bytes(chunk,'little')
def snapshot(probe):
 out={}
 for key,size in SIZES.items():
  base=ADDR[key]
  out[key]=bytes((probe.read(base+i,1) for i in range(size)))
 return out
class RetainedProbe(VehicleProbe):
 def write(self,address,value,n=4):
  assert n in (1,2,4) and address%4+n<=4,(hex(address),n)
  old=self.arm.read(address&~3);shift=8*(address&3);mask=((1<<(8*n))-1)<<shift
  self.arm.mem[address&~3]=((old&~mask)|((value<<shift)&mask))&MASK

def replay(audit_path):
 raw=audit_path.read_bytes();audit=json.loads(raw)
 if audit.get('status')!='ORIGINAL_VEHICLE_RETURN_ABI_PASS':raise ValueError('Completed original movement call required')
 before,after=audit['before'],audit['after']
 if len(before)!=16 or len(after)!=16 or before[0]!=0x02002634:raise ValueError('Invalid captured ARM register state')
 state={}
 for which in ('state_before','state_after'):
  if not set(SIZES).issubset(audit[which]):raise ValueError('Missing captured region '+which)
  state[which]={k:bytes.fromhex(audit[which][k]) for k in SIZES}
 speed=int.from_bytes(state['state_before']['0x2002634'][0x544:0x548],'little',signed=True)
 actor=state['state_before']['0x2002634']
 angle=int.from_bytes(actor[0x578:0x57a],'little',signed=True)
 pitch=int.from_bytes(actor[0x18:0x1a],'little',signed=True)
 roll=int.from_bytes(actor[0x1a:0x1c],'little',signed=True)
 heading=int.from_bytes(actor[0x1c:0x20],'little',signed=True)//4096
 delta=int.from_bytes(state['state_before']['0x30053c0'],'little',signed=True)
 drift=int.from_bytes(actor[0x69c:0x6a0],'little',signed=True)
 timer=int.from_bytes(actor[0x2a:0x2c],'little',signed=True)
 status=int.from_bytes(actor[0x564:0x568],'little',signed=True)
 flags=actor[0x28]
 case=(speed,angle,pitch,heading,roll,delta,drift,timer,status,flags)
 probe=RetainedProbe(case)
 probe.base=0x02002634
 load_regions(probe,state['state_before'])
 probe.reg=list(before)
 probe.reg[15]=0x080126dc
 try:
  probe.run(entry=0x080126dc,stop=before[14]&~1)
 except Exception as exc:
  result={'status':'ORIGINAL_080126DC_REPLAY_DEPENDENCY','error':repr(exc),'registers_before':before,'registers_after':probe.reg,'thumb_instructions':probe.steps,'scope':'Focused single-call replay from the captured RAM/register state; no full frame loop.'}
  (HERE/'reports/native-retained-motion-replay.json').write_text(json.dumps(result,indent=2)+'\n')
  raise
 got=snapshot(probe);diff=[]
 for key in SIZES:
  expected=state['state_after'][key]
  diff.extend({'region':key,'offset':i,'before':state['state_before'][key][i],'rom_capture':expected[i],'replay':got[key][i]} for i in range(SIZES[key]) if expected[i]!=got[key][i])
 regs_match=probe.reg==after
 result={'status':'ORIGINAL_080126DC_REPLAY_PASS' if not diff and regs_match else 'ORIGINAL_080126DC_REPLAY_MISMATCH','audit_sha256':hashlib.sha256(raw).hexdigest(),'thumb_instructions':probe.steps,'rom_capture_regions':sum(SIZES.values()),'registers_match':regs_match,'different_byte_count':len(diff),'first_differences':diff[:80],'scope':'Replayed only the captured original 080126DC routine from saved actual pre-call registers/RAM and compared captured post-call regions/registers. This verifies the retained capture/harness path, not native parity or full gameplay.'}
 (HERE/'reports/native-retained-motion-replay.json').write_text(json.dumps(result,indent=2)+'\n')
 print(result['status'], 'instructions',probe.steps,'different bytes',len(diff),'registers',regs_match)
 if diff or not regs_match:raise ValueError('Focused original ROM replay differs from retained capture; inspect report')
if __name__=='__main__':
 if len(sys.argv)!=2:raise SystemExit('usage: replay_retained_motion_original.py AUDIT.json')
 replay(pathlib.Path(sys.argv[1]))
