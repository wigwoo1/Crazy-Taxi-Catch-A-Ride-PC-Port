/* Optional observer; separate explicitly enabled diagnostic input selector below. */
#include "earned_forecast_impl.inc"
#include "earned_candidates_impl.inc"
static struct {unsigned frames,skipped,checked,clear,hazard,unsupported;double total_ms,max_ms;int pending;EarnedForecastResult baseline;} earned_eval;
static struct {unsigned frames,changes,unsupported,no_clear,boundary_checks;int pending;EarnedForecastState expected;} earned_select;
static EarnedForecastState earned_eval_state(void){
 EarnedForecastState s={0};s.car=car;s.context=drive_context;s.buttons=earned_capture_buttons;
 memcpy(s.traffic,traffic_actors,sizeof s.traffic);s.traffic_count=taxi_traffic_density(passenger_scene.session.settings.bytes[3]);
 s.camera=(EarnedForecastCamera){.enabled=1,.heading=camera_heading,.pitch=camera_pitch,.yaw_offset=camera_yaw_offset,.yaw_target=camera_yaw_target,.base=camera_base,.preset_y=live_game_start.axes[1],.preset_z=live_game_start.axes[2],.offsets=camera_offsets,.mode=live_camera_mode_64c};return s;
}
#include "earned_decision_viewer.inc"
static void earned_evaluation_write(void){
 if(!earned_evaluation_path)return;
 FILE *f=fopen(earned_evaluation_path,"wb");if(!f){fputs("Earned evaluation output open failed.\n",stderr);return;}
 fprintf(f,"{\"frames\":%u,\"skipped\":%u,\"baseline_checks\":%u,\"clear_candidates\":%u,\"hazard_candidates\":%u,\"unsupported_candidates\":%u,\"batch_total_ms\":%.9f,\"batch_max_ms\":%.9f,\"controls_changed\":%s,\"horizon_steps\":8,",earned_eval.frames,earned_eval.skipped,earned_eval.checked,earned_eval.clear,earned_eval.hazard,earned_eval.unsupported,earned_eval.total_ms,earned_eval.max_ms,earned_select.changes?"true":"false");
 fprintf(f,"\"selection_frames\":%u,\"selection_changes\":%u,\"selection_unsupported\":%u,\"selection_no_clear\":%u,\"selection_boundary_checks\":%u,\"traffic_only\":%s}\n",earned_select.frames,earned_select.changes,earned_select.unsupported,earned_select.no_clear,earned_select.boundary_checks,earned_selection_traffic_only?"true":"false");
 int bad=ferror(f);if(fclose(f))bad=1;if(bad)fputs("Earned evaluation output write failed.\n",stderr);
}
static void earned_evaluation_begin(TaxiXboxSample pad,int32_t dt){
 earned_eval.pending=0;
 if(earned_select.pending){EarnedForecastState actual=earned_eval_state();if(memcmp(&actual,&earned_select.expected,sizeof actual))fail("Pre-sample forecast boundary differs after native clock/context service.");earned_select.pending=0;earned_select.boundary_checks++;}
 if(source_earned.phase!=3||dt<1||dt>8||passenger_scene.session.clock.remaining_16a8<=dt*8||passenger_scene.session.trip.time_16ac<=dt*8||passenger_scene.pickup.pending_1704||passenger_scene.session.ended_1697||passenger_scene.session.outer_state_1ea8||(live_camera_mode_64c&~16u)!=2){earned_eval.skipped++;return;}
 EarnedForecastState state=earned_eval_state(),held=state;TaxiPassengerScene passenger_before=passenger_scene;TaxiHostTime time_before=live_host_time;TaxiInput buttons_before=driving_buttons;
 unsigned char settings_before[sizeof native_settings];memcpy(settings_before,native_settings,sizeof native_settings);
 EarnedForecastResources resources={&runtime,traffic_assets.roads,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,taxi_world_asset_height};
 EarnedForecastState driver_only=state;driver_only.traffic_count=0;driver_only.camera.enabled=0;
 if(earned_forecast(&driver_only,&resources,&pad,&dt,1,&earned_eval.baseline))fail("Live forecast baseline failed.");
 TaxiXboxSample choices[7];int16_t steering[6]={-32768,-21845,-10922,10922,21845,32767};choices[0]=pad;
 for(unsigned i=1;i<7;i++)choices[i]=taxi_xbox_decode(pad.look_back?0x80:0,steering[i-1],0,pad.accelerator,pad.brake);
 int32_t delta[8];for(unsigned i=0;i<8;i++)delta[i]=dt;
 LARGE_INTEGER begin,end,freq;QueryPerformanceFrequency(&freq);QueryPerformanceCounter(&begin);
 EarnedCandidates result;if(earned_candidates(&state,&resources,choices,7,delta,8,&result))fail("Live candidate evaluation failed.");
 QueryPerformanceCounter(&end);double ms=1000.0*(double)(end.QuadPart-begin.QuadPart)/(double)freq.QuadPart;
 earned_eval.total_ms+=ms;if(ms>earned_eval.max_ms)earned_eval.max_ms=ms;
 for(unsigned i=0;i<7;i++){if(result.candidate[i].status==EARNED_CANDIDATE_CLEAR)earned_eval.clear++;else if(result.candidate[i].status==EARNED_CANDIDATE_HAZARD)earned_eval.hazard++;else if(result.candidate[i].status==EARNED_CANDIDATE_UNSUPPORTED)earned_eval.unsupported++;else fail("Invalid live forecast outcome.");}
 EarnedForecastState after=earned_eval_state();
 if(memcmp(&held,&state,sizeof state)||memcmp(&held,&after,sizeof held)||memcmp(&passenger_before,&passenger_scene,sizeof passenger_before)||memcmp(&time_before,&live_host_time,sizeof time_before)||memcmp(&buttons_before,&driving_buttons,sizeof buttons_before)||memcmp(settings_before,native_settings,sizeof settings_before))fail("Live forecast changed canonical state.");
 earned_eval.frames++;earned_eval.pending=1;
}
static void earned_evaluation_check(int step_result,unsigned world_hit){
 if(!earned_eval.pending)return;earned_eval.pending=0;
 if(step_result||earned_eval.baseline.completed_steps!=1||earned_eval.baseline.world_hit!=world_hit||memcmp(&earned_eval.baseline.state.car,&car,sizeof car)||memcmp(&earned_eval.baseline.state.context,&drive_context,sizeof drive_context)||memcmp(&earned_eval.baseline.state.buttons,&driving_buttons,sizeof driving_buttons))fail("Live forecast baseline differs from actual native driver/world update.");
 earned_eval.checked++;
}

/* Test driver only. Original source dynamics and passenger owners are never
   replaced; returned sample passes through normal source_input processing. */
static TaxiXboxSample earned_selection_pad(TaxiXboxSample pad,int32_t dt){
 if(earned_select.pending)fail("Selected input did not reach the verified native movement boundary.");
 if(source_earned.phase!=3||!source_frame_active||!pad.connected||dt<1||dt>8||((native_settings[0]>>1)&3)||native_pause.menu.frozen||live_game_start.active||city_fade.active_177b||passenger_scene.session.ended_1697||passenger_scene.session.outer_state_1ea8||passenger_scene.session.gate_1700||passenger_scene.pickup.pending_1704||!passenger_scene.session.trip.active_1694||passenger_scene.session.clock.remaining_16a8<=dt*9||passenger_scene.session.trip.time_16ac<=dt*9||(live_camera_mode_64c&~16u)!=2||!live_delivery_route||live_delivery_waypoint>=live_delivery_route->count)return pad;
 EarnedForecastState raw=earned_eval_state();raw.buttons=driving_buttons;EarnedForecastState state=raw;
 state.context.driver.motion.orientation.flag_1694=passenger_scene.session.trip.active_1694;
 state.context.driver.motion.orientation.flag_1697=passenger_scene.session.ended_1697;
 state.context.driver.motion.orientation.flag_1ea8=passenger_scene.session.outer_state_1ea8;
 state.context.driver.motion.orientation.flag_177b=city_fade.active_177b;
 state.context.driver.motion.traction.flag_1696=(uint8_t)live_game_start.active;
 TaxiPassengerScene passengers=passenger_scene;TaxiHostTime time=live_host_time;unsigned char settings[sizeof native_settings];memcpy(settings,native_settings,sizeof settings);
 EarnedForecastResources resources={&runtime,traffic_assets.roads,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,taxi_world_asset_height};
 TaxiXboxSample choices[7];choices[0]=pad;choices[1]=taxi_xbox_decode(pad.look_back?0x80:0,pad.steering,0,0,255);
 const int16_t steering[5]={-32768,-16384,0,16384,32767};for(unsigned i=2;i<7;i++)choices[i]=taxi_xbox_decode(pad.look_back?0x80:0,steering[i-2],0,pad.accelerator,pad.brake);
 int32_t deltas[8];for(unsigned i=0;i<8;i++)deltas[i]=dt;
 EarnedCandidates candidates;if(earned_candidates(&state,&resources,choices,7,deltas,8,&candidates))fail("Pre-sample candidate evaluation failed.");
 unsigned chosen=0;
 if(candidates.candidate[0].status==EARNED_CANDIDATE_UNSUPPORTED)earned_select.unsupported++;
 else if(candidates.candidate[0].status==EARNED_CANDIDATE_HAZARD&&(!earned_selection_traffic_only||candidates.candidate[0].traffic_hit)){
  TaxiRoutePoint target=live_delivery_route->points[live_delivery_waypoint];double best=0;
  for(unsigned i=1;i<7;i++)if(candidates.candidate[i].status==EARNED_CANDIDATE_CLEAR){
   double dx=(double)candidates.candidate[i].position.x-target.x,dz=(double)candidates.candidate[i].position.z-target.z,score=dx*dx+dz*dz;
   if(!chosen||score<best){chosen=i;best=score;}
  }
  if(!chosen)earned_select.no_clear++;
 }
 EarnedForecastState after=earned_eval_state();after.buttons=driving_buttons;
 if(memcmp(&raw,&after,sizeof raw)||memcmp(&passengers,&passenger_scene,sizeof passengers)||memcmp(&time,&live_host_time,sizeof time)||memcmp(settings,native_settings,sizeof settings))fail("Pre-sample selection changed canonical game owners.");
 earned_select.frames++;if(chosen)earned_select.changes++;
 earned_decision_record(&state,choices,&candidates,chosen,dt);
 earned_select.expected=state;earned_select.pending=1;
 return choices[chosen];
}
