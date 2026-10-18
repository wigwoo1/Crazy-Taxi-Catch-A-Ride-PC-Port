/* Opt-in live ordinary Retry, retaining actual native game owners. Loading
   executes native translated helpers; elapsed events stay on the one existing
   viewer clock/audio path. No ROM instructions or legacy reset_drive call. */
#include "normal_restart_native.h"
#include "normal_restart_presentation_owned.h"
#include "normal_retry_owned.h"
typedef struct {
 TaxiNormalRestartNative root;TaxiNormalRestartOwners music;TaxiNormalRestartPrefixOwners prefix;TaxiRestartResources resources;
 TaxiRestartVisualOwners visual;TaxiRestartSetupNative setup;TaxiNormalRestartPresentationOwned bridge;TaxiNormalRestartFlow flow;TaxiNormalRetryOwned retry;
 TaxiOrdinaryLoadingPending loading;TaxiLoadingPresent state;TaxiOrdinaryLoadingOwner content;TaxiLoadingFrameBuffers buffers;TaxiLoadingPublishedFrame published;TaxiLoadingPaletteRetained retained;
 TaxiDisplayMode display;TaxiGraphicsMemoryBase bases;TaxiGraphicsBootstrapAssets assets;TaxiHudStartInput hud;TaxiViewTransform transform;
 TaxiPassengerInitPending passengers;TaxiPassengerInitWorld passenger_world;TaxiPassengerSetupOwned passenger_setup;TaxiNoncustomerState npcs[64];TaxiNoncustomerPool pool;uint32_t bitmaps[4];
 TaxiWorldFrame panel_engine;TaxiDestinationPanelContext panel;TaxiRenderQueue panel_queue;TaxiWorldFramePacket *packets;TaxiRenderPacket *queue_packets;uint8_t *normal,*special;
 uint8_t scratch[38400],trig[16388],traffic_sprites[576];uint32_t selected,fill,frame[2],graphics_pointers[2],counter_67d0;
 uint16_t mask,alpha[2],scroll[2],rate;uint8_t loaded,exit;LARGE_INTEGER frequency;unsigned initialized,mode,completed;uint64_t generation,draws,waits,purity;
 TaxiOrdinaryPublishedFrame draw_packet;GLuint texture;
} TaxiSourceRestart;
static TaxiSourceRestart source_restart;
static struct {unsigned cursor,ready_commits,done;uint64_t resume;TaxiPoint position;} source_retry_check;
static int source_frame_draw_packet_from(const TaxiOrdinaryPublishedFrame *,int,int);
static void source_advance(LARGE_INTEGER,LARGE_INTEGER,uint32_t);
/* Resource replacement is a quiescent host operation, not a game reset.
   Keep persistent NPC slots and counters; invalidate only world-sized helpers. */
static int source_restart_resources_idle(void){return !source_restart.mode&&source_viewer.owner.phase==TAXI_FRAME_IDLE&&source_restart.loading.phase==TAXI_LOADING_IDLE;}
static void source_restart_resources_invalidate(void){
 TaxiSourceRestart *r=&source_restart;if(!source_restart_resources_idle())fail("Scene resources replaced with an active native owner.");
 free(r->packets);free(r->queue_packets);free(r->normal);free(r->special);
 r->packets=NULL;r->queue_packets=NULL;r->normal=NULL;r->special=NULL;r->initialized=0;
}
static int source_restart_busy(void){return source_retry_requested&&source_restart.mode;}
static void source_restart_path(char out[MAX_PATH],const char *suffix){
 DWORD n=GetModuleFileNameA(NULL,out,MAX_PATH);if(!n||n>=MAX_PATH)fail("Native Retry asset path failed.");char *tail=strrchr(out,'\\');if(!tail)fail("Native Retry directory failed.");snprintf(tail+1,MAX_PATH-(size_t)(tail+1-out),"../assets/%s",suffix);
}
static void source_restart_camera(void){
 uint8_t flags=car.world.camera_flags_29;TaxiCameraView view=taxi_camera_mode2_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,car.camera_offset_690,live_camera_mode_64c,&flags,&runtime.trig);
 taxi_view_transform_position(&source_restart.transform,view.position.x,view.position.y,view.position.z);taxi_view_transform_angles(&source_restart.transform,view.pitch,view.yaw,view.roll,runtime.trig.sine,runtime.trig.cosine);
 memcpy(native_vehicle_start_owner.actor+0xe18,source_restart.transform.words,sizeof source_restart.transform.words);
}
static int source_restart_before_wait(void *context,TaxiHostTime *host){
 TaxiSourceRestart *r=context;if(r!=&source_restart||host!=&live_host_time||r->frequency.QuadPart<=0)return -1;
 LARGE_INTEGER now;QueryPerformanceCounter(&now);source_advance(now,r->frequency,4);memcpy(native_session_init.settings,native_settings,512);memcpy(passenger_scene.session.settings.bytes,native_settings,8);r->waits++;return 0;
}
static int source_restart_call(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t a[7],uint32_t *value){
 TaxiSourceRestart *r=context;memcpy(native_session_init.settings,native_settings,512);
 int rc=taxi_normal_restart_native(&r->root,s,target,a,value);if(rc<0){fprintf(stderr,"Live Retry helper%08x phase%u graphics%u setup%u entry%u loading%u settings%u\n",target,s->phase,r->visual.graphics_index,r->setup.index,r->visual.entry_index,r->loading.phase,native_session_init.settings[0]);return rc;}
 if(!rc&&target==0x08027110)music_loaded=r->loaded;
 if(!rc&&target==0x0802277c)r->exit=native_pause.menu.exit_flag;
 /* Actual native1EE28 now owns HUD initialization on first entry and Retry.
    Publish its host presentation-cache revision; never rerun legacy HUD. */
 if(!rc&&target==0x0801ee28){++bound_hud_initializations;++bound_start_art_revision;}
 if(source_cold_entry_check_requested&&!rc){printf("COLD NATIVE HELPER:%08x phase%u RNG%u counter%u\n",target,s->phase,passenger_scene.random_state,live_host_time.counter);fflush(stdout);}
 /* Cached GPU alternate selection describes22508 transfers. Actual E154 and
    normal mask1 fade setup replace those BG colors; invalidate that view. */
 if(!rc&&(target==0x0800e154||(target==0x0800e5a8&&a[2]==1&&a[3]==1)))native_pause.alternate=0;
 if(!rc&&target==0x0802ac6c){
  runtime.trig=native_graphics_bootstrap.trig;native_scene_viewport=native_bootstrap_render.viewport;native_scene_frame=native_graphics_bootstrap.frame;native_scene_viewport_ready=native_scene_frame_ready=1;
  r->panel_engine.projection=native_scene_projection();r->panel.background_descriptor=native_scene_registration.background;r->panel.border_color=live_taxi_registry.records[16][14];native_scene_border=r->panel.border_color;
 }
 if(!rc&&target==0x08014a58){
  camera_heading=car.initial_camera_heading_654;camera_pitch=car.initial_camera_pitch_650;live_camera_mode_64c=car.initial_camera_mode_64c;memcpy(&camera_base,native_vehicle_start_owner.actor+0x680,4);live_taxi_cache=(uint32_t)native_vehicle_start_owner.last_sprite_53e0;
 }
 if(!rc&&target==0x0800b6ec){memcpy(&camera_offsets.zoom_2628,native_session_init.sound_2618+16,4);native_pause.menu.zoom=(int16_t)camera_offsets.zoom_2628;}
 if(!rc&&target==0x08011490)music_fade_factor=256;
 return rc;
}
static void source_restart_cancel(void *context,uint32_t target){TaxiSourceRestart *r=context;taxi_normal_restart_native_cancel(&r->root,target);}
static void source_restart_bind(LARGE_INTEGER frequency){
 TaxiSourceRestart *r=&source_restart;if(source_viewer.owner.phase!=TAXI_FRAME_IDLE||r->mode||scene_index<0||scene_index>1||!audio_live)fail("Live Retry owner boundary invalid.");
 r->frequency=frequency;
 music_context.main_mode=0;music_context.sound_active=0;music_context.random=music_shared_random;music_context.context=&passenger_scene.random_state;
 if(!r->initialized){
  char name[MAX_PATH];source_restart_path(name,"driving.ctr");FILE *f=fopen(name,"rb");if(!f||fread(r->trig,1,sizeof r->trig,f)!=sizeof r->trig||fclose(f)||memcmp(r->trig,"CTR1",4))fail("Native Retry trig data invalid.");
  source_restart_path(name,"noncustomer-bitmaps.bin");uint8_t raw[16];f=fopen(name,"rb");if(!f||fread(raw,1,16,f)!=16||fgetc(f)!=EOF||fclose(f)||taxi_noncustomer_bitmaps_decode(r->bitmaps,raw,16))fail("Native Retry NPC data invalid.");
  size_t capacity=(size_t)world.np+world.special_count+1;r->packets=calloc(capacity,sizeof *r->packets);r->queue_packets=calloc(capacity,sizeof *r->queue_packets);r->normal=calloc(capacity,1);r->special=calloc(capacity,1);if(!r->packets||!r->queue_packets||!r->normal||!r->special)fail("Native Retry panel allocation failed.");taxi_render_queue_init(&r->panel_queue,r->queue_packets,capacity);
  r->panel_engine=(TaxiWorldFrame){.world=&world,.trig=&runtime.trig,.bounds_corners=native_graphics_bootstrap.frame.corners,.projection=(native_scene_viewport_ready&&native_scene_frame_ready?native_scene_projection():(TaxiTrafficProjection){0}),.right=240,.bottom=160,.textures=&native_indexed_scene,.texture_count=native_indexed_scene.count,.material_cache=native_materials.cache,.material_cache_size=512,.queue=&r->panel_queue,.packets=r->packets,.capacity=capacity,.normal_seen=r->normal,.normal_seen_size=capacity,.special_seen=r->special,.special_seen_size=capacity,.normal_stamps=native_graphics_bootstrap.storage.pool_1b6ec,.normal_stamp_count=12288,.special_stamps=native_graphics_bootstrap.storage.pool_276ec,.special_stamp_count=4096};
  r->panel=(TaxiDestinationPanelContext){native_panel_pixels,r->scratch,&r->panel_engine,&native_indexed_scene,native_scene_registration.background,native_scene_lookup,native_scene_border};r->pool=(TaxiNoncustomerPool){r->npcs,64,&native_session_init.reset_430c,&world,r->bitmaps};r->selected=0x0849dfe0;r->mask=1;r->initialized=1;
 }
 memcpy(native_session_init.settings,native_settings,512);native_session_init.session_165e[51]=passenger_scene.session.fare.qualified_1691;
 r->state=(TaxiLoadingPresent){{240,160,native_settings[0],0},native_session_init.session_165e[38]};r->fill=native_bootstrap_render.fill_06900;
 r->buffers=(TaxiLoadingFrameBuffers){{0x06000000,0x0600a000},{native_bootstrap_first,native_bootstrap_second},{38400,38400}};
 r->content=(TaxiOrdinaryLoadingOwner){&live_taxi_owner,&source_viewer.copies,&source_viewer.immediate,&r->selected,&native_bootstrap_render.frame,38400,&r->fill,&r->mask,NULL,NULL};
 r->loading=(TaxiOrdinaryLoadingPending){.state=&r->state,.loading=&r->content,.presentation=&source_viewer.graphics,.host=&live_host_time,.service=&source_viewer.service,.wait=&live_frame_wait,.platform=source_input,.published=&r->published,.retained_palette=&r->retained,.before_wait=source_restart_before_wait,.wait_context=r};
 r->passenger_world.asset=&passenger_asset;r->passenger_world.world=&world;r->passenger_world.selection=&native_materials;r->passenger_world.settings_1eb0=native_session_init.settings;r->passenger_world.noncustomers=&r->pool;
 r->passengers=(TaxiPassengerInitPending){.states=passenger_scene.customers,.asset=&passenger_asset,.animation_1684=native_session_init.session_165e+38,.settings_1eb0=native_session_init.settings,.override_1708=&r->state.gate.override_1708};
 r->passenger_setup=(TaxiPassengerSetupOwned){.passengers=&r->passengers,.world=&r->passenger_world,.loading=&r->loading,.buffers=&r->buffers,.panel=&r->panel};
 r->setup=(TaxiRestartSetupNative){.car=&car,.drive=&drive_context,.vehicle=&native_vehicle_start_owner,.runtime=&runtime,.marker=runtime.starts+scene_index,.session=&native_session_init,.scene=&passenger_scene,.passengers=&r->passenger_setup,.traffic=traffic_actors,.traffic_capacity=50,.traffic_parameters=traffic_scales,.traffic_sprites=r->traffic_sprites,.traffic_globals=&traffic_globals,.traffic_timer=&traffic_timer,.counter_67d0=&r->counter_67d0,.audio=&live_sound_state,.engine_audio=&live_engine_audio,.audio_auxiliary=&live_sound_auxiliary_timer,.effects=&native_actor_effects.pool,.effect_ring=&native_actor_effects.ring,.pause=&native_pause.menu,.intro_1696=&live_game_start.active,.flag_262c=&drive_context.driver.motion.traction.flag_262c,.yaw_2618=&camera_yaw_offset,.yaw_target_261c=&camera_yaw_target,.use_loading_scratch=1};
 r->assets=(TaxiGraphicsBootstrapAssets){taxi_owned_package.bytes+0x7556cc+104*(unsigned)scene_index,104,r->trig+4,r->trig+8196,8192,8192,&native_indexed_scene,native_scene_palette};
 source_restart_camera();native_bootstrap_render.source_transform_address=0x0200344c;native_bootstrap_render.transform_owner=&r->transform;
 r->hud=(TaxiHudStartInput){0,native_settings[0],0,(uint8_t)native_game_driver,(uint8_t)native_game_driver};
 /* Explicit current native blend owner; original00E0/E2 initialization remains
    separately unqualified. No fabricated source8/8 coefficients. */
 r->alpha[0]=(uint16_t)(source_viewer.service.blend_alpha>>8);r->alpha[1]=(uint16_t)(source_viewer.service.blend_alpha&255);
 r->visual=(TaxiRestartVisualOwners){.loading=&r->loading,.buffers=&r->buffers,.display=&r->display,.bases=&r->bases,.bindings=&native_graphics_bindings,.bootstrap=&native_graphics_bootstrap,.render=&native_bootstrap_render,.registration=&native_scene_registration,.assets=&r->assets,.guidance=&guidance_route,.hud=&source_viewer.hud,.hud_input=&r->hud,.session=&native_session_init,.pause=&native_pause.menu,.alpha_00e0=r->alpha,.alpha_00e2=r->alpha+1,.driver_graphics_187c=r->graphics_pointers,.intro_graphics_1804=r->graphics_pointers+1,.fill_06900=&r->fill,.frame_34d4=r->frame,.frame_34d8=r->frame+1,.scroll_0010=r->scroll,.scroll_0012=r->scroll+1};
 r->resources=(TaxiRestartResources){&live_taxi_registry,&r->display,&r->bases};r->loaded=(uint8_t)music_loaded;r->music=(TaxiNormalRestartOwners){&music_tracker,&music_asset,music_bytes,181241,&passenger_scene.random_state,audio_settings_state.music_enabled,&r->loaded,taxi_restart_resources_call,&r->resources};r->prefix=(TaxiNormalRestartPrefixOwners){&r->music,native_session_init.session_165e+51,&drive_context.height.flag_153c,&drive_context.height.flag_153d};
 r->root=(TaxiNormalRestartNative){.prefix=&r->prefix,.resources=&r->resources,.visual=&r->visual,.setup=&r->setup};r->bridge=(TaxiNormalRestartPresentationOwned){.loading=&r->loading,.buffers=&r->buffers,.remainder=source_restart_call,.context=r,.cancel_delegated=source_restart_cancel};
 r->rate=(uint16_t)audio_source_frame.rate;r->retry=(TaxiNormalRetryOwned){.pause=&native_pause.menu,.loading=&r->loading,.session=&native_session_init,.scene=&passenger_scene,.sounds=&live_sound_state,.channels=audio_channels,.audio=&audio_settings_state,.tracker=&music_tracker,.output_rate=&r->rate,.override_1708=&r->state.gate.override_1708,.exit_5f1a=&r->exit};
}
static void source_restart_fade(LARGE_INTEGER frequency){source_restart_bind(frequency);if(taxi_normal_retry_owned_fade(&source_restart.retry,&native_pause_frame.events))fail("Native Retry fade owner failed.");source_restart.mode=1;city_fade=source_viewer.fade.fade;}
/* Both first entry and Retry enter a FRESH coordinator after their own
   parent boundary. No terminal cleanup/music prefix is silently replayed. */
static void source_restart_start(void){
 TaxiSourceRestart *r=&source_restart;
 if(r->mode||source_viewer.owner.phase!=TAXI_FRAME_IDLE||r->loading.phase!=TAXI_LOADING_IDLE||r->visual.graphics_index||r->visual.entry_index||r->setup.phase||r->root.flow)fail("Native entry requires fresh quiescent bindings.");
 r->flow=(TaxiNormalRestartFlow){.prefix={passenger_scene.session.fare.qualified_1691,drive_context.height.flag_153c,drive_context.height.flag_153d},.graphics={.retained_sl=1,.context_base=0x02002634,.city=(uint8_t)scene_index}};r->mode=2;r->generation=r->published.generation;
 if(taxi_normal_restart_flow_begin(&r->flow,taxi_normal_restart_presentation_owned,&r->bridge)<0)fail("Native entry restart begin failed.");
}
static void source_restart_begin(void){
 TaxiSourceRestart *r=&source_restart;if(taxi_normal_retry_owned_transfer(&r->retry)!=2)fail("Native Retry terminal cleanup failed.");
 r->mode=0;source_restart_start();
 printf("LIVE NATIVE RETRY BEGIN: city%d driver%u RNG%u; actual AC70 owners, no legacy reset.\n",scene_index+1,native_game_driver,passenger_scene.random_state);fflush(stdout);
}
static void source_restart_draw(void){
 TaxiSourceRestart *r=&source_restart;const TaxiLoadingPublishedFrame *p=&r->published;if(!p->valid)return;
 TaxiDriveWorld before_car=car;TaxiPassengerScene before_passengers=passenger_scene;TaxiActorEffects before_effects=native_actor_effects;TaxiLoadingPublishedFrame before_publication=r->published;TaxiNormalRestartFlow before_flow=r->flow;TaxiOrdinaryLoadingPending before_loading=r->loading;
 static uint8_t rgba[153600];for(unsigned i=0;i<38400;i++){uint16_t c=p->banks.background[p->background[i]];for(unsigned k=0;k<3;k++)rgba[i*4+k]=(uint8_t)((((c>>(k*5))&31)*255)/31);rgba[i*4+3]=255;}
 r->draw_packet.assets=p->assets;r->draw_packet.banks=p->banks;memcpy(r->draw_packet.oam,p->oam,sizeof p->oam);r->draw_packet.valid=1;
 target_begin();glViewport(0,0,target_width,target_height);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_ALPHA_TEST);glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
 if(!r->texture)glGenTextures(1,&r->texture);glBindTexture(GL_TEXTURE_2D,r->texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
 glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,target_width,target_height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();double scale=target_height/160.0,left=(target_width-240*scale)/2;glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2d(left,0);glTexCoord2i(1,0);glVertex2d(left+240*scale,0);glTexCoord2i(1,1);glVertex2d(left+240*scale,target_height);glTexCoord2i(0,1);glVertex2d(left,target_height);glEnd();source_frame_draw_packet_from(&r->draw_packet,target_width,target_height);
 if(glGetError()!=GL_NO_ERROR)fail("Native Retry loading GPU failed.");
 if(memcmp(&before_car,&car,sizeof car)||memcmp(&before_passengers,&passenger_scene,sizeof passenger_scene)||memcmp(&before_effects,&native_actor_effects,sizeof native_actor_effects)||memcmp(&before_publication,&r->published,sizeof r->published)||memcmp(&before_flow,&r->flow,sizeof r->flow)||memcmp(&before_loading,&r->loading,sizeof r->loading))fail("Native Retry loading GPU mutated game/publication state.");r->draws++;r->purity++;
}
static void source_restart_poll(void){
 TaxiSourceRestart *r=&source_restart;int rc=taxi_normal_restart_flow_step(&r->flow,taxi_normal_restart_presentation_owned,&r->bridge);if(rc<0)fail("Live native Retry restart failed.");
 if(r->published.generation!=r->generation){r->generation=r->published.generation;city_fade=source_viewer.fade.fade;source_restart_draw();}
 if(rc==1){
  if(music_context.main_mode||music_context.context!=&passenger_scene.random_state||music_context.random!=music_shared_random||native_pause.menu.palette_request||source_viewer.service.pending_5f24||native_pause.alternate||camera_offsets.zoom_2628!=0||native_pause.menu.zoom!=0||passenger_scene.session.distance_display.offset_16c4!=-68)fail("Native Retry consumer aliases inconsistent at READY.");
  ordinary_resources_pending=0;
  passenger_scene_initialized=1;native_vehicle_start_initialized=1;native_start_art_owned_active=1;
  r->mode=0;r->completed++;city_fade=source_viewer.fade.fade;city_fade_cap=0;live_taxi_cache=(uint32_t)native_vehicle_start_owner.last_sprite_53e0;live_scene_palette_index=16;ordinary_graphics_ready=1;
  taxi_game_start_begin_ordinary(&live_game_start,240);memcpy(session_settings_snapshot,native_session_init.snapshot,512);native_scene_frame=native_graphics_bootstrap.frame;native_scene_viewport=native_bootstrap_render.viewport;native_effect_request_count=0;traffic_submission_count=0;native_panel_generation++;
  previous_position=car.world.height.position;previous_vehicle_heading=car.world.heading_1c;previous_motion_heading=car.world.heading_20;previous_body_roll=car.world.angle_1a;previous_camera_heading=camera_heading;previous_camera_pitch=camera_pitch;previous_camera_yaw_offset=camera_yaw_offset;previous_camera_offsets=camera_offsets;previous_camera_mode_64c=live_camera_mode_64c;memcpy(previous_customers,passenger_scene.customers,sizeof previous_customers);memcpy(previous_traffic_actors,traffic_actors,sizeof previous_traffic_actors);
  source_viewer.trace_ns=0;source_viewer.trace_ticks=0;source_retry_check.resume=source_commits;source_retry_check.cursor=0;source_retry_check.ready_commits=0;source_retry_check.position=car.world.height.position;source_frame_pad=taxi_xbox_decode(0,0,0,0,0);
  printf("LIVE NATIVE RETRY READY: restart%u city%d pubs%llu beforewait%llu RNG%u NPC%u intro%u fade%u; actual loading/setup/entry complete.\n",r->completed,scene_index+1,(unsigned long long)r->published.generation,(unsigned long long)r->waits,passenger_scene.random_state,native_session_init.reset_430c,live_game_start.active,city_fade.active_177b);fflush(stdout);
 }
}
static TaxiXboxSample source_restart_smoke_pad(void){
 if(source_restart.mode==2)return taxi_xbox_decode(0,0,0,0,0);
 if(!source_restart.mode&&!city_fade.active_177b&&!live_game_start.active&&source_commits>=source_retry_check.resume+60){
  if(source_restart.completed>=2){
   if(++source_retry_check.ready_commits>=90){TaxiPoint p=car.world.height.position;if(p.x==source_retry_check.position.x&&p.z==source_retry_check.position.z)fail("Native Retry did not resume driving.");source_retry_check.done=1;}
  }else{
   static const uint16_t buttons[9]={0x10,0,2,0,0x1000,0,1,0,0x1000};if(source_retry_check.cursor<9)return taxi_xbox_decode(buttons[source_retry_check.cursor++],0,0,0,0);
  }
 }
 return source_retry_check.cursor?taxi_xbox_decode(0,0,0,0,0):taxi_xbox_decode(0,0,0,255,0);
}
static void source_start_phase_owned(void){
 if(taxi_start_art_replace_owned(&live_taxi_owner,&source_viewer.copies,&source_viewer.immediate,live_game_start.phase,0)||source_viewer.copies.count<3)fail("Native ordinary intro phase enqueue failed.");++bound_start_art_revision;
}
static void source_restart_capture(void){
 static int captured;const char *name=getenv("TAXI_NATIVE_RETRY_CAPTURE");if(!source_retry_smoke||!source_retry_check.done||captured||!name)return;
 size_t stride=(size_t)target_width*3,bytes=stride*(size_t)target_height;uint8_t *pixels=malloc(bytes);if(!pixels)fail("Native Retry capture allocation failed.");GLint pack;glGetIntegerv(GL_PACK_ALIGNMENT,&pack);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(target_bound?0x8ce0:GL_BACK);glFinish();glReadPixels(0,0,target_width,target_height,GL_RGB,GL_UNSIGNED_BYTE,pixels);glPixelStorei(GL_PACK_ALIGNMENT,pack);if(glGetError()!=GL_NO_ERROR)fail("Native Retry capture read failed.");
 FILE *f=fopen(name,"wb");if(!f||fprintf(f,"P6\n%d %d\n255\n",target_width,target_height)<0)fail("Native Retry capture output failed.");for(int y=target_height;y--;)if(fwrite(pixels+(size_t)y*stride,1,stride,f)!=stride)fail("Native Retry capture write failed.");if(fclose(f))fail("Native Retry capture close failed.");free(pixels);captured=1;
}
static void source_restart_report(void){
 if(!source_retry_requested)return;if(source_retry_smoke&&(native_pause.alternate||native_pause.menu.palette_request||source_viewer.service.pending_5f24))fail("Native Retry retained pause palette selection.");if(source_retry_smoke&&(!source_retry_check.done||source_restart.completed!=2||source_restart.purity!=source_restart.draws))fail("Live Retry smoke did not complete two restarts and resumed driving.");
 printf("LIVE NATIVE RETRY REPORT: restarts%u loadingpubs%llu GPU%llu prewait%llu resumed%u; current camera/backend adaptation retained, full original frame unqualified.\n",source_restart.completed,(unsigned long long)source_restart.published.generation,(unsigned long long)source_restart.draws,(unsigned long long)source_restart.waits,source_retry_check.done);
 if(source_restart.texture)glDeleteTextures(1,&source_restart.texture);free(source_restart.packets);free(source_restart.queue_packets);free(source_restart.normal);free(source_restart.special);
}
