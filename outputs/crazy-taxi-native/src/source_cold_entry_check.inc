/* Controlled first-entry check. This route NEVER called legacy load/reset,
   ordinary OBJ/HUD initialization or graphics bootstrap before the native root.
   Selected city/driver and current native palette flags are explicit fixtures;
   menu-parent reachability / original blend inheritance remain separate. */
static int source_cold_entry_check(LARGE_INTEGER frequency){
 if(host_reset_revision||source_resets||source_viewer.initialized||native_scene_viewport_ready||native_scene_frame_ready||ordinary_graphics_ready||passenger_scene_initialized||native_vehicle_start_initialized||live_taxi_registry.count||live_taxi_fonts.cursor||!ordinary_resources_pending||passenger_scene.count)fail("Cold entry was initialized through a legacy route.");
 TaxiSourceResourceSnapshot *before=malloc(sizeof *before);if(!before)fail("Cold entry snapshot allocation failed.");
 source_resources_snapshot(before);
 TaxiOrdinaryFrameOwner expected=source_viewer.owner;
 expected.graphics=&source_viewer.graphics;expected.host=&live_host_time;expected.service=&source_viewer.service;expected.wait=&live_frame_wait;expected.pause=&native_pause;expected.pause_frame=&native_pause_frame;expected.cache=&live_taxi_cache;expected.anchor9ab0=source_viewer.anchor;expected.shared1534=native_session_init.shared_1534;expected.platform=source_input;expected.published=&source_viewer.published;
 if(source_bind()||memcmp(&expected,&source_viewer.owner,sizeof expected))fail("Cold source owner binding changed retained fields.");
 memcpy(before->f32,&expected,sizeof expected);source_resources_unchanged(before);
 source_resources_snapshot(before);if(source_bind())fail("Cold owner repeated binding failed.");source_resources_unchanged(before);
 source_viewer.owner.phase=TAXI_FRAME_WAITING;source_resources_snapshot(before);if(source_bind()!=-1)fail("Cold binding admitted active owner.");source_resources_unchanged(before);source_viewer.owner.phase=TAXI_FRAME_IDLE;
 TaxiRestartRenderContext storage=native_bootstrap_render;
 storage.frame=native_bootstrap_first;storage.frame_size=sizeof native_bootstrap_first;storage.alternate_frame=native_bootstrap_second;storage.alternate_size=sizeof native_bootstrap_second;storage.width=240;storage.height=160;storage.bitmap_mode=4;storage.shade_lookup=native_scene_lookup;storage.shade_lookup_size=sizeof native_scene_lookup;
 source_resources_snapshot(before);native_scene_bind_storage();if(memcmp(&storage,&native_bootstrap_render,sizeof storage))fail("Cold framebuffer binding changed retained render fields.");memcpy(before->f45,&storage,sizeof storage);source_resources_unchanged(before);
 puts("PASS COLD NATIVE BIND:77 owners retained except explicit owner/storage pointer fields; repeated bind unchanged; active owner rejected; no helper/reset/clock event.");
 /* Current viewer palette mode is explicit, not claimed original startup data. */
 live_taxi_registry.flags=4;
 source_viewer.service=(TaxiPresentationService){.current_3520=live_host_time.counter,.previous_3524=live_host_time.frame.previous_3524,.delta_3b00=(uint32_t)live_host_time.frame.delta_3b00,.elapsed_755c=live_host_time.frame.elapsed_755c,.cursor_1e10=live_host_time.frame.animation_1e10,.parity_7540=live_host_time.frame.page_7540};
 source_viewer.reset_seen=host_reset_revision;source_frame_active=1;QueryPerformanceCounter(&source_viewer.previous);
 uint32_t initial_rng=passenger_scene.random_state,initial_counter=live_host_time.counter;
 source_restart_bind(frequency);
 if(native_scene_viewport_ready||native_scene_frame_ready||source_restart.panel_engine.projection.focal||live_taxi_registry.count||source_restart.visual.graphics_index||source_restart.setup.index||source_restart.root.flow)fail("Cold bindings performed native initialization early.");
 source_restart_start();unsigned polls=0;
 while(source_restart.mode==2){
  LARGE_INTEGER now;QueryPerformanceCounter(&now);source_advance(now,frequency,5);source_restart_poll();
  if(++polls>60000||window_quit_requested)fail("Cold native entry exceeded its owned polling limit.");Sleep(1);
 }
 if(ordinary_resources_pending||host_reset_revision||source_resets||source_viewer.initialized||!native_scene_viewport_ready||!native_scene_frame_ready||!ordinary_graphics_ready||!passenger_scene_initialized||!native_vehicle_start_initialized||!native_start_art_owned_active||source_restart.flow.phase!=TAXI_RESTART_READY||source_restart.visual.graphics_index!=15||source_restart.setup.index!=8||source_restart.visual.entry_index!=8||!city_fade.active_177b||!live_game_start.active||passenger_scene.count!=128||source_restart.purity!=source_restart.draws||source_restart.completed!=1||source_restart.waits!=source_restart.published.generation||live_host_time.counter<=initial_counter||!live_host_audio_ticks)fail("Cold native root did not complete actual owners and retain entry fade.");
 if(live_taxi_registry.count!=17||live_scene_palette_index!=16||source_restart.panel_engine.projection.focal!=native_scene_projection().focal||source_restart.panel_engine.world!=&world||native_bootstrap_render.shade_lookup!=native_scene_lookup)fail("Cold native root publication bindings invalid.");
 printf("PASS COLD NATIVE ENTRY: city%u driver%u helpers15/8/8 publications%llu GPU%llu prewait%llu RNG%u->%u counter%u->%u audio%llu fade%u intro%u legacyresets%u; selected-input/native backend fixture, not full menu/camera proof.\n",(unsigned)scene_index+1,native_game_driver,(unsigned long long)source_restart.published.generation,(unsigned long long)source_restart.draws,(unsigned long long)source_restart.waits,initial_rng,passenger_scene.random_state,initial_counter,live_host_time.counter,(unsigned long long)live_host_audio_ticks,city_fade.active_177b,live_game_start.active,host_reset_revision);
 free(before);source_restart_report();return 0;
}