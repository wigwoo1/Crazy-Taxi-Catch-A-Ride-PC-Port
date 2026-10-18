"""Isolated headless original-ROM reference capture; never part of the native port."""
import ctypes as C,pathlib,struct,json,hashlib,sys,math,zlib
root=pathlib.Path('outputs/crazy-taxi-native').resolve();out=root/'reports/original-core-reference';out.mkdir(exist_ok=True)
corepath=pathlib.Path('./')
source=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba');rom=source.read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
rng_callsite_trace='--rng-callsite-trace' in sys.argv or '--rng-boundary-trace' in sys.argv or '--rng-caller-r4-trace' in sys.argv
combined_callback_dispatch_trace='--combined-callback-dispatch-trace' in sys.argv
traffic_dispatch_counter_trace='--traffic-dispatch-counter-trace' in sys.argv
traffic_dispatch_trace='--traffic-dispatch-entry-trace' in sys.argv or combined_callback_dispatch_trace or traffic_dispatch_counter_trace
voice_reset_trace='--voice-reset-write-trace' in sys.argv
callback_cycle_trace='--callback-cycle-trace' in sys.argv or combined_callback_dispatch_trace
assert (not combined_callback_dispatch_trace or not (rng_callsite_trace or voice_reset_trace)) and (combined_callback_dispatch_trace or sum((rng_callsite_trace,traffic_dispatch_trace,voice_reset_trace,callback_cycle_trace))<=1),'Use one temporary ROM instrumentation mode at a time; callback+dispatcher is available through --combined-callback-dispatch-trace.'
core_rom=rom
instrumentation=None
if rng_callsite_trace:
 # Diagnostic-only in-memory ROM instrumentation. The supplied file is never written.
 # Log LR (the address after each Thumb BL) for every raw call to 0801E72C.
 rng_target=0x0801e72c;stub_address=0x08044c00;log_address=0x0203f000
 callsites=[]
 for offset in range(0,len(rom)-3,2):
  first,second=struct.unpack_from('<HH',rom,offset)
  if first&0xf800==0xf000 and second&0xf800==0xf800:
   relative=((first&0x7ff)<<12)|((second&0x7ff)<<1)
   if relative&0x400000:relative-=0x800000
   if 0x08000000+offset+4+relative==rng_target:callsites.append(0x08000000+offset)
 assert len(callsites)==47,('unexpected random call-site count',len(callsites))
 if '--rng-caller-r4-trace' in sys.argv:
  boundary_sites={0x08006cb0}
  assert boundary_sites.issubset(set(callsites))
  callsites=[site for site in callsites if site in boundary_sites]
 elif '--rng-caller-voice-prompt-trace' in sys.argv:
  boundary_sites={0x0800b19c,0x0800b1b4}
  assert boundary_sites.issubset(set(callsites))
  callsites=[site for site in callsites if site in boundary_sites]
 elif '--rng-boundary-trace' in sys.argv:
  boundary_sites={0x08006b08,0x08006b1e,0x08006dd0,0x08006de6,0x0802302e}
  assert boundary_sites.issubset(set(callsites))
  callsites=[site for site in callsites if site in boundary_sites]
 stub_words=[0xb407,0x4672,0x480a,0x6801,0x1c0b,0x0a0b,0x2b0f,0xd20b,0x1c03,0x3304,0x505a,0x462a,0x3304,0x505a,0x4a05,0x6812,0x3304,0x505a,0x310c,0x6001,0xbc07,0x4b03,0x4718,0x46c0]
 if '--rng-caller-r4-trace' in sys.argv:stub_words[11]=0x4622
 stub=struct.pack('<24HIII',*stub_words,log_address,0x02003b04,0x0801e72d)
 stub_offset=stub_address-0x08000000
 assert len(stub)==60 and rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'instrumentation stub location is not ROM padding'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 for site in callsites:
  relative=stub_address-(site+4)
  assert relative&1==0 and -0x400000<=relative<0x400000,(hex(site),relative)
  struct.pack_into('<HH',patched,site-0x08000000,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 instrumentation=dict(kind='diagnostic_thumb_rng_return_address_r4_and_rng_before_log' if '--rng-caller-r4-trace' in sys.argv else 'diagnostic_thumb_rng_return_address_r5_and_rng_before_log',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_call_count=len(callsites),call_sites=[hex(x) for x in callsites],stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log_address=hex(log_address),ewram_log_capacity=320,scope='Only the temporary libretro content buffer is patched after verifying the supplied ROM hash. The original ROM file and native runtime remain unchanged.')
traffic_dispatch_instrumentation=None;traffic_dispatch_record_bytes=11;traffic_dispatch_capacity=240
if traffic_dispatch_trace and not traffic_dispatch_counter_trace:
 # Log every entry, including possible indirect calls, then replay the four
 # displaced Thumb prologue instructions and resume inside the original code.
 dispatcher=0x080066b4;stub_address=0x08044c00 if combined_callback_dispatch_trace else 0x08044c80
 displaced=bytes.fromhex('f0b557464e464546')
 # Capture the dispatcher input plus slots 8 and 9 before each service call.
 # Compact records avoid touching the higher EWRAM region used by runtime state.
 stub=bytes.fromhex('ffb51c4c2568f02d29d21b4e36682f1cff00281c40003f18281c3f1804373f193e70360a7e70009ebe70144e3068000171684018f87030693871000a7871b069b871000af8710e4e30693872000a78723068000171684018b87201352560ffbc009b9e4601b0f0b557464e464546054b1847000000f0030220350002d403000230040002bd660008')
 stub_offset=stub_address-0x08000000
 assert len(stub)==136 and rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'dispatcher trace stub location is not ROM padding'
 entry_offset=dispatcher-0x08000000
 assert rom[entry_offset:entry_offset+8]==displaced,'traffic dispatcher prologue changed'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HHI',patched,entry_offset,0x4b00,0x4718,stub_address|1)
 core_rom=bytes(patched)
 traffic_dispatch_instrumentation=dict(kind='temporary_thumb_dispatcher_entry_probe_logs_compact_slots_8_9_then_replays_displaced_prologue',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_entry=hex(dispatcher),displaced_bytes=displaced.hex(),stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log_address='0x0203F000',ewram_log_count_capacity=240,record_bytes=11,record_fields=['source_counter_u16','incoming_r0_delta_u8','slot8_flags_type_nibbles','slot8_countdown_s16','slot8_y_u16','slot9_countdown_s16','slot9_flags_type_nibbles'],scope='The entry branch and logger exist only in the in-memory libretro content buffer. Before every dispatcher service it records compact source fields for actors 8 and 9, then restores registers, replays the displaced original Thumb prologue and resumes at 0x080066BC. This diagnostic probe is not used by the native runtime and does not write the supplied ROM.')
if traffic_dispatch_counter_trace:
 # Minimal diagnostic event record: only the source counter and incoming
 # delta are copied; the displaced original dispatcher prologue is replayed.
 dispatcher=0x080066b4;stub_address=0x08044c80
 displaced=bytes.fromhex('f0b557464e464546')
 words=[0xb53c,0x4c0b,0x6825,0x4c0b,0x6823,0x00da,0x1912,0x3204,0x6015,0x6050,0x3301,0x6023,0x9b04,0x469e,0xbc3c,0xb001]
 stub=struct.pack('<16H',*words)+displaced+struct.pack('<4HIII',0x4b03,0xb408,0xbd00,0x46c0,0x02003520,0x0203f000,0x080066bd)
 stub_offset=stub_address-0x08000000;entry_offset=dispatcher-0x08000000
 assert len(stub)==60 and rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'minimal dispatcher stub location is not ROM padding'
 assert rom[entry_offset:entry_offset+8]==displaced,'traffic dispatcher prologue changed'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HHI',patched,entry_offset,0x4b00,0x4718,stub_address|1)
 core_rom=bytes(patched);traffic_dispatch_record_bytes=8;traffic_dispatch_capacity=(0x1000-4)//8
 traffic_dispatch_instrumentation=dict(kind='minimal_thumb_dispatcher_counter_delta_trace_replays_prologue',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_entry=hex(dispatcher),displaced_bytes=displaced.hex(),stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log_address='0x0203F000',record_bytes=8,record_fields=['source_counter_u32','incoming_r0_delta_s32'],ewram_log_count_capacity=traffic_dispatch_capacity,scope='A temporary 60-byte Thumb hook logs only the current source counter and incoming delta, restores r2-r5/LR, replays the original 8-byte dispatcher prologue and branches to 0x080066BC. The capture host drains the diagnostic count after each retro_run. Only the in-memory libretro buffer is patched; the supplied ROM and native runtime are not modified.')
voice_reset_instrumentation=None
if voice_reset_trace:
 # Instrument the observed sentinel store. The original instruction is replayed
 # in the wrapper; the following literal load is reproduced before resuming.
 hook=0x08011446;stub_address=0x08044c80
 displaced=bytes.fromhex('08700e49')
 stub=bytes.fromhex('ffb50d4c2568502d0ed20c4e36682f1cff0004373f193e60009e3e71019e30787871be6001352560ffbc009b9e4601b00870034b1968704700f003022035000284140108')
 stub_offset=stub_address-0x08000000
 assert len(stub)==68 and rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'voice reset trace stub location is not ROM padding'
 hook_offset=hook-0x08000000
 assert rom[hook_offset:hook_offset+4]==displaced,'voice sentinel store bytes changed'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 relative=stub_address-(hook+4)
 assert relative&1==0 and -0x400000<=relative<0x400000
 struct.pack_into('<HH',patched,hook_offset,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 voice_reset_instrumentation=dict(kind='temporary_thumb_voice_sentinel_store_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_instruction=hex(hook),displaced_bytes=displaced.hex(),replayed_store='strb r0,[r1]',replayed_literal_load_address='0x08011484',stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log_address='0x0203F000',ewram_log_capacity=80,record_bytes=12,record_fields=['source_counter_u32','write_value_u8','before_value_u8','target_pointer_u32'],scope='The temporary BL hook logs only dynamic executions of the original byte store. It restores the saved stack/registers, performs the original store, reproduces the displaced literal load, and returns to the original stream. The patch exists only in the in-memory libretro content buffer; the source ROM and native runtime are not written.')
callback_cycle_instrumentation=None
if callback_cycle_trace:
 # Measure a ROM callback with the GBA timer around its indirect call. The
 # stub replays the original BX-through-thunk behavior and is ROM-padding only.
 hook=0x0802c848;stub_address=0x08044d00;log_address=0x0203d000
 stub_path=root/'tools/callback_cycle_probe_stub_v9.bin';stub=stub_path.read_bytes()
 assert len(stub)==192 and hashlib.sha256(stub).hexdigest()=='c5624f8da18713a519fcc1874d1009a44e892f6e9ae73a693a2c44b39050a233','callback cycle stub hash/size mismatch'
 stub_offset=stub_address-0x08000000;hook_offset=hook-0x08000000
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'callback cycle stub location is not ROM padding'
 first,second=struct.unpack_from('<HH',rom,hook_offset)
 relative=((first&0x7ff)<<12)|((second&0x7ff)<<1)
 if relative&0x400000:relative-=0x800000
 old_target=(hook+4+relative)&0xffffffff
 assert (first&0xf800)==0xf000 and (second&0xf800)==0xf800 and old_target==0x0803244c,('callback thunk callsite changed',hex(old_target))
 patched=bytearray(core_rom);patched[stub_offset:stub_offset+len(stub)]=stub
 displacement=stub_address-(hook+4)
 assert displacement&1==0 and -0x400000<=displacement<0x400000
 hi=0xf000|((displacement>>12)&0x7ff);lo=0xf800|((displacement>>1)&0x7ff)
 struct.pack_into('<HH',patched,hook_offset,hi,lo)
 core_rom=bytes(patched)
 callback_cycle_instrumentation=dict(kind='temporary_thumb_callback_timer2_timer3_cycle_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_callsite=hex(hook),original_target=hex(old_target),stub_address=hex(stub_address),stub_sha256=hashlib.sha256(stub).hexdigest(),ewram_log_address=hex(log_address),record_bytes=28,record_fields=['source_counter_u32','callback_pointer_u32','timer2_entry_u16','timer2_return_u16','timer3_timer2_overflows_u16','timer3_control_return_u16','timer2_control_return_u16','timer2_original_registers_u32','timer3_original_registers_u32'],timer2_address='0x04000108',timer3_address='0x0400010C',timer_mode='temporarily uses Timer2 as a divide-by-one cycle counter and Timer3 as its overflow counter; captures and restores both original registers',scope='The temporary in-memory probe measures Timer2/Timer3 around the original indirect callback and restores their saved register state. It rejects cycle bounds if the callback changes the counter controls. It does not modify the supplied ROM file or native runtime.')
if combined_callback_dispatch_trace:
 combined_sha=hashlib.sha256(core_rom).hexdigest()
 traffic_dispatch_instrumentation['instrumented_rom_sha256']=combined_sha
 callback_cycle_instrumentation['instrumented_rom_sha256']=combined_sha
 traffic_dispatch_instrumentation['combined_probe_mode']='dispatcher entry plus callback cycle timer in one in-memory ROM; separate EWRAM log regions'
 callback_cycle_instrumentation['combined_probe_mode']='dispatcher entry plus callback cycle timer in one in-memory ROM; separate EWRAM log regions'
route_follow='--follow-route31' in sys.argv
frames=int(sys.argv[1]) if len(sys.argv)>1 else 900;assert 0<frames<=(16000 if route_follow else 4000)
label=sys.argv[2] if len(sys.argv)>2 else 'boot900'
assert label.isalnum()
route_points=[]
if route_follow:
 assert '--actor-trace' in sys.argv and len(sys.argv)>3,'Route-follow reference capture requires a source save state and actor trace.'
 route_points=json.loads((root/'reports/live-delivery-route31.json').read_text())['points'];assert len(route_points)>1
core=C.CDLL(str(corepath));pixel=0;image_data=None;keys=set();env_calls=set();directory=str(out).encode();callbacks=[];memory_maps={}
def mapped_memory(address,size):
 for base,(pointer,offset,length) in memory_maps.items():
  if pointer and base<=address and address+size<=base+length:return C.string_at(pointer+offset+address-base,size)
 raise AssertionError(("unmapped source capture address",hex(address),size))
class Descriptor(C.Structure):_fields_=[("flags",C.c_uint64),("ptr",C.c_void_p),("offset",C.c_size_t),("start",C.c_size_t),("select",C.c_size_t),("disconnect",C.c_size_t),("length",C.c_size_t),("addrspace",C.c_char_p)]
class Maps(C.Structure):_fields_=[("descriptors",C.POINTER(Descriptor)),("count",C.c_uint)]
ENV=C.CFUNCTYPE(C.c_bool,C.c_uint,C.c_void_p)
VIDEO=C.CFUNCTYPE(None,C.c_void_p,C.c_uint,C.c_uint,C.c_size_t)
AUDIO=C.CFUNCTYPE(None,C.c_int16,C.c_int16);BATCH=C.CFUNCTYPE(C.c_size_t,C.c_void_p,C.c_size_t)
POLL=C.CFUNCTYPE(None);INPUT=C.CFUNCTYPE(C.c_int16,C.c_uint,C.c_uint,C.c_uint,C.c_uint)
def environment(cmd,data):
 global pixel
 cmd&=0xffff;env_calls.add(cmd)
 if cmd in [9,31]:C.cast(data,C.POINTER(C.c_char_p))[0]=directory;return True
 if cmd==10:pixel=C.cast(data,C.POINTER(C.c_int))[0];return pixel in [0,1,2]
 if cmd==17:C.cast(data,C.POINTER(C.c_bool))[0]=False;return True
 if cmd==52:C.cast(data,C.POINTER(C.c_uint))[0]=0;return True
 if cmd==36:
  maps=C.cast(data,C.POINTER(Maps)).contents
  for i in range(maps.count):
   d=maps.descriptors[i];memory_maps[d.start]=(d.ptr,d.offset,d.length)
  return True
 if cmd==15:return False
 if cmd in [11,16,18,36,42]:return True
 return False
def video(data,w,h,pitch):
 global image_data
 if data and data!=C.c_void_p(-1).value:image_data=(C.string_at(data,pitch*h),w,h,pitch,pixel)
for name,typ,fn in [('environment',ENV,environment),('video_refresh',VIDEO,video),('audio_sample',AUDIO,lambda l,r:None),('audio_sample_batch',BATCH,lambda p,n:n),('input_poll',POLL,lambda:None),('input_state',INPUT,lambda port,device,index,button:int(port==0 and button in keys))]:
 callback=typ(fn);callbacks.append(callback);setter=getattr(core,'retro_set_'+name);setter.argtypes=[typ];setter(callback)
class Info(C.Structure):_fields_=[('path',C.c_char_p),('data',C.c_void_p),('size',C.c_size_t),('meta',C.c_char_p)]
core.retro_init();core.retro_load_game.argtypes=[C.POINTER(Info)];core.retro_load_game.restype=C.c_bool
buffer=C.create_string_buffer(core_rom);info=Info(str(out/'reference.gba').encode(),C.cast(buffer,C.c_void_p),len(core_rom),None);assert core.retro_load_game(C.byref(info))
core.retro_serialize_size.restype=C.c_size_t;core.retro_serialize.argtypes=[C.c_void_p,C.c_size_t];core.retro_serialize.restype=C.c_bool
core.retro_unserialize.argtypes=[C.c_void_p,C.c_size_t];core.retro_unserialize.restype=C.c_bool
state_crc_rebase=None
if len(sys.argv)>3:
 prior=bytearray((out/(sys.argv[3]+'.state')).read_bytes())
 if '--rebase-state-crc' in sys.argv:
  old_crc=struct.unpack_from('<I',prior,8)[0];new_crc=zlib.crc32(core_rom)&0xffffffff
  struct.pack_into('<I',prior,8,new_crc);state_crc_rebase=dict(original_saved_state_crc=hex(old_crc),loaded_core_rom_crc=hex(new_crc),source_state_file_sha256=hashlib.sha256((out/(sys.argv[3]+'.state')).read_bytes()).hexdigest(),scope='Only the copied serialized-state ROM CRC word is rebased in memory; the saved state and supplied ROM files are not modified.')
 prior_buffer=C.create_string_buffer(bytes(prior));assert core.retro_unserialize(prior_buffer,len(prior))
if rng_callsite_trace or traffic_dispatch_trace or voice_reset_trace:
 pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
 log=C.string_at(pointer+offset+0x3f000,0x1000);assert not any(log),'refusing to clear nonzero diagnostic buffer region'
 C.memset(pointer+offset+0x3f000,0,0x1000)
if callback_cycle_trace:
 pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
 log=C.string_at(pointer+offset+0x3d000,0x1000);assert not any(log),'refusing to clear nonzero callback-cycle buffer region'
 C.memset(pointer+offset+0x3d000,0,0x1000)
try:
 schedule=[]
 actor_trace=[]
 cpu_trace=[]
 rng_trace_events=[];rng_trace_used=0
 traffic_dispatch_events=[];traffic_dispatch_used=0;traffic_dispatch_ticks=set();traffic_dispatch_log_drains=0
 voice_reset_events=[];voice_reset_used=0
 callback_cycle_events=[];callback_cycle_used=0;callback_cycle_log_drains=0;callback_cycle_max_records_in_batch=0
 route_follow_waypoint=0
 route_trig=[(int(math.sin(a*2*math.pi/4096)*4096),int(math.cos(a*2*math.pi/4096)*4096)) for a in range(4096)]
 def route_follow_input():
  global route_follow_waypoint
  ap,ao,al=memory_maps[0x2000000]
  actor=C.string_at(ap+ao+0x2634,0xea0)
  x,y,z=struct.unpack_from('<iii',actor,0);heading=struct.unpack_from('<i',actor,0x1c)[0];speed=struct.unpack_from('<i',actor,0x544)[0]
  trip_active=C.string_at(ap+ao+0x1694,1)[0]!=0
  if trip_active and route_follow_waypoint==0:route_follow_waypoint=1
  if trip_active:
   while route_follow_waypoint+1<len(route_points):
    target=route_points[route_follow_waypoint];previous=route_points[route_follow_waypoint-1]
    dx=target[0]-x;dz=target[1]-z;px=x-previous[0];pz=z-previous[1];sx=target[0]-previous[0];sz=target[1]-previous[1];length=sx*sx+sz*sz
    reached=dx*dx+dz*dz<1024*1024 or (length>0 and px*sx+pz*sz>=length and (px*sz-pz*sx)**2<=4096*4096*length)
    if not reached:break
    route_follow_waypoint+=1
  target=route_points[route_follow_waypoint];dx=target[0]-x;dz=target[1]-z;distance2=dx*dx+dz*dz;route_lateral=0.0
  if trip_active and route_follow_waypoint:
   previous=route_points[route_follow_waypoint-1];sx=target[0]-previous[0];sz=target[1]-previous[1];length=math.hypot(sx,sz)
   if length>0:
    px=x-previous[0];pz=z-previous[1];along=(px*sx+pz*sz)/length;route_lateral=(px*sz-pz*sx)/length
    lookahead=min(4096.0,1024.0+max(0,speed)/32.0);along=max(0.0,min(length,along+lookahead))
    dx=round(previous[0]+sx*along/length)-x;dz=round(previous[1]+sz*along/length)-z
  alignment=-0x7fffffffffffffff;desired=0
  for angle,(sinv,cosv) in enumerate(route_trig):
   dot=-sinv*dx+cosv*dz
   if dot>alignment:alignment=dot;desired=angle
  current=(heading>>12)&4095;difference=((desired-current+2048)&4095)-2048
  approaching_bend=False
  if trip_active and route_follow_waypoint+1<len(route_points) and distance2<8192*8192:
   nxt=route_points[route_follow_waypoint+1];nx=nxt[0]-target[0];nz=nxt[1]-target[1]
   next_angle=max(range(4096),key=lambda a:-route_trig[a][0]*nx+route_trig[a][1]*nz)
   next_difference=((next_angle-current+2048)&4095)-2048;approaching_bend=abs(next_difference)>256
  brake=speed>40000 and (abs(difference)>256 or approaching_bend or abs(route_lateral)>1024)
  if not trip_active and distance2<1024*1024:brake=True
  keys={0} if brake else {8}
  if not brake:
   if difference>32:keys.add(6)
   elif difference < -32:keys.add(7)
  state=dict(position=[x,y,z],heading=heading,speed=speed,trip_active=int(trip_active),waypoint=route_follow_waypoint,difference=difference,brake=int(brake),distance_squared=distance2)
  return keys,state
 def packed_snapshot(tick):
  ap,ao,al=memory_maps[0x2000000];ip,io,il=memory_maps[0x3000000]
  assert al==0x40000 and il>=0x53f0
  return dict(tick=tick,actor_base='02002634',actor=C.string_at(ap+ao+0x2634,0xea0).hex(),
   input_base='020034DC',input_history=C.string_at(ap+ao+0x34dc,68).hex(),
   command_base='030053C0',time_commands=C.string_at(ip+io+0x53c0,48).hex(),
   shared_owners={f'{0x2000000+offset:08X}':C.string_at(ap+ao+offset,size).hex() for offset,size in ((0x1534,32),(0x164c,192),(0x1770,32),(0x1ea0,16),(0x1eb0,32),(0x25d0,100),(0x7600,4))},
   frame_delta=struct.unpack('<i',C.string_at(ap+ao+0x3b00,4))[0],
   counter=struct.unpack('<I',C.string_at(ap+ao+0x3520,4))[0],
   previous=struct.unpack('<I',C.string_at(ap+ao+0x3524,4))[0])
 def cpu_snapshot(tick):
  n=core.retro_serialize_size();assert n==397888
  state=C.create_string_buffer(n);assert core.retro_serialize(state,n)
  data=state.raw;version,_,crc,master=struct.unpack_from('<4I',data)
  assert version==0x01000009 and data[0x10:0x20]==rom[0xa0:0xb0]
  import zlib
  # Instrumented probes change only the in-memory content. Validate mGBA's
  # serialized CRC against those exact bytes, not the untouched user ROM.
  assert crc==zlib.crc32(core_rom)&0xffffffff
  regs=list(struct.unpack_from('<16I',data,0x20));cpsr,spsr,cycles,next_event=struct.unpack_from('<4I',data,0x60)
  pc=regs[15];window_base=0;window=b''
  if 0x3000010<=pc<0x3007fd0:
   pointer,offset,length=memory_maps[0x3000000];window_base=(pc&~3)-16;assert window_base+64<=0x3000000+length
   window=C.string_at(pointer+offset+window_base-0x3000000,64)
  result=dict(tick=tick,version=version,master_cycles=master,gprs=regs,cpsr=cpsr,spsr=spsr,cycles_since_event=cycles,next_event_cycles=next_event,iwram_window_base=window_base,iwram_window=window.hex())
  if '--column-context-trace' in sys.argv and not cpsr&32 and 0x3000c0c<=pc<0x3000d1c:
   def read_mapped(address,size):
    for base,(pointer,offset,length) in memory_maps.items():
     if pointer and base<=address and address+size<=base+length:return C.string_at(pointer+offset+address-base,size)
    raise AssertionError(('columncontext unmapped',hex(address),size))
   result['column_context']=dict(descriptor_address=regs[10],descriptor=read_mapped(regs[10],32).hex(),bounds=list(struct.unpack('<2I',read_mapped(0x3000558,4)+read_mapped(0x3000560,4))),framebuffer=struct.unpack('<I',read_mapped(0x30016cc,4))[0],stack_parameters=read_mapped(regs[13],20).hex(),column_destination=struct.unpack('<I',read_mapped(regs[13]+0x2c,4))[0])
   first,last=struct.unpack('<2I',read_mapped(regs[13]+0x64,8))
   assert 0<=first<=last<=120,('columnrange',first,last)
   table=0x20344f0
   result['column_context']['outer_range']=[first,last]
   result['column_context']['outer_descriptors']=read_mapped(table+first*32,(last-first)*32).hex()
   result['column_context']['current_descriptor_index']=(regs[10]-table)//32

  row_rom_pc=pc+0x050006f8 if 0x03000000<=pc<0x03006800 else pc
  row_clipped='--clipped-row-context-trace' in sys.argv and 0x08001640<=row_rom_pc<0x08001724
  if not cpsr&32 and (('--row-context-trace' in sys.argv and 0x080018dc<=row_rom_pc<0x08001980) or row_clipped):
   def row_read(address,size):
    for base,(pointer,offset,length) in memory_maps.items():
     if pointer and base<=address and address+size<=base+length:return C.string_at(pointer+offset+address-base,size)
    raise AssertionError(('rowcontext unmapped',hex(address),size))
   sp=regs[13];first,last=struct.unpack('<2I',row_read(sp+0x30,8));assert 0<=first<last<=160,('rowrange',first,last)
   parameters=row_read(sp,20);table=0x020344f0
   result['row_context']=dict(source_rom_pc=row_rom_pc,outer_range=[first,last],stack_parameters=parameters.hex(),row_descriptors=row_read(table+first*32,(last-first)*32).hex(),row_destination=struct.unpack('<I',row_read(sp+0x2c,4))[0],row_table=table)
   if row_clipped:result['row_context']['clipped_bounds']=list(struct.unpack('<2I',row_read(0x030016b8,4)+row_read(0x030016c0,4)))

  if '--perspective-row-context-trace' in sys.argv and not cpsr&32 and 0x08000efc<=row_rom_pc<0x08001038:
   def perspective_read(address,size):
    for base,(pointer,offset,length) in memory_maps.items():
     if pointer and base<=address and address+size<=base+length:return C.string_at(pointer+offset+address-base,size)
    raise AssertionError(('perspectiverow unmapped',hex(address),size))
   sp=regs[13];first,last=struct.unpack('<2I',perspective_read(sp+0x30,8));assert 0<=first<last<=160,('perspectiverowrange',first,last)
   table=0x020344f0
   result['perspective_row_context']=dict(source_rom_pc=row_rom_pc,outer_range=[first,last],stack_parameters=perspective_read(sp,20).hex(),row_descriptors=perspective_read(table+first*32,(last-first)*32).hex(),row_destination=struct.unpack('<I',perspective_read(sp+0x2c,4))[0],row_table=table,clipped_bounds=list(struct.unpack('<2I',perspective_read(0x03000554,4)+perspective_read(0x0300055c,4))))

  if '--flat-row-context-trace' in sys.argv and not cpsr&32 and 0x08001f14<=row_rom_pc<0x08001f88:
   def flat_read(address,size):
    for base,(pointer,offset,length) in memory_maps.items():
     if pointer and base<=address and address+size<=base+length:return C.string_at(pointer+offset+address-base,size)
    raise AssertionError(('flatrow unmapped',hex(address),size))
   sp=regs[13];first,last=struct.unpack('<2I',flat_read(sp+0x30,8));assert 0<=first<last<=160,('flatrowrange',first,last)
   table=0x020344f0
   result['flat_row_context']=dict(source_rom_pc=row_rom_pc,outer_range=[first,last],row_descriptors=flat_read(table+first*32,(last-first)*32).hex(),pattern=struct.unpack('<I',flat_read(sp+0x58,4))[0],row_table=table,current_framebuffer=regs[11],clipped_bounds=list(struct.unpack('<2I',flat_read(0x030016b8,4)+flat_read(0x030016c0,4))))

  if '--frame-service-owner-trace' in sys.argv:
   pointer,offset,length=memory_maps[0x2000000];assert length==0x40000
   result['frame_service_owners']=dict(owners_7540=C.string_at(pointer+offset+0x7540,32).hex(),pending_5f24=C.string_at(pointer+offset+0x5f24,1).hex(),freeze_5f20=C.string_at(pointer+offset+0x5f20,1).hex(),cursor_1e10=C.string_at(pointer+offset+0x1e10,4).hex())
   copy_count=struct.unpack('<H',C.string_at(pointer+offset+0x94ec,2))[0];assert copy_count<=64
   scratch_pointer,scratch_offset,scratch_length=memory_maps[0x3000000];assert scratch_length>=0x5440
   result['frame_service_owners']['scratch_5400']=C.string_at(scratch_pointer+scratch_offset+0x5400,64).hex()
   result['frame_service_owners']['sprite_copy_count']=copy_count
   result['frame_service_owners']['sprite_copy_records']=C.string_at(pointer+offset+0x91ec,copy_count*12).hex()

  if '--bus-timing-trace' in sys.argv:
   pointer,offset,length=memory_maps[0x4000000];assert pointer and length>=0x20a
   def io_half(address):return struct.unpack('<H',C.string_at(pointer+offset+address,2))[0]
   result['bus_timing']=dict(waitcnt=io_half(0x204),display_control=io_half(0),display_status=io_half(4),vcount=io_half(6),interrupt_enable=io_half(0x200),interrupt_flags=io_half(0x202),interrupt_master=io_half(0x208),dma_registers=C.string_at(pointer+offset+0xb0,48).hex(),timer_registers=C.string_at(pointer+offset+0x100,16).hex())

  return result
 if '--cpu-timing-trace' in sys.argv:cpu_trace.append(cpu_snapshot(-1))
 packed_trace=[packed_snapshot(-1)] if '--packed-driving-trace' in sys.argv else []
 packed_checkpoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--packed-driving-checkpoints=')),None)
 packed_checkpoint_ticks=set(map(int,packed_checkpoint_option.split(','))) if packed_checkpoint_option else set()
 traffic_actor_trace='--traffic-actor-trace' in sys.argv
 traffic_player_checkpoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-player-checkpoints=')),None)
 traffic_player_checkpoint_ticks=set()
 if traffic_player_checkpoint_option:
  checkpoint_report=json.loads(pathlib.Path(traffic_player_checkpoint_option).read_text())
  traffic_player_checkpoint_ticks=set(map(int,checkpoint_report['schedule_trace_indices']))
 previous_traffic_actor_raw=None
 if traffic_actor_trace:
  ap,ao,al=memory_maps[0x2000000];assert al==0x40000
  previous_traffic_actor_raw=C.string_at(ap+ao+0xf4,50*0x5c)
 checkpoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--ewram-checkpoints=')),None)
 ewram_checkpoint_ticks=set(map(int,checkpoint_option.split(','))) if checkpoint_option else ({85,90,95} if '--ewram-checkpoints' in sys.argv else set())
 for tick in range(frames):
  route_driver_state=None
  if route_follow:keys,route_driver_state=route_follow_input()
  else:
   keys={3,8} if '--no-input' not in sys.argv and tick>=300 and tick%180<5 else set()
   if '--drive-left' in sys.argv:keys={8} | ({6} if tick>=180 else set())
   if '--drive-right' in sys.argv:keys={8} | ({7} if tick>=180 else set())
  if keys:schedule.append(tick)
  core.retro_run()
  if callback_cycle_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3d000,4))[0]
   assert callback_cycle_used<=used<=(0x1000-4)//28,('invalid callback-cycle trace count',callback_cycle_used,used)
   for event_index in range(callback_cycle_used,used):
    counter,pointer,start_timer,end_timer,overflow_count,timer3_control_return,timer2_control_return,timer2_original_registers,timer3_original_registers=struct.unpack('<IIHHHHH2xII',C.string_at(ap+ao+0x3d004+event_index*28,28))
    callback_cycle_events.append(dict(call_index=len(callback_cycle_events),observed_after_core_tick=tick,source_counter=counter,callback_pointer=hex(pointer),timer2_entry=start_timer,timer2_return=end_timer,timer3_timer2_overflows=overflow_count,timer3_control_return=hex(timer3_control_return),timer2_control_return=hex(timer2_control_return),timer2_original_registers=hex(timer2_original_registers),timer3_original_registers=hex(timer3_original_registers),timer2_signed_elapsed=end_timer-start_timer,estimated_master_cycles=None))
   callback_cycle_max_records_in_batch=max(callback_cycle_max_records_in_batch,used)
   if used:
    C.memset(ap+ao+0x3d000,0,4)
    callback_cycle_log_drains+=1
   callback_cycle_used=0
  if rng_callsite_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3f000,4))[0]
   assert rng_trace_used<=used<=0xf00 and used%12==0,('invalid RNG trace pointer',rng_trace_used,used)
   for log_offset in range(rng_trace_used,used,12):
    return_address=struct.unpack('<I',C.string_at(ap+ao+0x3f004+log_offset,4))[0]
    caller_r5=struct.unpack('<I',C.string_at(ap+ao+0x3f008+log_offset,4))[0]
    rng_before=struct.unpack('<I',C.string_at(ap+ao+0x3f00c+log_offset,4))[0]
    call_site=((return_address&~1)-4)&0xffffffff
    rng_trace_events.append(dict(tick=tick,return_address=hex(return_address),call_site=hex(call_site),caller_r5=hex(caller_r5),rng_before=rng_before))
   rng_trace_used=used
  if traffic_dispatch_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3f000,4))[0]
   if used<traffic_dispatch_used:
    assert traffic_dispatch_used==240 and used==0,('unexpected dispatcher trace reset',traffic_dispatch_used,used)
    traffic_dispatch_used=0
   assert traffic_dispatch_used<=used<=traffic_dispatch_capacity,('invalid dispatcher trace count',traffic_dispatch_used,used)
   for event_index in range(traffic_dispatch_used,used):
    if traffic_dispatch_counter_trace:
     counter,delta=struct.unpack('<Ii',C.string_at(ap+ao+0x3f004+event_index*traffic_dispatch_record_bytes,traffic_dispatch_record_bytes))
     traffic_dispatch_events.append(dict(call_index=len(traffic_dispatch_events),source_counter=counter,frame_delta=delta,observed_after_core_tick=tick))
    else:
     counter,delta,slot8_ft,slot8_countdown,slot8_y,slot9_countdown,slot9_ft=struct.unpack('<HBBhhhB',C.string_at(ap+ao+0x3f004+event_index*traffic_dispatch_record_bytes,traffic_dispatch_record_bytes))
     traffic_dispatch_events.append(dict(call_index=len(traffic_dispatch_events),source_counter=counter,frame_delta=delta,slot8=dict(flags=slot8_ft>>4,type=slot8_ft&15,countdown=slot8_countdown,y=slot8_y),slot9=dict(flags=slot9_ft>>4,type=slot9_ft&15,countdown=slot9_countdown),observed_after_core_tick=tick))
    traffic_dispatch_ticks.add(tick)
   traffic_dispatch_used=used
   if used>0:
    C.memset(ap+ao+0x3f000,0,4)
    traffic_dispatch_used=0
    traffic_dispatch_log_drains+=1
  if voice_reset_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3f000,4))[0]
   assert voice_reset_used<=used<=80,('invalid voice-reset trace count',voice_reset_used,used)
   for event_index in range(voice_reset_used,used):
    counter,write_value,before_value,target=struct.unpack('<IBB2xI',C.string_at(ap+ao+0x3f004+event_index*12,12))
    voice_reset_events.append(dict(call_index=event_index,source_counter=counter,write_value=write_value,before_value=before_value,target=hex(target),observed_after_core_tick=tick))
   voice_reset_used=used
  if tick in ewram_checkpoint_ticks:
   pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
   (out/(label+f'.ewram-vblank-{tick:05d}.bin')).write_bytes(C.string_at(pointer+offset,length))
  if '--cpu-timing-trace' in sys.argv:cpu_trace.append(cpu_snapshot(tick))
  if '--packed-driving-trace' in sys.argv or tick in packed_checkpoint_ticks:packed_trace.append(packed_snapshot(tick))
  if '--actor-trace' in sys.argv:
   ap,ao,al=memory_maps[0x2000000];ip,io,il=memory_maps[0x3000000]
   ab=C.string_at(ap+ao+0x2634,0x780);ib=C.string_at(ip+io+0x53c0,4)
   entry=dict(tick=tick,input_keys=sorted(keys),keyinput=struct.unpack('<H',C.string_at(memory_maps[0x4000000][0]+memory_maps[0x4000000][1]+0x130,2))[0],heading=struct.unpack_from('<i',ab,0x1c)[0],speed=struct.unpack_from('<i',ab,0x544)[0],counter=struct.unpack('<I',C.string_at(ap+ao+0x3520,4))[0],previous=struct.unpack('<I',C.string_at(ap+ao+0x3524,4))[0],frame_delta=struct.unpack('<i',C.string_at(ap+ao+0x3b00,4))[0],delta=struct.unpack('<i',ib)[0],intro=C.string_at(ap+ao+0x1696,1)[0],position=list(struct.unpack_from('<iii',ab,0)),angles=list(struct.unpack_from('<hh',ab,0x18)),orientation=list(struct.unpack_from('<iii',ab,0x6f4)),gravity=struct.unpack_from('<i',ab,0x698)[0],airborne=bool(ab[0x28]&8))
   entry['raw_input_history']=struct.unpack('<H',C.string_at(ap+ao+0x34de,2))[0]
   camera_pointer=struct.unpack_from('<I',C.string_at(ip+io+0x6814,4))[0]
   camera_position=list(struct.unpack('<iii',mapped_memory(camera_pointer,12))) if camera_pointer else None
   entry['traffic_camera']=dict(pointer=hex(camera_pointer),position=camera_position)
   if tick in traffic_player_checkpoint_ticks or tick in traffic_dispatch_ticks:
    entry['traffic_camera']['raw_40']=mapped_memory(camera_pointer,0x40).hex() if camera_pointer else None
    entry['traffic_player_state']=packed_snapshot(tick)
   entry['driver_owners']=dict(flag_1697=C.string_at(ap+ao+0x1697,1)[0],flag_153c=C.string_at(ap+ao+0x153c,1)[0],freeze_2620=C.string_at(ap+ao+0x2620,1)[0],freeze_177b=C.string_at(ap+ao+0x177b,1)[0])
   if traffic_actor_trace:
    assert al==0x40000 and 0xf4+50*0x5c<=al
    actor_raw=C.string_at(ap+ao+0xf4,50*0x5c)
    entry['traffic_actor_records']=dict(address='0x020000F4',stride=0x5c,count=50,raw=actor_raw.hex())
    if previous_traffic_actor_raw is not None and actor_raw!=previous_traffic_actor_raw:
     changed_slots=[i for i in range(50) if actor_raw[i*0x5c:(i+1)*0x5c]!=previous_traffic_actor_raw[i*0x5c:(i+1)*0x5c]]
     entry['traffic_pool_changed_from_previous_vblank']=changed_slots
     entry['traffic_camera']['raw_40']=mapped_memory(camera_pointer,0x40).hex() if camera_pointer else None
     entry['traffic_player_state']=packed_snapshot(tick)
    previous_traffic_actor_raw=actor_raw
    entry['traffic_globals']=dict(cooldown_e4=struct.unpack_from('<h',C.string_at(ap+ao+0xe4,2))[0],near_miss_25d3=C.string_at(ap+ao+0x25d3,1)[0],timer_25d5=C.string_at(ap+ao+0x25d5,1)[0],voice_25d6=C.string_at(ap+ao+0x25d6,1)[0],rng_3b04=struct.unpack_from('<I',C.string_at(ap+ao+0x3b04,4))[0],combo_flag_9aa6=C.string_at(ap+ao+0x9aa6,1)[0],combo_9aa8=struct.unpack_from('<i',C.string_at(ap+ao+0x9aa8,4))[0],combo_9aac=struct.unpack_from('<i',C.string_at(ap+ao+0x9aac,4))[0],selection_counter_4c98=struct.unpack_from('<I',C.string_at(ip+io+0x4c98,4))[0])
    # Preserve the outer-frame gates adjacent to traffic state. These are raw
    # source values for differential work; their exact ownership in the full
    # frame dispatcher is not inferred by this capture helper.
   entry['traffic_frame_context']=dict(freeze_5f20=C.string_at(ap+ao+0x5f20,1)[0],sound_7602=C.string_at(ap+ao+0x7602,1)[0],trip_active_1694=C.string_at(ap+ao+0x1694,1)[0])
   if 138<=tick<=153:
    entry['rng_boundary_audio_context']=dict(ewram_02007500=C.string_at(ap+ao+0x7500,0x140).hex(),ewram_02001e80=C.string_at(ap+ao+0x1e80,0x40).hex())
   if route_driver_state:
    entry['route_driver']=route_driver_state;entry['completed_count']=C.string_at(ap+ao+0x1692,1)[0];entry['fare_whole']=struct.unpack_from('<i',C.string_at(ap+ao+0x1698,4))[0];entry['fare_fraction']=struct.unpack_from('<i',C.string_at(ap+ao+0x169c,4))[0];entry['remaining_clock']=struct.unpack_from('<i',C.string_at(ap+ao+0x16a8,4))[0]
   actor_trace.append(entry)
   if route_follow and entry.get('completed_count',0)>0:break
 if route_follow:frames=len(actor_trace)
 if cpu_trace:(out/(label+'.cpu-timing-trace.json')).write_text(json.dumps(dict(scope='Offline original core saved CPU register/cycle snapshots at VBlank returns; raw savedPC includes CPU pipeline semantics, not instruction-entry tracing. State format guarded by observed magic9/size/title/CRC; installed core release not identified. Never used by native runtime.',format_reference='https://raw.githubusercontent.com/mgba-emu/mgba/master/include/mgba/internal/gba/serialize.h',records=cpu_trace),indent=2))
 if actor_trace:(out/(label+'.actor-trace.json')).write_text(json.dumps(actor_trace,indent=2))
 if packed_trace:(out/(label+'.packed-driving-trace.json')).write_text(json.dumps({'scope':'Offline original VBlank snapshots, including pre-run baseline; not exact routine entry or native execution.','records':packed_trace},indent=2))
 assert image_data
 data,w,h,pitch,fmt=image_data;assert (w,h)==(240,160)
 rows=[]
 for y in range(h-1,-1,-1):
  row=bytearray()
  for x in range(w):
   if fmt==1:b,g,r,_=data[y*pitch+x*4:y*pitch+x*4+4]
   else:
    v=struct.unpack_from('<H',data,y*pitch+x*2)[0]
    if fmt==2:r=((v>>11)&31)*255//31;g=((v>>5)&63)*255//63;b=(v&31)*255//31
    else:r=((v>>10)&31)*255//31;g=((v>>5)&31)*255//31;b=(v&31)*255//31
   row.extend((b,g,r))
  row.extend(b'\0'*((-len(row))%4));rows.append(row)
 pixels=b''.join(rows);header=b'BM'+struct.pack('<IHHI',54+len(pixels),0,0,54)+struct.pack('<IiiHHIIiiII',40,w,h,1,24,0,len(pixels),2835,2835,0,0)
 (out/(label+'.bmp')).write_bytes(header+pixels)
 core.retro_get_memory_data.argtypes=[C.c_uint];core.retro_get_memory_data.restype=C.c_void_p;core.retro_get_memory_size.argtypes=[C.c_uint];core.retro_get_memory_size.restype=C.c_size_t
 address,offset,size=memory_maps[0x2000000];assert size==0x40000 and address;address+=offset
 ram=C.string_at(address,size);(out/(label+'.ewram')).write_bytes(ram)
 if rng_callsite_trace:
  used=struct.unpack_from('<I',ram,0x3f000)[0];assert used<=0xf00 and used%12==0,('invalid RNG log pointer',used)
  returns=[struct.unpack_from('<I',ram,0x3f004+i*12)[0] for i in range(used//12)]
  caller_r5=[struct.unpack_from('<I',ram,0x3f008+i*12)[0] for i in range(used//12)]
  rng_before=[struct.unpack_from('<I',ram,0x3f00c+i*12)[0] for i in range(used//12)]
  sites=[((value&~1)-4)&0xffffffff for value in returns]
  counts={hex(site):sites.count(site) for site in sorted(set(sites))}
  assert len(rng_trace_events)==len(returns)
  (out/(label+'.rng-callers.json')).write_text(json.dumps(dict(instrumentation=instrumentation,logged_count=len(returns),log_saturated=used==0xf00,return_addresses=[hex(x) for x in returns],caller_r5=[hex(x) for x in caller_r5],rng_before=[hex(x) for x in rng_before],call_sites=[hex(x) for x in sites],call_counts=counts,call_events=rng_trace_events,scope='Dynamic calls observed in the instrumented original-ROM execution. Each event includes LR-4 caller, r5 and the shared RNG value before the helper executes, tagged with the VBlank at which the post-frame capture first observed it. Register ownership still requires call-site/dataflow review. This is an offline diagnostic; not used by the native runtime.'),indent=2))
 if traffic_dispatch_trace:
  used=struct.unpack_from('<I',ram,0x3f000)[0];assert used<=traffic_dispatch_capacity and used==traffic_dispatch_used
  (out/(label+'.traffic-dispatch-entry-trace.json')).write_text(json.dumps(dict(instrumentation=traffic_dispatch_instrumentation,logged_count=len(traffic_dispatch_events),final_buffer_count=used,log_drains=traffic_dispatch_log_drains,record_bytes=traffic_dispatch_record_bytes,log_saturated=False,call_events=traffic_dispatch_events,scope='Diagnostic per-entry source counter/delta observations from the supplied ROM. Temporary records are drained after each source frame; the minimal probe remains provisional until complete actor-pool baseline parity is shown.'),indent=2))
 if voice_reset_trace:
  used=struct.unpack_from('<I',ram,0x3f000)[0];assert used<=80 and used==len(voice_reset_events)
  (out/(label+'.voice-reset-writes.json')).write_text(json.dumps(dict(instrumentation=voice_reset_instrumentation,logged_count=used,log_saturated=used==80,call_events=voice_reset_events,scope='Dynamic writes observed at the single hooked original store. A callsite match is evidence for this store only; it does not identify every possible writer or prove audio playback.'),indent=2))
 if callback_cycle_trace:
  used=struct.unpack_from('<I',ram,0x3d000)[0];capacity=(0x1000-4)//28
  assert used==0 and callback_cycle_used==0 and callback_cycle_max_records_in_batch<capacity
  if cpu_trace and cpu_trace[-1].get('bus_timing'):
   controls_ok=all(event['timer2_control_return']=='0x80' and event['timer3_control_return']=='0x84' for event in callback_cycle_events)
   callback_cycle_instrumentation['timer_counter_controls_preserved']=controls_ok
   if controls_ok:
    for event in callback_cycle_events:
     raw=event['timer3_timer2_overflows']*65536+event['timer2_signed_elapsed']
     event['measured_window_lower_master_cycles']=max(0,raw-65536)
     event['measured_window_upper_master_cycles']=raw+65536
     event['estimate_includes_trampoline_overhead']=True
  (out/(label+'.callback-cycle-trace.json')).write_text(json.dumps(dict(instrumentation=callback_cycle_instrumentation,logged_count=len(callback_cycle_events),final_buffer_count=used,log_drains=callback_cycle_log_drains,max_records_per_frame=callback_cycle_max_records_in_batch,log_saturated=False,call_events=callback_cycle_events,scope='Approximate callback-call window counts Timer2 counter cycles and Timer3 overflows. The host drains the temporary EWRAM counter after each retro_run so long captures do not exceed the 146-record buffer; each source frame must remain below that capacity. Bounds allow one Timer2 period for the counter/cascade sampling race; the measured window includes trampoline cycles before the indirect call and after return. Both original timer register pairs are logged and restored. This is an offline diagnostic, not exact routine cycles or native runtime behavior.'),indent=2))
 for base,(pointer,offset,length) in memory_maps.items():
  if base in [0x3000000,0x4000000,0x5000000,0x6000000,0x7000000] and pointer:(out/(label+f'.{base:08x}.bin')).write_bytes(C.string_at(pointer+offset,length))
 n=core.retro_serialize_size();state=C.create_string_buffer(n);assert core.retro_serialize(state,n);(out/(label+'.state')).write_bytes(state.raw)
 report=dict(status='PASS',frames=frames,dimensions=[w,h],pixel_format=fmt,ewram_bytes=size,input_press_ticks=schedule,environment_commands=sorted(env_calls),rom_sha256=hashlib.sha256(rom).hexdigest(),core_sha256=hashlib.sha256(corepath.read_bytes()).hexdigest(),instrumentation=instrumentation,traffic_dispatch_instrumentation=traffic_dispatch_instrumentation,voice_reset_instrumentation=voice_reset_instrumentation,callback_cycle_instrumentation=callback_cycle_instrumentation,state_crc_rebase=state_crc_rebase,scope='Independent mGBA headless original-ROM execution and raw pixel/RAM capture with recorded scripted input; not native-port runtime or completed gameplay proof.')
 (out/(label+'.json')).write_text(json.dumps(report,indent=2));print({'status':report['status'],'frames':frames,'dimensions':report['dimensions'],'pressed_frames':len(schedule),'rom_sha256':report['rom_sha256']})
 if route_follow:
  first_active=next((entry for entry in actor_trace if entry['route_driver']['trip_active']),None)
  last=actor_trace[-1]['route_driver'];hist={}
  for a,b in zip(actor_trace,actor_trace[1:]):
   if a['previous']!=b['previous']:
    delta=(b['previous']-a['previous'])&0xffffffff;hist[str(delta)]=hist.get(str(delta),0)+1
  capture_status='PASS_SOURCE_ROUTE31_DELIVERY_CAPTURE' if first_active and actor_trace[-1]['completed_count']>0 else 'INCOMPLETE_SOURCE_ROUTE31_CAPTURE'
  route_report=dict(status=capture_status,frames=frames,route_source=31,route_points=len(route_points),first_trip_tick=first_active['tick'] if first_active else None,first_trip_counter=first_active['counter'] if first_active else None,final_waypoint=last['waypoint'],trip_active_at_end=last['trip_active'],completed_count=actor_trace[-1]['completed_count'],fare=[actor_trace[-1]['fare_whole'],actor_trace[-1]['fare_fraction']],remaining_clock=actor_trace[-1]['remaining_clock'],source_update_interval_histogram=dict(sorted(hist.items(),key=lambda kv:int(kv[0]))),traffic_actor_trace=traffic_actor_trace,traffic_actor_record_count=50 if traffic_actor_trace else 0,traffic_actor_record_layout='EWRAM+0xF4, stride0x5C, raw records only; route/path dispatch and undocumented bytes remain uninterpreted.' if traffic_actor_trace else None,input_schedule_scope='Per-VBlank input_keys in the paired actor trace are the exact adaptive test-driver schedule; the controller used only captured ROM vehicle pose/state and recovered route31 coordinates. It did not write ROM RAM. This is not human input or a general source cadence model.',scope='Original ROM route-following reference capture from a saved gameplay state, using a scripted adaptive test driver. Only count as a full source-route capture if trip_active starts and completed_count advances; otherwise this remains a pickup/navigation probe.',rom_sha256=report['rom_sha256'],core_sha256=report['core_sha256'],state_sha256=hashlib.sha256((out/(sys.argv[3]+'.state')).read_bytes()).hexdigest(),actor_trace_sha256=hashlib.sha256((out/(label+'.actor-trace.json')).read_bytes()).hexdigest())
  (out/(label+'.route-follow.json')).write_text(json.dumps(route_report,indent=2))
  print({'route_follow':route_report['status'],'first_trip_tick':route_report['first_trip_tick'],'waypoint':route_report['final_waypoint'],'completed_count':route_report['completed_count'],'fare':route_report['fare']})
finally:core.retro_unload_game();core.retro_deinit()
assert source.read_bytes()==rom

