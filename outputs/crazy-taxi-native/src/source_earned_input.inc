/* Opt-in diagnostic navigator. Only ordinary XInput samples leave this
   helper. It never enables the legacy command/physics overrides. Canonical
   game owners are compared around every navigation calculation. */
static struct {
 unsigned begun,samples,read_only_checks,pickup_seen,trip_seen,done,phase,rows,written;
 uint32_t begin_counter;int32_t initial_clock;int selected,destination;
 int64_t trace[12000][23];
} source_earned;
static void source_earned_write(void){
 if(!source_earned_requested||source_earned.written||!source_earned_trace_path)return;
 source_earned.written=2;FILE *f=fopen(source_earned_trace_path,"wb");
 if(!f){fputs("Earned input trace open failed.\n",stderr);return;}
 fputs("counter,delta,clock,x,y,z,speed,heading,selected,destination,pending,trip,completed,cash,fraction,rank,qualified,phase,rt,lt,lx,keys,intro\n",f);
 for(unsigned i=0;i<source_earned.rows;i++)for(unsigned j=0;j<23;j++)fprintf(f,"%s%lld%s",j?",":"",(long long)source_earned.trace[i][j],j==22?"\n":"");
 int error=ferror(f);if(fclose(f))error=1;if(error){fputs("Earned input trace write failed.\n",stderr);return;}source_earned.written=1;
}
static int16_t source_earned_stick(int command){
 if(command>192||command< -192)fail("Earned navigator analog range invalid.");
 if(!command)return 0;
 int magnitude=abs(command);if(magnitude<8)magnitude=8;
 int maximum=command>0?32768:32767;
 int value=7850+((magnitude-8)*(maximum-7850)+183)/184;
 int16_t stick=(int16_t)(command>0?-value:value);
 if(taxi_xbox_steering_command(stick)!=(command>0?magnitude:-magnitude))fail("Earned navigator XInput inverse mismatch.");
 return stick;
}
static TaxiXboxSample source_earned_pad(int32_t delta){
 TaxiXboxSample pad=taxi_xbox_decode(0,0,0,0,0);
 if(source_parent_phase!=SOURCE_PARENT_DRIVE)return pad;
 if(route_probe_active||source_retry_smoke||source_parent_results_smoke||source_parent_reentry_smoke||source_frame_lifecycle_smoke)fail("Earned input check has a competing diagnostic override.");
 if(!source_earned.begun){
  if(scene_index||native_game_driver||native_settings[7]||(native_settings[0]&224)||passenger_scene.session.fare.total_whole_1698||passenger_scene.session.end.completed_1692)fail("Earned input check requires fresh City1 driver0 ordinary state.");
  source_earned.begun=1;source_earned.begin_counter=live_host_time.counter;source_earned.initial_clock=passenger_scene.session.clock.remaining_16a8;
  source_earned.selected=source_earned.destination=-1;
  if(source_earned.initial_clock!=taxi_game_initial_clock(&passenger_scene.session.settings,0))fail("Earned input initial clock differs from source settings.");
  printf("NATIVE EARNED BEGIN: counter%u clock%ld mode%u city1 driver0 unlock%u; normal sampled XInput only, no clock/rank/pose/fare fixture.\n",live_host_time.counter,(long)source_earned.initial_clock,native_settings[0]&224,native_settings[7]);fflush(stdout);
 }
 if(passenger_scene.pickup.pending_1704){source_earned.pickup_seen=1;source_earned.selected=passenger_scene.pickup.selected_index;}
 if(passenger_scene.session.trip.active_1694){source_earned.trip_seen=1;source_earned.selected=passenger_scene.pickup.selected_index;source_earned.destination=passenger_scene.pickup.destination_index;}
 if(passenger_scene.session.end.completed_1692){
  if(passenger_scene.session.end.completed_1692!=1||!source_earned.pickup_seen||!source_earned.trip_seen||passenger_scene.session.trip.active_1694||passenger_scene.pickup.pending_1704||passenger_scene.pickup.destination_index!=-1||passenger_scene.session.fare.total_whole_1698<=0||passenger_scene.session.fare.pending_whole_16a0||car.world.speed_544||!live_delivery_route)fail("Earned input observed incomplete native settlement.");
  source_earned.done=1;
 }
 source_earned.phase=source_earned.done?4:live_game_start.active||city_fade.active_177b?0:passenger_scene.session.trip.active_1694?3:passenger_scene.pickup.pending_1704?2:1;
 if(source_earned.phase>0&&source_earned.phase<4){
  TaxiDriveWorld held_car=car;TaxiPassengerScene held_scene=passenger_scene;TaxiHostTime held_time=live_host_time;
  uint8_t held_context[sizeof drive_context],held_settings[sizeof native_settings];memcpy(held_context,&drive_context,sizeof held_context);memcpy(held_settings,native_settings,sizeof held_settings);
  int old_brake=route_probe_brake,old_steer=script_steer,old_analog=route_probe_analog_command;
  if(source_earned.phase==3){++startup_pickup_audio_driving;live_delivery_input(1);}
  else startup_pickup_audio_input();
  int brake=route_probe_brake,command=source_earned.phase==3?route_probe_analog_command:script_steer<0?192:script_steer>0?-192:0;
  route_probe_brake=old_brake;script_steer=old_steer;route_probe_analog_command=old_analog;
  if(memcmp(&held_car,&car,sizeof car)||memcmp(&held_scene,&passenger_scene,sizeof passenger_scene)||memcmp(&held_time,&live_host_time,sizeof live_host_time)||memcmp(held_context,&drive_context,sizeof drive_context)||memcmp(held_settings,native_settings,sizeof native_settings))fail("Earned navigator modified canonical game owners.");
  source_earned.read_only_checks++;pad=taxi_xbox_decode(0,source_earned_stick(command),0,brake?0:255,brake?255:0);
 }else if(source_earned.done)pad=taxi_xbox_decode(0,0,0,0,255);
 if(earned_selection_enabled)pad=earned_selection_pad(pad,delta);
 if(source_earned.rows>=12000)fail("Earned input diagnostic trace capacity reached.");
 int64_t row[23]={live_host_time.counter,delta,passenger_scene.session.clock.remaining_16a8,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,car.world.heading_20,passenger_scene.pickup.selected_index,passenger_scene.pickup.destination_index,passenger_scene.pickup.pending_1704,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692,passenger_scene.session.fare.total_whole_1698,passenger_scene.session.fare.total_fraction_169c,passenger_scene.session.rank_16d4,passenger_scene.session.fare.qualified_1691,source_earned.phase,pad.accelerator,pad.brake,pad.steering,taxi_xbox_driving_keys(pad,(uint8_t)((native_settings[0]>>1)&3)),live_game_start.active};
 memcpy(source_earned.trace[source_earned.rows++],row,sizeof row);source_earned.samples++;
 if(source_earned.samples%120==1||source_earned.done){printf("NATIVE EARNED INPUT: sample%u counter%u phase%u pending%ld trip%u complete%u cash%ld position%ld,%ld speed%ld pad%d/%u/%u.\n",source_earned.samples,live_host_time.counter,source_earned.phase,(long)passenger_scene.pickup.pending_1704,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698,(long)car.world.height.position.x,(long)car.world.height.position.z,(long)car.world.speed_544,pad.steering,pad.accelerator,pad.brake);fflush(stdout);}
 return pad;
}
