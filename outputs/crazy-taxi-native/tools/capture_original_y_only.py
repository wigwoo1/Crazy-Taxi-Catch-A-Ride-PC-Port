"""Isolated headless original-ROM reference capture; never part of the native port."""
import ctypes as C,pathlib,struct,json,hashlib,sys,math,zlib,gzip,csv
root=pathlib.Path('./');out=root/'reports/original-core-reference';out.mkdir(exist_ok=True)
core_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--reference-core=')),None)
core_sha_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--reference-core-sha256=')),None)
assert core_option and core_sha_option,'Y-only reference requires explicit core path and expected SHA256'
corepath=pathlib.Path(core_option)
assert corepath.is_absolute() and corepath.drive.upper()=='Y:','Reference core must be on Y:'
assert hashlib.sha256(corepath.read_bytes()).hexdigest()==core_sha_option,'Reference core identity mismatch'
source=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba');rom=source.read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert len(sys.argv)>3 and all(pathlib.Path(sys.argv[k]).name==sys.argv[k] and ':' not in sys.argv[k] and '/' not in sys.argv[k] and chr(92) not in sys.argv[k] for k in [2,3]),'Capture/state labels must be plain file names'
rng_callsite_trace='--rng-callsite-trace' in sys.argv or '--rng-callsite-only=' in ' '.join(sys.argv) or '--rng-boundary-trace' in sys.argv or '--rng-caller-r4-trace' in sys.argv
combined_callback_dispatch_trace='--combined-callback-dispatch-trace' in sys.argv
traffic_spawn_return_trace='--traffic-spawn-return-trace' in sys.argv
traffic_spawn_return_slot_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-spawn-return-slot=')),None)
traffic_spawn_return_slot=int(traffic_spawn_return_slot_option) if traffic_spawn_return_slot_option else 10
assert 0<=traffic_spawn_return_slot<50,('invalid spawn-return target slot',traffic_spawn_return_slot)
traffic_slot_y_write_trace='--traffic-slot-y-write-trace' in sys.argv
traffic_slot_y_write_slot_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-slot-y-write-slot=')),None)
traffic_slot_y_write_slot=int(traffic_slot_y_write_slot_option) if traffic_slot_y_write_slot_option else 10
assert 0<=traffic_slot_y_write_slot<50,('invalid Y-write target slot',traffic_slot_y_write_slot)
traffic_dispatch_entry_state_trace='--traffic-dispatch-entry-state-trace' in sys.argv
traffic_postimpact_counter_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-postimpact-entry-counter=')),None)
traffic_postimpact_entry_counter=int(traffic_postimpact_counter_option) if traffic_postimpact_counter_option else 4376
traffic_postimpact_slot_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-postimpact-entry-slot=')),None)
traffic_postimpact_entry_slot=int(traffic_postimpact_slot_option) if traffic_postimpact_slot_option else 0
assert 0<=traffic_postimpact_entry_slot<50,('invalid postimpact target slot',traffic_postimpact_entry_slot)
traffic_postimpact_entry_state_trace='--traffic-postimpact-entry-state-trace' in sys.argv or traffic_postimpact_counter_option is not None
traffic_collision_entry_state_trace='--traffic-collision-entry-state-trace' in sys.argv or traffic_postimpact_entry_state_trace
traffic_follow_entry_selector_trace='--traffic-follow-entry-selector-trace' in sys.argv
traffic_follow_entry_trace='--traffic-follow-entry-trace' in sys.argv or traffic_follow_entry_selector_trace
traffic_follow_entry_log_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--traffic-follow-entry-buffer=')),None)
traffic_follow_entry_log_address=int(traffic_follow_entry_log_option,0) if traffic_follow_entry_log_option else 0x02037000
assert traffic_follow_entry_log_address in (0x02026000,0x02037000,0x0203c000,0x0203d000,0x0203e000,0x03006c00),('unsupported traffic trace buffer',hex(traffic_follow_entry_log_address))
traffic_follow_entry_memory_base=0x03000000 if traffic_follow_entry_log_address>=0x03000000 else 0x02000000
traffic_follow_entry_log_offset=traffic_follow_entry_log_address-traffic_follow_entry_memory_base
traffic_follow_entry_capacity=9 if traffic_follow_entry_memory_base==0x03000000 else 15
traffic_follow_entry_output_count_offset=4+traffic_follow_entry_capacity*256
traffic_follow_entry_log_bytes=0xc00 if traffic_follow_entry_memory_base==0x03000000 else 0x1000
traffic_follow_entry_output_count_address=traffic_follow_entry_log_address+traffic_follow_entry_output_count_offset
traffic_dispatch_trace='--traffic-dispatch-entry-trace' in sys.argv or combined_callback_dispatch_trace
voice_reset_trace='--voice-reset-write-trace' in sys.argv
audio_busy_return_trace='--audio-busy-return-trace' in sys.argv
audio_context_start_trace='--audio-context-start-trace' in sys.argv
callback_cycle_trace='--callback-cycle-trace' in sys.argv or combined_callback_dispatch_trace
assert (not combined_callback_dispatch_trace or not (rng_callsite_trace or traffic_spawn_return_trace or traffic_slot_y_write_trace or voice_reset_trace or audio_busy_return_trace or audio_context_start_trace or traffic_dispatch_entry_state_trace or traffic_collision_entry_state_trace or traffic_follow_entry_trace)) and (combined_callback_dispatch_trace or sum((rng_callsite_trace,traffic_spawn_return_trace,traffic_slot_y_write_trace,traffic_dispatch_trace,traffic_dispatch_entry_state_trace,traffic_collision_entry_state_trace,traffic_follow_entry_trace,voice_reset_trace,audio_busy_return_trace,audio_context_start_trace,callback_cycle_trace))<=1),'Use one temporary ROM instrumentation mode at a time; callback+dispatcher is available through --combined-callback-dispatch-trace.'
core_rom=rom
instrumentation=None
traffic_spawn_return_instrumentation=None
if traffic_spawn_return_trace:
 hook=0x080067b8;stub_address=0x08044e80
 stub_path=root/'work/slot10-spawn-probe/slot10_spawn_probe.bin'
 stub=stub_path.read_bytes()
 base_stub_sha256=hashlib.sha256(stub).hexdigest()
 assert len(stub)==80 and base_stub_sha256=='a95c958bd3559ff5d1cdeb4633e27f68be0bab80b439e2bce0f26badeb9be4a9',(len(stub),base_stub_sha256)
 if traffic_spawn_return_slot!=10:
  old_pointer=struct.pack('<I',0x0200048c)
  target_pointer=struct.pack('<I',0x020000f4+traffic_spawn_return_slot*0x5c)
  assert stub.count(old_pointer)==1,'slot10 pointer literal not unique in spawn-return stub'
  stub=stub.replace(old_pointer,target_pointer)
 stub_sha256=hashlib.sha256(stub).hexdigest()
 hook_offset=hook-0x08000000;stub_offset=stub_address-0x08000000
 assert rom[hook_offset:hook_offset+4]==bytes.fromhex('00f002fa'),'traffic spawn callsite changed'
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'spawn-return stub location is not ROM padding'
 relative=stub_address-(hook+4)
 assert relative&1==0 and -0x400000<=relative<0x400000
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HH',patched,hook_offset,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 traffic_spawn_return_instrumentation=dict(kind='temporary_thumb_spawn_return_state_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_spawn_call=hex(hook),original_target='0x08006BC0',stub_address=hex(stub_address),stub_bytes_sha256=stub_sha256,base_stub_sha256=base_stub_sha256,stub_bytes=stub.hex(),actor_slot=traffic_spawn_return_slot,actor_pointer=hex(0x020000f4+traffic_spawn_return_slot*0x5c),logger_address='0x0203F000',logger_record_bytes=16,logger_record_fields=['source_traffic_counter_u32','actor_y_i32','actor_flags_u32','actor_countdown_i32'],scope=f'The in-memory content buffer redirects the original slot-spawn call through a tiny Thumb wrapper, calls the same original spawn routine once, and records actor slot{traffic_spawn_return_slot} after spawn returns. It then resumes the original dispatcher at its common presentation path. The supplied ROM and native runtime are not modified.')
traffic_slot_y_write_instrumentation=None
if traffic_slot_y_write_trace:
 hook=0x08006820;stub_address=0x08045f10
 stub_path=root/'work/slot10-spawn-probe/slot10_y_write_probe.bin'
 stub=stub_path.read_bytes();base_stub_sha256=hashlib.sha256(stub).hexdigest()
 assert len(stub)==68 and base_stub_sha256=='98b71d001cb1e8c676a2c2dc2e95cb0317ad6853c101408e804da3409f554bf1',(len(stub),base_stub_sha256)
 if traffic_slot_y_write_slot!=10:
  old_pointer=struct.pack('<I',0x0200048c)
  target_pointer=struct.pack('<I',0x020000f4+traffic_slot_y_write_slot*0x5c)
  assert stub.count(old_pointer)==1,'slot10 pointer literal not unique in Y-write stub'
  stub=stub.replace(old_pointer,target_pointer)
 stub_sha256=hashlib.sha256(stub).hexdigest()
 hook_offset=hook-0x08000000;stub_offset=stub_address-0x08000000
 assert rom[hook_offset:hook_offset+4]==bytes.fromhex('b061b06d'),'traffic height store/load sequence changed'
 assert rom[stub_offset:stub_offset+len(stub)]==bytes([255])*len(stub),'Y-write stub location is not ROM padding'
 relative=stub_address-(hook+4)
 assert relative&1==0 and -0x400000<=relative<0x400000
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HH',patched,hook_offset,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 traffic_slot_y_write_instrumentation=dict(kind='temporary_thumb_original_actor_y_store_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_original_store=hex(hook),displaced_instruction='ldr r0,[r6,#0x58] at 0x08006822',stub_address=hex(stub_address),stub_bytes_sha256=stub_sha256,base_stub_sha256=base_stub_sha256,stub_bytes=stub.hex(),actor_slot=traffic_slot_y_write_slot,actor_pointer=hex(0x020000f4+traffic_slot_y_write_slot*0x5c),logger_address='0x0203F000',logger_record_bytes=16,logger_record_fields=['source_traffic_counter_u32','written_actor_y_i32','actor_flags_u32','actor_route_pointer_u32'],scope=f'The temporary Thumb wrapper executes the original STR r0,[r6,#0x18] first and logs writes for actor slot{traffic_slot_y_write_slot}, then restores scratch registers, executes the displaced LDR, and returns. Instrumentation is confined to the in-memory ROM content buffer.')
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
 rng_only_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--rng-callsite-only=')),None)
 if rng_only_option:
  boundary_sites={int(value,0) for value in rng_only_option.split(',')}
  assert boundary_sites and boundary_sites.issubset(set(callsites)),('requested RNG caller is not an original callsite',sorted(hex(x) for x in boundary_sites))
  callsites=[site for site in callsites if site in boundary_sites]
 elif '--rng-caller-r4-trace' in sys.argv:
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
traffic_dispatch_instrumentation=None
traffic_dispatch_entry_state_instrumentation=None
traffic_collision_entry_state_instrumentation=None
traffic_follow_entry_instrumentation=None
if traffic_dispatch_entry_state_trace:
 # One-shot state snapshot at the exact original Thumb dispatcher entry.
 # Only the temporary libretro content buffer is patched; source ROM is read-only.
 dispatcher=0x080066b4;stub_address=0x08044d40
 displaced=bytes.fromhex('f0b557464e464546')
 stub_path=root/'tools/dispatch_entry_state_hook.bin'
 stub=bytearray(stub_path.read_bytes())
 stub_sha256=hashlib.sha256(stub).hexdigest()
 assert stub_sha256=='44513120d3db44448e68930dc377ce73655ddf66ba7421c62e42351e051f175e'
 assert len(stub)==188
 stub_offset=stub_address-0x08000000
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'entry-state hook location is not ROM padding'
 entry_offset=dispatcher-0x08000000
 assert rom[entry_offset:entry_offset+8]==displaced,'traffic dispatcher prologue changed'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HHI',patched,entry_offset,0x4b00,0x4718,stub_address|1)
 core_rom=bytes(patched)
 traffic_dispatch_entry_state_instrumentation=dict(kind='one_shot_thumb_dispatcher_entry_state_capture_then_replays_displaced_prologue',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_entry=hex(dispatcher),displaced_bytes=displaced.hex(),stub_address=hex(stub_address),stub_bytes_sha256=stub_sha256,ewram_snapshot_address='0x0203FB00',snapshot_bytes=136,snapshot_layout=['magic_u32','source_counter_u32','incoming_r0_u32','player_x_y_z_i32[3]','player_heading_i32','player_speed_544_i32','player_maximum_550_i32','actor0_raw_92_bytes','cooldown_e4_u16','near_miss_25d3_u8','padding_u8','rng_3b04_u32'],scope='A one-shot diagnostic hook captures selected collision inputs, the complete 92-byte actor0 record, and nearby traffic globals at dispatcher entry before original code executes. The hook is in temporary mGBA content only; the supplied ROM and native runtime are unchanged.')
if traffic_collision_entry_state_trace:
 # Capture the collision/follow routine's actual incoming actor pointer and
 # adjacent player/traffic state at its first actor0 call for counter 4286.
 dispatcher=0x08006ce8 if traffic_postimpact_entry_state_trace else 0x08006908;stub_address=0x08044d40
 displaced=bytes.fromhex('f0b557464e464546')
 stub_path=root/'tools/collision_entry_state_hook.bin'
 stub=bytearray(stub_path.read_bytes());source_hook_sha256=hashlib.sha256(stub).hexdigest()
 assert source_hook_sha256=='3ada93e766e3010fdad00a974e39b897808318930191ba9fd18d3c47ed510339'
 assert len(stub)==196
 if traffic_postimpact_entry_state_trace:
  assert struct.pack('<I',4286) in stub and struct.pack('<I',0x08006911) in stub
  assert stub.count(struct.pack('<H',0x2901))==1
  struct.pack_into('<I',stub,148,traffic_postimpact_entry_counter)
  struct.pack_into('<H',stub,38,0x2900)
  struct.pack_into('<I',stub,192,0x08006cf1)
  target_actor=0x020000f4+traffic_postimpact_entry_slot*0x5c
  old_actor=struct.pack('<I',0x020000f4)
  assert stub.count(old_actor)==1,('postimpact target actor literal count',stub.count(old_actor))
  stub=stub.replace(old_actor,struct.pack('<I',target_actor))
 stub=bytes(stub)
 stub_sha256=hashlib.sha256(stub).hexdigest()
 stub_offset=stub_address-0x08000000
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'collision-entry hook location is not ROM padding'
 entry_offset=dispatcher-0x08000000
 assert rom[entry_offset:entry_offset+8]==displaced,'traffic collision routine prologue changed'
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 struct.pack_into('<HHI',patched,entry_offset,0x4b00,0x4718,stub_address|1)
 core_rom=bytes(patched)
 traffic_collision_entry_state_instrumentation=dict(kind='one_shot_thumb_postimpact_entry_capture_then_replays_displaced_prologue' if traffic_postimpact_entry_state_trace else 'one_shot_thumb_collision_routine_entry_capture_then_replays_displaced_prologue',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_entry=hex(dispatcher),displaced_bytes=displaced.hex(),stub_address=hex(stub_address),source_hook_sha256=source_hook_sha256,stub_bytes_sha256=stub_sha256,expected_actor_pointer=hex(0x020000f4+traffic_postimpact_entry_slot*0x5c) if traffic_postimpact_entry_state_trace else '0x020000F4',target_slot=traffic_postimpact_entry_slot if traffic_postimpact_entry_state_trace else 0,expected_source_counter=traffic_postimpact_entry_counter if traffic_postimpact_entry_state_trace else 4286,ewram_snapshot_address='0x0203FB00',snapshot_bytes=136,snapshot_layout=['magic_u32','source_counter_u32','incoming_r0_actor_pointer_u32','player_x_y_z_i32[3]','player_heading_i32','player_speed_544_i32','player_maximum_550_i32','target_actor_raw_92_bytes','cooldown_e4_u16','near_miss_25d3_u8','padding_u8','rng_3b04_u32'],scope=(f'One-shot exact 0x08006CE8 entry state for actor slot{traffic_postimpact_entry_slot} at counter{traffic_postimpact_entry_counter}; captures player, actor and globals before the original prologue, then replays the displaced prologue and resumes at0x08006CF0. In-memory ROM probe only.' if traffic_postimpact_entry_state_trace else 'A one-shot diagnostic hook at 0x08006908 requires source counter4286, r0==actor0 base 0x020000F4, and actor0 active. It captures selected player fields and raw actor0/globals before the original routine prologue. This hook is in temporary mGBA content only; it does not modify the supplied ROM or native runtime.'))
if traffic_follow_entry_trace:
 # Pair exact actor-call entry data with state at the routine's sole return.
 # The two Thumb hooks patch only the in-memory ROM supplied to this core.
 dispatcher=0x08006908;stub_address=0x08044d40;return_hook_address=0x08044e00
 displaced=bytes.fromhex('f0b557464e464546')
 if traffic_follow_entry_selector_trace:
  stub_path=root/'tools/collision_entry_stream_return_hook_entry_selector.bin'
  stub=bytearray(stub_path.read_bytes())
  assert hashlib.sha256(stub).hexdigest()=='7bdb57941afdefdf81ae99a28196d7782dc17f5456fbb61e07ef19107614db84'
 else:
  stub_path=root/'tools/collision_entry_stream_return_hook_entry.bin'
  stub=bytearray(stub_path.read_bytes())
  assert hashlib.sha256(stub).hexdigest()=='de28c27ff2b6a6ac87a7f2ab53cddf26ba1375e630416310087cc2a88092c347'
 assert len(stub)==192
 return_stub=bytearray((root/'tools/collision_entry_stream_return_hook.bin').read_bytes())
 assert hashlib.sha256(return_stub).hexdigest()=='1224ad4721372462e07f793df54a19dc115ad9afbfccb485719969ff532b126b'
 assert len(return_stub)==168
 struct.pack_into('<H',stub,6,0x2d00|traffic_follow_entry_capacity)
 assert struct.unpack_from('<H',return_stub,6)[0]==0x2d0f and struct.unpack_from('<H',return_stub,14)[0]==0x2f0f
 struct.pack_into('<H',return_stub,6,0x2d00|traffic_follow_entry_capacity)
 struct.pack_into('<H',return_stub,14,0x2f00|traffic_follow_entry_capacity)
 old_log=struct.pack('<I',0x02037000);new_log=struct.pack('<I',traffic_follow_entry_log_address)
 old_output=struct.pack('<I',0x02037f04);new_output=struct.pack('<I',traffic_follow_entry_output_count_address)
 assert stub.count(old_log)==1,('entry buffer literal count',stub.count(old_log))
 assert return_stub.count(old_log)==1 and return_stub.count(old_output)==1,('return buffer literals',return_stub.count(old_log),return_stub.count(old_output))
 stub=stub.replace(old_log,new_log)
 return_stub=return_stub.replace(old_log,new_log).replace(old_output,new_output)
 stub_sha256=hashlib.sha256(stub).hexdigest();return_stub_sha256=hashlib.sha256(return_stub).hexdigest()
 stub_offset=stub_address-0x08000000
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'collision-entry stream hook location is not ROM padding'
 return_stub_offset=return_hook_address-0x08000000
 assert rom[return_stub_offset:return_stub_offset+272]==b'\xff'*272,'collision-entry return hook location is not ROM padding'
 entry_offset=dispatcher-0x08000000
 assert rom[entry_offset:entry_offset+8]==displaced,'traffic follow routine prologue changed'
 return_offset=0x08006bbc-0x08000000
 assert rom[return_offset:return_offset+4]==bytes.fromhex('00470000'),'traffic follow return site changed'
 call_delta=return_hook_address-(0x08006bbc+4)
 assert call_delta&1==0 and -0x400000<=call_delta<0x400000
 patched=bytearray(rom);patched[stub_offset:stub_offset+len(stub)]=stub
 patched[return_stub_offset:return_stub_offset+len(return_stub)]=return_stub
 struct.pack_into('<HHI',patched,entry_offset,0x4b00,0x4718,stub_address|1)
 struct.pack_into('<HH',patched,return_offset,0xf000|((call_delta>>12)&0x7ff),0xf800|((call_delta>>1)&0x7ff))
 core_rom=bytes(patched)
 follow_input_layout=(['source_counter_u32','incoming_actor_pointer_u32','incoming_r1_u32','player_x_y_z_i32[3]','player_heading_i32','player_speed_544_i32','player_maximum_550_i32','incoming_actor_raw_92_bytes','selector_counter_4c98_u32','reserved_u32','rng_3b04_u32'] if traffic_follow_entry_selector_trace else ['source_counter_u32','incoming_actor_pointer_u32','incoming_r1_u32','player_x_y_z_i32[3]','player_heading_i32','player_speed_544_i32','player_maximum_550_i32','incoming_actor_raw_92_bytes','cooldown_e4_u16','near_miss_25d3_u8','padding_u8','rng_3b04_u32'])
 traffic_follow_entry_instrumentation=dict(kind='bounded_thumb_follow_entry_and_single_return_state_stream_with_selector' if traffic_follow_entry_selector_trace else 'bounded_thumb_follow_entry_and_single_return_state_stream',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),patched_entry=hex(dispatcher),patched_return='0x08006BBC',displaced_bytes=displaced.hex(),entry_stub_address=hex(stub_address),entry_stub_sha256=stub_sha256,return_stub_address=hex(return_hook_address),return_stub_sha256=return_stub_sha256,memory_region='iwram' if traffic_follow_entry_memory_base==0x03000000 else 'ewram',memory_log_address=hex(traffic_follow_entry_log_address),memory_log_bytes=traffic_follow_entry_log_bytes,ewram_log_address=hex(traffic_follow_entry_log_address) if traffic_follow_entry_memory_base==0x02000000 else None,records_per_vblank_capacity=traffic_follow_entry_capacity,record_bytes=256,input_bytes=136,output_bytes=120,output_counter_address=hex(traffic_follow_entry_output_count_address),input_layout=follow_input_layout,output_layout=['player_x_y_z_i32[3]','player_heading_i32','player_speed_544_i32','actor_raw_92_bytes','cooldown_after_e4_u16','near_miss_after_25d3_u8','padding_u8','rng_after_3b04_u32'],selector_read_address='0x03004C98' if traffic_follow_entry_selector_trace else None,scope='Captures each 0x08006908 input before its body and selected mutable outputs immediately before return. Selector mode stores the full live IWRAM selector at entry-record offset128 instead of the input cooldown/near-miss fields; output layout is unchanged. Temporary in-memory ROM content only.')
if traffic_dispatch_trace:
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
audio_busy_return_instrumentation=None
audio_context_start_instrumentation=None
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
if audio_busy_return_trace:
 hook=0x0801134e;stub_address=0x08044d80
 stub=bytes.fromhex('01b4094a116850290ad20b0001331360c90004325218054b1b681360009b536001bc00067047000000f0030220350002')
 hook_offset=hook-0x08000000;stub_offset=stub_address-0x08000000
 assert rom[hook_offset:hook_offset+2]==bytes.fromhex('0006'),'post-busy return instruction changed'
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'busy-return stub location not ROM padding'
 patched=bytearray(core_rom);patched[stub_offset:stub_offset+len(stub)]=stub
 relative=stub_address-(hook+4);assert relative&1==0 and -0x400000<=relative<0x400000
 struct.pack_into('<HH',patched,hook_offset,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 audio_busy_return_instrumentation=dict(kind='temporary_thumb_post_busy_return_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),hook=hex(hook),displaced='lsls r0,r0,#24',busy_target='0x080277A4',stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log='0x0203F000',record_bytes=8,record_fields=['source_counter_u32','busy_helper_return_u32'],scope='A temporary Thumb wrapper at the original post-call instruction records raw r0 from 0x080277A4, then replays the displaced LSLS and resumes the original branch sequence. Only the in-memory libretro ROM copy is patched.')
 instrumentation=audio_busy_return_instrumentation
if audio_context_start_trace:
 hook=0x0801130e;stub_address=0x08044dc0
 stub=bytes.fromhex('01b40c4b1968502910d20a0001321a60090104335b18084a12681a60074a12785a60074a12789a60009ada6001bc0549054a104700f0030220350002d625000210260002042600024d140108')
 hook_offset=hook-0x08000000;stub_offset=stub_address-0x08000000
 assert rom[0x0801130a-0x08000000:0x0801130e-0x08000000]==bytes.fromhex('15f05dfa'),'context audio start call changed'
 assert rom[hook_offset:hook_offset+4]==bytes.fromhex('04499ce0'),'post-start literal load/branch changed'
 assert rom[stub_offset:stub_offset+len(stub)]==b'\xff'*len(stub),'context-start stub location not ROM padding'
 patched=bytearray(core_rom);patched[stub_offset:stub_offset+len(stub)]=stub
 relative=stub_address-(hook+4);assert relative&1==0 and -0x400000<=relative<0x400000
 struct.pack_into('<HH',patched,hook_offset,0xf000|((relative>>12)&0x7ff),0xf800|((relative>>1)&0x7ff))
 core_rom=bytes(patched)
 audio_context_start_instrumentation=dict(kind='temporary_thumb_context_voice_start_return_probe',source_rom_sha256=hashlib.sha256(rom).hexdigest(),instrumented_rom_sha256=hashlib.sha256(core_rom).hexdigest(),hook=hex(hook),displaced='ldr r1,[pc,#16] ; 0x02002604; b 0x0801144C',start_target='0x080267C8',stub_address=hex(stub_address),stub_bytes=stub.hex(),ewram_log='0x0203F000',record_bytes=16,record_fields=['source_counter_u32','context_selection_u32','classification_u32','returned_handle_u32'],scope='A temporary Thumb wrapper after the original contextual voice start logs shared selection/classification and the returned handle, then replays the displaced literal load and branch path to 0x0801144C. Only the in-memory libretro ROM copy is patched.')
 instrumentation=audio_context_start_instrumentation
if combined_callback_dispatch_trace:
 combined_sha=hashlib.sha256(core_rom).hexdigest()
 traffic_dispatch_instrumentation['instrumented_rom_sha256']=combined_sha
 callback_cycle_instrumentation['instrumented_rom_sha256']=combined_sha
 traffic_dispatch_instrumentation['combined_probe_mode']='dispatcher entry plus callback cycle timer in one in-memory ROM; separate EWRAM log regions'
 callback_cycle_instrumentation['combined_probe_mode']='dispatcher entry plus callback cycle timer in one in-memory ROM; separate EWRAM log regions'
route_follow='--follow-route31' in sys.argv
route_steer_during_brake='--route-steer-during-brake' in sys.argv
route_brake_speed_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-brake-speed=')),None)
route_brake_speed=int(route_brake_speed_option) if route_brake_speed_option else 40000
route_shoulder_once='--route-shoulder-once' in sys.argv or '--route-boost-once' in sys.argv
route_shoulder_interval_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-shoulder-every=') or item.startswith('--route-boost-every=')),None)
route_shoulder_interval=int(route_shoulder_interval_option) if route_shoulder_interval_option else 0
assert route_shoulder_interval==0 or 30<=route_shoulder_interval<=600,('invalid route shoulder interval',route_shoulder_interval)
route_shoulder_count_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-shoulder-count=')),None)
route_shoulder_limit=int(route_shoulder_count_option) if route_shoulder_count_option else (1 if route_shoulder_once else 20 if route_shoulder_interval else 0)
assert 0<=route_shoulder_limit<=20,('invalid route shoulder count',route_shoulder_limit)
route_direct_destination='--route-direct-destination' in sys.argv
route_avoid_traffic='--route-avoid-traffic' in sys.argv
route_avoid_collision_gate='--route-avoid-collision-gate' in sys.argv
route_gate_margin_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-gate-margin=')),None)
route_gate_margin=float(route_gate_margin_option) if route_gate_margin_option else 3.0
route_gate_hold_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-gate-hold=')),None)
route_gate_hold=int(route_gate_hold_option) if route_gate_hold_option else 12
assert 1<=route_gate_margin<=8 and 3<=route_gate_hold<=60 and not(route_avoid_traffic and route_avoid_collision_gate)
route_avoid_distance_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-avoid-distance=')),None)
route_avoid_distance=int(route_avoid_distance_option) if route_avoid_distance_option else 5000
route_avoid_width_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-avoid-width=')),None)
route_avoid_width=int(route_avoid_width_option) if route_avoid_width_option else 1600
route_avoid_hold_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-avoid-hold=')),None)
route_avoid_hold=int(route_avoid_hold_option) if route_avoid_hold_option else 30
assert 1000<=route_avoid_distance<=20000 and 500<=route_avoid_width<=6000 and 5<=route_avoid_hold<=120
route_replay_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-replay-csv=')),None)
route_replay_path=pathlib.Path(route_replay_option) if route_replay_option else None
route_replay_invert_steer='--route-replay-invert-steer' in sys.argv
route_replay_update_boundary='--route-replay-update-boundary' in sys.argv
assert not route_replay_update_boundary or route_replay_path is not None,'Update-boundary replay requires --route-replay-csv.'
input_callback_trace='--input-callback-trace' in sys.argv
route_gate_avoid_on_replay='--route-gate-avoid-on-replay' in sys.argv
assert not route_gate_avoid_on_replay or (route_replay_path is not None and route_avoid_collision_gate),'Gate override on replay requires an input replay and collision-gate mode.'
if route_replay_path and not route_replay_path.is_absolute():route_replay_path=root/route_replay_path
route_replay_rows=list(csv.DictReader(route_replay_path.open(newline=''))) if route_replay_path else []
assert not (traffic_spawn_return_trace or traffic_slot_y_write_trace) or route_replay_path is not None,'traffic spawn/Y-write probes require a fixed input CSV replay'
route_replay_sha256=hashlib.sha256(route_replay_path.read_bytes()).hexdigest() if route_replay_path else None
route_waypoint_radius_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-waypoint-radius=')),None)
route_waypoint_radius=int(route_waypoint_radius_option) if route_waypoint_radius_option else 1024
route_lookahead_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-lookahead=')),None)
route_lookahead=int(route_lookahead_option) if route_lookahead_option else 4096
route_lookahead_divisor_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-lookahead-speed-divisor=')),None)
route_lookahead_divisor=int(route_lookahead_divisor_option) if route_lookahead_divisor_option else 32
assert 1<=route_waypoint_radius<=32768 and 1<=route_lookahead<=32768 and 1<=route_lookahead_divisor<=256
assert 0<=route_brake_speed<=200000,('invalid route brake speed',route_brake_speed)
route_start_waypoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--route-start-waypoint=')),None)
route_start_waypoint=int(route_start_waypoint_option) if route_start_waypoint_option else 0
assert 0<=route_start_waypoint<27,('invalid starting waypoint',route_start_waypoint)
frames=int(sys.argv[1]) if len(sys.argv)>1 else 900;assert 0<frames<=(16000 if route_follow else 4000)
label=sys.argv[2] if len(sys.argv)>2 else 'boot900'
assert label.isalnum()
direct_input_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--direct-input-keys-json=')),None)
direct_input_path=pathlib.Path(direct_input_option) if direct_input_option else None
if direct_input_path and not direct_input_path.is_absolute():direct_input_path=root/direct_input_path
direct_input_rows=json.loads(direct_input_path.read_text()) if direct_input_path else None
assert direct_input_rows is None or (len(direct_input_rows)==frames and all(isinstance(row,dict) and isinstance(row.get('input_keys'),list) and all(type(key) is int and 0<=key<=15 for key in row['input_keys']) for row in direct_input_rows)),('direct input trace must provide one valid input_keys list per frame',frames)
direct_input_sha256=hashlib.sha256(direct_input_path.read_bytes()).hexdigest() if direct_input_path else None
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
input_callback_events=[];input_callback_counts={};input_callback_seen=set();input_poll_events=[];input_poll_seen=set()
def input_poll():
 if input_callback_trace:
  if tick in input_poll_seen:return
  input_poll_seen.add(tick)
  ap,ao,_=memory_maps[0x2000000]
  input_poll_events.append(dict(tick=tick,keys=sorted(keys),counter=struct.unpack('<I',C.string_at(ap+ao+0x3520,4))[0],previous=struct.unpack('<I',C.string_at(ap+ao+0x3524,4))[0],frame_delta=struct.unpack('<i',C.string_at(ap+ao+0x3b00,4))[0],raw_input_history=struct.unpack('<H',C.string_at(ap+ao+0x34de,2))[0]))
def input_state(port,device,index,button):
 pressed=int(port==0 and button in keys)
 if input_callback_trace:
  input_callback_counts[tick]=input_callback_counts.get(tick,0)+1
  if tick not in input_callback_seen:
   input_callback_seen.add(tick)
   ap,ao,_=memory_maps[0x2000000]
   input_callback_events.append(dict(tick=tick,port=port,device=device,index=index,button=button,pressed=pressed,keys=sorted(keys),keyinput=struct.unpack('<H',mapped_memory(0x04000130,2))[0],counter=struct.unpack('<I',C.string_at(ap+ao+0x3520,4))[0],previous=struct.unpack('<I',C.string_at(ap+ao+0x3524,4))[0],frame_delta=struct.unpack('<i',C.string_at(ap+ao+0x3b00,4))[0],raw_input_history=struct.unpack('<H',C.string_at(ap+ao+0x34de,2))[0]))
 return pressed
for name,typ,fn in [('environment',ENV,environment),('video_refresh',VIDEO,video),('audio_sample',AUDIO,lambda l,r:None),('audio_sample_batch',BATCH,lambda p,n:n),('input_poll',POLL,input_poll),('input_state',INPUT,input_state)]:
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
if rng_callsite_trace or traffic_spawn_return_trace or traffic_slot_y_write_trace or traffic_dispatch_trace or voice_reset_trace or audio_busy_return_trace or audio_context_start_trace:
 pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
 log=C.string_at(pointer+offset+0x3f000,0x1000);assert not any(log),'refusing to clear nonzero diagnostic buffer region'
 C.memset(pointer+offset+0x3f000,0,0x1000)
if traffic_follow_entry_trace:
 pointer,offset,length=memory_maps[traffic_follow_entry_memory_base]
 expected_length=0x8000 if traffic_follow_entry_memory_base==0x03000000 else 0x40000
 assert pointer and length==expected_length and traffic_follow_entry_log_offset+traffic_follow_entry_log_bytes<=length
 log_region=C.string_at(pointer+offset+traffic_follow_entry_log_offset,traffic_follow_entry_log_bytes)
 assert not any(log_region),('refusing to overwrite nonzero traffic trace page',hex(traffic_follow_entry_log_address))
 C.memset(pointer+offset+traffic_follow_entry_log_offset,0,traffic_follow_entry_log_bytes)
traffic_dispatch_entry_state=None
traffic_dispatch_entry_state_tick=None
traffic_collision_entry_state=None
traffic_collision_entry_state_tick=None
traffic_follow_entry_events=[]
traffic_follow_entry_log_drains=0
traffic_follow_entry_max_records=0
traffic_follow_entry_pending=[]
traffic_follow_entry_deferred_returns=0
traffic_follow_entry_batches=[]
if traffic_dispatch_entry_state_trace or traffic_collision_entry_state_trace:
 pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
 initial=C.string_at(pointer+offset+0x3fb00,136)
 assert not any(initial),'refusing to use nonzero dispatcher-entry snapshot buffer'
if callback_cycle_trace:
 pointer,offset,length=memory_maps[0x2000000];assert pointer and length==0x40000
 log=C.string_at(pointer+offset+0x3d000,0x1000);assert not any(log),'refusing to clear nonzero callback-cycle buffer region'
 C.memset(pointer+offset+0x3d000,0,0x1000)
try:
 schedule=[]
 actor_trace=[]
 cpu_trace=[]
 rng_trace_events=[];rng_trace_used=0
 audio_busy_return_events=[]
 audio_context_start_events=[]
 traffic_dispatch_events=[];traffic_dispatch_used=0;traffic_dispatch_ticks=set();traffic_dispatch_log_drains=0
 voice_reset_events=[];voice_reset_used=0
 callback_cycle_events=[];callback_cycle_used=0;callback_cycle_log_drains=0;callback_cycle_max_records_in_batch=0
 route_follow_waypoint=route_start_waypoint
 route_shoulder_stage=0;route_shoulder_sent=False;route_shoulder_last_tick=-100000;route_shoulder_count=0;route_replay_index=0;route_replay_last_previous=None;route_replay_current_row=None
 route_avoid_direction=0;route_avoid_until=-1;route_avoid_actor=-1;route_avoid_events=0
 route_trig=[(int(math.sin(a*2*math.pi/4096)*4096),int(math.cos(a*2*math.pi/4096)*4096)) for a in range(4096)]
 traffic_path_manifest={};traffic_path_cache={}
 if route_avoid_collision_gate:
  roads=json.loads((root/'assets/traffic/city1-roads.json').read_text())
  traffic_path_manifest={int(item['descriptor_address'],16):item for item in roads['paths']}
 def source_traffic_path_at(descriptor,distance):
  if descriptor not in traffic_path_cache:
   item=traffic_path_manifest[descriptor];path=root/'assets/traffic'/f"city1-road-{item['index']}.ctrp";raw=path.read_bytes()
   assert raw[:4]==b'CTP1' and hashlib.sha256(raw[8:]).hexdigest()==item['sha256']
   count=struct.unpack_from('<I',raw,4)[0];assert count==item['count'] and len(raw)==8+count*16
   traffic_path_cache[descriptor]=(item,[struct.unpack_from('<IiiI',raw,8+i*16) for i in range(count)])
  item,points=traffic_path_cache[descriptor];phase=distance%item['length'];segment=0
  while segment+1<len(points) and points[segment+1][0]<=phase:segment+=1
  assert segment+1<len(points)
  d0,x0,z0,_=points[segment];d1,x1,z1,_=points[segment+1];span=d1-d0;offset=phase-d0
  return x0+int((x1-x0)*offset/span),z0+int((z1-z0)*offset/span)
 def route_follow_input():
  global route_follow_waypoint,route_shoulder_stage,route_shoulder_sent,route_shoulder_last_tick,route_shoulder_count,route_replay_index,route_replay_last_previous,route_replay_current_row,route_avoid_direction,route_avoid_until,route_avoid_actor,route_avoid_events
  ap,ao,al=memory_maps[0x2000000]
  actor=C.string_at(ap+ao+0x2634,0xea0)
  x,y,z=struct.unpack_from('<iii',actor,0);heading=struct.unpack_from('<i',actor,0x1c)[0];speed=struct.unpack_from('<i',actor,0x544)[0]
  trip_active=C.string_at(ap+ao+0x1694,1)[0]!=0
  if trip_active and route_follow_waypoint==0:route_follow_waypoint=1
  if trip_active and not route_direct_destination:
   while route_follow_waypoint+1<len(route_points):
    target=route_points[route_follow_waypoint];previous=route_points[route_follow_waypoint-1]
    dx=target[0]-x;dz=target[1]-z;px=x-previous[0];pz=z-previous[1];sx=target[0]-previous[0];sz=target[1]-previous[1];length=sx*sx+sz*sz
    reached=dx*dx+dz*dz<route_waypoint_radius*route_waypoint_radius or (length>0 and px*sx+pz*sz>=length and (px*sz-pz*sx)**2<=4096*4096*length)
    if not reached:break
    route_follow_waypoint+=1
  target=[-257694,274082] if trip_active and route_direct_destination else route_points[route_follow_waypoint]
  dx=target[0]-x;dz=target[1]-z;distance2=dx*dx+dz*dz;route_lateral=0.0
  if trip_active and route_follow_waypoint and not route_direct_destination:
   previous=route_points[route_follow_waypoint-1];sx=target[0]-previous[0];sz=target[1]-previous[1];length=math.hypot(sx,sz)
   if length>0:
    px=x-previous[0];pz=z-previous[1];along=(px*sx+pz*sz)/length;route_lateral=(px*sz-pz*sx)/length
    lookahead=min(float(route_lookahead),1024.0+max(0,speed)/route_lookahead_divisor);along=max(0.0,min(length,along+lookahead))
    dx=round(previous[0]+sx*along/length)-x;dz=round(previous[1]+sz*along/length)-z
  alignment=-0x7fffffffffffffff;desired=0
  for angle,(sinv,cosv) in enumerate(route_trig):
   dot=-sinv*dx+cosv*dz
   if dot>alignment:alignment=dot;desired=angle
  current=(heading>>12)&4095;difference=((desired-current+2048)&4095)-2048
  approaching_bend=False
  if trip_active and not route_direct_destination and route_follow_waypoint+1<len(route_points) and distance2<8192*8192:
   nxt=route_points[route_follow_waypoint+1];nx=nxt[0]-target[0];nz=nxt[1]-target[1]
   next_angle=max(range(4096),key=lambda a:-route_trig[a][0]*nx+route_trig[a][1]*nz)
   next_difference=((next_angle-current+2048)&4095)-2048;approaching_bend=abs(next_difference)>256
  brake=speed>route_brake_speed and (abs(difference)>256 or approaching_bend or abs(route_lateral)>1024)
  if not trip_active and distance2<1024*1024:brake=True
  keys={0} if brake else {8}
  if route_steer_during_brake or not brake:
   if difference>32:keys.add(6)
   elif difference < -32:keys.add(7)
  shoulder_pulse=0
  if trip_active and route_shoulder_stage:
   keys.discard(11);route_shoulder_stage=0
   if not brake:keys.add(8)
  elif (route_shoulder_once or route_shoulder_interval) and trip_active and route_shoulder_count<route_shoulder_limit and not brake and speed>20000 and abs(struct.unpack_from('<h',actor,0x578)[0])<=767 and ((route_shoulder_once and not route_shoulder_sent) or (route_shoulder_interval and tick-route_shoulder_last_tick>=route_shoulder_interval)):
   keys.discard(8);keys.add(11);route_shoulder_stage=1;route_shoulder_sent=True;route_shoulder_last_tick=tick;route_shoulder_count+=1;shoulder_pulse=1
  replay_tick=None;replay_steer=0;replay_brake=0;replay_exhausted=0
  if route_replay_path and trip_active:
   if route_replay_update_boundary:
    source_previous=struct.unpack('<I',C.string_at(ap+ao+0x3524,4))[0]
    if route_replay_last_previous is None or source_previous!=route_replay_last_previous:
     route_replay_last_previous=source_previous
     route_replay_current_row=route_replay_rows[route_replay_index] if route_replay_index<len(route_replay_rows) else None
     if route_replay_current_row is not None:route_replay_index+=1
    replay_row=route_replay_current_row
   else:
    replay_row=route_replay_rows[route_replay_index] if route_replay_index<len(route_replay_rows) else None
    if replay_row is not None:route_replay_index+=1
   if replay_row is not None:
    replay_steer=int(replay_row['steer']);replay_brake=int(replay_row['brake']);replay_tick=int(replay_row['tick'])
    keys={0} if replay_brake else {8}
    replay_direction=-replay_steer if route_replay_invert_steer else replay_steer
    if not replay_brake and replay_direction==-1:keys.add(6)
    elif not replay_brake and replay_direction==1:keys.add(7)
   else:replay_exhausted=1
  traffic_avoid_active=0;traffic_avoid_slot=-1;traffic_avoid_forward=0;traffic_avoid_lateral=0;traffic_avoid_event=0
  if route_avoid_traffic and trip_active and not route_replay_path:
   pool=C.string_at(ap+ao+0xf4,50*0x5c);current_heading=(heading>>12)&4095;sinv,cosv=route_trig[current_heading];traffic=[]
   for slot in range(50):
    off=slot*0x5c;flags=struct.unpack_from('<I',pool,off)[0];descriptor=struct.unpack_from('<I',pool,off+0x24)[0]
    if not(flags&1) or not descriptor:continue
    tx=struct.unpack_from('<i',pool,off+0x14)[0];tz=struct.unpack_from('<i',pool,off+0x1c)[0];dx=tx-x;dz=tz-z
    forward=(-sinv*dx+cosv*dz)//4096;lateral=(cosv*dx+sinv*dz)//4096
    traffic.append((slot,forward,lateral))
   latched=next((item for item in traffic if item[0]==route_avoid_actor),None)
   if route_avoid_direction and tick<=route_avoid_until and latched and -2000<latched[1]<route_avoid_distance*2 and abs(latched[2])<route_avoid_width*3:
    traffic_avoid_active=1;traffic_avoid_slot=latched[0];traffic_avoid_forward=latched[1];traffic_avoid_lateral=latched[2]
   else:
    route_avoid_direction=0;route_avoid_actor=-1
   if not route_avoid_direction:
    hazards=[item for item in traffic if 0<item[1]<route_avoid_distance and abs(item[2])<route_avoid_width]
    if hazards:
     nearest=min(hazards,key=lambda item:item[1]);slot,forward,lateral=nearest
     if abs(lateral)>256:direction=-1 if lateral>0 else 1
     else:
      def side_cost(side):return sum(max(0,route_avoid_width*4-abs(other[2]-side*route_avoid_width*2))*max(0,route_avoid_distance-abs(other[1]-route_avoid_distance//2)) for other in hazards)
      left_cost=side_cost(-1);right_cost=side_cost(1)
      direction=-1 if left_cost<right_cost else 1 if right_cost<left_cost else (-1 if difference>=0 else 1)
     route_avoid_direction=direction;route_avoid_actor=slot;route_avoid_until=tick+route_avoid_hold;route_avoid_events+=1;traffic_avoid_event=1
     traffic_avoid_active=1;traffic_avoid_slot=slot;traffic_avoid_forward=forward;traffic_avoid_lateral=lateral
   if traffic_avoid_active:
    keys.discard(6);keys.discard(7);keys.add(6 if route_avoid_direction<0 else 7)
  traffic_gate_active=0;traffic_gate_slot=-1;traffic_gate_forward=0;traffic_gate_lateral=0;traffic_gate_distance=0;traffic_gate_radius=0;traffic_gate_event=0
  if route_avoid_collision_gate and trip_active and (not route_replay_path or route_gate_avoid_on_replay):
   pool=C.string_at(ap+ao+0xf4,50*0x5c);source_delta=struct.unpack('<i',C.string_at(ap+ao+0x3b00,4))[0];current_heading=(heading>>12)&4095;sinv,cosv=route_trig[current_heading];risks=[]
   for slot in range(50):
    off=slot*0x5c;flags=struct.unpack_from('<I',pool,off)[0];descriptor=struct.unpack_from('<I',pool,off+0x24)[0]
    if not(flags&1) or descriptor not in traffic_path_manifest:continue
    timer=struct.unpack_from('<i',pool,off+0x58)[0]
    if timer>0:tx,tz=struct.unpack_from('<ii',pool,off+0x34)
    else:
     route_distance=struct.unpack_from('<i',pool,off+0x28)[0];tx,tz=source_traffic_path_at(descriptor,route_distance+source_delta)
    dx=tx-x;dz=tz-z;forward=(-sinv*dx+cosv*dz)//4096;lateral=(cosv*dx+sinv*dz)//4096
    scale=min(speed//0x472,200);radius=scale*192//200+384;limit=radius*route_gate_margin
    if forward<0 or forward>10000 or abs(lateral)>limit*2 or dx*dx+dz*dz>limit*limit:continue
    risks.append((dx*dx+dz*dz,slot,forward,lateral,radius,tx,tz))
   latched=next((item for item in risks if item[1]==route_avoid_actor),None)
   if route_avoid_direction and tick<=route_avoid_until and latched:
    selected=latched;traffic_gate_active=1
   else:
    route_avoid_direction=0;route_avoid_actor=-1;selected=None
   if not route_avoid_direction and risks:
    selected=min(risks);_,slot,forward,lateral,radius,tx,tz=selected
    if abs(lateral)>128:direction=-1 if lateral>0 else 1
    else:direction=-1 if difference>=0 else 1
    route_avoid_direction=direction;route_avoid_actor=slot;route_avoid_until=tick+route_gate_hold;route_avoid_events+=1;traffic_gate_event=1;traffic_gate_active=1
   if selected:
    _,traffic_gate_slot,traffic_gate_forward,traffic_gate_lateral,traffic_gate_radius,tx,tz=selected
    traffic_gate_distance=math.isqrt(selected[0]);keys.discard(6);keys.discard(7);keys.add(6 if route_avoid_direction<0 else 7)
  state=dict(position=[x,y,z],heading=heading,speed=speed,steering=struct.unpack_from('<h',actor,0x578)[0],gear_flags=actor[0x4ac],trip_active=int(trip_active),waypoint=route_follow_waypoint,target_mode='direct_destination' if route_direct_destination and trip_active else 'source_route31',difference=difference,brake=int(brake),steer_during_brake=int(route_steer_during_brake),brake_speed=route_brake_speed,distance_squared=distance2,traffic_gate_active=traffic_gate_active,traffic_gate_event=traffic_gate_event,traffic_gate_slot=traffic_gate_slot,traffic_gate_direction=route_avoid_direction,traffic_gate_forward=traffic_gate_forward,traffic_gate_lateral=traffic_gate_lateral,traffic_gate_distance=traffic_gate_distance,traffic_gate_radius=traffic_gate_radius,traffic_gate_margin=route_gate_margin,traffic_gate_hold=route_gate_hold,traffic_gate_events=route_avoid_events,traffic_avoid_active=traffic_avoid_active,traffic_avoid_event=traffic_avoid_event,traffic_avoid_slot=traffic_avoid_slot,traffic_avoid_direction=route_avoid_direction,traffic_avoid_forward=traffic_avoid_forward,traffic_avoid_lateral=traffic_avoid_lateral,traffic_avoid_events=route_avoid_events,traffic_avoid_distance=route_avoid_distance,traffic_avoid_width=route_avoid_width,traffic_avoid_hold=route_avoid_hold,shoulder_pulse=shoulder_pulse,shoulder_sent=int(route_shoulder_sent),shoulder_count=route_shoulder_count,replay_tick=replay_tick,replay_steer=replay_steer,replay_brake=replay_brake,replay_exhausted=replay_exhausted)
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
 state_checkpoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--state-checkpoints=')),None)
 state_checkpoint_ticks=set(map(int,state_checkpoint_option.split(','))) if state_checkpoint_option else set()
 assert all(0<=tick<frames for tick in state_checkpoint_ticks),('state checkpoint tick outside capture',sorted(state_checkpoint_ticks),frames)
 state_checkpoint_hashes={}
 iwram_checkpoint_option=next((item.split('=',1)[1] for item in sys.argv if item.startswith('--iwram-checkpoints=')),None)
 iwram_checkpoint_ticks=set(map(int,iwram_checkpoint_option.split(','))) if iwram_checkpoint_option else set()
 assert all(0<=tick<frames for tick in iwram_checkpoint_ticks),('IWRAM checkpoint tick outside capture',sorted(iwram_checkpoint_ticks),frames)
 iwram_checkpoint_hashes={}
 for tick in range(frames):
  route_driver_state=None
  if route_follow:keys,route_driver_state=route_follow_input()
  else:
   keys={3,8} if '--no-input' not in sys.argv and tick>=300 and tick%180<5 else set()
   if '--drive-left' in sys.argv:keys={8} | ({6} if tick>=180 else set())
   if '--drive-right' in sys.argv:keys={8} | ({7} if tick>=180 else set())
  if direct_input_rows is not None:keys=set(direct_input_rows[tick]['input_keys'])
  if keys:schedule.append(tick)
  core.retro_run()
  if audio_busy_return_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3f000,4))[0];assert used<=80,('busy-return trace overflow',used)
   for event_index in range(used):
    counter,value=struct.unpack('<II',C.string_at(ap+ao+0x3f004+event_index*8,8))
    audio_busy_return_events.append(dict(observed_after_core_tick=tick,source_counter=counter,busy_helper_return=value))
   if used:C.memset(ap+ao+0x3f000,0,4)
  if audio_context_start_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   used=struct.unpack('<I',C.string_at(ap+ao+0x3f000,4))[0];assert used<=80,('context-start trace overflow',used)
   for event_index in range(used):
    counter,selection,classification,handle=struct.unpack('<IIII',C.string_at(ap+ao+0x3f004+event_index*16,16))
    audio_context_start_events.append(dict(observed_after_core_tick=tick,source_counter=counter,selection=selection&255,classification=classification&255,returned_handle=hex(handle)))
   if used:C.memset(ap+ao+0x3f000,0,4)
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
   assert traffic_dispatch_used<=used<=240,('invalid dispatcher trace count',traffic_dispatch_used,used)
   for event_index in range(traffic_dispatch_used,used):
    counter,delta,slot8_ft,slot8_countdown,slot8_y,slot9_countdown,slot9_ft=struct.unpack('<HBBhhhB',C.string_at(ap+ao+0x3f004+event_index*11,11))
    traffic_dispatch_events.append(dict(call_index=len(traffic_dispatch_events),source_counter=counter,frame_delta=delta,slot8=dict(flags=slot8_ft>>4,type=slot8_ft&15,countdown=slot8_countdown,y=slot8_y),slot9=dict(flags=slot9_ft>>4,type=slot9_ft&15,countdown=slot9_countdown),observed_after_core_tick=tick))
    traffic_dispatch_ticks.add(tick)
   traffic_dispatch_used=used
   if used>0:
    C.memset(ap+ao+0x3f000,0,4)
    traffic_dispatch_used=0
    traffic_dispatch_log_drains+=1
  if traffic_dispatch_entry_state_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   candidate=C.string_at(ap+ao+0x3fb00,136)
   if candidate[:4]==b'PCSD':
    if traffic_dispatch_entry_state is None:
     traffic_dispatch_entry_state=candidate;traffic_dispatch_entry_state_tick=tick
    else:assert candidate==traffic_dispatch_entry_state,'one-shot dispatcher state buffer changed after capture'
  if traffic_collision_entry_state_trace:
   ap,ao,al=memory_maps[0x2000000];assert al==0x40000
   candidate=C.string_at(ap+ao+0x3fb00,136)
   if candidate[:4]==b'PCSD':
    if traffic_collision_entry_state is None:
     traffic_collision_entry_state=candidate;traffic_collision_entry_state_tick=tick
    else:assert candidate==traffic_collision_entry_state,'one-shot collision-entry buffer changed after capture'
  if traffic_follow_entry_trace:
   fp,fo,fl=memory_maps[traffic_follow_entry_memory_base]
   assert fl==(0x8000 if traffic_follow_entry_memory_base==0x03000000 else 0x40000)
   follow_log=fp+fo+traffic_follow_entry_log_offset
   used=struct.unpack('<I',C.string_at(follow_log,4))[0]
   assert not (used&0x80000000),('collision-entry stream buffer overflow',used,tick)
   assert used<=traffic_follow_entry_capacity,('invalid collision-entry stream count',used,tick)
   output_used=struct.unpack('<I',C.string_at(follow_log+traffic_follow_entry_output_count_offset,4))[0]
   assert not (output_used&0x80000000),('collision-entry return stream overflow',output_used,tick)
   pending_before=len(traffic_follow_entry_pending)
   assert used>=pending_before and output_used<=used,('collision-entry stream counter regressed',used,output_used,pending_before,tick)
   new_count=used-pending_before
   batch_inputs=[]
   for event_index in range(used):
    event_head=C.string_at(follow_log+4+event_index*256,8)
    batch_inputs.append(dict(counter=struct.unpack_from('<I',event_head,0)[0],actor_pointer=hex(struct.unpack_from('<I',event_head,4)[0])))
   traffic_follow_entry_batches.append(dict(tick=tick,input_records=used,return_records=output_used,pending_before=pending_before,new_entries=new_count,inputs=batch_inputs))
   for event_index in range(pending_before,used):
    raw=C.string_at(follow_log+4+event_index*256,136)
    traffic_follow_entry_pending.append(dict(entry_tick=tick,source_counter=struct.unpack_from('<I',raw,0)[0]))
   if used-output_used>1:
    incomplete=C.string_at(follow_log,traffic_follow_entry_log_bytes)
    (out/(label+'.traffic-follow-entry-return-incomplete.bin')).write_bytes(incomplete)
    candidates={}
    for base in (0x3c000,0x3d000,0x3e000,0x3f000):
     snapshot=C.string_at(ap+ao+base,0x1000)
     candidates[hex(base)]=dict(input_count=struct.unpack_from('<I',snapshot,0)[0],output_count=struct.unpack_from('<I',snapshot,0xf04)[0],first_record=snapshot[4:12].hex(),nonzero_bytes=sum(byte!=0 for byte in snapshot))
    candidates[hex(traffic_follow_entry_log_address)]=dict(input_count=used,output_count=output_used,first_record=C.string_at(follow_log+4,8).hex(),nonzero_bytes=sum(byte!=0 for byte in incomplete),memory_region='iwram' if traffic_follow_entry_memory_base==0x03000000 else 'ewram')
    incomplete_report=dict(tick=tick,recent_batches=traffic_follow_entry_batches[-20:],candidate_regions=candidates,source_rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Fail-closed diagnostic. The host records recent batch counts and candidate RAM buffer contents to localize mismatched per-call return pairing.')
    (out/(label+'.traffic-follow-entry-return-incomplete.json')).write_text(json.dumps(incomplete_report,indent=2)+'\n')
    if '--actor-trace' in sys.argv:
     (out/(label+'.actor-trace.partial.json')).write_text(json.dumps(actor_trace,indent=2))
    (out/(label+'.traffic-follow-entry-trace.partial.json')).write_text(json.dumps(dict(status='INCOMPLETE',completed_events=len(traffic_follow_entry_events),call_events=traffic_follow_entry_events,scope='Partial routine entry/return observations only; the failing VBlank batch is excluded and no events from it are accepted.'),indent=2))
    (out/(label+'.capture-incomplete.json')).write_text(json.dumps(dict(status='INCOMPLETE_TRAFFIC_ENTRY_RETURN_PAIRING',tick=tick,input_records=used,return_records=output_used,partial_actor_trace=label+'.actor-trace.partial.json' if '--actor-trace' in sys.argv else None,incomplete_batch=label+'.traffic-follow-entry-return-incomplete.json'),indent=2)+'\n')
    raise AssertionError(('more than one unreturned follow call at VBlank',used,output_used,tick))
   if used>output_used:traffic_follow_entry_deferred_returns+=1
   traffic_follow_entry_max_records=max(traffic_follow_entry_max_records,used)
   for event_index in range(output_used):
    raw=C.string_at(follow_log+4+event_index*256,256)
    counter,actor_pointer,argument_r1=struct.unpack_from('<III',raw,0)
    meta=traffic_follow_entry_pending[event_index]
    assert counter==meta['source_counter'],('entry/return source counter mismatch',counter,meta,tick)
    event=dict(call_index=len(traffic_follow_entry_events),observed_after_core_tick=meta['entry_tick'],observed_entry_tick=meta['entry_tick'],observed_return_tick=tick,source_counter=counter,actor_pointer=hex(actor_pointer),argument_r1=argument_r1,player_position=list(struct.unpack_from('<iii',raw,12)),player_heading=struct.unpack_from('<i',raw,24)[0],player_speed_544=struct.unpack_from('<i',raw,28)[0],player_maximum_550=struct.unpack_from('<i',raw,32)[0],actor_raw=raw[36:128].hex(),player_after_position=list(struct.unpack_from('<iii',raw,136)),player_after_heading=struct.unpack_from('<i',raw,148)[0],player_after_speed_544=struct.unpack_from('<i',raw,152)[0],actor_after_raw=raw[156:248].hex(),cooldown_after_e4=struct.unpack_from('<H',raw,248)[0],near_miss_after_25d3=raw[250],rng_after_3b04=struct.unpack_from('<I',raw,252)[0])
    if traffic_follow_entry_selector_trace:event['selector_counter_4c98_entry']=struct.unpack_from('<I',raw,128)[0]
    else:event.update(cooldown_e4=struct.unpack_from('<H',raw,128)[0],near_miss_25d3=raw[130],rng_3b04=struct.unpack_from('<I',raw,132)[0])
    traffic_follow_entry_events.append(event)
   remaining_records=[C.string_at(follow_log+4+i*256,256) for i in range(output_used,used)]
   traffic_follow_entry_pending=traffic_follow_entry_pending[output_used:]
   C.memset(follow_log,0,traffic_follow_entry_log_bytes)
   for event_index,pending_record in enumerate(remaining_records):
    pending_buffer=C.create_string_buffer(pending_record,256)
    C.memmove(follow_log+4+event_index*256,pending_buffer,256)
   if remaining_records:
    count_buffer=C.create_string_buffer(struct.pack('<I',len(remaining_records)),4)
    C.memmove(follow_log,count_buffer,4)
   if used or output_used or new_count:
    traffic_follow_entry_log_drains+=1
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
  if tick in state_checkpoint_ticks:
   n=core.retro_serialize_size();checkpoint=C.create_string_buffer(n);assert core.retro_serialize(checkpoint,n)
   checkpoint_bytes=checkpoint.raw
   version,_,crc=struct.unpack_from('<3I',checkpoint_bytes)
   assert n==397888 and version==0x01000009 and crc==zlib.crc32(core_rom)&0xffffffff
   checkpoint_path=out/(label+f'.state-vblank-{tick:05d}.state')
   checkpoint_path.write_bytes(checkpoint_bytes)
   state_checkpoint_hashes[str(tick)]=dict(path=str(checkpoint_path),sha256=hashlib.sha256(checkpoint_bytes).hexdigest(),core_crc=hex(crc))
  if tick in iwram_checkpoint_ticks:
   iwram_pointer,iwram_offset,iwram_length=memory_maps[0x3000000]
   assert iwram_pointer and iwram_length==0x8000
   iwram_bytes=C.string_at(iwram_pointer+iwram_offset,iwram_length)
   iwram_path=out/(label+f'.iwram-vblank-{tick:05d}.bin');iwram_path.write_bytes(iwram_bytes)
   iwram_checkpoint_hashes[str(tick)]=dict(path=str(iwram_path),sha256=hashlib.sha256(iwram_bytes).hexdigest())
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
    entry['route_driver']=route_driver_state
    entry['completed_count']=C.string_at(ap+ao+0x1692,1)[0]
    entry['fare_whole']=struct.unpack_from('<i',C.string_at(ap+ao+0x1698,4))[0]
    entry['fare_fraction']=struct.unpack_from('<i',C.string_at(ap+ao+0x169c,4))[0]
    entry['remaining_clock']=struct.unpack_from('<i',C.string_at(ap+ao+0x16a8,4))[0]
   entry['trip_clock_state']=dict(trip_time=struct.unpack_from('<i',C.string_at(ap+ao+0x16ac,4))[0],initial_trip_time=struct.unpack_from('<i',C.string_at(ap+ao+0x16b0,4))[0],trip_active=C.string_at(ap+ao+0x1694,1)[0],trip_display_state=C.string_at(ap+ao+0x1695,1)[0],ended=C.string_at(ap+ao+0x1697,1)[0],bonus_gate=struct.unpack_from('<i',C.string_at(ap+ao+0x1700,4))[0],trip_allowance=struct.unpack_from('<i',C.string_at(ap+ao+0x16e0,4))[0],end_bonus=struct.unpack_from('<i',C.string_at(ap+ao+0x16e8,4))[0],bonus_display_timer=struct.unpack_from('<i',C.string_at(ap+ao+0x16e4,4))[0])
   entry['trip_start_inputs']={'settings_0':C.string_at(ap+ao+0x1eb0,1)[0],'settings_3':C.string_at(ap+ao+0x1eb3,1)[0]}
   active_pointer=struct.unpack_from('<I',C.string_at(ap+ao+0x1688,4))[0]
   if 0x02005f2c<=active_pointer<0x02005f2c+128*0x2c:
    customer_index=(active_pointer-0x02005f2c)//0x2c
    customer=C.string_at(ap+ao+(active_pointer-0x02000000),0x2c)
    entry['trip_start_inputs'].update(customer_index=customer_index,customer_category=customer[2],route_length=struct.unpack_from('<i',customer,4)[0],customer_record_hex=customer.hex())
   actor_trace.append(entry)
   if route_follow and entry.get('completed_count',0)>0:break
 if route_follow:frames=len(actor_trace)
 if cpu_trace:(out/(label+'.cpu-timing-trace.json')).write_text(json.dumps(dict(scope='Offline original core saved CPU register/cycle snapshots at VBlank returns; raw savedPC includes CPU pipeline semantics, not instruction-entry tracing. State format guarded by observed magic9/size/title/CRC; installed core release not identified. Never used by native runtime.',format_reference='https://raw.githubusercontent.com/mgba-emu/mgba/master/include/mgba/internal/gba/serialize.h',records=cpu_trace),indent=2))
 if actor_trace:
  trace_rows=actor_trace
  if route_follow and '--route-compact-trace' in sys.argv:
   fields=('tick','input_keys','heading','speed','delta','position','route_driver','completed_count','fare_whole','fare_fraction','remaining_clock','trip_clock_state')
   trace_rows=[]
   for i,row in enumerate(actor_trace):
    before=actor_trace[i-1] if i else None
    event=before is None or i==len(actor_trace)-1 or row['tick']%25==0
    if before:
     event=event or row.get('route_driver',{}).get('waypoint')!=before.get('route_driver',{}).get('waypoint')
     event=event or row.get('route_driver',{}).get('shoulder_pulse',0)!=before.get('route_driver',{}).get('shoulder_pulse',0)
     event=event or row.get('route_driver',{}).get('traffic_avoid_events',0)!=before.get('route_driver',{}).get('traffic_avoid_events',0)
     event=event or row.get('route_driver',{}).get('traffic_gate_events',0)!=before.get('route_driver',{}).get('traffic_gate_events',0)
     event=event or row.get('trip_clock_state',{}).get('trip_active')!=before.get('trip_clock_state',{}).get('trip_active')
     event=event or row.get('completed_count')!=before.get('completed_count') or row.get('remaining_clock')==0
    if event:trace_rows.append({key:row[key] for key in fields if key in row})
  trace_json=json.dumps(trace_rows,indent=2)
  trace_path=out/(label+'.actor-trace.json.gz' if '--actor-trace-gzip' in sys.argv else label+'.actor-trace.json')
  if '--actor-trace-gzip' in sys.argv:
   with gzip.open(trace_path,'wt',encoding='utf-8') as trace_file:trace_file.write(trace_json)
  else:trace_path.write_text(trace_json)
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
 if traffic_spawn_return_trace:
  used=struct.unpack_from('<I',ram,0x3f000)[0]
  assert used<=255,('spawn-return log overflow',used)
  events=[]
  for index in range(used):
   counter,y,flags,countdown=struct.unpack_from('<IiIi',ram,0x3f004+index*16)
   ticks=[row['tick'] for row in actor_trace if row.get('counter')==counter]
   events.append(dict(index=index,source_counter=counter,matching_vblank_ticks=ticks,actor_slot=traffic_spawn_return_slot,actor_pointer=hex(0x020000f4+traffic_spawn_return_slot*0x5c),y=y,flags=flags,countdown=countdown))
  assert events,'selected actor spawn-return event was not recorded'
  spawn_report=dict(status='PASS_ORIGINAL_SPAWN_RETURN_CAPTURE',rom_sha256=hashlib.sha256(rom).hexdigest(),core_sha256=hashlib.sha256(corepath.read_bytes()).hexdigest(),frames=frames,instrumentation=traffic_spawn_return_instrumentation,record_count=used,events=events,scope='Short source-ROM diagnostic capture. Each record is after the original selected actor spawn routine returns and before the original dispatcher proceeds to its common actor presentation/height query. Per-VBlank actor state is in the paired actor trace. Instrumentation exists only in a temporary in-memory ROM copy; it is not proof of a complete frame cadence or native game behavior.')
  (out/(label+'.traffic-spawn-return-trace.json')).write_text(json.dumps(spawn_report,indent=2))
 if traffic_slot_y_write_trace:
  used=struct.unpack_from('<I',ram,0x3f000)[0]
  assert used<=255,('Y-write log overflow',used)
  events=[]
  for index in range(used):
   counter,y,flags,route=struct.unpack_from('<IiII',ram,0x3f004+index*16)
   ticks=[row['tick'] for row in actor_trace if row.get('counter')==counter]
   events.append(dict(index=index,source_counter=counter,matching_vblank_ticks=ticks,actor_slot=traffic_slot_y_write_slot,actor_pointer=hex(0x020000f4+traffic_slot_y_write_slot*0x5c),written_y=y,flags=flags,route_pointer=hex(route)))
  assert events,'slot10 Y store was not recorded'
  report=dict(status='PASS_ORIGINAL_ACTOR_Y_STORE_CAPTURE',rom_sha256=hashlib.sha256(rom).hexdigest(),core_sha256=hashlib.sha256(corepath.read_bytes()).hexdigest(),frames=frames,instrumentation=traffic_slot_y_write_instrumentation,record_count=used,events=events,scope='Short source-ROM diagnostic. The wrapper executes the original actor Y store and logs its r0 value for the selected actor slot, then replays the displaced next instruction. Probe impact is assessed separately against an uninstrumented fixed-input trace; this does not establish complete native gameplay parity.')
  (out/(label+'.traffic-slot-y-write-trace.json')).write_text(json.dumps(report,indent=2))
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
  used=struct.unpack_from('<I',ram,0x3f000)[0];assert used<=240 and used==traffic_dispatch_used
  (out/(label+'.traffic-dispatch-entry-trace.json')).write_text(json.dumps(dict(instrumentation=traffic_dispatch_instrumentation,logged_count=len(traffic_dispatch_events),final_buffer_count=used,log_drains=traffic_dispatch_log_drains,log_saturated=False,call_events=traffic_dispatch_events,scope='Diagnostic per-entry snapshots from the supplied ROM. The temporary log buffer is drained and cleared when full; each service tick also captures full player and camera state. This narrows dispatch phase and does not establish native gameplay parity.'),indent=2))
 if traffic_dispatch_entry_state_trace:
  assert traffic_dispatch_entry_state is not None,'requested dispatcher-entry event was not observed'
  magic,counter,delta=struct.unpack_from('<III',traffic_dispatch_entry_state,0)
  assert magic==0x44534350 and counter==4286
  (out/(label+'.traffic-dispatch-entry-state.bin')).write_bytes(traffic_dispatch_entry_state)
  (out/(label+'.traffic-dispatch-entry-state.json')).write_text(json.dumps(dict(instrumentation=traffic_dispatch_entry_state_instrumentation,observed_after_core_tick=traffic_dispatch_entry_state_tick,source_counter=counter,incoming_r0_delta=delta,player_position=list(struct.unpack_from('<iii',traffic_dispatch_entry_state,12)),player_heading=struct.unpack_from('<i',traffic_dispatch_entry_state,24)[0],player_speed_544=struct.unpack_from('<i',traffic_dispatch_entry_state,28)[0],player_maximum_550=struct.unpack_from('<i',traffic_dispatch_entry_state,32)[0],actor0_raw=traffic_dispatch_entry_state[36:128].hex(),cooldown_e4=struct.unpack_from('<H',traffic_dispatch_entry_state,128)[0],near_miss_25d3=traffic_dispatch_entry_state[130],rng_3b04=struct.unpack_from('<I',traffic_dispatch_entry_state,132)[0],raw_sha256=hashlib.sha256(traffic_dispatch_entry_state).hexdigest(),scope='Exact entry-time state written before the original dispatcher prologue and body. VBlank actor trace from the same tick remains a post-retro_run result and is stored separately; this probe is not used by the native runtime.'),indent=2))
 if traffic_collision_entry_state_trace:
  assert traffic_collision_entry_state is not None,'requested actor0 routine entry was not observed'
  magic,counter,actor_pointer=struct.unpack_from('<III',traffic_collision_entry_state,0)
  expected_counter=traffic_postimpact_entry_counter if traffic_postimpact_entry_state_trace else 4286
  expected_actor=0x020000f4+traffic_postimpact_entry_slot*0x5c if traffic_postimpact_entry_state_trace else 0x020000f4
  assert magic==0x44534350 and counter==expected_counter and actor_pointer==expected_actor
  suffix='traffic-postimpact-entry-state' if traffic_postimpact_entry_state_trace else 'traffic-collision-entry-state'
  routine='0x08006CE8 post-impact' if traffic_postimpact_entry_state_trace else '0x08006908 collision/follow'
  (out/(label+'.'+suffix+'.bin')).write_bytes(traffic_collision_entry_state)
  (out/(label+'.'+suffix+'.json')).write_text(json.dumps(dict(instrumentation=traffic_collision_entry_state_instrumentation,observed_after_core_tick=traffic_collision_entry_state_tick,source_counter=counter,target_slot=traffic_postimpact_entry_slot if traffic_postimpact_entry_state_trace else 0,incoming_r0_actor_pointer=hex(actor_pointer),player_position=list(struct.unpack_from('<iii',traffic_collision_entry_state,12)),player_heading=struct.unpack_from('<i',traffic_collision_entry_state,24)[0],player_speed_544=struct.unpack_from('<i',traffic_collision_entry_state,28)[0],player_maximum_550=struct.unpack_from('<i',traffic_collision_entry_state,32)[0],actor_raw=traffic_collision_entry_state[36:128].hex(),actor0_raw=traffic_collision_entry_state[36:128].hex(),cooldown_e4=struct.unpack_from('<H',traffic_collision_entry_state,128)[0],near_miss_25d3=traffic_collision_entry_state[130],rng_3b04=struct.unpack_from('<I',traffic_collision_entry_state,132)[0],raw_sha256=hashlib.sha256(traffic_collision_entry_state).hexdigest(),scope=f'Exact {routine} input state for actor slot{traffic_postimpact_entry_slot if traffic_postimpact_entry_state_trace else 0} at counter{counter}; original prologue/body execute immediately afterward. Probe is temporary in-memory ROM only and does not alter the supplied ROM or native runtime.'),indent=2))
 if traffic_follow_entry_trace:
   fp,fo,fl=memory_maps[traffic_follow_entry_memory_base]
   follow_log=fp+fo+traffic_follow_entry_log_offset
   used=struct.unpack('<I',C.string_at(follow_log,4))[0];output_used=struct.unpack('<I',C.string_at(follow_log+traffic_follow_entry_output_count_offset,4))[0]
   assert used==output_used==0 and not traffic_follow_entry_pending and traffic_follow_entry_log_drains<=frames,('unreturned follow calls at capture end',used,output_used,len(traffic_follow_entry_pending))
   (out/(label+'.traffic-follow-entry-trace.json')).write_text(json.dumps(dict(instrumentation=traffic_follow_entry_instrumentation,record_count=len(traffic_follow_entry_events),max_records_per_vblank=traffic_follow_entry_max_records,buffer_drains=traffic_follow_entry_log_drains,vblank_spanning_return_frames=traffic_follow_entry_deferred_returns,call_events=traffic_follow_entry_events,scope='ROM-derived exact per-actor inputs and return-state outputs from 0x08006908, drained after each source retro_run. A call that straddles VBlank is retained and paired with its later return before the host compacts the buffer. The return probe is at the function sole exit; complete per-call record pairs are required. Diagnostic only; not used by native runtime.'),indent=2))
 if audio_busy_return_trace:
  (out/(label+'.audio-busy-return-trace.json')).write_text(json.dumps(dict(instrumentation=audio_busy_return_instrumentation,record_count=len(audio_busy_return_events),call_events=audio_busy_return_events,scope='Direct raw return values from the original busy helper call in the temporary source-ROM capture. The displaced post-call instruction is replayed. Does not alter production runtime.'),indent=2))
 if audio_context_start_trace:
  (out/(label+'.audio-context-start-trace.json')).write_text(json.dumps(dict(instrumentation=audio_context_start_instrumentation,record_count=len(audio_context_start_events),call_events=audio_context_start_events,scope='Direct start-handle returns from the original contextual voice caller in a temporary source-ROM capture. The displaced literal load and branch are replayed. Does not alter production runtime.'),indent=2))
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
 report=dict(status='PASS',frames=frames,dimensions=[w,h],pixel_format=fmt,ewram_bytes=size,input_press_ticks=schedule,environment_commands=sorted(env_calls),rom_sha256=hashlib.sha256(rom).hexdigest(),core_sha256=hashlib.sha256(corepath.read_bytes()).hexdigest(),instrumentation=instrumentation,traffic_spawn_return_instrumentation=traffic_spawn_return_instrumentation,traffic_dispatch_instrumentation=traffic_dispatch_instrumentation,traffic_dispatch_entry_state_instrumentation=traffic_dispatch_entry_state_instrumentation,traffic_collision_entry_state_instrumentation=traffic_collision_entry_state_instrumentation if not traffic_postimpact_entry_state_trace else None,traffic_postimpact_entry_state_instrumentation=traffic_collision_entry_state_instrumentation if traffic_postimpact_entry_state_trace else None,traffic_follow_entry_instrumentation=traffic_follow_entry_instrumentation,voice_reset_instrumentation=voice_reset_instrumentation,callback_cycle_instrumentation=callback_cycle_instrumentation,state_crc_rebase=state_crc_rebase,state_checkpoints=state_checkpoint_hashes,iwram_checkpoints=iwram_checkpoint_hashes,direct_input_schedule=dict(path=str(direct_input_path),sha256=direct_input_sha256,rows=len(direct_input_rows)) if direct_input_rows is not None else None,scope='Independent mGBA headless original-ROM execution and raw pixel/RAM capture with recorded scripted input; not native-port runtime or completed gameplay proof.')
 (out/(label+'.json')).write_text(json.dumps(report,indent=2));print({'status':report['status'],'frames':frames,'dimensions':report['dimensions'],'pressed_frames':len(schedule),'rom_sha256':report['rom_sha256']})
 if route_follow:
  first_active=next((entry for entry in actor_trace if entry['route_driver']['trip_active']),None)
  last=actor_trace[-1]['route_driver'];hist={}
  for a,b in zip(actor_trace,actor_trace[1:]):
   if a['previous']!=b['previous']:
    delta=(b['previous']-a['previous'])&0xffffffff;hist[str(delta)]=hist.get(str(delta),0)+1
  capture_status='PASS_SOURCE_ROUTE31_DELIVERY_CAPTURE' if first_active and actor_trace[-1]['completed_count']>0 else 'INCOMPLETE_SOURCE_ROUTE31_CAPTURE'
  trace_path=out/(label+'.actor-trace.json.gz' if '--actor-trace-gzip' in sys.argv else label+'.actor-trace.json')
  route_report=dict(status=capture_status,frames=frames,route_source=31,route_points=len(route_points),route_start_waypoint=route_start_waypoint,route_waypoint_radius=route_waypoint_radius,route_lookahead=route_lookahead,route_lookahead_speed_divisor=route_lookahead_divisor,route_brake_speed=route_brake_speed,route_steer_during_brake=route_steer_during_brake,route_avoid_traffic=route_avoid_traffic,route_avoid_distance=route_avoid_distance,route_avoid_width=route_avoid_width,route_avoid_hold=route_avoid_hold,route_avoid_collision_gate=route_avoid_collision_gate,route_gate_avoid_on_replay=route_gate_avoid_on_replay,route_gate_margin=route_gate_margin,route_gate_hold=route_gate_hold,first_trip_tick=first_active['tick'] if first_active else None,first_trip_counter=first_active['counter'] if first_active else None,final_waypoint=last['waypoint'],trip_active_at_end=last['trip_active'],completed_count=actor_trace[-1]['completed_count'],fare=[actor_trace[-1]['fare_whole'],actor_trace[-1]['fare_fraction']],remaining_clock=actor_trace[-1]['remaining_clock'],source_update_interval_histogram=dict(sorted(hist.items(),key=lambda kv:int(kv[0]))),traffic_actor_trace=traffic_actor_trace,traffic_actor_record_count=50 if traffic_actor_trace else 0,traffic_actor_record_layout='EWRAM+0xF4, stride0x5C, raw records only; route/path dispatch and undocumented bytes remain uninterpreted.' if traffic_actor_trace else None,input_schedule_scope='Per-VBlank input_keys in the paired actor trace are the exact adaptive test-driver schedule; the controller used only captured ROM vehicle pose/state and recovered route31 coordinates. It did not write ROM RAM. This is not human input or a general source cadence model.',scope='Original ROM route-following reference capture from a saved gameplay state, using a scripted adaptive test driver. Steering while braking is an opt-in input-schedule variant, not a source physics change. Only count as a full source-route capture if trip_active starts and completed_count advances; otherwise this remains a pickup/navigation probe.',rom_sha256=report['rom_sha256'],core_sha256=report['core_sha256'],state_sha256=hashlib.sha256((out/(sys.argv[3]+'.state')).read_bytes()).hexdigest(),actor_trace_sha256=hashlib.sha256(trace_path.read_bytes()).hexdigest(),actor_trace_file=trace_path.name)
  if route_replay_path:
   route_report['input_replay']={'file':str(route_replay_path),'sha256':route_replay_sha256,'rows':len(route_replay_rows),'consumed':route_replay_index,'steer_inverted':route_replay_invert_steer,'update_boundary_mode':route_replay_update_boundary,'fields':'steer (-1=left, 1=right), brake; accelerate held except brake; no shoulder actions','source':'Native route31 input-only delivery probe CSV. This is a cross-runtime input experiment, not a recorded original-human input schedule.'}
   route_report['input_schedule_scope']='Replay of the native route31 test-driver steering/brake decisions in original ROM. In update-boundary mode a decision is held until the source previous-counter value changes, then one input row is consumed; the command is applied before the following core run. This aligns the test harness with observed KEYINPUT commit boundaries but does not imply controller parity or a fully matched host-loop phase. The original ROM supplies movement, collisions, clocks, passenger state, delivery and fare; this probe does not modify ROM RAM.'
  (out/(label+'.route-follow.json')).write_text(json.dumps(route_report,indent=2))
  print({'route_follow':route_report['status'],'first_trip_tick':route_report['first_trip_tick'],'waypoint':route_report['final_waypoint'],'completed_count':route_report['completed_count'],'fare':route_report['fare']})
 if input_callback_trace:
  callback_report={'status':'PASS_INPUT_CALLBACK_BOUNDARY_TRACE','rom_sha256':hashlib.sha256(rom).hexdigest(),'core_sha256':hashlib.sha256(pathlib.Path(corepath).read_bytes()).hexdigest(),'frames':frames,'input_poll_rows':len(input_poll_events),'input_state_first_query_rows':len(input_callback_events),'input_state_calls_by_tick':input_callback_counts,'input_poll_first_by_tick':input_poll_events,'input_state_first_by_tick':input_callback_events,'scope':'Read-only logging in libretro frontend callbacks. First input_poll and input_state observations per core frame are paired with source counters/input state while preserving the existing input return values. This identifies host callback entry observations but cannot by itself reconstruct sub-instruction CPU ordering inside the ROM.'}
  (out/(label+'.input-callback-trace.json')).write_text(json.dumps(callback_report,indent=2))
  print({'input_callback_trace':callback_report['status'],'poll_rows':len(input_poll_events),'state_rows':len(input_callback_events)})
finally:core.retro_unload_game();core.retro_deinit()
assert source.read_bytes()==rom