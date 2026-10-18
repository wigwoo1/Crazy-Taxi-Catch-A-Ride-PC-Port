/* Pause Quit continuation over actual canonical native owners. UI/credits
   presentation remains the existing host backend; source exit requests run once. */
static void source_parent_game_to_menu(TaxiMenuInput *to){
 memcpy(to->held,driving_buttons.held,10);memcpy(to->previous,driving_buttons.previous,10);memcpy(to->repeat,driving_buttons.repeat,10);memcpy(to->pressed,driving_buttons.pressed,10);memcpy(to->timer,driving_buttons.countdown,20);to->pressed_mask=driving_buttons.pressed_mask;
}
static void source_parent_menu_to_game(const TaxiMenuInput *from){
 /* Only shared15C3C arrays/timers/mask; raw/auxiliary words have no menu view. */
 memcpy(driving_buttons.held,from->held,10);memcpy(driving_buttons.previous,from->previous,10);memcpy(driving_buttons.repeat,from->repeat,10);memcpy(driving_buttons.pressed,from->pressed,10);memcpy(driving_buttons.countdown,from->timer,20);driving_buttons.pressed_mask=from->pressed_mask;
}
static int source_parent_reentry_complete(void){return source_parent.reentry_done!=0||source_results.done!=0;}
static void source_parent_exit_aliases(void){
 drive_context.height.flag_1697=drive_context.driver.motion.orientation.flag_1697=passenger_scene.session.ended_1697;
 drive_context.driver.motion.orientation.flag_1ea8=passenger_scene.session.outer_state_1ea8;
 music_context.main_mode=source_restart.retry.parent.main_mode;
}
static void source_parent_quit_fade(LARGE_INTEGER frequency){
 if(!source_parent_requested||source_parent_phase!=SOURCE_PARENT_DRIVE||native_pause.menu.outer_state!=2||!native_pause.menu.exit_flag)fail("Native parent Quit fade boundary invalid.");
 source_restart_bind(frequency);if(taxi_normal_quit_owned_fade(&source_restart.retry,&native_pause_frame.events))fail("Native pause Quit fade failed.");city_fade=source_viewer.fade.fade;
 printf("NATIVE QUIT FADE: entry%u city%u driver%u counter%u qualified%u; actual Yes/201 masks.\n",source_parent.entries,(unsigned)scene_index+1,native_game_driver,live_host_time.counter,passenger_scene.session.fare.qualified_1691);fflush(stdout);
}
static void source_parent_return_menu(void){
 TaxiNormalRetryOwned *q=&source_restart.retry;
 if(q->phase!=4||q->parent.callback!=0x080174dd||q->parent.frozen||q->parent.outer||q->parent.ended||(q->parent.results&&(source_results.flow.stage!=6||source_results.flow.state!=&q->parent))||!q->parent.main_mode||!q->parent.redraw||source_parent.returns+1!=source_parent.entries||!source_restart_resources_idle())fail("Native Quit menu callback boundary invalid.");
 source_parent_exit_aliases();source_parent.last_exit=q->parent;source_parent.quit_stops+=q->stops;source_parent.returns++;source_parent.return_counter=live_host_time.counter;
 source_parent_game_to_menu(&normal_live_input);normal_live_flow.fade=city_fade;
 drive_mode=0;source_parent_phase=SOURCE_PARENT_MENU;
 startup_parent_begin(1,q->parent.redraw);
 if(source_parent_reentry_smoke||source_parent_results_smoke){source_parent.target_city^=1;source_parent.target_driver=(source_parent.target_driver+1)%4;source_parent.button_tick=0;}
 QueryPerformanceCounter(&source_viewer.previous);
 printf("NATIVE %s MENU: returns%u counter%u RNG%u frozen%u callback%08x stops%u; retained owners/input, no boot/reset.\n",q->parent.results?"RESULTS":"QUIT",source_parent.returns,live_host_time.counter,passenger_scene.random_state,native_pause.menu.frozen,q->parent.callback,q->stops);fflush(stdout);
}
static void source_parent_quit_finish(void){
 TaxiNormalRetryOwned *q=&source_restart.retry;
 if(source_parent_phase!=SOURCE_PARENT_DRIVE||source_viewer.owner.phase!=TAXI_FRAME_IDLE||city_fade.active_177b)fail("Native parent Quit cleanup boundary invalid.");
 if(taxi_normal_quit_owned_transfer(q)!=1)fail("Native pause Quit cleanup failed.");
 int rc=taxi_normal_quit_owned_prepare(q);if(rc<1)fail("Native pause Quit preparation failed.");source_parent_exit_aliases();
 if(rc==2){
  source_parent_phase=SOURCE_PARENT_CREDITS;source_parent.credits_entries++;source_parent.credits_tick_begin=credits_live_ticks;source_parent_game_to_menu(&source_parent.credits_input);
  credits_live_begin(2);QueryPerformanceCounter(&source_viewer.previous);
 }else source_parent_return_menu();
}
static void source_parent_credits_ticks(uint32_t ticks){
 if(!credits_live_active||source_restart.retry.phase!=3)fail("Native parent credits owner invalid.");
 for(uint32_t i=0;i<ticks&&source_parent_phase==SOURCE_PARENT_CREDITS;i++){
  uint16_t mask=source_parent_smoke?(credits_live_ticks-source_parent.credits_tick_begin>=60?8:0):name_device_mask(taxi_xbox_poll_any());
  uint16_t pressed=(uint16_t)((source_parent.credits_input.pressed[0]?1:0)|(source_parent.credits_input.pressed[3]?8:0));
  credits_live_tick(pressed);taxi_menu_input_step(&source_parent.credits_input,mask,1);
  int rc=taxi_normal_quit_owned_resume(&source_restart.retry,credits_live_runtime.complete);
  if(rc<0)fail("Native Quit credits return failed.");
  if(rc){
   if(!credits_live_runtime.complete)fail("Native Quit fabricated credits completion.");
   credits_live_active=0;glDeleteTextures(1,&credits_live_texture);credits_live_texture=0;source_parent_menu_to_game(&source_parent.credits_input);source_parent_return_menu();
  }
 }
}
static TaxiXboxSample source_parent_reentry_pad(void){
 if(source_parent_phase!=SOURCE_PARENT_DRIVE||city_fade.active_177b||live_game_start.active)return taxi_xbox_decode(0,0,0,0,0);
 if(++source_parent.drive_samples<60)return taxi_xbox_decode(0,0,0,255,0);
 if(source_parent.entries>=3){
  if(source_parent.drive_samples>=90){TaxiPoint p=car.world.height.position;if(p.x==source_parent.entry_position.x&&p.z==source_parent.entry_position.z)fail("Native reentry did not resume driving.");source_parent.reentry_done=1;}
  return taxi_xbox_decode(0,0,0,255,0);
 }
 if(source_parent_credits_smoke&&!source_parent.forced_qualified&&!source_parent.returns){
  passenger_scene.session.fare.qualified_1691=1;source_parent.forced_qualified=1;
  puts("NATIVE QUIT FIXTURE: qualified fare flag explicitly set for credits-branch coverage; not an earned fare/progression claim.");
 }
 static const uint16_t buttons[11]={0x10,0,2,0,2,0,0x1000,0,1,0,0x1000};
 if(source_parent.quit_cursor<11)return taxi_xbox_decode(buttons[source_parent.quit_cursor++],0,0,0,0);
 return taxi_xbox_decode(0,0,0,0,0);
}
static void source_parent_reentry_report(void){
 unsigned expected_credits=source_parent_credits_smoke?1:0;
 if(!source_parent.reentry_done||source_parent.entries!=3||source_parent.ready_entries!=3||source_parent.returns!=2||source_parent.credits_entries!=expected_credits||source_parent.resource_checks!=3||source_parent_phase!=SOURCE_PARENT_DRIVE||source_restart.completed!=3||!source_parent.quit_stops||source_resets||host_reset_revision||native_pause.menu.frozen||native_pause.menu.outer_state||native_pause.menu.exit_flag||source_restart.exit||source_parent.last_exit.callback!=0x080174dd||source_parent.last_exit.results||source_parent.last_exit.frozen||live_host_audio_ticks!=live_source_events||live_source_events!=live_source_callbacks||live_source_events!=live_source_acknowledged)fail("Native Quit/reentry coverage or retained-owner invariant failed.");
 printf("NATIVE REENTRY REPORT: entries%u returns%u credits%u resources%u roots%u city%u driver%u counter%u events%llu audio%llu commits%llu stops%u resumed%u legacyresets%u sourceinits%llu; original child contracts, native UI/renderer adaptation retained.\n",source_parent.entries,source_parent.returns,source_parent.credits_entries,source_parent.resource_checks,source_restart.completed,(unsigned)scene_index+1,native_game_driver,live_host_time.counter,(unsigned long long)live_source_events,(unsigned long long)live_host_audio_ticks,(unsigned long long)source_commits,source_parent.quit_stops,source_parent.reentry_done,host_reset_revision,(unsigned long long)source_resets);
}