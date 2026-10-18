/* Ordinary city/driver parent using existing native UI children and one host
   elapsed/audio owner. Menu pacing remains the current native-UI adaptation;
   this does not assert a whole original menu/IRQ/blend-inheritance proof. */
static struct {
 LARGE_INTEGER frequency;unsigned target_city,target_driver,button_tick,entries;
 uint32_t begin_counter,ready_counter,begin_rng;uint64_t menu_ticks,ready_audio;
 unsigned ready_entries,returns,credits_entries,quit_cursor,reentry_done,forced_qualified,resource_checks,root_before,drive_samples;
 uint32_t return_counter;uint64_t entry_commits,credits_tick_begin;TaxiPoint entry_position;TaxiNormalExit last_exit;unsigned quit_stops;TaxiMenuInput credits_input;
} source_parent;
static struct {
 TaxiNormalExitChildren flow;TaxiMenuInput input;unsigned active,armed,done,returns,started,completed,name_skips,child_ticks;
 uint32_t original_clock,entry_counter,arm_clock;uint64_t frozen_commits;unsigned frozen_updates;
 TaxiDriveWorld held_car;TaxiActorEffects held_effects;TaxiPassengerState held_actors[128];int32_t held_clock;
} source_results;
static void source_parent_bind_scores(void){
 if(!native_settings_loaded)fail("Native source ranking table not loaded.");
 unsigned differences=0;for(unsigned i=0;i<100;i++)if(passenger_scene.session.ranking_scores[i]!=default_ranking_scores[i])differences++;
 printf("NATIVE SCORE VIEW: previousloaded%u differences%u rows100; bind genuine loaded/runtime table, no score writes.\n",passenger_scene.session.ranking_loaded,differences);
 memcpy(passenger_scene.session.ranking_scores,default_ranking_scores,sizeof default_ranking_scores);passenger_scene.session.ranking_loaded=1;
}
static void source_parent_initialize(LARGE_INTEGER frequency,const char *scene,unsigned driver){
 if(drive_mode||startup_stage!=1||host_reset_revision||source_resets||source_viewer.initialized||source_parent.entries||frequency.QuadPart<=0)fail("Native menu parent boot boundary invalid.");
 /* Full original10B54 owner supplies the canonical ten cleanup handles.
    The old engine-only preview keeps temporary/shared diagnostic owners. */
 recovered_game_audio=recovered_engine_audio=1;
 source_parent_bind_scores();source_parent.frequency=frequency;source_parent.target_city=(unsigned)!strcmp(scene,"city2");source_parent.target_driver=driver;
 if(source_parent.target_driver>3)fail("Native menu parent driver invalid.");
 if(source_parent_smoke&&(source_parent.target_city||source_parent_reentry_smoke||source_parent_results_smoke)){native_settings[7]=1;puts("NATIVE PARENT FIXTURE: city2 unlock bit set explicitly for menu-input coverage; no earned progression claim.");}
 if(source_bind())fail("Native menu parent immutable binding failed.");
 /* Existing UI backend uses actual colors; original menu palette flag/header
    propagation is still a separately qualified dependency. Never guess8/8. */
 live_taxi_registry.flags=4;
 source_viewer.service=(TaxiPresentationService){.current_3520=live_host_time.counter,.previous_3524=live_host_time.frame.previous_3524,.delta_3b00=(uint32_t)live_host_time.frame.delta_3b00,.elapsed_755c=live_host_time.frame.elapsed_755c,.cursor_1e10=live_host_time.frame.animation_1e10,.parity_7540=live_host_time.frame.page_7540};
 source_viewer.reset_seen=host_reset_revision;source_viewer.focus=1;QueryPerformanceCounter(&source_viewer.previous);source_frame_active=1;source_parent_phase=SOURCE_PARENT_MENU;
 if(source_parent_profile_mode>=0)startup_parent_begin(0,1);
 printf("NATIVE PARENT BOOT: counter%u RNG%u; one retained elapsed/audio path, native city/driver input.\n",live_host_time.counter,passenger_scene.random_state);fflush(stdout);
}
static uint16_t source_parent_menu_mask(void){
 if(source_parent_smoke){
  if(++source_parent.button_tick%4)return 0;
  if((source_parent_reentry_smoke||source_parent_results_smoke)&&(startup_stage==8||startup_stage==9)&&!normal_live_flow.fade.active_177b&&!normal_live_flow.choice.exiting){unsigned target=startup_stage==8&&source_parent_profile_mode>=0?(unsigned)source_parent_profile_mode>>5:0;return normal_live_flow.choice.selection!=target?128:1;}
  if(startup_stage==1&&!city_flow.fade.active_177b&&!city_flow.choice.exiting)return native_settings[4]!=source_parent.target_city?16:1;
  if(startup_stage==2&&!driver_flow.fade.active_177b&&!driver_flow.choice.exiting)return driver_flow.choice.driver!=source_parent.target_driver?16:1;
  return 0;
 }
 TaxiXboxSample pad=taxi_xbox_poll_any();uint16_t mask=name_device_mask(pad);
 if(GetAsyncKeyState(VK_UP)&0x8000)mask|=64;if(GetAsyncKeyState(VK_DOWN)&0x8000)mask|=128;
 return mask;
}
#include "source_parent_exit.inc"
#include "source_parent_results.inc"
static void source_parent_begin(void){
 if(!source_parent_requested||source_parent_phase!=SOURCE_PARENT_MENU||startup_stage!=2||!driver_flow.complete||driver_flow.choice.exiting!=1||native_settings[4]>1||source_parent.entries!=source_parent.returns||source_parent.entries!=source_parent.ready_entries||host_reset_revision||source_resets||!source_restart_resources_idle())fail("Native driver-confirm entry boundary invalid.");
 source_parent.begin_counter=live_host_time.counter;source_parent.begin_rng=passenger_scene.random_state;source_parent.root_before=source_restart.completed;
 source_parent_menu_to_game(&driver_input);
 native_game_driver=driver_flow.choice.driver;drive_mode=passenger_diagnostic=traffic_preview=endgame_diagnostic=1;
 TaxiSourceResourceSnapshot *retained=NULL;if(source_parent_reentry_smoke||source_parent_results_smoke){retained=malloc(sizeof *retained);if(!retained)fail("Reentry snapshot allocation failed.");source_resources_snapshot(retained);}
 if(load_ordinary_resources(native_settings[4]?"city2":"city1"))fail("Native menu selected-city resource binding failed.");
 if(retained){source_resources_unchanged(retained);free(retained);source_parent.resource_checks++;}
 if(source_bind())fail("Native menu frame binding failed.");
 native_scene_bind_storage();source_restart_bind(source_parent.frequency);
 source_parent_phase=SOURCE_PARENT_ENTRY;startup_stage=18;source_parent.entries++;
 /* Blocking immutable asset reads are host loading time, not game IRQ events.
    Do not reset the retained source counter, frame baseline, RNG or queues. */
 QueryPerformanceCounter(&source_viewer.previous);
 printf("NATIVE PARENT ENTRY: city%u driver%u counter%u RNG%u; genuine driver confirmation, no legacy music/reset.\n",(unsigned)scene_index+1,native_game_driver,live_host_time.counter,passenger_scene.random_state);fflush(stdout);
 source_restart_start();
}
static void source_parent_ready(void){
 if(ordinary_resources_pending||source_restart.mode||source_restart.flow.phase!=TAXI_RESTART_READY||!city_fade.active_177b||!live_game_start.active||source_parent.ready_entries+1!=source_parent.entries||source_restart.completed!=source_parent.root_before+1||((source_parent.ready_entries==0)!=(!source_viewer.initialized))||host_reset_revision||source_resets)fail("Native menu parent READY boundary invalid.");
 TaxiSourceResourceSnapshot *before=malloc(sizeof *before);if(!before)fail("Native menu graphics snapshot allocation failed.");source_resources_snapshot(before);
 load_scene_graphics();load_drive_graphics();source_resources_unchanged(before);free(before);
 source_viewer.initialized=1;source_viewer.focus=1;source_viewer.reset_seen=host_reset_revision;
 source_parent.ready_counter=live_host_time.counter;source_parent.ready_audio=live_host_audio_ticks;
 startup_stage=3;source_parent_phase=SOURCE_PARENT_DRIVE;source_parent.ready_entries++;source_parent.entry_commits=source_commits;source_parent.entry_position=car.world.height.position;source_parent.quit_cursor=0;source_parent.drive_samples=0;
 source_viewer.trace_ns=0;source_viewer.trace_ticks=0;QueryPerformanceCounter(&source_viewer.previous);
 if(source_parent_smoke&&!source_parent_reentry_smoke&&!source_parent_results_smoke&&!source_earned_requested)source_retry_smoke=1; /* One genuine Retry AFTER cold entry. */
 if(source_parent_profile_mode>=0){if((native_settings[0]&224)!=(unsigned)source_parent_profile_mode)fail("Native mode menu did not select requested mode.");printf("NATIVE PROFILE MODE: mode%u entry%u; selected through actual mode-menu input.\n",native_settings[0]&224,source_parent.entries);}
 if(source_parent_results_smoke&&source_parent.entries==1){source_results.original_clock=(uint32_t)passenger_scene.session.clock.remaining_16a8;source_results.entry_counter=live_host_time.counter;if(!source_results.original_clock||source_results.original_clock!=(uint32_t)taxi_game_initial_clock(&passenger_scene.session.settings,0))fail("Native results initial clock not original.");}
 printf("NATIVE PARENT DRIVE: city%u driver%u counter%u RNG%u intro%u fade%u; retained native owners, no source_initialize/reset.\n",(unsigned)scene_index+1,native_game_driver,live_host_time.counter,passenger_scene.random_state,live_game_start.active,city_fade.active_177b);fflush(stdout);
}
static void source_parent_iteration(LARGE_INTEGER frequency,int focus){
 if(!source_parent_requested)return;
 if(source_parent_phase==SOURCE_PARENT_DRIVE){source_frame_iteration(frequency,focus);return;}
 LARGE_INTEGER now;QueryPerformanceCounter(&now);
 if(!focus){source_viewer.previous=now;source_viewer.focus=0;return;}
 if(!source_viewer.focus){source_viewer.previous=now;source_viewer.focus=1;}
 uint32_t previous=live_host_time.counter;source_advance(now,frequency,3);
 uint32_t ticks=live_host_time.counter-previous;
 if(source_parent_phase==SOURCE_PARENT_ENTRY){source_restart_poll();if(!source_restart.mode)source_parent_ready();return;}
 if(source_parent_phase==SOURCE_PARENT_CREDITS){source_parent_credits_ticks(ticks);return;}
 if(source_parent_phase==SOURCE_PARENT_CHILD){source_parent_results_ticks(ticks);return;}
 if(source_parent_phase!=SOURCE_PARENT_MENU||drive_mode||ordinary_resources_pending)fail("Native menu parent phase invalid.");
 for(uint32_t i=0;i<ticks&&source_parent_phase==SOURCE_PARENT_MENU;i++){uint16_t mask=source_parent_menu_mask();source_context_menu(mask);startup_tick_control(mask);source_parent.menu_ticks++;}
}
static void source_parent_report(void){
 if(!source_parent_requested)return;
 if(source_earned_requested){
  source_earned_write();
  if(!source_earned.done||source_earned.written!=1||!source_earned.read_only_checks||source_parent_phase!=SOURCE_PARENT_DRIVE||source_parent.entries!=1||source_parent.returns||source_restart.completed!=1||source_resets||host_reset_revision||route_probe_active||source_retry_smoke||passenger_scene.session.ended_1697||passenger_scene.session.outer_state_1ea8||passenger_scene.session.clock.remaining_16a8<=0||live_host_audio_ticks!=live_source_events||live_source_events!=live_source_callbacks||live_source_events!=live_source_acknowledged)fail("Current native earned pickup/delivery input coverage incomplete.");
  printf("NATIVE EARNED REPORT: entries%u roots%u samples%u readonly%u pickup%u trip%u completed%u cash%ld.%02ld initialclock%ld remaining%ld selected%d destination%d route%u points%u counter%u audio%llu commits%llu sourceinits%llu legacyresets%u; normal sampled XInput, actual native settlement, original clock unchanged.\n",source_parent.entries,source_restart.completed,source_earned.samples,source_earned.read_only_checks,source_earned.pickup_seen,source_earned.trip_seen,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698,(long)passenger_scene.session.fare.total_fraction_169c,(long)source_earned.initial_clock,(long)passenger_scene.session.clock.remaining_16a8,source_earned.selected,source_earned.destination,live_delivery_route?live_delivery_route->source_index:0,live_delivery_route?live_delivery_route->count:0,live_host_time.counter,(unsigned long long)live_host_audio_ticks,(unsigned long long)source_commits,(unsigned long long)source_resets,host_reset_revision);fflush(stdout);return;
 }
 if(source_parent_results_smoke){source_parent_results_report();return;}
 if(source_parent_reentry_smoke){source_parent_reentry_report();return;}
 if(source_parent_smoke&&(source_parent_phase!=SOURCE_PARENT_DRIVE||source_parent.entries!=1||!source_parent.menu_ticks||host_reset_revision||source_resets||!source_retry_check.done||source_restart.completed!=2||source_commits<100||scene_index!=(int)source_parent.target_city||native_game_driver!=source_parent.target_driver||source_parent.ready_counter<=source_parent.begin_counter||live_host_time.counter<source_parent.ready_counter||live_host_audio_ticks!=live_source_events||live_source_events!=live_source_callbacks||live_source_events!=live_source_acknowledged))fail("Native menu parent smoke incomplete or clock/audio ownership duplicated.");
 printf("NATIVE PARENT REPORT: entries%u menu%llu city%u driver%u counter%u/%u/%u events%llu audio%llu commits%llu roots%u resumed%u legacyresets%u sourceinits%llu; native UI pacing/backend adaptation, full original menus/camera unqualified.\n",source_parent.entries,(unsigned long long)source_parent.menu_ticks,(unsigned)scene_index+1,native_game_driver,source_parent.begin_counter,source_parent.ready_counter,live_host_time.counter,(unsigned long long)live_source_events,(unsigned long long)live_host_audio_ticks,(unsigned long long)source_commits,source_restart.completed,source_retry_check.done,host_reset_revision,(unsigned long long)source_resets);
}