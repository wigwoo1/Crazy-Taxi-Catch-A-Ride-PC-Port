/* Direct ordinary-driving candidate. Full startup/parent routes remain separate.
   Source frame work and GPU publication precede the next simulation update;
   physical presentation only blits the last complete native4K framebuffer. */
typedef struct {
 TaxiSpriteCopies copies;TaxiSpriteImmediateCopy immediate;TaxiPaletteFadeStart fade;
 TaxiCityPaletteOwners palette;uint16_t mosaic;TaxiOrdinaryPresentationOwner graphics;
 TaxiPresentationService service;TaxiHudStartOwners hud;TaxiOrdinaryFrameOwner owner;
 TaxiOrdinaryPublishedFrame published;uint32_t anchor[3],begin_counter;
 LARGE_INTEGER previous;unsigned reset_seen;int initialized,focus,shown_cap;
 uint64_t trace_ns;uint32_t trace_ticks;
} TaxiSourceViewer;
static TaxiSourceViewer source_viewer;
static uint64_t source_prepares,source_commits,source_samples,source_gpu_draws,source_gpu_objects,source_purity_checks,source_resets,source_blend_skips;
#include "source_frame_profile.inc"
#include "source_frame_lifecycle_check.inc"
static TaxiXboxSample source_restart_smoke_pad(void);
static int source_unexpected(void *context,TaxiPresentationService *service,uint32_t target,uint32_t argument){(void)context;(void)service;(void)target;(void)argument;return -1;}
static uint16_t source_keyboard_keys(TaxiXboxSample pad){
 uint16_t held=(uint16_t)(taxi_xbox_driving_keys(pad,(uint8_t)((native_settings[0]>>1)&3))^1023);
 if(GetAsyncKeyState(VK_RETURN)&0x8000)held|=8;
 if(GetAsyncKeyState('Z')&0x8000)held|=1;if(GetAsyncKeyState('X')&0x8000)held|=2;
 if(native_pause.menu.frozen){if(GetAsyncKeyState(VK_UP)&0x8000)held|=64;if(GetAsyncKeyState(VK_DOWN)&0x8000)held|=128;}
 else{
  static const int keys[6]={VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,'D','S'};
  TaxiActionContext actions={.profile=(uint8_t)((native_settings[0]>>1)&3),.vehicle_flags=car.driver.power.gears.flags_4ac};
  for(unsigned action=0;action<6;action++)if(GetAsyncKeyState(keys[action])&0x8000){
   for(unsigned button=0;button<10;button++){TaxiInput trial={0};trial.held[button]=trial.pressed[button]=trial.repeat[button]=1;if(taxi_action_query(&trial,actions,action)){held|=(uint16_t)(1u<<button);break;}}
  }
 }
 return (uint16_t)(held^1023);
}
static int source_input(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 (void)context;if(host!=&live_host_time||target!=0x08015c3c||argument)return -1;
 source_frame_pad=source_earned_requested?source_earned_pad(service->delta_3b00):source_parent_results_smoke?source_parent_results_pad():source_parent_reentry_smoke?source_parent_reentry_pad():source_retry_smoke?source_restart_smoke_pad():source_frame_lifecycle_smoke?source_lifecycle_pad():audio_smoke_frames?taxi_xbox_decode(0,0,0,255,0):taxi_xbox_poll_any();
 uint16_t keys=audio_smoke_frames?taxi_xbox_driving_keys(source_frame_pad,(uint8_t)((native_settings[0]>>1)&3)):source_keyboard_keys(source_frame_pad);
 if(earned_capture_path||earned_evaluation_path){earned_capture_buttons=driving_buttons;earned_capture_sample_delta=service->delta_3b00;}
 source_context_input_before(source_frame_pad,keys,service->delta_3b00);
 taxi_input_update(&driving_buttons,(TaxiInputSample){.active_low_keys=keys,.elapsed_ticks=service->delta_3b00});source_samples++;
 source_context_input_after();
 if(input_time_trace){fprintf(input_time_trace,"%u %u %u\n",service->delta_3b00,service->delta_3b00,driving_buttons.raw_keys);fflush(input_time_trace);}return 0;
}
#include "source_restart_viewer.inc"
#include "source_taxi_capture.inc"
static void source_frame_fade_tick(int request,int32_t delta){
 (void)delta;if(request&&source_parent_requested&&passenger_scene.session.outer_state_1ea8==2&&!native_pause.menu.exit_flag){source_parent_expiry_fade();return;}if(request){taxi_palette_fade_begin(&source_viewer.fade.fade,0);source_viewer.fade.masks.mode=0;}
 city_fade=source_viewer.fade.fade;
}
static void source_advance(LARGE_INTEGER now,LARGE_INTEGER frequency,uint32_t site){
 SourceProfileToken measured=source_profile_begin(SP_EVENTS);
 if(now.QuadPart<source_viewer.previous.QuadPart)fail("Source frame clock moved backwards.");
 uint64_t ns=(uint64_t)((double)(now.QuadPart-source_viewer.previous.QuadPart)*1e9/frequency.QuadPart);source_viewer.previous=now;
 uint32_t ticks;source_timing_advance(site,&ns,&ticks);
 source_viewer.trace_ns+=ns;source_viewer.trace_ticks+=ticks;source_profile_end(measured);
}
/* Wire a quiescent retained frame owner without initializing game state,
   clearing queues, importing a clock baseline or running original helpers. */
static int source_bind(void){
 if(!source_restart_resources_idle())return -1;
 taxi_owned_bind();
 source_viewer.hud.graphics=&live_taxi_owner;source_viewer.hud.copies=&source_viewer.copies;source_viewer.hud.immediate=&source_viewer.immediate;
 source_viewer.palette=(TaxiCityPaletteOwners){&live_taxi_registry,&live_taxi_banks,&source_viewer.fade,taxi_owned_package.bytes,taxi_owned_package.size,240,160};
 source_viewer.graphics=(TaxiOrdinaryPresentationOwner){&live_taxi_owner,&source_viewer.palette,&source_viewer.copies,&source_viewer.immediate,&source_viewer.mosaic,source_unexpected,NULL};
 source_viewer.owner.graphics=&source_viewer.graphics;source_viewer.owner.host=&live_host_time;source_viewer.owner.service=&source_viewer.service;source_viewer.owner.wait=&live_frame_wait;
 source_viewer.owner.pause=&native_pause;source_viewer.owner.pause_frame=&native_pause_frame;source_viewer.owner.cache=&live_taxi_cache;source_viewer.owner.anchor9ab0=source_viewer.anchor;
 source_viewer.owner.shared1534=native_session_init.shared_1534;source_viewer.owner.platform=source_input;source_viewer.owner.published=&source_viewer.published;
 return taxi_ordinary_presentation_owner_validate(&source_viewer.graphics);
}
static void source_initialize(void){
 SourceProfileToken measured=source_profile_begin(SP_INITIALIZE);
 if(source_viewer.initialized)taxi_ordinary_frame_cancel(&source_viewer.owner);
 memset(&source_viewer,0,sizeof source_viewer);source_viewer.reset_seen=host_reset_revision;
 source_frame_active=1;live_taxi_registry=(TaxiPaletteRegistration){.flags=4};ordinary_graphics_ready=0;live_scene_palette_index=-1;taxi_owned_initialize();
 if(source_bind())fail("Source frame binding failed.");
 TaxiHudStartInput hud={0,native_settings[0],0,(uint8_t)native_game_driver,(uint8_t)native_game_driver};if(taxi_hud_start_owned(&source_viewer.hud,&hud))fail("Source frame HUD initialization failed.");
 live_taxi_cache=UINT32_MAX;source_frame_pad=(TaxiXboxSample){.active_low_keys=1023};memset(&native_pause,0,sizeof native_pause);memset(&native_pause_frame,0,sizeof native_pause_frame);
 source_viewer.fade.fade=city_fade;if(taxi_palette_fade_setup(&source_viewer.fade.masks,city_fade.mode_1773,32,1,1))fail("Source frame scalar palette configuration failed.");
 live_host_time.frame.previous_3524=live_host_time.counter;live_host_time.frame.delta_3b00=1;live_host_time.frame.animation_1e10=live_taxi_fonts.cursor;
 source_viewer.service=(TaxiPresentationService){.current_3520=live_host_time.counter,.previous_3524=live_host_time.counter,.delta_3b00=1,.elapsed_755c=live_host_time.frame.elapsed_755c,.cursor_1e10=live_taxi_fonts.cursor,.parity_7540=live_host_time.frame.page_7540};
 source_viewer.initialized=1;source_resets++;live_host_ready=0;presented_pose_valid=0;
 target_begin();glViewport(0,0,target_width,target_height);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 QueryPerformanceCounter(&source_viewer.previous);
 source_profile_end(measured);
 printf("SOURCE FRAME INIT: reset%llu driver%u registry%u counter%u; deferred copy0, native wait/input/palette/OAM/copies, PC bitmap backend.\n",(unsigned long long)source_resets,native_game_driver,(unsigned)live_taxi_registry.count,live_host_time.counter);
}
static void source_prepare(LARGE_INTEGER frequency){
 SourceProfileToken measured=source_profile_begin(SP_PREPARE);
 if(source_retry_requested&&source_restart.mode==1&&!city_fade.active_177b){source_restart_begin();source_profile_end(measured);return;}
 if(taxi_pause_menu_control_sampled(&native_pause.menu,&driving_buttons,city_fade.active_177b,(uint8_t)live_game_start.active,passenger_scene.session.ended_1697,&native_pause_frame.events))fail("Source sampled pause control failed.");
 pause_consume_audio(&native_pause_frame.events);source_lifecycle_after_control();if(native_pause_frame.events.fade_out){if(source_retry_requested&&native_pause.menu.outer_state==1)source_restart_fade(frequency);else if(source_parent_requested&&native_pause.menu.outer_state==2)source_parent_quit_fade(frequency);else source_frame_fade_tick(1,live_host_time.frame.delta_3b00);}
 if(native_pause.menu.outer_state&&!city_fade.active_177b){if(source_parent_requested&&native_pause.menu.outer_state==2){if(native_pause.menu.exit_flag)source_parent_quit_finish();else source_parent_results_finish();source_profile_end(measured);return;}fail("Source-frame parent retry/quit integration is pending; original executable remains available.");}
 SourceProfileToken simulation=source_profile_begin(SP_SIMULATION);drive_tick(source_frame_pad,0);source_profile_end(simulation);live_host_updates++;if(simulation_error)fail("Source-frame native driving stopped on invalid state.");
 SourceProfileToken projection=source_profile_begin(SP_PROJECTION);
 TaxiCameraView view=presented_camera_view(1.0);TaxiPoint anchor=car.world.height.position,projected;anchor.y-=192;
 if(!taxi_camera_project(&projected,anchor,view.position,view.pitch,view.yaw,view.roll,192,&runtime.trig)||projected.z<=0)fail("Source-frame current taxi projection unsupported.");
 unsigned angle=(unsigned)(0u-(uint16_t)car.world.angle_1a)&4095;TaxiCameraOffsets offsets=presentation_offsets(camera_offsets);TaxiHalfPlan plan;
 if(taxi_half_plan(&plan,projected.x,projected.y,192,offsets.distance_770,camera_base,runtime.trig.sine[angle],runtime.trig.cosine[angle]))fail("Source-frame taxi half projection failed.");
 TaxiOrdinaryFrameInput input={.frame=taxi_sprite_frame(view.yaw,car.world.heading_1c),.scale=(uint32_t)plan.scale,.driver=(uint8_t)native_game_driver,.roll=(uint16_t)car.world.angle_1a,.sine=runtime.trig.sine[angle],.cosine=runtime.trig.cosine[angle]};
 for(unsigned h=0;h<2;h++){uint32_t *point=h?input.right:input.left;point[0]=(uint32_t)plan.centers[h][0];point[1]=(uint32_t)plan.centers[h][1];point[2]=(uint32_t)projected.z;}
 native_session_init.shared_1534[0]=passenger_scene.session.audio_muted_1534;
 source_profile_end(projection);SourceProfileToken graphics=source_profile_begin(SP_GRAPHICS);
 if(taxi_ordinary_frame_prepare(&source_viewer.owner,&input)||taxi_ordinary_frame_effects(&source_viewer.owner,native_effect_requests,(size_t)native_effect_request_count))fail("Source-frame native graphics preparation failed.");
 source_taxi_capture_prepare(view,&plan,projected,&offsets,&input);
 source_profile_end(graphics);source_viewer.shown_cap=source_viewer.fade.fade.progress_178a;
 LARGE_INTEGER now;QueryPerformanceCounter(&now);source_advance(now,frequency,1);source_viewer.begin_counter=live_host_time.counter;
 SourceProfileToken wait=source_profile_begin(SP_WAIT_BEGIN);
 if(taxi_ordinary_frame_begin(&source_viewer.owner)<0)fail("Source-frame native fresh wait failed.");source_prepares++;live_frame_wait_begins++;source_profile_end(wait);source_profile_end(measured);
}
static int source_frame_draw_packet_from(const TaxiOrdinaryPublishedFrame *p,int width,int height){
 if(!p)return 1;SourceProfileToken measured=source_profile_begin(SP_OBJECTS);
 static const unsigned widths[3][4]={{8,16,32,64},{16,32,32,64},{8,8,16,32}},heights[3][4]={{8,16,32,64},{8,8,16,32},{16,32,32,64}};
 double pixel=(double)height/160;glPushAttrib(GL_ALL_ATTRIB_BITS);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 for(int i=127;i>=0;i--){
  uint16_t a=p->oam[i][0],b=p->oam[i][1],c=p->oam[i][2];int affine=!!(a&256);if(!affine&&(a&512))continue;
  unsigned shape=a>>14,size=b>>14;if(shape>=3||(a&8192)||(a&0xc00)>0x400)fail("Source published OBJ shape/mode unsupported.");
  unsigned w=widths[shape][size],h=heights[shape][size];int doubled=affine&&!!(a&512);unsigned fieldw=w*(unsigned)(doubled+1),fieldh=h*(unsigned)(doubled+1);
  int left=b&511,top=a&255;if(left>=256)left-=512;if(top>=160)top-=256;
  uint8_t pixels[16384],flipped[16384];if(taxi_obj_tiles_rgba(pixels,sizeof pixels,&p->assets,&p->banks,live_taxi_owner.destination,c,w,h))fail("Source published OBJ pixels invalid.");
  if(!affine&&(b&0x3000)){for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++)memcpy(flipped+(y*w+x)*4,pixels+(((b&0x2000)?h-1-y:y)*w+((b&0x1000)?w-1-x:x))*4,4);memcpy(pixels,flipped,w*h*4);}
  int16_t matrix[4]={256,0,0,256};if(affine){unsigned slot=(b>>9)&31;for(unsigned j=0;j<4;j++)matrix[j]=(int16_t)p->oam[slot*4+j][3];}
  GLuint texture;glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,(GLsizei)w,(GLsizei)h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  if(!taxi_obj_shader_frame_begin(matrix,doubled,w,h,w,h,0,0,0))fail("Source published OBJ shader failed.");
  if((a&0xc00)==0x400){if(!native_effect_background_valid||!native_effect_background_coverage_valid){source_blend_skips++;taxi_obj_shader_end();glDeleteTextures(1,&texture);continue;}if(!taxi_obj_shader_background(native_effect_background,(unsigned)native_effect_background_width,(unsigned)native_effect_background_height,0x1f4f,0x0808))fail("Source published smoke background invalid.");}
  double x=width*0.5+(left-120)*pixel,y=top*pixel;glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2d(x,y);glTexCoord2i((int)fieldw,0);glVertex2d(x+fieldw*pixel,y);glTexCoord2i((int)fieldw,(int)fieldh);glVertex2d(x+fieldw*pixel,y+fieldh*pixel);glTexCoord2i(0,(int)fieldh);glVertex2d(x,y+fieldh*pixel);glEnd();taxi_obj_shader_end();glDeleteTextures(1,&texture);source_gpu_objects++;
 }
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();source_profile_end(measured);return 1;
}
static int source_frame_draw_packet(int width,int height){return source_frame_draw_packet_from(taxi_ordinary_frame_published(&source_viewer.owner),width,height);}
static void source_gpu_render(void){
 SourceProfileToken measured=source_profile_begin(SP_RENDER),snap=source_profile_begin(SP_SNAPSHOT);
 TaxiTitleAssets assets=live_taxi_assets;TaxiPaletteBanks banks=live_taxi_banks;TaxiPaletteRegistration registry=live_taxi_registry;TaxiFontSetup fonts=live_taxi_fonts;TaxiStartupObjQueue queue=live_taxi_queue;TaxiStartupObjTransport transport=live_taxi_transport;
 TaxiHudStartOwners hud_before=source_viewer.hud;
 uint32_t cache=live_taxi_cache;uint8_t shared[8];memcpy(shared,native_session_init.shared_1534,8);TaxiOrdinaryPublishedFrame packet=source_viewer.published;TaxiDriveWorld driving=car;TaxiDriveWorldContext context=drive_context;TaxiPassengerScene passengers=passenger_scene;TaxiActorEffects effects=native_actor_effects;
 source_profile_end(snap);presented_pose_valid=0;target_begin();
 SourceProfileToken submit=source_profile_begin(SP_DRAW);unsigned gpu_query=source_profile_gpu_begin();draw(target_width,target_height);source_profile_gpu_end(gpu_query);source_profile_end(submit);
 if(glGetError()!=GL_NO_ERROR)fail("Source-frame native4K GPU error.");
 SourceProfileToken pure=source_profile_begin(SP_PURITY);
 if(memcmp(&hud_before,&source_viewer.hud,sizeof hud_before)||memcmp(&assets,&live_taxi_assets,sizeof assets)||memcmp(&banks,&live_taxi_banks,sizeof banks)||memcmp(&registry,&live_taxi_registry,sizeof registry)||memcmp(&fonts,&live_taxi_fonts,sizeof fonts)||memcmp(&queue,&live_taxi_queue,sizeof queue)||memcmp(&transport,&live_taxi_transport,sizeof transport)||cache!=live_taxi_cache||memcmp(shared,native_session_init.shared_1534,8)||memcmp(&packet,&source_viewer.published,sizeof packet)||memcmp(&driving,&car,sizeof driving)||memcmp(&context,&drive_context,sizeof context)||memcmp(&passengers,&passenger_scene,sizeof passengers)||memcmp(&effects,&native_actor_effects,sizeof effects))fail("Source-frame GPU drawing changed canonical game owners.");
 source_gpu_draws++;source_purity_checks++;source_profile_end(pure);source_profile_end(measured);source_restart_capture();
}
static void source_frame_iteration(LARGE_INTEGER frequency,int focus){
 if(!source_frame_requested)return;
 if(ordinary_resources_pending&&source_restart.mode!=2)fail("Ordinary resources require native initialization before driving.");
 if(!drive_mode||(startup_mode&&(!source_parent_requested||source_parent_phase!=SOURCE_PARENT_DRIVE))||live_attract||source_attract_ai||challenge_jump_drive_preview)fail("Source-frame candidate requires direct ordinary driving; startup/other modes remain pending.");
 if(source_parent_requested&&(!source_viewer.initialized||source_viewer.reset_seen!=host_reset_revision))fail("Native parent lost its retained ordinary frame binding.");
 if(!source_viewer.initialized||source_viewer.reset_seen!=host_reset_revision)source_initialize();
 LARGE_INTEGER now;QueryPerformanceCounter(&now);if(!focus){source_viewer.previous=now;source_viewer.focus=0;return;}
 if(!source_viewer.focus){source_viewer.previous=now;source_viewer.focus=1;}else source_advance(now,frequency,2);
 if(source_retry_requested&&source_restart.mode==2){source_restart_poll();return;}
 if(source_viewer.owner.phase==TAXI_FRAME_WAITING){
  source_lifecycle_before_poll();
  SourceProfileToken poll=source_profile_begin(SP_PUBLICATION);int32_t prior=live_host_time.frame.delta_3b00;int result=taxi_ordinary_frame_poll(&source_viewer.owner);poll.stage=result?SP_PUBLICATION:SP_EMPTY_POLL;source_profile_end(poll);if(result<0)fail("Source-frame native publication failed.");
  if(result){
   if(live_host_time.counter==source_viewer.begin_counter)fail("Source-frame consumed a stale pre-begin event.");source_commits++;live_frame_wait_commits++;
   city_fade=source_viewer.fade.fade;city_fade_cap=source_viewer.shown_cap<0?0:source_viewer.shown_cap>31?31:source_viewer.shown_cap;
   if(host_time_trace){fprintf(host_time_trace,"%llu %u %d %u %d %u 1 %u %llu %u\n",(unsigned long long)source_viewer.trace_ns,source_viewer.trace_ticks,prior,live_host_time.counter,live_host_time.frame.delta_3b00,(unsigned)native_pause.menu.frozen,live_host_updates,(unsigned long long)live_host_audio_ticks,audio_frames);fflush(host_time_trace);}source_viewer.trace_ns=0;source_viewer.trace_ticks=0;
   source_lifecycle_after_commit();
   source_gpu_render();
   source_taxi_capture_publish();
  }
 }
 if(source_viewer.owner.phase==TAXI_FRAME_IDLE)source_prepare(frequency);
}
static void source_frame_report(void){source_restart_report();if(source_frame_requested)printf("SOURCE FRAME LIVE: prepared%llu commits%llu inputs%llu gpu%llu objects%llu purity%llu resets%llu blend_skips%llu; ordinary direct candidate, mask1 palette backend and upstream/parent routes remain unqualified.\n",(unsigned long long)source_prepares,(unsigned long long)source_commits,(unsigned long long)source_samples,(unsigned long long)source_gpu_draws,(unsigned long long)source_gpu_objects,(unsigned long long)source_purity_checks,(unsigned long long)source_resets,(unsigned long long)source_blend_skips);}