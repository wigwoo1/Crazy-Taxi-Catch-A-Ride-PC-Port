"""Compare one actual retained ROM 080126DC motion return to taxi_motion_prepare."""
import hashlib,json,pathlib,subprocess,sys,tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
ROM_SHA='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
SIZES={'0x2002634':0xe88,'0x2001534':0x980,'0x20025d4':4,'0x200262c':8,'0x30053c0':4,'0x30053d0':16,'0x202c4f0':16384,'0x20304f0':16384}
ORDER=['0x2002634','0x2001534','0x30053c0','0x30053d0','0x20025d4','0x200262c','0x202c4f0','0x20304f0']
def verify(audit_path,rom_path):
 raw=audit_path.read_bytes();a=json.loads(raw)
 if a.get('status')!='ORIGINAL_VEHICLE_RETURN_ABI_PASS':raise ValueError('A completed actual original 080126DC return is required')
 before,after=a['before'],a['after']
 if len(before)!=16 or len(after)!=16 or before[0]!=0x02002634 or before[13]!=after[13] or before[4:12]!=after[4:12]:raise ValueError('Invalid original vehicle calling convention')
 if not isinstance(a.get('instructions'),int) or a['instructions']<=0:raise ValueError('Missing original instruction count')
 states=[]
 for key in ('state_before','state_after'):
  if not set(SIZES).issubset(a[key]):raise ValueError('Incomplete captured RAM regions')
  state={k:bytes.fromhex(a[key][k]) for k in SIZES}
  if any(len(state[k])!=n for k,n in SIZES.items()):raise ValueError('Incorrect capture size')
  for alias,base,offset,size in [('0x2001694','0x2001534',0x1694-0x1534,4),('0x2001eb0','0x2001534',0x1eb0-0x1534,4)]:
   if alias in state and state[alias]!=state[base][offset:offset+size]:raise ValueError('Overlapping RAM capture is inconsistent: '+alias)
  states.append(state)
 initial,original=states
 for k in ('0x30053c0','0x30053d0','0x202c4f0','0x20304f0'):
  if initial[k]!=original[k]:raise ValueError('Original vehicle call unexpectedly changed input region '+k)
 rom=rom_path.read_bytes()
 if len(rom)!=8388608 or hashlib.sha256(rom).hexdigest()!=ROM_SHA:raise ValueError('Supplied ROM identity mismatch')
 angle=rom[0x7f09ee:0x7f0bf0]
 if len(angle)!=514:raise ValueError('Original angle table is incomplete')
 payload=b''.join(initial[k] for k in ORDER)+angle
 binary=ROOT/'bin/retained-motion-compare.exe'
 with tempfile.TemporaryDirectory(prefix='taxi-retained-vehicle-') as tmp:
  src=pathlib.Path(tmp)/'before.bin';dst=pathlib.Path(tmp)/'native.bin';src.write_bytes(payload)
  run=subprocess.run([str(binary),str(src),str(dst)],check=True,capture_output=True,text=True)
  native=dst.read_bytes()
 expected=b''.join(original[k] for k in ORDER)+angle
 diff=[i for i in range(min(len(native),len(expected))) if native[i]!=expected[i]]
 def fields(blob):
  def i32(off):return int.from_bytes(blob[off:off+4],'little',signed=True)
  return {'position_xyz':[i32(x) for x in (0,4,8)],'displacement_xyz':[i32(x) for x in (12,16,20)],'heading_1c':i32(0x1c),'heading_20':i32(0x20),'flags_28':blob[0x28],'angle_578':int.from_bytes(blob[0x578:0x57a],'little',signed=True),'response_57c':int.from_bytes(blob[0x57c:0x57e],'little',signed=True),'drift_69c':i32(0x69c),'speed_544':i32(0x544),'timer_2a':int.from_bytes(blob[0x2a:0x2c],'little',signed=True)} 
 report={'status':'NATIVE_RETAINED_MOTION_STATE_PASS' if not diff and len(native)==len(expected) else 'NATIVE_RETAINED_MOTION_STATE_MISMATCH',
  'audit_sha256':hashlib.sha256(raw).hexdigest(),'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),
  'rom_sha256':ROM_SHA,'instructions':a['instructions'],'native_bytes':len(native),'expected_bytes':len(expected),
  'actor_bytes_compared':0xe88,'global_context_bytes_compared':0x980,'traction_context_bytes_compared':12,
  'native_effects':run.stdout.strip(),'actor_before':fields(initial['0x2002634']),'actor_original_after':fields(original['0x2002634']),'actor_native_after':fields(native[:0xe88]),'scope':'One actual retained original 080126DC motion call only. Complete actor, orientation context, command/input timing/trig and original ROM angle table compared. Excludes outer 11A3C helper at 0802DD74, acceleration before this call, scene/world/collision, full multi-frame driving and gameplay.'}
 if diff or len(native)!=len(expected):
  report['different_byte_count']=len(diff);report['first_differences']=[{'payload_offset':i,'original':expected[i],'native':native[i]} for i in diff[:64]]
  (ROOT/'reports/native-retained-motion-mismatch.json').write_text(json.dumps(report,indent=2)+'\n')
  raise ValueError('Native 080126DC motion differs from actual ROM capture; see mismatch report')
 (ROOT/'reports/native-retained-motion-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
 print(report['status'])
if __name__=='__main__':
 if len(sys.argv)!=3:raise SystemExit('usage: verify_retained_vehicle_native.py AUDIT.json ORIGINAL.gba')
 verify(pathlib.Path(sys.argv[1]),pathlib.Path(sys.argv[2]))

