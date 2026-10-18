/* Native ordinary expiry parent: actual translated child runtimes, each
   acknowledged only at its real complete flag. Existing UI pacing/backend is
   retained; this is not a new original whole-child26318/PPU proof. */
static void source_parent_result_aliases(void){
 TaxiNormalRetryOwned *q=&source_restart.retry;TaxiNormalExit *s=&q->parent;
 native_pause.menu.outer_state=passenger_scene.session.outer_state_1ea8=native_session_init.outer_1ea8=s->outer;
 native_session_init.session_165e[57]=passenger_scene.session.ended_1697=s->ended;
 native_pause.menu.frozen=native_session_init.effect_5f20[0]=source_viewer.service.frozen_5f20=s->frozen;
 source_parent_exit_aliases();
}
static void source_parent_results_advance(int rc);
static void source_parent_expiry_fade(void){
 if(source_parent_phase!=SOURCE_PARENT_DRIVE||source_viewer.owner.phase!=TAXI_FRAME_IDLE||passenger_scene.session.outer_state_1ea8!=2||passenger_scene.session.ended_1697||native_pause.menu.frozen||native_pause.menu.exit_flag)fail("Native expiry fade boundary invalid.");
 /* Same original1EA8 byte: observe the real BA04 expiry writer. */
 native_pause.menu.outer_state=passenger_scene.session.outer_state_1ea8;
 source_restart_bind(source_parent.frequency);
 if(taxi_normal_expiry_owned_fade(&source_restart.retry))fail("Native original expiry fade failed.");
 city_fade=source_viewer.fade.fade;native_pause.alternate=0;
 printf("NATIVE EXPIRY FADE: entry%u counter%u remaining%ld masks%u/%u qualified%u rank%u; actual BA04 expiry, no pause event.\n",source_parent.entries,live_host_time.counter,(long)passenger_scene.session.clock.remaining_16a8,source_viewer.fade.masks.background_mask,source_viewer.fade.masks.object_mask,passenger_scene.session.fare.qualified_1691,passenger_scene.session.rank_16d4);fflush(stdout);
}
static void source_parent_results_frozen(void){
 if(memcmp(&source_results.held_car,&car,sizeof car)||memcmp(&source_results.held_effects,&native_actor_effects,sizeof native_actor_effects)||memcmp(source_results.held_actors,passenger_scene.customers,sizeof source_results.held_actors)||source_results.held_clock!=passenger_scene.session.clock.remaining_16a8||source_results.frozen_updates!=live_host_updates||source_results.frozen_commits!=source_commits)fail("Native result child advanced driving/car/actors/effects/clock.");
}
static void source_parent_results_finish(void){
 TaxiNormalRetryOwned *q=&source_restart.retry;
 if(source_parent_phase!=SOURCE_PARENT_DRIVE||source_results.active||source_viewer.owner.phase!=TAXI_FRAME_IDLE||city_fade.active_177b||native_pause.menu.exit_flag)fail("Native expiry cleanup boundary invalid.");
 if(taxi_normal_expiry_owned_transfer(q)!=1)fail("Native expiry cleanup failed.");
 source_results.flow=(TaxiNormalExitChildren){0};source_results.active=1;
 source_results.held_car=car;source_results.held_effects=native_actor_effects;memcpy(source_results.held_actors,passenger_scene.customers,sizeof source_results.held_actors);source_results.held_clock=passenger_scene.session.clock.remaining_16a8;source_results.frozen_updates=live_host_updates;source_results.frozen_commits=source_commits;
 source_parent_game_to_menu(&source_results.input);source_parent_phase=SOURCE_PARENT_CHILD;
 int rc=taxi_normal_exit_children_begin(&source_results.flow,&q->parent,&q->requests);source_parent_result_aliases();source_parent_results_advance(rc);
}
static void source_parent_results_advance(int rc){
 TaxiNormalRetryOwned *q=&source_restart.retry;
 while(rc==2){
  if(q->requests.count!=1||q->requests.calls[0]!=source_results.flow.pending)fail("Native result child dispatch invalid.");
  uint32_t target=source_results.flow.pending;source_results.started++;
  printf("NATIVE RESULT CHILD BEGIN: %08x counter%u callback%08x frozen%u redraw%u results%u.\n",target,live_host_time.counter,q->parent.callback,q->parent.frozen,q->parent.redraw,q->parent.results);fflush(stdout);
  switch(target){
  case 0x080243a4:
   source_parent.credits_entries++;source_parent.credits_tick_begin=credits_live_ticks;credits_live_begin(3);break;
  case 0x0802392c:{
   TaxiFareSession *s=&passenger_scene.session;load_results_preview();result_customers=s->end.completed_1692;result_rank=s->rank_16d4;result_class=s->fare.rating_1693;
   if(result_rank>99||result_class>8||taxi_results_flow_begin(&result_live_flow,s->fare.total_whole_1698,s->fare.total_fraction_169c,(int8_t)result_customers,(int8_t)result_rank,(int8_t)result_class,city_fade.interval_1771,city_fade.reload_1772))fail("Native results child initialization failed.");
   result_state=result_live_flow.animation;result_snapshot_cap(0);results_preview=result_live_active=1;break;}
  case 0x080253e4:
   if(!taxi_name_entry_eligible(passenger_scene.session.settings.bytes[0],result_rank)){
    source_results.name_skips++;source_results.completed++;puts("NATIVE RESULT NAME: actual source eligibility guard returned without screen.");
    rc=taxi_normal_exit_children_poll(&source_results.flow,target,&q->requests);continue;
   }
   load_name_ring_preview();name_display_rank=result_rank;
   if(taxi_name_flow_begin(&name_live_flow,&name_ring_layout,160,city_fade.interval_1771,city_fade.reload_1772))fail("Native name child initialization failed.");
   name_live_input=source_results.input;name_preview_state=name_live_flow.entry;name_upload_cap(0);name_live_active=1;break;
  case 0x08024098:game_over_begin_flow(city_fade.interval_1771,city_fade.reload_1772);break;
  case 0x08024674:
   profile_live_begin(city_fade.interval_1771,city_fade.reload_1772);profile_input=source_results.input;
   /* Actual24728..24740 stops/starts menu music, then clears3897. */
   if(q->parent.redraw){if(taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_STOP))fail("Native profile music stop failed.");music_context.sound_active=0;music_test_start(4);q->parent.redraw=0;}
   break;
  default:fail("Unsupported original result child request.");
  }
  QueryPerformanceCounter(&source_viewer.previous);return;
 }
 if(rc!=1||source_results.flow.stage!=6||q->parent.results!=1||q->parent.callback!=0x080174dd)fail("Native results completed before actual child returns.");
 source_parent_results_frozen();source_results.active=0;source_results.returns++;q->phase=4;
 source_parent_menu_to_game(&source_results.input);source_parent_result_aliases();result_live_reset();results_preview=0;
 source_parent_return_menu();
}
static uint16_t source_parent_results_mask(uint32_t target){
 if(!source_parent_results_smoke){uint16_t mask=name_device_mask(taxi_xbox_poll_any());if(GetAsyncKeyState(VK_UP)&0x8000)mask|=64;if(GetAsyncKeyState(VK_DOWN)&0x8000)mask|=128;return mask;}
 if(source_results.child_ticks%4)return 0;
 if(target==0x080243a4)return credits_live_ticks-source_parent.credits_tick_begin>=60?8:0;
 if(target==0x080253e4)return !name_live_flow.fade.active_177b&&!name_live_flow.entry.flying?1:0;
 if(target==0x08024674&&!profile_flow.fade.active_177b&&!profile_flow.choice.exiting){
  if(source_parent_profile_register_smoke){if(profile_flow.choice.editing&&profile_names[profile_flow.choice.current][0]=='A')return 64;return 1;}
  unsigned last=taxi_profile_option_count(profile_preview&1,profile_preview&2)-1;
  return profile_flow.choice.option!=last?128:1;
 }
 return 0;
}
static void source_parent_results_ticks(uint32_t ticks){
 if(!source_results.active||!source_parent_children_active())fail("Native result child scheduler invalid.");
 for(uint32_t i=0;i<ticks&&source_parent_children_active();i++){
  source_results.child_ticks++;uint32_t target=source_results.flow.pending;uint16_t mask=source_parent_results_mask(target);int returned=0;
  switch(target){
  case 0x080243a4:{
   uint16_t pressed=(uint16_t)((source_results.input.pressed[0]?1:0)|(source_results.input.pressed[3]?8:0));credits_live_tick(pressed);taxi_menu_input_step(&source_results.input,mask,1);
   returned=credits_live_runtime.complete;if(returned){city_fade=credits_live_runtime.fade;credits_live_active=0;glDeleteTextures(1,&credits_live_texture);credits_live_texture=0;}break;}
  case 0x0802392c:
   taxi_results_flow_step(&result_live_flow,1,passenger_scene.session.settings.bytes[7]);result_state=result_live_flow.animation;result_snapshot_cap(result_live_flow.palette_cap);taxi_menu_input_step(&source_results.input,mask,1);
   if(result_live_flow.unlock_city)native_settings[7]=passenger_scene.session.settings.bytes[7]=1;
   returned=result_live_flow.complete;if(returned){city_fade=result_live_flow.fade;result_live_active=results_preview=0;}break;
  case 0x080253e4:
   name_interactive_tick(mask);source_results.input=name_live_input;returned=name_live_flow.complete;if(returned){city_fade=name_live_flow.fade;name_live_active=0;}break;
  case 0x08024098:
   game_over_tick_flow();taxi_menu_input_step(&source_results.input,mask,1);returned=game_over_complete;if(returned){city_fade=game_over_fade;game_over_active=0;}break;
  case 0x08024674:
   profile_tick(mask);source_results.input=profile_input;returned=profile_flow.complete&&profile_commit_done;
   if(returned){
    city_fade=profile_flow.fade;memcpy(passenger_scene.session.settings.bytes,native_settings,8);unsigned current=profile_flow.choice.current;const uint8_t *name=native_settings+8+208*current;
    printf("NATIVE PROFILE PROOF: mode%u choice%u current%u fresh%u flags%u/%u name%02x%02x%02x persist%d status%d configured%d write%d.\n",native_settings[0]&224,profile_flow.choice.choice,current,profile_flow.choice.new_profile,native_settings[11],native_settings[219],name[0],name[1],name[2],profile_persist_requested,profile_commit_status,native_save_path!=NULL,native_save_write_status);fflush(stdout);
   }break;
  default:fail("Native result pending target invalid.");
  }
  source_parent_results_frozen();TaxiNormalRetryOwned *q=&source_restart.retry;
  int rc=taxi_normal_exit_children_poll(&source_results.flow,returned?target:0,&q->requests);
  if(returned){source_results.completed++;printf("NATIVE RESULT CHILD RETURN: %08x counter%u next%08x callback%08x redraw%u results%u.\n",target,live_host_time.counter,source_results.flow.pending,q->parent.callback,q->parent.redraw,q->parent.results);fflush(stdout);source_parent_result_aliases();source_parent_results_advance(rc);}
  else if(rc||q->requests.count)fail("Native result child returned before actual completion.");
 }
}
static TaxiXboxSample source_parent_results_pad(void){
 if(source_parent_phase!=SOURCE_PARENT_DRIVE||city_fade.active_177b||live_game_start.active)return taxi_xbox_decode(0,0,0,0,0);
 if(source_parent.entries==1&&!source_results.armed){
  source_results.arm_clock=(uint32_t)passenger_scene.session.clock.remaining_16a8;
  if(!source_parent_results_natural){passenger_scene.session.clock.remaining_16a8=1;puts("NATIVE RESULTS FIXTURE: remaining clock explicitly reduced to1; not a natural full countdown claim.");}
  if(source_parent_results_name_fixture){passenger_scene.session.rank_16d4=1;puts("NATIVE RESULTS FIXTURE: rank1 explicitly set for real name-screen coverage; not earned ranking.");}
  if(source_parent_results_credits_fixture){passenger_scene.session.fare.qualified_1691=1;puts("NATIVE RESULTS FIXTURE: qualified flag explicitly set for real credits branch; not earned qualification.");}
  source_results.armed=1;printf("NATIVE RESULTS CLOCK: original%u armed%u counter%u natural%d rankingloaded%u.\n",source_results.original_clock,source_results.arm_clock,live_host_time.counter,source_parent_results_natural,passenger_scene.session.ranking_loaded);fflush(stdout);
 }
 if(source_parent.entries>=2){
  if(++source_parent.drive_samples>=90){TaxiPoint p=car.world.height.position;if(p.x==source_parent.entry_position.x&&p.z==source_parent.entry_position.z)fail("Native post-results driving did not move.");source_results.done=1;}
  return taxi_xbox_decode(0,0,0,255,0);
 }
 return taxi_xbox_decode(0,0,0,0,0);
}
static void source_parent_results_report(void){
 unsigned expected=source_parent_results_credits_fixture?5:4;
 if(profile_commit_status!=(int)(source_parent_profile_register_smoke!=0)||profile_persist_requested!=(int)(source_parent_profile_register_smoke!=0))fail("Native profile result was rejected or persistence request incorrect.");
 if(!source_results.done||source_results.active||source_parent_phase!=SOURCE_PARENT_DRIVE||source_parent.entries!=2||source_parent.returns!=1||source_results.returns!=1||source_results.started!=expected||source_results.completed!=expected||source_results.name_skips!=(unsigned)!source_parent_results_name_fixture||source_parent.credits_entries!=(unsigned)source_parent_results_credits_fixture||source_parent.resource_checks!=2||source_restart.completed!=2||source_resets||host_reset_revision||native_pause.menu.outer_state||native_pause.menu.frozen||native_pause.menu.exit_flag||source_restart.exit||source_parent.last_exit.results!=1||source_parent.last_exit.callback!=0x080174dd||live_host_audio_ticks!=live_source_events||live_source_events!=live_source_callbacks||live_source_events!=live_source_acknowledged)fail("Native live results/reentry coverage incomplete.");
 printf("NATIVE RESULTS REPORT: entries%u returns%u children%u/%u nameskips%u credits%u resources%u roots%u counter%u events%llu audio%llu commits%llu originalclock%u natural%d legacyresets%u sourceinits%llu; real child completions, existing native UI/backend adaptation.\n",source_parent.entries,source_parent.returns,source_results.completed,source_results.started,source_results.name_skips,source_parent.credits_entries,source_parent.resource_checks,source_restart.completed,live_host_time.counter,(unsigned long long)live_source_events,(unsigned long long)live_host_audio_ticks,(unsigned long long)source_commits,source_results.original_clock,source_parent_results_natural,host_reset_revision,(unsigned long long)source_resets);fflush(stdout);
}
