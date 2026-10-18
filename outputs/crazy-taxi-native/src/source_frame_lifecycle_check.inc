/* Controlled lifecycle coverage of the actual viewer path. Never physical OS
   focus/device proof; no source clocks, palette results or physics fabricated. */
static void source_frame_iteration(LARGE_INTEGER frequency,int focus);
typedef struct {
 TaxiSourceViewer source;TaxiHostTime host;TaxiFrameEvent event;TaxiFrameWait wait;
 TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiPassengerScene passengers;
 TaxiActorEffects effects;TaxiTitleAssets assets;TaxiPaletteBanks banks;
 TaxiPaletteRegistration registry;TaxiFontSetup fonts;TaxiStartupObjQueue queue;
 TaxiStartupObjTransport transport;TaxiInput input;TaxiXboxSample pad;
 TaxiAudioChannel channels[4];uint8_t shared[8];uint32_t cache;
 unsigned audio,updates;uint64_t prepares,commits,samples,gpu,events;
} TaxiSourceFrozenCheck;
static struct {
 unsigned stage,freeze_start,freeze_end,last_display,freeze_checks,reset_checks;
 uint64_t resume_commits,reset_commits;int script_cursor,pending_step,checked_step,published_step;
 unsigned control_checks,pause_commits,palette_checks,resume_checks;
 TaxiPaletteBanks before_banks;TaxiPaletteFadeStart before_fade;uint16_t before_mosaic;uint8_t before_request;int32_t before_delta;
 int holding;TaxiDriveWorld paused_car;TaxiActorEffects paused_effects;
 int32_t paused_clock;uint32_t paused_elapsed;TaxiSourceFrozenCheck frozen;
} source_lifecycle={.pending_step=-1,.checked_step=-1,.published_step=-1};
static void source_lifecycle_capture(TaxiSourceFrozenCheck *s){
 memset(s,0,sizeof *s);s->source=source_viewer;s->host=live_host_time;s->event=live_frame_event;s->wait=live_frame_wait;
 s->car=car;s->context=drive_context;s->passengers=passenger_scene;s->effects=native_actor_effects;
 s->assets=live_taxi_assets;s->banks=live_taxi_banks;s->registry=live_taxi_registry;s->fonts=live_taxi_fonts;
 s->queue=live_taxi_queue;s->transport=live_taxi_transport;s->input=driving_buttons;s->pad=source_frame_pad;
 memcpy(s->channels,audio_channels,sizeof s->channels);memcpy(s->shared,native_session_init.shared_1534,8);
 s->cache=live_taxi_cache;s->audio=audio_frames;s->updates=live_host_updates;
 s->prepares=source_prepares;s->commits=source_commits;s->samples=source_samples;s->gpu=source_gpu_draws;s->events=live_source_events;
}
static void source_lifecycle_assert_frozen(void){
 TaxiSourceFrozenCheck now;source_lifecycle_capture(&now);
 /* Only the host wall-time anchor/focus marker change during suspension. */
 now.source.previous=source_lifecycle.frozen.source.previous;now.source.focus=source_lifecycle.frozen.source.focus;
 if(memcmp(&now,&source_lifecycle.frozen,sizeof now))fail("Source lifecycle focus suspension changed native frame/game/audio owners.");
}
static uint16_t source_lifecycle_buttons(int step){
 if(step==0||step==73||step==75||step==81||step==83||step==85)return 0x10;
 if(step==61||step==67||step==77)return 2;
 if(step==63||step==65||step==69||step==79)return 0x1000;
 if(step==71)return 0x2000;
 return 0;
}
static TaxiXboxSample source_lifecycle_pad(void){
 if(source_lifecycle.stage>=3&&source_commits>=source_lifecycle.reset_commits+30&&
    !city_fade.active_177b&&!live_game_start.active&&source_lifecycle.script_cursor<87){
  int step=source_lifecycle.script_cursor++;source_lifecycle.pending_step=step;
  return taxi_xbox_decode(source_lifecycle_buttons(step),0,0,0,0);
 }
 source_lifecycle.pending_step=-1;
 return taxi_xbox_decode(0,0,0,255,0);
}
static void source_lifecycle_after_control(void){
 if(!source_frame_lifecycle_smoke)return;
 int step=source_lifecycle.pending_step;
 if(step>=0){
  if(step==source_lifecycle.checked_step)fail("Source lifecycle sampled controller consumed twice.");
  source_lifecycle.checked_step=step;
  unsigned frozen=step<=72||(step>=75&&step<=80)||(step>=83&&step<=84);
  unsigned selection=step<61?0:step<67?1:step<75?2:step<77?0:step<83?1:0;
  unsigned dialog=(step>=63&&step<=64)||(step>=79&&step<=82)?1:(step>=69&&step<=70?2:0);
  if(native_pause.menu.frozen!=frozen||native_pause.menu.selection!=selection||native_pause.menu.dialog!=dialog||!native_pause.menu.no_selected||native_pause.menu.outer_state||native_pause.menu.exit_flag){
   fprintf(stderr,"Lifecycle pause step%d actual %u %u %u %u expected %u %u %u 1\n",step,native_pause.menu.frozen,native_pause.menu.selection,native_pause.menu.dialog,native_pause.menu.no_selected,frozen,selection,dialog);
   fail("Source lifecycle native pause/confirmation state mismatch.");
  }
  if(frozen&&(music_tracker.state.playing||audio_settings_state.effects_enabled))fail("Source lifecycle pause did not suspend native music/effects flags.");
  if(step==73||step==85){if(native_pause.menu.palette_request!=1||!audio_settings_state.effects_enabled)fail("Source lifecycle Resume restore request missing.");source_lifecycle.resume_checks++;}
  if(step==81){if(native_pause.menu.palette_request!=2||!audio_settings_state.effects_enabled)fail("Source lifecycle confirmation-Start source request2 missing.");source_lifecycle.resume_checks++;}
  source_lifecycle.control_checks++;
 }
 if(native_pause.menu.frozen){
  if(!source_lifecycle.holding){source_lifecycle.paused_car=car;source_lifecycle.paused_effects=native_actor_effects;source_lifecycle.paused_clock=passenger_scene.session.clock.remaining_16a8;source_lifecycle.paused_elapsed=live_host_time.frame.elapsed_755c;}
  source_lifecycle.holding=1;
 }else source_lifecycle.holding=0;
}
static void source_lifecycle_before_poll(void){
 if(!source_frame_lifecycle_smoke)return;
 source_lifecycle.before_banks=live_taxi_banks;source_lifecycle.before_fade=source_viewer.fade;source_lifecycle.before_mosaic=source_viewer.mosaic;source_lifecycle.before_request=native_pause.menu.palette_request;source_lifecycle.before_delta=live_host_time.frame.delta_3b00;
}
static void source_lifecycle_after_commit(void){
 if(!source_frame_lifecycle_smoke)return;
 if(source_lifecycle.holding){
  if(memcmp(&car,&source_lifecycle.paused_car,sizeof car)||memcmp(&native_actor_effects,&source_lifecycle.paused_effects,sizeof native_actor_effects)||passenger_scene.session.clock.remaining_16a8!=source_lifecycle.paused_clock||live_host_time.frame.elapsed_755c!=source_lifecycle.paused_elapsed)fail("Source lifecycle paused car/effects/game clock advanced.");
  source_lifecycle.pause_commits++;
 }
 if(source_lifecycle.checked_step>=0&&source_lifecycle.checked_step!=source_lifecycle.published_step){
  int step=source_lifecycle.checked_step;if(step!=source_lifecycle.published_step+1)fail("Source lifecycle lost prepared/published input step.");source_lifecycle.published_step=step;unsigned alternate=(unsigned)(native_pause.menu.frozen||step==81||step==82);
  if(native_pause.alternate!=alternate||native_pause.menu.palette_request)fail("Source lifecycle native after-wait palette request consumption mismatch.");
  /*22508 alternate intentionally retains records0/2/11 as they were BEFORE
     the transfer. Restoring every record first is an invalid reference. */
  TaxiPaletteBanks expected=source_lifecycle.before_banks;TaxiPaletteFadeStart fade=source_lifecycle.before_fade;uint16_t mosaic=source_lifecycle.before_mosaic;
  TaxiCityPaletteOwners reference=source_viewer.palette;reference.banks=&expected;reference.fade=&fade;
  if(source_lifecycle.before_request&&taxi_city_palette_gameplay_transfer(&reference,source_lifecycle.before_request==2))fail("Source lifecycle palette transfer reference failed.");
  if(taxi_city_palette_frame(&reference,source_lifecycle.before_delta,&mosaic))fail("Source lifecycle after-transfer fade reference failed.");
  if(memcmp(&expected,&source_viewer.published.banks,sizeof expected)||memcmp(&fade,&source_viewer.fade,sizeof fade)||mosaic!=source_viewer.mosaic){
   for(unsigned bank=0;bank<2;bank++)for(unsigned color=0;color<256;color++){const uint16_t *wanted=bank?expected.objects:expected.background,*actual=bank?source_viewer.published.banks.objects:source_viewer.published.banks.background;if(wanted[color]!=actual[color]){fprintf(stderr,"Lifecycle palette step%d bank%u color%u expected%04x actual%04x request%u fade%u\n",step,bank,color,wanted[color],actual[color],source_lifecycle.before_request,source_lifecycle.before_fade.fade.active_177b);break;}}
   fail("Source lifecycle published palette/fade differs from22508 thenEE10 reference.");
  }
  source_lifecycle.palette_checks++;
 }
}
static void source_frame_lifecycle_iteration(LARGE_INTEGER frequency,int focus){
 if(!source_frame_lifecycle_smoke){source_frame_iteration(frequency,focus);return;}
 if(!passenger_diagnostic||!traffic_preview||((native_settings[0]>>1)&3))fail("Source lifecycle fixture requires ordinary passengers/traffic and control profile0.");
 if(!source_lifecycle.stage&&source_viewer.initialized&&audio_presented>=60&&source_viewer.owner.phase==TAXI_FRAME_WAITING&&!(live_frame_wait.handshake&1)){
  source_lifecycle.stage=1;source_lifecycle.freeze_start=audio_presented;source_lifecycle.freeze_end=audio_presented+60;source_lifecycle.last_display=UINT_MAX;source_lifecycle_capture(&source_lifecycle.frozen);
 }
 if(source_lifecycle.stage==1){
  source_lifecycle_assert_frozen();
  if(audio_presented<source_lifecycle.freeze_end){
   source_frame_iteration(frequency,0);source_lifecycle_assert_frozen();
   if(source_lifecycle.last_display!=audio_presented){source_lifecycle.freeze_checks++;source_lifecycle.last_display=audio_presented;}
   return;
  }
  source_frame_iteration(frequency,1);source_lifecycle_assert_frozen();
  source_lifecycle.resume_commits=source_commits;source_lifecycle.stage=2;
  printf("PASS SOURCE FOCUS: %u physical frames preserved pending frame, source/event/input/audio and game owners; immediate resume rebased wall time without changing prior delta. Controlled focus, not OS test.\n",source_lifecycle.freeze_checks);return;
 }
 if(source_lifecycle.stage==2&&source_commits>=source_lifecycle.resume_commits+12&&source_viewer.owner.phase==TAXI_FRAME_WAITING){
  TaxiHostTime before=live_host_time;uint64_t audio_before=live_host_audio_ticks,commits_before=source_commits;
  live_host_time.frame.delta_3b00=11;reset_drive();Sleep(120);LARGE_INTEGER end_load;QueryPerformanceCounter(&end_load);
  source_frame_iteration(frequency,focus);LARGE_INTEGER after;QueryPerformanceCounter(&after);
  uint64_t post_ns=(uint64_t)((double)(after.QuadPart-end_load.QuadPart)*1e9/frequency.QuadPart);
  TaxiHostTime reference=before;uint32_t ticks;if(taxi_host_time_advance(&reference,source_viewer.trace_ns,&ticks))fail("Source lifecycle reset clock reference invalid.");
  if(source_viewer.trace_ns>post_ns+1000||live_host_time.clock.phase!=reference.clock.phase||live_host_time.counter!=reference.counter||live_host_audio_ticks-audio_before!=ticks||source_viewer.trace_ticks!=ticks||live_host_time.frame.previous_3524!=before.counter||source_viewer.owner.preparation_delta!=1||source_viewer.service.delta_3b00!=1||source_viewer.published.valid||source_resets!=2||source_commits!=commits_before||source_viewer.owner.phase!=TAXI_FRAME_WAITING)fail("Source lifecycle reset retained stale frame/delta, lost phase, or included blocking load time.");
  source_lifecycle.reset_checks++;source_lifecycle.reset_commits=source_commits;source_lifecycle.stage=3;
  printf("PASS SOURCE RESET: pending generation canceled, stale delta11 ->1, fresh graphics/HUD owners,120ms injected loading excluded, source clock phase/counter retained.\n");return;
 }
 source_frame_iteration(frequency,focus);
}
static void source_lifecycle_report(void){
 if(!source_frame_lifecycle_smoke)return;
 if(source_lifecycle.stage!=3||source_lifecycle.freeze_checks!=60||source_lifecycle.reset_checks!=1||source_lifecycle.control_checks!=87||source_lifecycle.pause_commits<80||source_lifecycle.palette_checks!=87||source_lifecycle.resume_checks!=3||native_pause.menu.frozen||native_pause.alternate||source_lifecycle.script_cursor!=87)fail("Source lifecycle coverage incomplete.");
 printf("PASS SOURCE LIFECYCLE: focus%u reset%u control%u paused_publications%u palettes%u resume_branches%u; actual source frame owner and4K GPU, controlled input/host fixtures only.\n",source_lifecycle.freeze_checks,source_lifecycle.reset_checks,source_lifecycle.control_checks,source_lifecycle.pause_commits,source_lifecycle.palette_checks,source_lifecycle.resume_checks);
}