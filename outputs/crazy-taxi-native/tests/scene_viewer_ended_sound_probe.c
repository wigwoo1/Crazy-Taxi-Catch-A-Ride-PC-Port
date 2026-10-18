#include "material_phase.h"
#include "reference_driving.h"
#include "hud_cash_pixels.h"
#include "render_pose_history.h"
#include "frame_time.h"
#include "host_time.h"
#include "obj_tiles_rgba.h"
#include "effect_render_owned.h"
#include "taxi_render_owned.h"
#include "taxi_oam_layer.h"
#include "world_cpu_scene.h"
#include "taxi_graphics_package.h"
#include "taxi_graphics_owned.h"
#include "taxi_tiles_rgba.h"
#include "vehicle_math.h"
#include "startup_intro_viewer.h"
#include "title_entry.h"
#include "title_requests.h"
#include "title_publish.h"
#include "title_pixels.h"
#include "audio_frame.h"
#include "music_session.h"
#include "vehicle_start_finalize.h"
#include "vehicle_start_gameplay.h"
#include "music_playback.h"
#include "pause_runtime.h"
#include "tip_hud.h"
#include "reward_session.h"
#include "challenge_bound_course.h"
#include "challenge_turn_course.h"
#include "challenge_drift_course.h"
#include "challenge_result_strings.h"
#include "challenge_completion.h"
#include "title_prompt.h"
#include "body_points.h"
#include "effect_presentation_slots.h"
#include "effect_presentation_interpolation.h"
#include "effect_emit.h"
#include "actor_effects.h"
#include "effect_asset.h"
#include "title_runtime.h"
#include "ai_controller.h"
#include "attract_session.h"
#include "title_scroll.h"
#include "title_flow.h"
#include "obj_shader.h"
#include "obj_order.h"
/* Native Win32/OpenGL scene inspection and optional --drive prototype.
   Recovered C simulation runs from extracted assets; no GBA CPU execution.
   Full game systems and original camera/actor presentation remain pending. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "impact_voice.h"
#include "audio_host.h"
#include "audio_channel.h"
#include "game_audio_engine.h"
#include "game_audio_frame_bridge.h"
#include "game_audio_cleanup.h"
#include "normal_exit.h"
#include "credits_asset.h"
#include "credits_runtime.h"
#include "game_audio_levels.h"
#include "game_audio_voice.h"
#include "audio_settings_state.h"
#include "audio_mix.h"
#include "music_tracker.h"
#include "random.h"
#include "passenger_idle.h"
#include "xbox.h"
#include "loading_image.h"
#include "loading_flow.h"
#include "game_start.h"
#include "runtime_asset.h"
#include "world_asset.h"
#include "world_frame.h"
#include "destination_panel.h"
#include "material_blink.h"
#include "shade_table.h"
#include "render_viewport.h"
#include "render_frame_init.h"
#include "projection_divide.h"
#include "graphics_bootstrap_tail.h"
#include "palette_registration.h"
#include "session_init_fare.h"
#include "world_background.h"
#include "scene_registration.h"
#include "material_shader.h"
#include "axis_shader.h"
#include "challenge_flag_indexed.h"
#include "challenge_ball_projection.h"
#include "challenge_ball_course.h"
#include "challenge_ball_hud.h"
#include "challenge_drift_hud.h"
#include "challenge_ball_palette.h"
#include "challenge_assets.h"
#include "challenge_flag_course.h"
#include "challenge_flag_hud.h"
static int challenge_flag_drive_preview;
static int challenge_ball_drive_preview,challenge_drift_drive_preview;
static int challenge_bound_drive_preview;
static TaxiChallengeBoundCourse live_bound_course;
static TaxiChallengeBound live_bound_state;
static uint8_t live_bound_option3;
static unsigned live_bound_warnings;
static int challenge_turn_drive_preview;
static unsigned turn_guidance_frame;
static const char *turn_input_replay_path;
static uint16_t turn_input_held[2400];
static uint8_t turn_input_masks[2400];static int16_t turn_input_analog[2400];
static unsigned turn_input_count,turn_input_index,turn_input_mask;
static TaxiChallengeTurnCourse live_turn_course;
static TaxiChallengeTurn live_turn_state;
static uint8_t live_turn_option3;
static unsigned live_turn_warnings;
static TaxiChallengeDriftCourse live_drift_course;
static TaxiChallengeDrift live_drift_state;
static uint8_t live_drift_option3;
static unsigned live_drift_warnings;
static const char *ball_input_replay_path;
static int drift_xbox_replay;static TaxiXboxSample drift_xbox_samples[1800];
static int jump_raw_input_replay;
static const char *drift_raw_replay_path;static uint16_t drift_raw_replay[1800];static unsigned drift_raw_count,drift_raw_index,drift_raw_held;
static uint8_t ball_input_masks[2700];static int16_t ball_input_analog[2700];
static unsigned ball_input_count,ball_input_index,ball_input_mask;static int16_t ball_input_steering;

static TaxiChallengeBallCourse live_ball_course;
static TaxiChallengeBall live_ball_state;
static TaxiPassengerState live_balls[20],live_balls_initial[20],previous_balls[20];
static unsigned live_ball_submissions,live_ball_ticks;
static GLuint ball_textures[8];
static TaxiPassengerState live_flag_actor,live_flag_initial;
static TaxiChallengeFlag live_flag_timer;
static uint8_t live_flag_indices[2048];
static GLuint flag_texture;
static int flag_scene_render_check;
static unsigned flag_scene_submissions;
static int flag_scene_mask;
static TaxiSpawnMarker live_challenge_spawn;
#include "traffic_assets.h"
#include "traffic_init.h"
#include "traffic_sprite.h"
#include "traffic_scene.h"
#include "traffic_drive.h"
#include "timing.h"
#include "taxi_sprite.h"
#include "taxi_half_plan.h"
#include "taxi_shadow.h"
#include "taxi_shadow_render.h"
#include "render_interpolation.h"
#include "camera_angle.h"
#include "vehicle_config.h"
#include "view_coordinates.h"
#include "passenger_scene.h"
#include "passenger_render.h"
#include "passenger_sprite_asset.h"
#include "destination_guidance.h"
#include "destination.h"
#include "palette_fade.h"
#include "font_asset.h"
#include "text_layout.h"
#include "normal_menu_text.h"
#include "control_layout.h"
#include "normal_menu_flow.h"
#include "options_menu_flow.h"
#include "options_settings_render.h"
#include "audio_menu_flow.h"
#include "results_grade.h"
#include "results_state.h"
#include "results_flow.h"
#include "game_over.h"
#include "name_entry.h"
#include "name_flow.h"
#include "menu_input.h"
#include "actions.h"
#include "profile_flow.h"
#include "save_block.h"
#include "save_file.h"
#include "profile_commit.h"
#include "challenge_profile_commit.h"
#include "driver_background.h"
#include "city_background.h"
#include "city_flow.h"
#include "driver_flow.h"

typedef struct { float x, y, z; } Vertex;
typedef struct { uint16_t count, flags, index[4]; float uv[8]; } Face;
typedef struct { uint32_t texture; unsigned char color[4]; uint32_t flags; } Material;
typedef struct { uint32_t width,height; unsigned char *pixels; GLuint id; int pause_bank; } Texture;
_Static_assert(sizeof(Vertex)==12 && sizeof(Face)==44 && sizeof(Material)==12,"CTM3 layout mismatch");
static Vertex *vertices;
static Face *faces;
static uint32_t vertex_count, face_count;
static uint32_t texture_count,material_count;
static Material *materials;
static Texture *textures;
static unsigned char scene_clear[4]={0,0,0,255};
static uint32_t sky_texture=UINT32_MAX;
static unsigned char sky_fill[4]={0,0,0,255};
static TaxiPaletteFade city_fade;
static int city_fade_cap=31,uploaded_city_cap=31;
static int uploaded_scene_alternate;
static void city_fade_tick(int request,int32_t frame_delta) {
    if(request)taxi_palette_fade_begin(&city_fade,0);
    if(city_fade.active_177b) {
        city_fade_cap=city_fade.progress_178a;
        taxi_palette_fade_step(&city_fade,frame_delta);
    }
}
static float focus[3];
static int textured=1, pan_drag;
static float yaw = -32, pitch = 62, distance = 3.1f;
static int dragging, last_x, last_y, wire_only;
static RECT saved_rect;
static int fullscreen;

/* Absolute presentation deadlines avoid accumulating Sleep overshoot every
   frame. Missed deadlines are skipped instead of producing catch-up bursts. */
/* Native fixed-resolution render target; no source CPU execution. */
static int native_4k_target,target_bound,target_blit_check;static GLuint target_fbo,target_color,target_depth;static unsigned target_draws,target_presents;
typedef void (APIENTRY *TaxiGenObjectsFn)(GLsizei,GLuint*);
typedef void (APIENTRY *TaxiDeleteObjectsFn)(GLsizei,const GLuint*);
typedef void (APIENTRY *TaxiBindObjectFn)(GLenum,GLuint);
typedef void (APIENTRY *TaxiAttachTextureFn)(GLenum,GLenum,GLenum,GLuint,GLint);
typedef void (APIENTRY *TaxiAttachRenderbufferFn)(GLenum,GLenum,GLenum,GLuint);
typedef void (APIENTRY *TaxiStorageFn)(GLenum,GLenum,GLsizei,GLsizei);
typedef GLenum (APIENTRY *TaxiFramebufferStatusFn)(GLenum);
typedef void (APIENTRY *TaxiBlitFn)(GLint,GLint,GLint,GLint,GLint,GLint,GLint,GLint,GLbitfield,GLenum);
static TaxiGenObjectsFn target_gen_fbo,target_gen_depth;static TaxiDeleteObjectsFn target_delete_fbo,target_delete_depth;
static TaxiBindObjectFn target_bind_fbo,target_bind_depth;static TaxiAttachTextureFn target_attach_texture;static TaxiAttachRenderbufferFn target_attach_depth;static TaxiStorageFn target_storage;static TaxiFramebufferStatusFn target_status;static TaxiBlitFn target_blit;
static void fail(const char *message);
static void target_symbol(void *out,size_t size,const char *name){FARPROC p=wglGetProcAddress(name);if((uintptr_t)p<=3||(uintptr_t)p==UINTPTR_MAX||size!=sizeof p)fail("Native4K framebuffer API unavailable.");memcpy(out,&p,size);}
#define TARGET_LOAD(member,name) target_symbol(&member,sizeof member,name)
static void target_create(void){
 TARGET_LOAD(target_gen_fbo,"glGenFramebuffers");TARGET_LOAD(target_delete_fbo,"glDeleteFramebuffers");TARGET_LOAD(target_bind_fbo,"glBindFramebuffer");TARGET_LOAD(target_attach_texture,"glFramebufferTexture2D");TARGET_LOAD(target_status,"glCheckFramebufferStatus");TARGET_LOAD(target_gen_depth,"glGenRenderbuffers");TARGET_LOAD(target_delete_depth,"glDeleteRenderbuffers");TARGET_LOAD(target_bind_depth,"glBindRenderbuffer");TARGET_LOAD(target_storage,"glRenderbufferStorage");TARGET_LOAD(target_attach_depth,"glFramebufferRenderbuffer");TARGET_LOAD(target_blit,"glBlitFramebuffer");
 target_gen_fbo(1,&target_fbo);target_bind_fbo(0x8d40,target_fbo);glGenTextures(1,&target_color);glBindTexture(GL_TEXTURE_2D,target_color);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,3840,2160,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);target_attach_texture(0x8d40,0x8ce0,GL_TEXTURE_2D,target_color,0);
 GLint width,height;glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);if(width!=3840||height!=2160)fail("Native4K color allocation dimensions failed.");
 target_gen_depth(1,&target_depth);target_bind_depth(0x8d41,target_depth);target_storage(0x8d41,0x81a6,3840,2160);target_attach_depth(0x8d40,0x8d00,0x8d41,target_depth);glDrawBuffer(0x8ce0);glReadBuffer(0x8ce0);if(target_status(0x8d40)!=0x8cd5||glGetError()!=GL_NO_ERROR)fail("Native4K framebuffer incomplete.");
 target_bind_depth(0x8d41,0);glBindTexture(GL_TEXTURE_2D,0);target_bind_fbo(0x8d40,0);glDrawBuffer(GL_BACK);glReadBuffer(GL_BACK);puts("NATIVE4K TARGET: GPU color3840x2160 RGBA8 depth24 framebuffer complete; independent window dimensions.");
}
static void target_begin(void){target_bind_fbo(0x8d40,target_fbo);glDrawBuffer(0x8ce0);glReadBuffer(0x8ce0);target_bound=1;target_draws++;}
static void target_present(int width,int height){
 target_bind_fbo(0x8ca8,target_fbo);glReadBuffer(0x8ce0);target_bind_fbo(0x8ca9,0);glDrawBuffer(GL_BACK);glPushAttrib(GL_ALL_ATTRIB_BITS);glDisable(GL_SCISSOR_TEST);glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
 if(width>0&&height>0){int w=width,h=(int)((int64_t)width*9/16);if(h>height){h=height;w=(int)((int64_t)height*16/9);}int x=(width-w)/2,y=(height-h)/2;target_blit(0,0,3840,2160,x,y,x+w,y+h,GL_COLOR_BUFFER_BIT,GL_LINEAR);target_presents++;}
 glPopAttrib();target_bind_fbo(0x8d40,0);glReadBuffer(GL_BACK);glDrawBuffer(GL_BACK);target_bound=0;if(glGetError()!=GL_NO_ERROR)fail("Native4K window blit failed.");
}
/* Diagnostic color fixture verifies GPU scaling and bars, not game pixels. */
static void target_verify_blit(HWND window){
 RECT original;GetWindowRect(window,&original);const int sizes[3][2]={{480,320},{320,400},{600,240}};
 for(unsigned test=0;test<3;test++){
  DWORD style=(DWORD)GetWindowLongPtrW(window,GWL_STYLE);RECT desired={0,0,sizes[test][0],sizes[test][1]};AdjustWindowRect(&desired,style,FALSE);if(!SetWindowPos(window,NULL,0,0,desired.right-desired.left,desired.bottom-desired.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE))fail("Blit fixture resize failed.");RECT client;GetClientRect(window,&client);int width=client.right,height=client.bottom;if(width<=0||height<=0)fail("Blit fixture client invalid.");
  target_begin();glPushAttrib(GL_ALL_ATTRIB_BITS);glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);glEnable(GL_SCISSOR_TEST);
  const unsigned char colors[4][3]={{255,0,0},{0,255,0},{0,0,255},{255,255,0}};
  for(int q=0;q<4;q++){glScissor((q%2)*1920,(q/2)*1080,1920,1080);glClearColor(colors[q][0]/255.0f,colors[q][1]/255.0f,colors[q][2]/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);}glPopAttrib();target_present(width,height);glFinish();
  size_t bytes=(size_t)width*height*4;unsigned char *pixels=malloc(bytes);if(!pixels)fail("Blit fixture read allocation failed.");glReadBuffer(GL_BACK);glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);if(glGetError()!=GL_NO_ERROR)fail("Blit fixture read failed.");
  int w=width,h=(int)((int64_t)width*9/16);if(h>height){h=height;w=(int)((int64_t)height*16/9);}int x=(width-w)/2,y=(height-h)/2;unsigned checked=0,bars=0;
  for(int py=0;py<height;py++)for(int px=0;px<width;px++){const unsigned char *v=pixels+((size_t)py*width+px)*4;if(px<x||px>=x+w||py<y||py>=y+h){if(v[0]||v[1]||v[2])fail("Blit letterbox pixel differs from black.");bars++;}else if(px>x+2&&px<x+w-3&&py>y+2&&py<y+h-3&&abs(px-(x+w/2))>2&&abs(py-(y+h/2))>2){int q=(px>=x+w/2)+2*(py>=y+h/2);if(memcmp(v,colors[q],3))fail("Blit quadrant pixel mismatch.");checked++;}}
  free(pixels);if(!checked||!bars)fail("Blit fixture coverage empty.");printf("PASS NATIVE4K BLIT: client%dx%d rectangle%d,%d,%d,%d interior%u blackbars%u.\n",width,height,x,y,w,h,checked,bars);
 }
 SetWindowPos(window,NULL,original.left,original.top,original.right-original.left,original.bottom-original.top,SWP_NOZORDER|SWP_NOACTIVATE);target_draws=target_presents=0;
}
static void target_destroy(void){target_bind_fbo(0x8d40,0);target_delete_fbo(1,&target_fbo);target_delete_depth(1,&target_depth);glDeleteTextures(1,&target_color);printf("NATIVE4K TARGET: rendered%u window blits%u.\n",target_draws,target_presents);}
typedef struct { LONGLONG epoch, frequency; uint64_t slot, skipped; } FramePacer;
static int window_quit_requested,loading_work_active;
static int presentation_timer_initialized,presentation_timer_active,precise_presentation_wait;
static HANDLE precise_presentation_timer;static unsigned precise_timer_waits,precise_spin_passes;
static void fail(const char *message);
static void presentation_timer_release(void) {
    if(precise_presentation_timer){CloseHandle(precise_presentation_timer);precise_presentation_timer=NULL;}
    if(presentation_timer_active)timeEndPeriod(1);
}
static int frame_due(FramePacer *pacer,LARGE_INTEGER now) {
    /* Request fine waits only while paced presentation is used. Balance the
       process-scoped request on both ordinary return and exit-based failure. */
    if(!presentation_timer_initialized) {
        presentation_timer_initialized=1;
        int cleanup_registered=!atexit(presentation_timer_release);
        if(cleanup_registered)presentation_timer_active=timeBeginPeriod(1)==TIMERR_NOERROR;
        if(precise_presentation_wait){
            if(!cleanup_registered)fail("Precise timer cleanup registration failed.");
            typedef HANDLE (WINAPI *TaxiCreateTimerFn)(LPSECURITY_ATTRIBUTES,LPCWSTR,DWORD,DWORD);
            FARPROC symbol=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"CreateWaitableTimerExW");TaxiCreateTimerFn create=NULL;if(symbol)memcpy(&create,&symbol,sizeof symbol);
            if(create)precise_presentation_timer=create(NULL,NULL,2,TIMER_MODIFY_STATE|SYNCHRONIZE);
            if(!precise_presentation_timer)fail("High resolution presentation timer unavailable.");
        }
    }
    LONGLONG deadline=pacer->epoch+(LONGLONG)(pacer->slot/60)*pacer->frequency+
        (LONGLONG)(pacer->slot%60)*pacer->frequency/60;
    LONGLONG remaining=deadline-now.QuadPart;
    if(remaining>0) {
        if(precise_presentation_wait){
            LONGLONG margin=pacer->frequency/5000; /* final200 microseconds only */
            if(remaining>margin){LARGE_INTEGER due;due.QuadPart=-(LONGLONG)((double)(remaining-margin)*10000000.0/pacer->frequency);if(due.QuadPart>=0)due.QuadPart=-1;
                if(!SetWaitableTimer(precise_presentation_timer,&due,0,NULL,NULL,FALSE)||WaitForSingleObject(precise_presentation_timer,1000)!=WAIT_OBJECT_0)fail("Precise presentation wait failed.");precise_timer_waits++;
            }else{YieldProcessor();precise_spin_passes++;}
        }else if(remaining>pacer->frequency/500) Sleep(1); else SwitchToThread();
        return 0;
    }
    uint64_t reached=(uint64_t)((now.QuadPart-pacer->epoch)*60/pacer->frequency);
    if(reached>pacer->slot) pacer->skipped+=reached-pacer->slot;
    pacer->slot=(reached>pacer->slot?reached:pacer->slot)+1;
    return 1;
}
static int compare_intervals(const void *a,const void *b) {
    double x=*(const double *)a,y=*(const double *)b;return (x>y)-(x<y);
}

/* Optional driving prototype: recovered simulation with a provisional PC
   follow camera and billboard sizing. Full game systems remain pending. */
static int challenge_jump_drive_preview,challenge_jump_retry_check,jump_effect_render_check;
static unsigned challenge_jump_retries;
static TaxiMenuInput challenge_jump_end_input;
static void challenge_jump_return_grid(void);
static void challenge_course_begin_live(void);
static int bound_retry_check,bound_failure_check,challenge_bound_menu_entry_check,turn_earned_capture_check,turn_retry_check,turn_failure_check,challenge_turn_menu_entry_check,challenge_jump_menu_entry_check,challenge_ball_menu_entry_check,challenge_drift_menu_entry_check,drift_retry_check,drift_failure_check,drift_earned_capture_check,ball_contact_flow_check,ball_earned_capture_check,ball_retry_check,ball_failure_check;
static int challenge_flag_menu_entry_check;
static TaxiChallengeFailure challenge_jump_failure;
static TaxiChallengeSuccess challenge_jump_success;
static TaxiFontAsset challenge_jump_end_font;
static GLuint challenge_jump_end_texture;
static int drive_mode,scene_index,simulation_error,script_steer,recovered_camera;
static void simulation_error_at(unsigned line){if(!simulation_error)fprintf(stderr,"Native gameplay error at scene_viewer.c:%u.\n",line);simulation_error=1;}
static int live_attract;
static TaxiGameStart live_game_start;
static unsigned live_game_start_events;
static unsigned live_game_start_sound_counts[3];
static uint32_t native_presentation_counter;
static int source_attract_ai,gear_input_sequence;static unsigned gear_input_tick;
static int attract_session_check;
static unsigned attract_probe_ticks,attract_probe_ground_misses,attract_probe_collisions;
static unsigned attract_probe_obstacle_records;
static TaxiAttractSession attract_session;
static TaxiPaletteFade attract_exit_fade;
static uint8_t attract_probe_settings[8];
static TaxiAiPathAsset attract_ai_path;
static int route_probe_active,route_probe_brake,native_multi_fare_probe,native_earned_quit_check,native_earned_audio_check;
static FILE *native_audio_capture;
static int reward_drive_probe,reward_drive_running,reward_image_captured;static unsigned reward_drive_tick;
static int route_probe_turn_brake,route_probe_speed_steering,route_probe_analog_command;
static int route_probe_speed_reference=180000,route_probe_steering_waypoint,route_probe_segment_distance,route_probe_avoid_distance,route_probe_avoid_waypoint,route_probe_avoid_end_margin,route_probe_bend_speed,route_probe_bend_distance=8192,route_probe_final_bend_speed,route_probe_avoid_late_straights,route_probe_avoid_extra_waypoint;
static int audio_preview,audio_live,audio_output_disabled;
static uint64_t audio_pcm_hash=UINT64_C(14695981039346656037);
static unsigned audio_smoke_frames,audio_presented;
static int startup_audio_smoke,startup_attract_audio_smoke;
static unsigned attract_audio_drive_ticks,attract_audio_return_ticks;
static int attract_audio_seen;
static int startup_game_audio_smoke,startup_pickup_audio_smoke,startup_tip_audio_smoke,startup_audio_cleanup_smoke;
static int startup_credits_audio_smoke,credits_live_active;
static TaxiCreditsAsset credits_live_asset;
static TaxiCreditsRuntime credits_live_runtime;
static unsigned credits_live_ticks,credits_live_draws,credits_music_frames_before,credits_music_nonzero_before;
static GLuint credits_live_texture;
static uint8_t credits_live_rgba[240*160*4];
static unsigned startup_quit_input_tick,startup_quit_fade_ticks;
static int startup_quit_complete,startup_quit_menu_started,startup_quit_credits_waiting,startup_quit_credits_entered;
static unsigned startup_quit_menu_frames;
static TaxiNormalExit startup_quit_parent;
static TaxiDriveWorld startup_quit_frozen_car;
static int32_t startup_quit_frozen_clock;
static unsigned startup_pickup_audio_hold,startup_pickup_audio_driving;
static int live_delivery_smoke;static const TaxiPassengerRouteAsset *live_delivery_route;static unsigned live_delivery_waypoint;
static int startup_tip_target=-1;
static double startup_tip_side;
static int startup_expiry_audio_smoke,expiry_audio_armed,natural_expiry_smoke;static int32_t natural_expiry_initial_clock;static uint32_t natural_expiry_initial_counter;
static int startup_return_audio_smoke;
static unsigned return_profile_input,return_music_frames;
static uint16_t return_profile_mask;
static unsigned music_faded_frames;
static unsigned startup_audio_tick,audio_test_stops;
static TaxiAudioAsset audio_bank;
static TaxiAudioHost *audio_host;
static TaxiAudioChannel audio_channels[4];
static int recovered_engine_audio,recovered_game_audio;
static unsigned recovered_game_frames,recovered_game_requests,recovered_game_rewards,recovered_game_random,recovered_game_near_miss,recovered_game_reward_gated;
static TaxiGameAudioEngine live_engine_audio;
static TaxiGameAudioControllerInput live_sound_input;
static TaxiGameAudioController live_sound_state;
static int32_t live_sound_auxiliary_timer;
static int16_t live_engine_factor=256;
static int32_t live_engine_cooldown;
static unsigned recovered_engine_frames;
static unsigned audio_requests,audio_rejected,audio_busy,audio_frames;
static int challenge_failed_channel=-1;
static unsigned audio_request_counts[203],audio_nonzero_frames,audio_nonzero_samples;
static uint8_t audio_global_volume;
static TaxiAudioSettingsState audio_settings_state;
static uint8_t *music_bytes,music_pitch[1536],music_divider;
static TaxiMusicAsset music_asset;
static TaxiMusicTracker music_tracker;
static TaxiMusicContext music_context={.main_mode=1};
static int music_loaded;
static int16_t music_fade_factor=256;
static int music_session_fade_gate(void);
static void fail(const char *message);
static unsigned music_frames,music_dispatches,music_nonzero_samples;
static unsigned music_starts[5];
static unsigned loading_audio_frames;
static uint32_t live_engine_dispatch(void *unused,TaxiGameAudioEngineRequest q){
    (void)unused;
    if(q.op==1){
        if(q.handle>=203)fail("Invalid recovered engine source.");
        int slot=taxi_audio_channel_start(audio_channels,audio_bank.sounds+q.handle,21024,
            (uint8_t)q.value,(uint8_t)((uint32_t)q.value>>8));
        ++audio_requests;++audio_request_counts[q.handle];
        return slot>=0?(uint32_t)slot+1:0;
    }
    if(q.handle<1||q.handle>4)fail("Recovered engine received invalid channel handle.");
    TaxiAudioChannel *channel=audio_channels+q.handle-1;
    if(q.op==2)taxi_audio_channel_volume(channel,(uint32_t)q.value);
    else if(q.op==3){if(taxi_audio_channel_pitch(channel,(uint32_t)q.value,21024))fail("Recovered engine pitch failed.");}
    else if(q.op==4)taxi_audio_channel_stop(channel,21024);
    else fail("Unexpected recovered engine request.");
    return 0;
}
static void music_test_start(unsigned choice){
    static const uint8_t orders[5]={63,88,115,34,5};
    if(!audio_live||choice>=5)return;
    /* Source27730 precedes module load/start both on test and menu exit. */
    music_tracker.state.playing=0;
    if(!audio_settings_state.music_enabled)return;
    if(!taxi_music_tracker_start(&music_tracker,&music_asset,orders[choice]))fail("Native music start failed.");
    music_tracker.state.state[0]=1;music_loaded=1;
    music_starts[choice]++;
}
static int audio_request_channel(unsigned index){
    if(!audio_live||index>=203)return -1;
    ++audio_requests;
    ++audio_request_counts[index];
    int channel=taxi_audio_channel_start(audio_channels,audio_bank.sounds+index,21024,0,0);
    if(channel<0)++audio_rejected;return channel;
}
static void audio_request(unsigned index){(void)audio_request_channel(index);}
static int challenge_failed_voice_busy(void){
    return audio_live&&challenge_failed_channel>=0&&challenge_failed_channel<4&&
        taxi_audio_channel_busy(audio_channels+challenge_failed_channel,audio_bank.sounds+70);
}
/* Source26488 bootstrap ends with processing7560/effectsactive75F7=1.
   Retain scratch and both output banks across source audio frames. */
static TaxiAudioFrame audio_source_frame={.rate=21024,.processing=1,.effects_active=1};
static int16_t audio_source_mix[352];
static uint8_t audio_source_output[2][352];
static void audio_frame(void){
    if(!audio_live)return;
    TaxiAudioOutputBuffer buffers[2]={{audio_source_output[0],352},{audio_source_output[1],352}};
    audio_source_frame.divider=music_divider;
    audio_source_frame.music_enabled=audio_settings_state.music_enabled;
    audio_source_frame.effects_enabled=audio_settings_state.effects_enabled;
    audio_source_frame.music_volume=audio_settings_state.music_volume;
    audio_source_frame.effects_volume=audio_global_volume;
    audio_source_frame.ended=(uint8_t)music_session_fade_gate();
    audio_source_frame.outer=0;audio_source_frame.fade=music_fade_factor;
    unsigned written_bank=audio_source_frame.bank;int dispatched;
    if(taxi_audio_frame_step(&audio_source_frame,&music_tracker,&music_asset,music_pitch,
        &music_context,audio_channels,&audio_bank,audio_source_mix,352,buffers,&dispatched))
        fail("Recovered native audio frame failed.");
    music_divider=audio_source_frame.divider;
    if(dispatched)music_dispatches++;
    if(audio_source_frame.music_mixed){
        if(music_session_fade_gate()&&music_fade_factor<256)music_faded_frames++;
        music_frames++;music_nonzero_samples+=audio_source_frame.music_nonzero;
    }
    uint8_t *pcm=audio_source_output[written_bank];
    for(unsigned i=0;i<352;i++){audio_pcm_hash^=pcm[i];audio_pcm_hash*=UINT64_C(1099511628211);}
    int queued=0;
    if(native_audio_capture){
        if(fwrite(pcm,1,352,native_audio_capture)!=352)fail("Native audio capture write failed.");
    }else if(audio_host)queued=taxi_audio_host_write_wait(audio_host,pcm,352,1000);
    if(queued<0)fail("Native audio output write failed.");
    if(queued){++audio_busy;fail("Native audio output queue timed out; source frame not discarded.");}
    else {
        unsigned nonzero=0;
        for(unsigned i=0;i<352;i++)if(pcm[i])++nonzero;
        ++audio_frames;audio_nonzero_samples+=nonzero;
        if(nonzero)++audio_nonzero_frames;
    }
}

static TaxiMovementResult route_probe_movement;
static FILE *route_probe_obstacles,*live_delivery_query_trace;static unsigned live_delivery_query_records;
static unsigned route_probe_obstacle_tick,route_probe_obstacle_waypoint;
static int route_obstacle_query(void *context,TaxiPoint point,TaxiObstacleHit *hit) {
    int kind=taxi_world_asset_obstacle(context,point,hit);
    if(live_delivery_query_trace&&kind>=0){fprintf(live_delivery_query_trace,"%u,%u,%ld,%ld,%ld,%d,%ld,%u\n",route_probe_obstacle_tick,route_probe_obstacle_waypoint,(long)point.x,(long)point.y,(long)point.z,kind,(long)hit->height,kind==2?hit->polygon:65535);live_delivery_query_records++;}
    if(attract_session_check&&attract_probe_ticks>=7500&&attract_probe_obstacle_records<12&&
       (kind==2||(kind==1&&hit->height!=INT32_MIN&&(int64_t)hit->height-point.y>=655))){
        attract_probe_obstacle_records++;
        printf("ATTRACT OBSTACLE: tick%u query%d,%d,%d kind%d height%d polygon%u corners%u",
            attract_probe_ticks+1,point.x,point.y,point.z,kind,hit->height,hit->polygon,hit->edge_count);
        for(unsigned i=0;i<hit->edge_count;i++)printf(" vertex%d,%d,%d",hit->vertices[i].x,hit->vertices[i].y,hit->vertices[i].z);
        putchar('\n');
    }
    if(route_probe_obstacles&&kind==2) {
        fprintf(route_probe_obstacles,"%u,%u,%ld,%ld,%ld,%d,%u,%u",
            route_probe_obstacle_tick,route_probe_obstacle_waypoint,
            (long)point.x,(long)point.y,(long)point.z,kind,hit->polygon,hit->edge_count);
        for(unsigned i=0;i<4;i++)fprintf(route_probe_obstacles,",%ld,%ld,%ld",
            (long)hit->vertices[i].x,(long)hit->vertices[i].y,(long)hit->vertices[i].z);
        fputc('\n',route_probe_obstacles);
    }
    return kind;
}

static TaxiRuntimeAsset runtime;
static TaxiWorldAsset world;
static TaxiDriveWorld car;static unsigned host_reset_revision;
static TaxiInput driving_buttons;
static TaxiDriveWorldContext drive_context;
static TaxiSimulationClock drive_clock;
static float raw_center[3],raw_scale;
static GLuint taxi_textures[64];
static unsigned taxi_diagnostic_half_mask=3;
static unsigned native_game_driver;
static int taxi_cache_check;
static int passenger_diagnostic;
static uint32_t default_ranking_scores[100];
static uint8_t native_settings[512],session_settings_snapshot[512],native_score_rows[800];
static int native_settings_loaded;
/* Native owner of original02001538. Initial0 remains a diagnostic startup
   choice until the original startup profile-selection screen is recovered. */
static uint8_t native_current_profile;
/* Source02001eaa selects recorded/demo input; settings start02001eb0.
   This runtime gate must never alias persistent profile initials+10. */
static uint8_t native_recorded_input_gate;
static const char *native_save_path;
static int native_save_write_status;
static void fail(const char *message);
static void load_native_settings(const char *path) {
    FILE *f=fopen(path,"rb");uint8_t bytes[808];
    if(!f||fread(bytes,1,sizeof bytes,f)!=sizeof bytes||fgetc(f)!=EOF||ferror(f)||memcmp(bytes,"CTS1",4)||bytes[4]!=100||bytes[5]||bytes[6]||bytes[7])fail("Raw original score defaults invalid.");fclose(f);
    taxi_save_block_defaults(native_settings,native_score_rows,bytes+8);
    if(memcmp(native_score_rows,bytes+8,800)||!taxi_save_block_checksum_valid(native_settings))fail("Native raw settings initialization mismatch.");
    if(native_save_path) {
        int status=taxi_save_file_load(native_save_path,native_settings,native_score_rows);
        printf("PC SAVE: load status%d (0 loaded,1 missing,-1 rejected; defaults retained on failure).\n",status);
    }
    for(unsigned i=0;i<100;i++) {
        const uint8_t *p=native_score_rows+i*8+4;
        default_ranking_scores[i]=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
    }
    native_settings_loaded=1;
    /* Normal boot05418 initializes volumes from settings, then05424..34
       enables both domains for its zero1ea5 guard. Alternate boot pending. */
    audio_settings_state.music_volume=native_settings[1];
    audio_settings_state.effects_volume=native_settings[2];
    taxi_audio_settings_start_gate(&audio_settings_state,0);
    audio_global_volume=audio_settings_state.effects_volume;
}
static TaxiPassengerAsset passenger_asset;
static TaxiPassengerScene passenger_scene;
static int music_session_fade_gate(void){
    return taxi_music_fade_gate(passenger_scene.session.ended_1697,passenger_scene.session.outer_state_1ea8);
}
static uint32_t music_shared_random(void *context){
    return (uint32_t)taxi_random_range(context,0,65535);
}
static TaxiTrafficAssets traffic_assets;
static TaxiTrafficActor traffic_actors[50];
static TaxiTrafficActor previous_traffic_actors[50];
static TaxiPassengerState previous_customers[128];
static uint32_t traffic_scales[3];
static GLuint traffic_textures[48];
static TaxiTrafficSubmission traffic_submissions[50];
static unsigned traffic_submission_count;
static int traffic_preview;
static TaxiTrafficGlobals traffic_globals;
static uint8_t traffic_timer;
static uint32_t traffic_counter;
static unsigned traffic_spawned,traffic_visible_frames;
static int passenger_scene_initialized;
static TaxiGuidanceRoute guidance_route;
static int32_t guidance_angle;
static GLuint passenger_textures[144],passenger_marker_textures[10];
static GLuint shadow_textures[64];
static GLuint guidance_textures[160];
static GLuint clock_textures[52];
static GLuint expiry_textures[15];
static GLuint cash_owned_slot_textures[19];
static GLuint cash_label_textures[4];
static GLuint font_textures[896];
static int pause_overlay_check,pause_freeze_check,pause_scene_rgba_check;
static int pause_host_frame_check,pause_host_frames;
static unsigned pause_overlay_dialog;
static TaxiPauseRuntime native_pause;
static TaxiPauseFrame native_pause_frame;
static GLuint pause_title_textures[9];
static unsigned pause_overlay_draws;
static uint16_t font_metrics[224][9];
static unsigned char font_kerning[224*224];
static TaxiCashGlyphs cash_glyphs;
static uint16_t native_tip_frame;static uint8_t native_tip_indices[12288];static GLuint native_tip_texture;
static FILE *native_reward_trace,*native_reward_update_trace,*native_tip_collection_trace;
static FILE *native_held_gear_trace;
static int native_tip_bitmap_gpu_check;
static unsigned native_gate_holds,native_gate_boarding_holds,native_gate_delivery_holds;
static TaxiPoint previous_position;
static int32_t previous_vehicle_heading,previous_motion_heading;
static uint16_t previous_body_roll;
static TaxiBodyPoints native_body_points;
static TaxiActorEffects native_actor_effects;
static int native_actor_effects_initialized;
static uint8_t native_material_prefix[28];
static int taxi_owned_graphics;
static TaxiGraphicsPackage taxi_owned_package;
/* Persistent original graphics owners shared by rendering and future frame service. */
 static TaxiTitleAssets live_taxi_assets;static TaxiPaletteRegistration live_taxi_registry;static TaxiPaletteBanks live_taxi_banks;static TaxiFontSetup live_taxi_fonts;
 static TaxiStartupObjQueue live_taxi_queue;static TaxiStartupObjTransport live_taxi_transport;static uint16_t live_taxi_base[128][4];static TaxiObjMatrixSlot live_taxi_matrices[32];
 static TaxiOrdinaryGraphicsOwners live_taxi_owner;static uint32_t live_taxi_cache=UINT32_MAX;static unsigned live_taxi_cached_driver=UINT32_MAX;

static GLuint taxi_owned_texture;
static int taxi_owned_lifecycle_check;
static TaxiObjField taxi_owned_fields[2];
static int16_t taxi_owned_matrices[2][4];
static uint8_t taxi_owned_visible[2];
static TaxiObjField effect_owned_fields[9];
static int16_t effect_owned_matrices[9][4];
static uint8_t effect_owned_visible[9];
static uint8_t effect_owned_pixels[9][8192];
static TaxiObjRequest shared_owned_requests[11];
static unsigned shared_owned_count;
static TaxiEffectSprite native_effect_requests[9];
static TaxiEffectAsset native_effect_asset;
static int native_effect_request_count;
static const char *native_effect_boundary_capture_path;
static TaxiCameraView native_effect_capture_camera;
static TaxiEffectRecord native_effect_capture_records[9];
static uint64_t native_effect_generations[9],native_effect_capture_generations[9];
static int32_t native_effect_capture_phase;
static int native_effect_capture_disabled;
static uint32_t native_effect_capture_random_before,native_effect_capture_random_after;
static unsigned native_effect_emissions,native_effect_visible_frames,native_effect_max_active;
static FILE *native_traffic_actor_trace;
static FILE *native_traffic_spawn_trace;
static FILE *native_traffic_heading_trace;
static FILE *native_traffic_frame_trace;
static FILE *native_random_trace;
static FILE *native_passenger_idle_trace;
static FILE *native_passenger_pickup_trace;
static FILE *native_impact_voice_trace;
static FILE *native_passenger_fare_trace;
static FILE *native_passenger_money_trace;
static FILE *native_cash_glyph_trace;
static FILE *native_destination_selection_trace;
static FILE *native_fare_settlement_trace;
static FILE *native_traffic_impact_trace;
static FILE *native_traffic_response_trace;
static FILE *native_traffic_effect_trace;
static FILE *native_traffic_contact_trace;
static FILE *native_camera_state_trace;
static int camera_time_owner_trace;
static void fail(const char *message);
static FILE *native_orientation_state_trace;
static FILE *native_motion_state_trace;
static FILE *native_response_state_trace;
static void observe_response(const TaxiVehicleMath *a,const TaxiVehicleMath *b){if(fprintf(native_response_state_trace,"%ld %ld %u %ld %ld\n",(long)a->state_540,(long)a->state_744,a->flags_4ac,(long)b->state_540,(long)b->state_744)<0)fail("Response write failed.");}
static FILE *live_delivery_movement_trace;
static void observe_delivery_movement(TaxiPoint start,TaxiPoint displacement,int preserve_y,TaxiObstacleQuery query,void *context,const TaxiMovementResult *result){
 if(query!=route_obstacle_query||context!=&world)return;
 if(fprintf(live_delivery_movement_trace,"%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u,%d\n",route_probe_obstacle_tick,route_probe_obstacle_waypoint,start.x,start.y,start.z,displacement.x,displacement.y,displacement.z,preserve_y,result->position.x,result->position.y,result->position.z,result->normal.x,result->normal.y,result->normal.z,result->queries,result->collided)<0)fail("Movement diagnostic write failed.");
}
static FILE *native_traction_state_trace;
static void observe_traction(const TaxiTraction *a,const TaxiTractionContext *c,const TaxiTraction *b,const TaxiTractionContext *d,const TaxiOrientationEffects *e){
 int32_t fields[]={a->force_53c,a->force_540,a->capacity_570,a->capacity_574,a->speed_544,a->phase_e6a,a->phase_e6b,a->flags_28,a->state_564,c->settings,c->flag_1696,c->flag_1697,c->flag_25d4,c->flag_262c,c->state_2630,b->state_564,b->speed_544,d->flag_25d4,d->flag_262c,e->score_requested,e->score_a,e->score_b,e->score_c};
 for(unsigned i=0;i<sizeof fields/sizeof *fields;i++)fprintf(native_traction_state_trace,"%ld%s",(long)fields[i],i+1==sizeof fields/sizeof *fields?"\n":" ");
 if(ferror(native_traction_state_trace))fail("Traction diagnostic write failed.");
}

static void observe_motion(const TaxiMotionState *a,const TaxiOrientationContext *c,const TaxiCommand *command,const TaxiMotionState *b){
 const TaxiOrientation *v=&a->orientation,*w=&b->orientation;
 int32_t fields[]={v->speed_544,a->steering.angle_578,v->angle_18,v->heading_1c,v->angle_1a,c->delta,v->drift_69c,v->timer_2a,v->state_564,v->flags_28,v->heading_20,v->flags_358,v->phase_e6a,a->steering.phase_e6b,a->steering.state_7b8,a->steering.limit_7b0,a->scale_7c8,a->scale_e84,a->dimension_594,v->force_540,v->position.x,v->position.y,v->position.z,c->settings,c->submode};
 for(unsigned i=0;i<sizeof fields/sizeof *fields;i++)fprintf(native_motion_state_trace,"%ld ",(long)fields[i]);
 for(unsigned i=0;i<16;i++)fprintf(native_motion_state_trace,"%u ",command->bytes[i]);
 int32_t outputs[]={b->response_57c,w->heading_1c,w->heading_20,b->displacement.x,b->displacement.y,b->displacement.z,w->force_540,w->drift_69c,w->timer_2a,w->speed_544,w->position.x,w->position.z,w->flags_28};
 for(unsigned i=0;i<sizeof outputs/sizeof *outputs;i++)fprintf(native_motion_state_trace,"%ld%s",(long)outputs[i],i+1==sizeof outputs/sizeof *outputs?"\n":" ");
 if(ferror(native_motion_state_trace))fail("Motion diagnostic write failed.");
}

static void observe_orientation(const TaxiOrientation *a,const TaxiOrientationContext *c,const TaxiOrientation *b,const TaxiOrientationEffects *e){
 fprintf(native_orientation_state_trace,"%u %ld %u %ld %ld %ld %d %ld %ld %u %ld %u %d %u %u %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %d\n",
 a->angle_18,(long)a->heading_1c,a->angle_1a,(long)a->heading_20,(long)a->drift_69c,(long)a->force_540,a->timer_2a,(long)a->state_564,(long)a->speed_544,a->flags_28,(long)c->delta,a->flags_358,a->phase_e6a,c->settings,c->submode,(long)a->position.x,(long)a->position.y,(long)a->position.z,
 (long)b->heading_1c,(long)b->heading_20,(long)b->drift_69c,b->timer_2a,(long)b->speed_544,(long)b->position.x,(long)b->position.z,e->score_requested);
 if(ferror(native_orientation_state_trace))fail("Orientation diagnostic write failed.");
}

static FILE *native_effect_state_trace;static unsigned native_effect_state_tick;
static FILE *native_effect_trace;static unsigned native_effect_trace_tick;
static int32_t native_effect_random(void *state,int32_t minimum,int32_t maximum){
    return taxi_random_range(state,minimum,maximum);
}
static int32_t camera_heading,previous_camera_heading;
static int32_t camera_yaw_offset,previous_camera_yaw_offset,camera_yaw_target;
static int32_t camera_pitch,previous_camera_pitch;
static int prototype_camera_framing;
static const char *taxi_source_capture_path;
static int taxi_source_capture_written;
static int taxi_live_pixels_check;
static int source_frame_cpu_requested,source_frame_cpu_valid,source_scene_gpu_requested;
static uint8_t source_scene_gpu_pixels[153600];static int source_scene_gpu_ready;
static uint8_t source_frame_cpu_indexed[38400];
static uint16_t source_frame_cpu_palette[256];
static int camera_distinct_headings;
static int live_flag_warning_palette;
static int flag_failure_check;
static int flag_retry_check;
static int flag_contact_flow_check,flag_earned_capture_check;
static const char *flag_input_replay_path;
static uint8_t flag_input_replay[1200],flag_input_mask;
static unsigned flag_input_count,flag_input_index;
/* PC presentation pullback, provisional until original running-view qualification. */
static TaxiCameraOffsets presentation_offsets(TaxiCameraOffsets value){
    if(prototype_camera_framing)value.distance_770+=2048*4096;
    return value;
}
static TaxiCameraOffsets camera_offsets;
static uint16_t live_camera_mode_64c,previous_camera_mode_64c;
static int camera_owner_trace;
static int camera_xbox_lookback_check;
static int camera_lookback_check;static unsigned camera_lookback_tick;
static TaxiCameraOffsets previous_camera_offsets;
static int32_t camera_base;
static TaxiVehicleStartPacked native_vehicle_start_owner;
static unsigned native_vehicle_start_initialized;
static float camera_orbit,camera_pitch_orbit;
static int capture_orbit_set;
static float capture_orbit;
static void reset_camera_view(void) {
    camera_orbit=0;camera_pitch_orbit=recovered_camera?0:20;
    distance=(recovered_camera?3072:6000)*raw_scale;
}
static void fail(const char *message);
/* Source-backed OBJ palettes selected by expiry count32; keep originals
   independent of their GL copies to avoid compounded fades. */
static Texture sprite_sources[1700];
static unsigned sprite_source_count;
static GLuint native_effect_background;
static int native_effect_background_width,native_effect_background_height,native_effect_background_valid;
static unsigned native_effect_background_captures,native_effect_background_checks;
static int native_effect_background_coverage_valid;
static unsigned native_effect_background_coverage_checks;
static void release_drive_graphics(void) {
    if(flag_texture){glDeleteTextures(1,&flag_texture);flag_texture=0;}
    glDeleteTextures(8,ball_textures);memset(ball_textures,0,sizeof ball_textures);
    if(native_effect_background)glDeleteTextures(1,&native_effect_background);
    native_effect_background=0;native_effect_background_width=native_effect_background_height=0;
    native_effect_background_valid=0;native_effect_background_coverage_valid=0;
    for(unsigned i=0;i<sprite_source_count;i++){
        glDeleteTextures(1,&sprite_sources[i].id);free(sprite_sources[i].pixels);
    }
    memset(sprite_sources,0,sizeof sprite_sources);sprite_source_count=0;taxi_owned_texture=0;
}
static Texture *upload_source_sprite(unsigned width,unsigned height,const unsigned char *pixels) {
    GLint id;glGetIntegerv(GL_TEXTURE_BINDING_2D,&id);
    Texture *t=NULL;
    for(unsigned i=0;i<sprite_source_count;i++)if(sprite_sources[i].id==(GLuint)id){t=sprite_sources+i;break;}
    size_t bytes=(size_t)width*height*4;
    if(!t){
        if(sprite_source_count>=1700)fail("Sprite source capacity exceeded.");
        t=sprite_sources+sprite_source_count++;
    }
    if(!t->pixels||t->width!=width||t->height!=height){
        unsigned char *replacement=malloc(bytes);if(!replacement)fail("Sprite source allocation failed.");
        free(t->pixels);t->pixels=replacement;
    }
    t->id=(GLuint)id;t->width=width;t->height=height;t->pause_bank=-1;
    memcpy(t->pixels,pixels,bytes);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,(GLsizei)width,(GLsizei)height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    return t;
}
static void upload_source_sprite_bank(unsigned width,unsigned height,const unsigned char *pixels,int bank){
    upload_source_sprite(width,height,pixels)->pause_bank=bank;
}
static int endgame_diagnostic,game_over_active,name_live_active;
static int profile_live_active;
static void profile_live_begin(uint8_t interval,uint8_t reload);
static void profile_live_reset(void);
static void profile_tick(uint16_t mask);
static void challenge_music_begin(void);
static int profile_challenge_owner,challenge_jump_discard_check,challenge_jump_new_profile_check,challenge_jump_unranked_check,challenge_jump_cross_profile_check,challenge_result_audio_check,challenge_entry_fade_check,challenge_camera_trace,challenge_analog_trace;
static int16_t challenge_trace_stick;
static uint16_t challenge_profile_check_mask(void);
static void result_live_reset(void);
static int result_live_tick(TaxiXboxSample pad,int32_t frame_delta);
static TaxiIndexedAsset native_indexed_scene;
static TaxiMaterialBlink native_materials={.destination=-1};
static uint8_t native_scene_lookup[2560],native_panel_pixels[4096],native_scene_border;
static unsigned native_panel_generation;
static uint16_t native_scene_palette[256];
static TaxiSceneRegistration native_scene_registration;
static TaxiRenderViewport native_scene_viewport;
static TaxiRenderFrameInit native_scene_frame;
static TaxiGraphicsBootstrapTail native_graphics_bootstrap;
static TaxiRestartRenderContext native_bootstrap_render;
static TaxiPaletteRegistration native_scene_materials;
static TaxiSessionInit native_session_init;
static TaxiPaletteTransfer native_scene_palette_request;
/* Original indexed buffer ownership for recovered startup helpers. Native4K
   presentation remains the direct GPU renderer; these are not scanout images. */
static uint8_t native_bootstrap_first[38400],native_bootstrap_second[38400];
static int native_scene_frame_ready;
static int native_scene_viewport_ready;
static TaxiTrafficProjection native_scene_projection(void){
    if(!native_scene_viewport_ready)fail("Native scene projection used before viewport initialization.");
    if(!challenge_jump_drive_preview&&!native_scene_frame_ready)fail("Native scene projection used before frame initialization.");
    return (TaxiTrafficProjection){.center_x=native_scene_viewport.center_x,.center_y=native_scene_viewport.center_y,
        .focal=native_scene_viewport.focal,
        .near_plane=challenge_jump_drive_preview?native_scene_viewport.near_plane:native_scene_frame.corners[0][2],
        .far_plane=challenge_jump_drive_preview?native_scene_viewport.far_plane:native_scene_frame.corners[4][2]};
}

static uint16_t native_pause_scene_palette[256];
static const uint16_t *pause_scene_palette(void){
    return native_pause.alternate&&!live_attract&&!challenge_jump_drive_preview?
        native_pause_scene_palette:native_scene_palette;
}

static GLuint native_panel_texture,native_flat_pattern_texture;
static unsigned native_flat_pattern_draws,native_attribute_pattern_draws,native_axis_draws;
static int native_axis_snapped_check,native_axis_source_check,native_taxi_facing_check;
static TaxiWorldFramePacket native_flat_source_packet;static int native_flat_source_valid;
static const uint8_t *native_shade_lookup_check;
static int native_pickup_only_check,native_material_reset_check;
static int native_cache_preview_pending;
static uint32_t native_cache_preview_generation;
static FILE *native_material_phase_trace;
static void native_world_cache_bind(TaxiWorldFrame *frame,int advance){
    if(scene_index<0||scene_index>=2)return;
    TaxiGraphicsStorage *storage=&native_graphics_bootstrap.storage;
    size_t normal_count=sizeof storage->pool_1b6ec/sizeof *storage->pool_1b6ec;
    size_t special_count=sizeof storage->pool_276ec/sizeof *storage->pool_276ec;
    if(!frame->world||frame->world->np>normal_count||frame->world->special_count>special_count)
        fail("Original graphics polygon cache capacity exceeded.");
    if(advance)storage->state_6844+=1u; /* Original2D18 geometry preparation. */
    frame->normal_stamps=storage->pool_1b6ec;frame->normal_stamp_count=normal_count;
    frame->special_stamps=storage->pool_276ec;frame->special_stamp_count=special_count;
    frame->generation=storage->state_6844;
    memcpy(&frame->phase,&storage->state_6904,sizeof frame->phase);
}

static int native_destination_selected(void *context,const TaxiPassengerScene *scene,int32_t destination) {
    (void)context;
    if(destination==-1)return taxi_material_blink_select(&native_materials,-1,NULL,0,0,NULL,NULL);
    if((unsigned)destination>=passenger_asset.destination_count)return -1;
    size_t capacity=(size_t)world.np+world.special_count+1;
    TaxiWorldFramePacket *packets=calloc(capacity,sizeof *packets);
    TaxiRenderPacket *queue_storage=calloc(capacity,sizeof *queue_storage);
    uint8_t *normal=calloc(capacity,1),*special=calloc(capacity,1),*scratch=malloc(38400);
    if(!packets||!queue_storage||!normal||!special||!scratch) {
        free(packets);free(queue_storage);free(normal);free(special);free(scratch);return -1;
    }
    TaxiRenderQueue queue;taxi_render_queue_init(&queue,queue_storage,capacity);
    TaxiWorldFrame engine={.world=&world,.trig=&runtime.trig,
        .bounds_corners=scene_index<2?native_scene_frame.corners:NULL,
        .projection=native_scene_projection(),.left=0,.top=0,.right=240,.bottom=160,
        .texture_count=native_indexed_scene.count,.textures=&native_indexed_scene,.material_cache=native_materials.cache,
        .material_cache_size=512,.shade_depth=0,.queue=&queue,.packets=packets,.capacity=capacity,
        .normal_seen=normal,.normal_seen_size=capacity,.special_seen=special,.special_seen_size=capacity};
    native_world_cache_bind(&engine,0);
    TaxiDestinationPanelContext panel={native_panel_pixels,scratch,&engine,&native_indexed_scene,
        0,native_scene_lookup,native_scene_border};
    int rc=taxi_material_blink_select(&native_materials,destination,
        passenger_asset.destinations[destination],48,scene->session.settings.bytes[0],
        taxi_destination_panel_selection,&panel);
    if(!rc){native_panel_generation++;
        if(native_pickup_only_check&&scene_index<2){
            if(native_graphics_bootstrap.storage.state_6844!=engine.generation)
                fail("Preview changed parent graphics generation.");
            unsigned normal=0,special=0;uint32_t stamp=engine.generation+1u;
            for(size_t i=0;i<world.np;i++)normal+=engine.normal_stamps[i]==stamp;
            for(size_t i=0;i<world.special_count;i++)special+=engine.special_stamps[i]==stamp;
            printf("CACHE PREVIEW: parent%u retained; local%u normal%u special%u\n",engine.generation,stamp,normal,special);
            if((scene->session.settings.bytes[0]&224)!=128){native_cache_preview_pending=1;native_cache_preview_generation=stamp;}
        }
    }
    free(packets);free(queue_storage);free(normal);free(special);free(scratch);return rc;
}
static void reset_drive_observed(TaxiPassengerInitObserver observer,void *context);
static void reset_drive(void) {reset_drive_observed(NULL,NULL);}
static void reset_drive_observed(TaxiPassengerInitObserver observer,void *context) {
    ++host_reset_revision;
    TaxiFareSession prior_session=passenger_scene.session;
    if(!passenger_scene_initialized)taxi_fare_session_init(&prior_session);
    challenge_failed_channel=-1;
    memset(&challenge_jump_end_input,0,sizeof challenge_jump_end_input);
    memset(&challenge_jump_success,0,sizeof challenge_jump_success);
    memset(&challenge_jump_failure,0,sizeof challenge_jump_failure);
    memset(&live_game_start,0,sizeof live_game_start);live_game_start_events=0;memset(live_game_start_sound_counts,0,sizeof live_game_start_sound_counts);
    memset(&driving_buttons,0,sizeof driving_buttons);
    music_fade_factor=256; /* Source11490/114d0 gameplay initialization. */
    result_live_reset();
    traffic_submission_count=0;traffic_spawned=traffic_visible_frames=0;
    if(challenge_jump_drive_preview)native_settings[0]=(uint8_t)((native_settings[0]&31)|128);
    if(native_settings_loaded||challenge_jump_drive_preview)memcpy(session_settings_snapshot,native_settings,512);
    memset(&city_fade,0,sizeof city_fade);city_fade_cap=31;
    if(challenge_jump_drive_preview){city_fade.interval_1771=city_fade.reload_1772=1;taxi_palette_fade_begin(&city_fade,1);city_fade_cap=0;}
    TaxiVehicleConfig camera_config;
    /* Original14c96 selects084a5794 for attract; normal uses084a554c. */
    unsigned template_index=taxi_runtime_template_bank((uint8_t)source_attract_ai);
    if(taxi_vehicle_config_decode(&camera_config,runtime.templates[template_index],584)) fail("Cannot decode original camera template.");
    camera_base=challenge_jump_drive_preview?camera_config.camera_base_680:taxi_vehicle_start_camera_base();
    if(!challenge_jump_drive_preview)printf("NATIVECAMERA original14D44 base680=%ld (template%ld).\n",(long)camera_base,(long)camera_config.camera_base_680);
    memset(&camera_offsets,0,sizeof(camera_offsets));
    if(taxi_material_blink_select(&native_materials,-1,NULL,0,0,NULL,NULL))fail("Material reset failed.");
    guidance_angle=-1;turn_guidance_frame=0;turn_input_index=turn_input_mask=0;
    taxi_cash_glyphs_init(&cash_glyphs);native_tip_frame=0;
    /* Original normal startup ae50 and alternate abc4 initialize vehicle before passengers. */
    uint8_t attract_settings=(uint8_t)(((native_settings_loaded?native_settings[0]:0)&31)|0x60);
    if(challenge_jump_drive_preview){
        if(taxi_simulation_start_challenge(&car,&drive_context,runtime.templates[0],584,&live_challenge_spawn,(uint8_t)((native_settings[0]&31)|128),(uint8_t)(challenge_bound_drive_preview?5:challenge_turn_drive_preview?4:challenge_drift_drive_preview?3:challenge_ball_drive_preview?2:challenge_flag_drive_preview),taxi_world_asset_ground,&world)!=1)fail("Cannot initialize native Jump vehicle.");
        taxi_game_start_begin_challenge(&live_game_start,240);
        if(challenge_bound_drive_preview){if(taxi_challenge_bound_begin(&live_bound_state,&live_bound_option3))fail("Bound scalar init failed.");live_bound_warnings=0;live_flag_warning_palette=0;}
        if(challenge_turn_drive_preview){if(taxi_challenge_turn_begin(&live_turn_state,&live_turn_option3))fail("Turn scalar init failed.");live_turn_warnings=0;live_flag_warning_palette=0;}
        if(challenge_drift_drive_preview){if(taxi_challenge_drift_begin(&live_drift_state,&drive_context.driver.motion.orientation,&live_drift_option3))fail("Drift scalar init failed.");live_drift_warnings=0;live_flag_warning_palette=0;}
        if(challenge_flag_drive_preview){live_flag_actor=live_flag_initial;taxi_challenge_flag_begin(&live_flag_timer);live_flag_warning_palette=0;}
        if(challenge_ball_drive_preview){memcpy(live_balls,live_balls_initial,sizeof live_balls);memcpy(previous_balls,live_balls,sizeof live_balls);taxi_challenge_ball_begin(&live_ball_state);live_flag_warning_palette=0;live_ball_ticks=live_ball_submissions=0;}
    }else{
    if(!source_attract_ai&&scene_index<2){
        TaxiVehicleStartPoint point;uint32_t prior_rng=passenger_scene.random_state;
        uint8_t settings=native_settings_loaded?native_settings[0]:0;
        if(taxi_vehicle_start_gameplay(&car,&drive_context,&native_vehicle_start_owner,&passenger_scene.random_state,
            &runtime.starts[scene_index],runtime.templates[template_index],584,settings,0,taxi_world_asset_ground,&world,&point)!=1)
            fail("Cannot initialize recovered native vehicle startup.");
        memcpy(&camera_base,native_vehicle_start_owner.actor+0x680,4);
        printf("NATIVEVEHICLE city%u warm=%u rng=%u->%u position=%ld,%ld,%ld; original14A58 owner with native ground backend\n",(unsigned)scene_index+1,native_vehicle_start_initialized,prior_rng,passenger_scene.random_state,(long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z);
        native_vehicle_start_initialized=1;
    }else if(taxi_simulation_start(&car,&drive_context,runtime.templates[template_index],584,&runtime.starts[scene_index],source_attract_ai?attract_settings:0,
        taxi_world_asset_ground,&world)!=1) fail("Cannot place taxi at original start.");
    }
    /* Original AE52..AE62: vehicle, traffic, then session/passengers. */
    if(scene_index<2){
        TaxiGameSettings settings;taxi_game_settings_defaults(&settings);
        uint8_t traffic_setting=native_settings_loaded?native_settings[3]:settings.bytes[3];
        if(taxi_traffic_actors_reinitialize(traffic_actors,50,taxi_traffic_density(traffic_setting),traffic_scales))
            fail("Cannot initialize original traffic actors.");
    }
    if(challenge_bound_drive_preview){
        uint32_t rng=passenger_scene.random_state;TaxiPassengerState actor=passenger_scene.customers[0];
        if(taxi_challenge_bound_course_actor(&live_bound_course,&actor))fail("Turn original passenger init failed.");
        memset(&passenger_scene,0,sizeof passenger_scene);passenger_scene.random_state=rng;
        passenger_scene.count=1;passenger_scene.candidate=-1;passenger_scene.customers[0]=actor;
        passenger_scene.spawns[0]=(TaxiRoutePoint){1177,13529};passenger_scene.destinations[0]=0;
        passenger_scene.pickup.selected_index=passenger_scene.pickup.dialogue_index=passenger_scene.pickup.destination_index=-1;
        passenger_scene.session.trip.active_customer=-1;passenger_scene.session.trip.direction_753c=-1;
        memcpy(passenger_scene.session.settings.bytes,native_settings,8);passenger_scene.session.settings.bytes[0]=(native_settings[0]&31)|128;
        passenger_scene.session.settings.bytes[3]=4;passenger_scene.session.clock.remaining_16a8=live_bound_state.remaining_1660;
        passenger_scene.use_native_random=1;passenger_scene_initialized=1;
    }else if(challenge_turn_drive_preview){
        uint32_t rng=passenger_scene.random_state;TaxiPassengerState actor=passenger_scene.customers[0];
        if(taxi_challenge_turn_course_actor(&live_turn_course,&actor))fail("Turn original passenger init failed.");
        memset(&passenger_scene,0,sizeof passenger_scene);passenger_scene.random_state=rng;
        passenger_scene.count=1;passenger_scene.candidate=-1;passenger_scene.customers[0]=actor;
        passenger_scene.spawns[0]=(TaxiRoutePoint){7424,-17740};passenger_scene.destinations[0]=0;
        passenger_scene.pickup.selected_index=passenger_scene.pickup.dialogue_index=passenger_scene.pickup.destination_index=-1;
        passenger_scene.session.trip.active_customer=-1;passenger_scene.session.trip.direction_753c=-1;
        memcpy(passenger_scene.session.settings.bytes,native_settings,8);passenger_scene.session.settings.bytes[0]=(native_settings[0]&31)|128;
        passenger_scene.session.settings.bytes[3]=4;passenger_scene.session.clock.remaining_16a8=live_turn_state.remaining_165c;
        passenger_scene.use_native_random=1;passenger_scene_initialized=1;
    }else if(passenger_diagnostic){
        int status;
        if(!challenge_jump_drive_preview&&!live_attract&&native_settings_loaded){
            memcpy(native_session_init.settings,native_settings,512);
            prior_session.fare.qualified_1691=0; /* Original AC prefix before B6EC. */
            if(taxi_session_init_fare(&native_session_init,&prior_session,240))fail("Original normal session reset failed.");
            status=taxi_passenger_scene_init_prepared_live(&passenger_scene,&passenger_asset,&world,0,0,
                (uint8_t)passenger_scene_initialized,&prior_session,&passenger_scene.random_state,observer,context);
            if(!status&&memcmp(&passenger_scene.session,&prior_session,sizeof prior_session))fail("Passenger initialization replaced prepared session state.");
            printf("NATIVESESSION scene=%s clock=%ld warm=%u retained-trip-time=%ld; full original startup ordering pending\n",scene_index==1?"city2":"city1",(long)prior_session.clock.remaining_16a8,(unsigned)passenger_scene_initialized,(long)prior_session.trip.initial_time_16b0);
        }else{
            status=passenger_scene_initialized?
                taxi_passenger_scene_reinit_live(&passenger_scene,&passenger_asset,&world,0,&passenger_scene.random_state,observer,context):
                taxi_passenger_scene_init_live(&passenger_scene,&passenger_asset,&world,0,0,&passenger_scene.random_state,observer,context);
            if(native_settings_loaded){memcpy(passenger_scene.session.settings.bytes,native_settings,8);passenger_scene.session.clock.remaining_16a8=taxi_game_initial_clock(&passenger_scene.session.settings,0);}
        }
        if(status)fail("Customer diagnostic initialization failed.");
        memcpy(passenger_scene.session.ranking_scores,default_ranking_scores,sizeof default_ranking_scores);
        passenger_scene.session.ranking_loaded=1;
        passenger_scene.use_native_random=1;passenger_scene_initialized=1;
    }


    if(recovered_engine_audio&&passenger_scene_initialized){
        /* OrdinaryAE62/AE66 and alternateABD4/ABD8:227AC then11490.
           Channel cleanup belongs to the prior exit, not this source reset. */
        TaxiGameAudioControllerInput prior={0};
        if(taxi_game_audio_frame_import(&live_sound_state,&prior,&car,&drive_context,
            &passenger_scene,&traffic_globals,traffic_timer))fail("Recovered sound reset shared owners invalid.");
        live_sound_state.handles[0]=live_engine_audio.handle[0];
        live_sound_state.handles[1]=live_engine_audio.handle[1];
        taxi_game_audio_controller_reset(&live_sound_state,&live_sound_auxiliary_timer);
        taxi_game_audio_frame_commit(&live_sound_state,&drive_context,&passenger_scene,
            &traffic_globals,&traffic_timer);
        live_engine_audio.handle[0]=live_sound_state.handles[0];
        live_engine_audio.handle[1]=live_sound_state.handles[1];
        live_engine_factor=live_sound_state.factor;
    }

    /* Original ordinary AE6A: explicit reward reset after sound reset. */
    if(!live_attract&&!challenge_jump_drive_preview&&passenger_scene_initialized&&
       taxi_reward_session_reset(&passenger_scene.session))fail("Original reward reset failed.");
    /* Cold boot clears owner state; source21c10 subsequent sessions clear
       records/ring only, retaining timers and pointer-slot tables. */
    if(!native_actor_effects_initialized){
        if(taxi_actor_effects_cold_init(&native_actor_effects))fail("Effect cold initialization failed.");
        native_actor_effects_initialized=1;
    }
    if(taxi_actor_effects_session_reset(&native_actor_effects))fail("Effect session reset failed.");
    native_effect_request_count=0;memset(native_effect_capture_records,0,sizeof native_effect_capture_records);
    native_effect_emissions=native_effect_visible_frames=native_effect_max_active=0;gear_input_tick=0;drift_raw_index=0;drift_raw_held=0;
    taxi_camera_mode2_offsets(&camera_offsets,car.world.height.scale_e84,car.driver.power.gears.flags_4ac,0,0,4096,12288);
    memcpy(previous_traffic_actors,traffic_actors,sizeof traffic_actors);
    memcpy(previous_customers,passenger_scene.customers,sizeof previous_customers);
    previous_camera_offsets=camera_offsets;
    previous_position=car.world.height.position;drive_clock.phase=0;simulation_error=0;
    previous_vehicle_heading=car.world.heading_1c;previous_motion_heading=car.world.heading_20;
    previous_body_roll=car.world.angle_1a;
    camera_yaw_offset=previous_camera_yaw_offset=camera_yaw_target=0;
    camera_heading=previous_camera_heading=car.initial_camera_heading_654;camera_orbit=0;
    live_camera_mode_64c=previous_camera_mode_64c=car.initial_camera_mode_64c;camera_lookback_tick=0;
    camera_pitch=previous_camera_pitch=car.initial_camera_pitch_650;camera_pitch_orbit=0;
    reset_camera_view();pitch=camera_pitch_orbit;
    yaw=(float)taxi_view_yaw(car.world.heading_1c,0);
    if(challenge_jump_drive_preview)challenge_music_begin();
}
static uint32_t live_game_sound_dispatch(void *unused,TaxiGameAudioEngineRequest q){
    (void)unused;++recovered_game_requests;
    if(q.op==5){
        TaxiActionContext actions={.profile=(uint8_t)((native_settings[0]>>1)&3),
            .vehicle_flags=car.driver.power.gears.flags_4ac};
        return (uint32_t)taxi_action_query(&driving_buttons,actions,(unsigned)q.value);
    }
    if(q.op==8){++recovered_game_random;
        return (uint32_t)taxi_random_range_tagged(&passenger_scene.random_state,0,65535,"game-sound-followup");}
    if(q.op==7){
        TaxiRewardInput input={.kind=1,.active_1694=passenger_scene.session.trip.active_1694,
            .settings_0=passenger_scene.session.settings.bytes[0],
            .sound_enabled=(uint8_t)audio_settings_state.effects_enabled,
            .whole=(int32_t)q.handle,.fraction=q.value};TaxiRewardEffects effects;
        if(taxi_reward_session_request(&passenger_scene.session,&passenger_scene.random_state,&input,&effects))fail("Recovered game sound reward failed.");
        ++recovered_game_rewards;
        if(effects.sound_table_index>=0)audio_request((unsigned)effects.sound_table_index);
        return 0;
    }
    if(q.op==9){audio_request(q.handle);return 0;}
    if(q.op==6){
        if(q.handle<1||q.handle>4||q.value<0||q.value>=203)fail("Recovered game sound busy owner invalid.");
        return (uint32_t)taxi_audio_channel_busy(audio_channels+q.handle-1,audio_bank.sounds+q.value);
    }
    uint32_t result=live_engine_dispatch(NULL,q);
    if(q.op==1&&!result)++audio_rejected;
    return result;
}
static void live_normal_exit_audio_check(void){
    /* Explicit diagnostic Quit trigger, not a user pause-menu verification. */
    uint8_t frozen=(uint8_t)(startup_audio_cleanup_smoke>=2);
    uint8_t effects_before=startup_audio_cleanup_smoke>=3?audio_settings_state.saved_effects_enabled:audio_settings_state.effects_enabled;
    TaxiMusicTracker retained_tracker=music_tracker;
    if(startup_audio_cleanup_smoke==2){
        if(taxi_pause_runtime_control(&native_pause,8,city_fade.active_177b,(uint8_t)live_game_start.active,
            passenger_scene.session.ended_1697,&native_pause_frame)||!native_pause.menu.frozen||!native_pause_frame.events.pause_audio)
            fail("Exit diagnostic Start did not pause through native controller.");
        if(taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_PAUSE)||
            taxi_audio_settings_disable_effects(&audio_settings_state)||music_tracker.state.playing||audio_settings_state.effects_enabled)
            fail("Exit diagnostic native pause audio failed.");
        retained_tracker.state.playing=0;
        if(memcmp(&retained_tracker,&music_tracker,sizeof retained_tracker))fail("Pause changed retained tracker position or voices.");
    }
    if(startup_audio_cleanup_smoke>=3&&(!native_pause.menu.frozen||native_pause.menu.outer_state!=2||
        !native_pause.menu.exit_flag||city_fade.active_177b||passenger_scene.session.outer_state_1ea8!=2))
        fail("Actual Quit exit did not complete its original fade.");
    TaxiNormalExit parent={.outer=startup_audio_cleanup_smoke>=3?native_pause.menu.outer_state:2,
        .frozen=frozen,.fade=startup_audio_cleanup_smoke>=3?city_fade.active_177b:0};TaxiNormalExitRequests requests;
    if(taxi_normal_exit_begin(&parent,&requests)!=1||requests.count!=(frozen?4u:2u)||
        requests.calls[0]!=0x08010aac||requests.calls[requests.count-1]!=0x08027730||
        (frozen&&(requests.calls[1]!=0x08027754||requests.calls[2]!=0x0802771c)))fail("Original normal exit audio order invalid.");
    uint32_t retained[10];memcpy(retained,live_sound_state.handles,sizeof retained);
    unsigned active_before=0;
    for(unsigned j=0;j<10;j++)if(retained[j]){
        if(retained[j]>4)fail("Normal exit sound owner invalid.");
        if(audio_channels[retained[j]-1].sound)++active_before;
    }
    unsigned stops=taxi_game_audio_cleanup_dispatch(live_sound_state.handles,live_game_sound_dispatch,NULL);
    if(memcmp(retained,live_sound_state.handles,sizeof retained))fail("Normal exit changed retained sound owners.");
    for(unsigned j=0;j<10;j++)if(retained[j]&&audio_channels[retained[j]-1].sound)fail("Normal exit sound channel still active.");
    /* Source27730 follows10AAC. Previously queued device buffers are separate. */
    if(frozen){
        if(taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_RESUME)||music_tracker.state.playing!=1)
            fail("Frozen exit did not resume native music owner.");
        retained_tracker.state.playing=1;
        if(memcmp(&retained_tracker,&music_tracker,sizeof retained_tracker))fail("Exit resume changed retained tracker position or voices.");
        if(taxi_audio_settings_restore_effects(&audio_settings_state)||audio_settings_state.effects_enabled!=effects_before)
            fail("Frozen exit did not restore saved effects gate.");
    }
    if(taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_STOP)||music_tracker.state.playing)
        fail("Normal exit music stop failed.");
    int16_t mixed[352]={0};
    for(unsigned j=0;j<4;j++)if(taxi_audio_channel_mix(audio_channels+j,mixed,352,audio_bank.codebook,sizeof audio_bank.codebook,
        taxi_audio_effective_volume(audio_global_volume,audio_channels[j].volume))<0)fail("Exit diagnostic effects mix failed.");
    for(unsigned j=0;j<352;j++)if(mixed[j])fail("Native effects continued after source cleanup.");
    if(!stops||!active_before)fail("Exit check did not reach live sound channels.");
    if(startup_audio_cleanup_smoke>=3){
        parent.ended=passenger_scene.session.ended_1697;
        parent.qualified=passenger_scene.session.fare.qualified_1691;
        parent.exit_flag=native_pause.menu.exit_flag;
        startup_quit_parent=parent;
    }
    if(startup_audio_cleanup_smoke>=3)printf("PASS LIVE QUIT YES AUDIO: actual native Start/Down/Down/Confirm/Yes/Confirm, %u exit fade ticks, taxi and countdown frozen, cleanup after fade; parent finish/menu reentry and queued-device drain unverified.\n",startup_quit_fade_ticks);
    if(startup_audio_cleanup_smoke==2)puts("PASS LIVE FROZEN EXIT AUDIO: native Start pause, retained tracker voices/position, cleanup then resume music then restore effects then stop music; diagnostic Quit trigger, actual Yes choice unverified.");
    printf("PASS LIVE NORMAL EXIT AUDIO: %u ordered stops, %u active owned slots; retained ten handles, next352 effects samples zero, music stopped after effects. User Quit/retry and queued-device drain not verified.\n",stops,active_before);
}
static int8_t live_game_sound_selection(void *unused){
    (void)unused;uint8_t v=passenger_scene.session.trip.voice_25d6;
    return (int8_t)(v<128?v:(int)v-256);
}
static FILE *gameplay_step_replay;
static FILE *gameplay_key_replay;
static FILE *gameplay_xbox_replay;
static TaxiFrameTime gameplay_frame_time;
static uint32_t gameplay_replay_counter;
static int32_t gameplay_global_delta=1;
static int respawn_clock_smoke;static int focus_suspend_smoke;static int native_elapsed_time;static TaxiHostTime live_host_time;static FILE *host_time_trace;
static unsigned timestamped_timer_draws;static int timestamped_presentation,presentation_delay_configured;static uint64_t presentation_delay_ns=33500000ull;static FILE *presentation_time_trace;static TaxiRenderPoseHistory presentation_history;
static TaxiCameraOffsets presentation_offset_history[TAXI_RENDER_HISTORY_CAPACITY];
static TaxiPassengerScene presentation_passenger_history[TAXI_RENDER_HISTORY_CAPACITY];
static TaxiCashGlyphs presentation_cash_history[TAXI_RENDER_HISTORY_CAPACITY];static uint16_t presentation_tip_frames[TAXI_RENDER_HISTORY_CAPACITY];
static unsigned timestamped_clock_draws,timestamped_cash_draws,timestamped_tip_requests,timestamped_expiry_draws;
static uint8_t presentation_panel_pixels[TAXI_RENDER_HISTORY_CAPACITY][4096];static unsigned presentation_panel_generations[TAXI_RENDER_HISTORY_CAPACITY],timestamped_panel_draws;
static TaxiEffectPresentationSlot presentation_effect_history[TAXI_RENDER_HISTORY_CAPACITY][9];
static uint64_t presentation_effect_generations[TAXI_RENDER_HISTORY_CAPACITY][9];static unsigned timestamped_effect_blends,timestamped_effect_snaps;
static TaxiEffectSprite presented_effect_requests[9];static unsigned presented_effect_count,timestamped_effect_requests;
static int32_t presentation_motion_headings[TAXI_RENDER_HISTORY_CAPACITY],presented_motion_heading;
static int32_t presentation_guidance_angles[TAXI_RENDER_HISTORY_CAPACITY];static unsigned presentation_turn_guidance_frames[TAXI_RENDER_HISTORY_CAPACITY],timestamped_guidance_draws,timestamped_shadow_views;
static unsigned timestamped_fallback_passenger_draws;static unsigned timestamped_passenger_views,timestamped_passenger_records,timestamped_passenger_skips,timestamped_passenger_attempts,timestamped_passenger_culled;static FILE *passenger_presentation_trace;
static TaxiTrafficActor presentation_traffic_history[TAXI_RENDER_HISTORY_CAPACITY][50];
static TaxiTrafficSubmission presentation_traffic_submissions[TAXI_RENDER_HISTORY_CAPACITY][50];
static unsigned presentation_traffic_counts[TAXI_RENDER_HISTORY_CAPACITY],timestamped_traffic_views;static TaxiRenderBracket presented_bracket;static uint16_t presentation_seen_mode;
static TaxiRenderPose presented_pose;static int presented_pose_valid;
static unsigned presentation_seen_updates,presentation_seen_revision;
static int presentation_seen_focus=-1;
static int32_t live_input_delta=1;static FILE *input_time_trace;
static const char *reference_driving_path;static TaxiReferenceDriving reference_driving;
static unsigned experimental_update_ticks=1;static int experimental_update_set;
static int live_host_ready=1;static unsigned live_host_updates;static uint64_t live_host_audio_ticks;
/* Variable-step live mode presents its current endpoint until timestamped
   interval interpolation is recovered; audio-clock phase is not its alpha. */
static double gameplay_render_alpha(void){return native_elapsed_time?1.0:taxi_clock_alpha(&drive_clock);}
static unsigned shown_effect_count(void){return presented_pose_valid?presented_effect_count:(unsigned)(native_effect_request_count>0?native_effect_request_count:0);}
static const TaxiFareSession *shown_fare_session(void){return presented_pose_valid?&presentation_passenger_history[presented_bracket.after_tag].session:&passenger_scene.session;}
static const TaxiEffectSprite *shown_effect_requests(void){return presented_pose_valid?presented_effect_requests:native_effect_requests;}
static void sample_native_driving_buttons(uint16_t keys,int32_t frame_delta) {
    int32_t elapsed=native_elapsed_time?live_input_delta:frame_delta;
    taxi_input_update(&driving_buttons,(TaxiInputSample){.active_low_keys=keys,.elapsed_ticks=(uint16_t)elapsed});
    if(input_time_trace){fprintf(input_time_trace,"%d %d %u\n",frame_delta,elapsed,driving_buttons.raw_keys);fflush(input_time_trace);}
}
static void native_drive_sound_requests(const TaxiDriveWorldEffects *effects) {
    if(simulation_error)return;
    if(effects->world.movement.collided)audio_request(effects->world.impact.sound_table_index);
    for(unsigned i=0;i<effects->driver.gear.count;i++)audio_request(effects->driver.gear.indices[i]);
    for(unsigned i=0;i<effects->driver.power.count;i++)audio_request(effects->driver.power.indices[i]);
}
static void native_drive_effect_tail(TaxiDriveWorldEffects effects,int start_active,int paused,int movement_blocked,int gas,TaxiActionContext actions) {
    /* move_position returns immediately on its first collision, hence one
       recovered impact request per step.217d4 uses preceding body vectors
       before145e4 refreshes attachments. Native cached requests feed the OBJ renderer. */
    if(!simulation_error&&effects.world.movement.collided&&
       taxi_actor_effects_impact(&native_actor_effects,effects.world.impact.effect_slot,
           0,gameplay_global_delta,60)<0)simulation_error_at(__LINE__);
    /* Original B296 skips the ordinary actor tail while intro1696 is active. */
    int original_intro_tail_blocked=start_active&&!challenge_jump_drive_preview;
    /* Source b2e0 actor tail:145e4, four emitters,208d4, then camera. */
    if(!simulation_error&&!paused&&!movement_blocked&&!original_intro_tail_blocked&&taxi_body_points(&native_body_points,car.world.height.position,
        car.world.angle_18,car.world.angle_1a,car.world.heading_1c,
        car.world.drift_69c,&runtime.trig)<0)simulation_error_at(__LINE__);
    if(!simulation_error&&!paused&&!original_intro_tail_blocked){
        /* Source signed material_e60 indexes28-byte records, flags+20.
           -1 reads the preserved bytes immediately before the ROM table. */
        int16_t material;memcpy(&material,&car.world.material_e60,sizeof material);
        if(material < -1||(material>=0&&(uint32_t)material>=world.nm))simulation_error_at(__LINE__);
        else {
            TaxiActorEffectInput effect_input={
                .actor_flags=car.driver.power.gears.flags_28,
                .material_flags=material==-1?native_material_prefix[20]:world.materials[28u*(unsigned)material+20],
                .alternate=0,.action2=(uint8_t)gas,
                .phase_e6a=(uint8_t)car.driver.power.gears.phase_e6a,
                .voice_25d4=drive_context.driver.gear.state_25d4,
                .speed=car.world.speed_544,.scale=car.world.height.scale_e84,
                .state_2630=drive_context.driver.gear.state_2630,.delta=gameplay_global_delta,.clock=60};
            int emitted=movement_blocked?taxi_actor_effects_advance(&native_actor_effects,effect_input,taxi_world_asset_height,&world):taxi_actor_effects_step(&native_actor_effects,&native_body_points,
                effect_input,taxi_world_asset_height,&world);
            if(emitted<0)simulation_error_at(__LINE__);
            else native_effect_emissions+=(unsigned)emitted;
            if(native_effect_state_trace&&emitted>=0){
                fprintf(native_effect_state_trace,"%u %ld %ld %ld %u %u %ld %ld %d %d %ld %ld %u %u %u %ld %u %u %u %ld %ld",
                    native_effect_state_tick++,(long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z,
                    (unsigned)car.world.angle_18,(unsigned)car.world.angle_1a,(long)car.world.heading_1c,(long)car.world.drift_69c,
                    material,effect_input.actor_flags,(long)effect_input.speed,(long)effect_input.scale,effect_input.phase_e6a,
                    effect_input.voice_25d4,effect_input.action2,(long)effect_input.state_2630,effect_input.material_flags,
                    effect_input.alternate,native_actor_effects.ring,(long)effect_input.delta,(long)effect_input.clock);
                for(unsigned j=0;j<4;j++)fprintf(native_effect_state_trace," %ld",(long)native_actor_effects.timers[j]);
                for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)fprintf(native_effect_state_trace," %d",native_actor_effects.pool.slots[g][j]);
                for(unsigned j=0;j<648;j++)fprintf(native_effect_state_trace," %u",((const unsigned char*)native_actor_effects.pool.records)[j]);
                for(unsigned j=0;j<10;j++)fprintf(native_effect_state_trace," %u %u %u",driving_buttons.held[j],driving_buttons.pressed[j],driving_buttons.repeat[j]);
                fprintf(native_effect_state_trace," %u %u %u %u %d",actions.profile,actions.vehicle_flags,actions.override_enabled,actions.override_flags,taxi_action_query(&driving_buttons,actions,2));
                fputc('\n',native_effect_state_trace);
                if(ferror(native_effect_state_trace))fail("Effect state trace write failed.");
            }
        }
    }
}
static void drive_tick_inner(TaxiXboxSample pad,int scripted) {
    int32_t frame_delta=native_elapsed_time?live_host_time.frame.delta_3b00:1;
    if(gameplay_step_replay){
        unsigned elapsed;
        if(fscanf(gameplay_step_replay,"%u",&elapsed)!=1||elapsed<1||elapsed>60)fail("Original timing replay exhausted or invalid.");
        gameplay_replay_counter+=elapsed;
        if(taxi_frame_time_update(&gameplay_frame_time,gameplay_replay_counter,(uint8_t)native_pause.menu.frozen))fail("Original frame timing invalid.");
        frame_delta=gameplay_frame_time.delta_3b00;
    }

    if(gameplay_key_replay){
        unsigned held;
        if(fscanf(gameplay_key_replay,"%u",&held)!=1||held>1023)fail("Original key replay exhausted or invalid.");
        pad=(TaxiXboxSample){.connected=1,.active_low_keys=(uint16_t)(held^1023u)};scripted=0;
    }
    if(gameplay_xbox_replay){
        unsigned buttons,rt,lt;int lx,ly;
        if(fscanf(gameplay_xbox_replay,"%u %d %d %u %u",&buttons,&lx,&ly,&rt,&lt)!=5||buttons>65535||lx<-32768||lx>32767||ly<-32768||ly>32767||rt>255||lt>255)fail("Native Xbox replay exhausted or invalid.");
        pad=taxi_xbox_decode((uint16_t)buttons,(int16_t)lx,(int16_t)ly,(uint8_t)rt,(uint8_t)lt);scripted=0;
    }
    uint8_t paused=(uint8_t)(native_pause.menu.frozen&&!live_attract&&!challenge_jump_drive_preview);
    if(profile_challenge_owner&&profile_live_active){
        uint16_t mask=pad.connected?(uint16_t)(pad.active_low_keys^0x3ff):0;
        if(scripted&&challenge_jump_retry_check==3)mask=challenge_profile_check_mask();
        if(!scripted){
            if(GetAsyncKeyState(VK_UP)&0x8000)mask|=64;
            if(GetAsyncKeyState(VK_DOWN)&0x8000)mask|=128;
            if((GetAsyncKeyState(VK_RETURN)|GetAsyncKeyState('Z'))&0x8000)mask|=1;
        }
        profile_tick(mask);
        return;
    }
    if(challenge_bound_drive_preview){
        /* Original206BC runs in Turn phase0 before scalar countdown.
           176C is the selected destination pointer, retained current532E
           resets to0 in original1EE28. Native preview pause is currently0. */
        int32_t selected=passenger_scene.pickup.destination_index;
        if(passenger_scene.session.trip.active_1694&&selected>=0&&(unsigned)selected<passenger_asset.destination_count){
            TaxiCameraView view=taxi_camera_normal_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,&runtime.trig);
            int32_t target;
            if(taxi_guidance_route_angle(guidance_route.segments,guidance_route.count,selected,passenger_asset.destinations[selected],car.world.height.position,view.position,view.yaw,&runtime.trig,&runtime.traction,&target))fail("Turn phase0 guidance target failed.");
            turn_guidance_frame=taxi_guidance_frame_step(turn_guidance_frame,((unsigned)target>>7)&31,1);
        }
        int was_ended=drive_context.height.flag_1697;
        TaxiChallengeBoundInput gates={.outer1ea8=drive_context.height.flag_153c?challenge_jump_success.outer1ea8:challenge_jump_failure.outer1ea8,.delta=frame_delta,.vehicle_y2638=car.world.height.position.y,.intro1696=(uint8_t)live_game_start.active,.effects_enabled=(uint8_t)audio_settings_state.effects_enabled};
        TaxiChallengeBoundStatus status={drive_context.height.flag_1697,drive_context.height.flag_153c};TaxiChallengeBoundEffects effects;
        if(taxi_challenge_bound_course_control(&live_bound_course,&drive_context,&live_bound_state,gates,&status,&effects))fail("Turn native controller failed.");
        if(effects.warning_palette){live_flag_warning_palette=1;live_bound_warnings++;}
        for(unsigned n=0;n<effects.sound_count;n++)audio_request((unsigned)effects.sound_index[n]);
        /* Original8EE6..8EF4 applies these shared writes after sound requests. */
        if(effects.failure1534){
            passenger_scene.session.audio_muted_1534=effects.failure1534;
            native_session_init.shared_1534[0]=effects.failure1534;
            native_session_init.shared_1534[1]=(uint8_t)effects.failure1535;
            if(bound_failure_check)printf("BOUND FAILURE OWNER: shared1534=%u shared1535=%d latch=%d\n",(unsigned)passenger_scene.session.audio_muted_1534,(int)effects.failure1535,(int)live_bound_state.failure_latch165e);
        }
        passenger_scene.session.clock.remaining_16a8=live_bound_state.remaining_1660;
        if(status.ended1697&&!was_ended)return;
    }
    if(challenge_turn_drive_preview){
        /* Original206BC runs in Turn phase0 before scalar countdown.
           176C is the selected destination pointer, retained current532E
           resets to0 in original1EE28. Native preview pause is currently0. */
        int32_t selected=passenger_scene.pickup.destination_index;
        if(passenger_scene.session.trip.active_1694&&selected>=0&&(unsigned)selected<passenger_asset.destination_count){
            TaxiCameraView view=taxi_camera_normal_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,&runtime.trig);
            int32_t target;
            if(taxi_guidance_route_angle(guidance_route.segments,guidance_route.count,selected,passenger_asset.destinations[selected],car.world.height.position,view.position,view.yaw,&runtime.trig,&runtime.traction,&target))fail("Turn phase0 guidance target failed.");
            turn_guidance_frame=taxi_guidance_frame_step(turn_guidance_frame,((unsigned)target>>7)&31,1);
        }
        int was_ended=drive_context.height.flag_1697;
        TaxiChallengeTurnInput gates={.outer1ea8=drive_context.height.flag_153c?challenge_jump_success.outer1ea8:challenge_jump_failure.outer1ea8,.delta=frame_delta,.intro1696=(uint8_t)live_game_start.active,.effects_enabled=(uint8_t)audio_settings_state.effects_enabled};
        TaxiChallengeTurnStatus status={drive_context.height.flag_1697,drive_context.height.flag_153c};TaxiChallengeTurnEffects effects;
        if(taxi_challenge_turn_course_control(&live_turn_course,&drive_context,&live_turn_state,gates,&status,&effects))fail("Turn native controller failed.");
        if(effects.warning_palette){live_flag_warning_palette=1;live_turn_warnings++;}
        if(effects.sound_index>=0)audio_request((unsigned)effects.sound_index);
        passenger_scene.session.clock.remaining_16a8=live_turn_state.remaining_165c;
        if(status.ended1697&&!was_ended)return;
    }
    if(challenge_drift_drive_preview){
        int was_ended=drive_context.height.flag_1697;
        TaxiChallengeDriftInput gates={.outer1ea8=drive_context.height.flag_153c?challenge_jump_success.outer1ea8:challenge_jump_failure.outer1ea8,.delta=frame_delta,.intro1696=(uint8_t)live_game_start.active,.effects_enabled=(uint8_t)audio_settings_state.effects_enabled};
        TaxiChallengeDriftStatus status={drive_context.height.flag_1697,drive_context.height.flag_153c};TaxiChallengeDriftEffects effects;
        if(taxi_challenge_drift_course_control(&live_drift_course,&drive_context,&live_drift_state,gates,&status,&effects))fail("Drift native controller failed.");
        if(effects.warning_palette){live_flag_warning_palette=1;live_drift_warnings++;}
        if(effects.sound_index>=0)audio_request((unsigned)effects.sound_index);
        if(effects.request_rank&&taxi_challenge_drift_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,(int16_t)drive_context.driver.motion.orientation.counter_164c,&effects,&drive_context.height.flag_153d)<0)fail("Drift combo rank failed.");
        /* Shared BA04 terminal handling starts on the following callback. */
        if(status.ended1697&&!was_ended)return;
    }
    if(challenge_ball_drive_preview){
        memcpy(previous_balls,live_balls,sizeof live_balls);
        TaxiBallInput gates={.delta=frame_delta,.intro=(uint8_t)live_game_start.active,.effects_enabled=(uint8_t)audio_settings_state.effects_enabled,
          .outer=drive_context.height.flag_153c?challenge_jump_success.outer1ea8:challenge_jump_failure.outer1ea8};
        TaxiBallStatus status={drive_context.height.flag_153c,drive_context.height.flag_1697};TaxiBallEffects effects;
        if(taxi_challenge_ball_course_control(&live_ball_course,&live_ball_state,live_balls,&car,gates,&status,&effects,&runtime.trig))fail("Ball livecontroller failed.");
        live_ball_ticks++;
        if(effects.warning_palette)live_flag_warning_palette=1;
        if(effects.warning_sound)audio_request(39);
        for(unsigned i=0;i<effects.pop_sound_count;i++)audio_request(45);
        drive_context.height.flag_153c=status.success;drive_context.height.flag_1697=status.ended;
        if(effects.request_rank&&taxi_challenge_ball_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,live_ball_state.remaining_1558,&effects,&drive_context.height.flag_153d)<0)fail("Ball elapsed-time rank failed.");
        TaxiCameraView view=taxi_camera_normal_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,&runtime.trig);
        TaxiBallProjection projections[20]={0};uint32_t visible;
        if(taxi_challenge_ball_course_render(&live_ball_course,live_balls,car.world.height.position,view.position,view.yaw,view.pitch,160,106,0,1,0,&runtime.trig,projections,&visible))fail("Ball source tick animation failed.");
    }
    if(challenge_flag_drive_preview){
        int was_ended=drive_context.height.flag_1697;
        int32_t x,z;memcpy(&x,live_flag_actor.bytes+16,4);memcpy(&z,live_flag_actor.bytes+24,4);
        TaxiChallengeFlagInput gates={.delta=frame_delta,.intro1696=(uint8_t)live_game_start.active,
            .effects_enabled=(uint8_t)audio_settings_state.effects_enabled,
            .outer1ea8=drive_context.height.flag_153c?challenge_jump_success.outer1ea8:challenge_jump_failure.outer1ea8,
            .car_x=car.world.height.position.x,.car_z=car.world.height.position.z,.flag_x=x,.flag_z=z};
        TaxiChallengeFlagStatus state={drive_context.height.flag_153c,drive_context.height.flag_1697};
        TaxiChallengeFlagEffects effects;taxi_challenge_flag_step(&live_flag_timer,&gates,&state,&effects);
        if(effects.warning_palette)live_flag_warning_palette=1;
        drive_context.height.flag_153c=state.success153c;drive_context.height.flag_1697=state.ended1697;
        if(effects.hide_flag)live_flag_actor.bytes[0]=(uint8_t)((live_flag_actor.bytes[0]&~1u)|2u);
        if(effects.sound_index>=0)audio_request((unsigned)effects.sound_index);
        if(effects.request_rank&&taxi_challenge_flag_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,live_flag_timer.remaining_1552,&effects,&drive_context.height.flag_153d)<0)fail("Flag capture score failed.");
        /* BA04 precedes this course scalar callback in the original. Newly
           ended state reaches shared failure handling on the following update.
           Flag success uses its retained remaining-time score. */
        if(state.ended1697&&!was_ended)return;
    }
    if(challenge_jump_drive_preview&&drive_context.height.flag_1697){
        if(challenge_jump_failure.outer1ea8||challenge_jump_success.outer1ea8){
            city_fade_tick(0,frame_delta);
            if(!city_fade.active_177b){
                /* Source79EC..7A02 stops tracker before Retry or menu return. */
                music_tracker.state.playing=0;
                if(challenge_jump_failure.outer1ea8==1){challenge_jump_retries++;reset_drive();}
                else if(challenge_jump_success.outer1ea8){profile_challenge_owner=challenge_bound_drive_preview?6:challenge_turn_drive_preview?5:challenge_drift_drive_preview?4:challenge_ball_drive_preview?3:challenge_flag_drive_preview?2:1;profile_live_begin(1,1);}
                else challenge_jump_return_grid();
            }
            return;
        }
        uint16_t mask=pad.connected?(uint16_t)(pad.active_low_keys^0x3ff):0;
        if(!scripted){
            if(GetAsyncKeyState(VK_UP)&0x8000)mask|=64;
            if(GetAsyncKeyState(VK_DOWN)&0x8000)mask|=128;
            if((GetAsyncKeyState(VK_RETURN)|GetAsyncKeyState('Z'))&0x8000)mask|=1;
        }
        if(challenge_jump_retry_check&&!challenge_jump_retries&&challenge_jump_failure.phase16bc>0&&!challenge_jump_failure.hold16be&&!challenge_failed_voice_busy())
            mask=challenge_jump_retry_check==2&&!challenge_jump_failure.selection153b?128:1;
        if(!drive_context.height.flag_153c){
            challenge_jump_failure.ended1697=1;
            int enabled=audio_live&&audio_settings_state.effects_enabled;
            if(!challenge_jump_failure.phase16bc&&enabled)audio_request(77);
            /* BF94 requests70 beforeBFBC queries its returned channel in this callback. */
            int starts_failed=challenge_jump_failure.phase16bc>0&&challenge_jump_failure.hold16be==1;
            if(starts_failed)challenge_failed_channel=audio_request_channel(70);
            unsigned voices=taxi_challenge_failure_step(&challenge_jump_failure,0,0,
                challenge_failed_voice_busy(),enabled,1,challenge_jump_end_input.pressed);
            if((voices&TAXI_CHALLENGE_FAILURE_VOICE)&&!starts_failed)challenge_failed_channel=audio_request_channel(70);
            if(voices&TAXI_CHALLENGE_RETRY_VOICE)audio_request(71);
            if(voices&TAXI_CHALLENGE_RETURN_VOICE)audio_request(72);
            if(challenge_jump_failure.outer1ea8){
                city_fade.interval_1771=city_fade.reload_1772=1;city_fade_tick(1,frame_delta);
            }
        }
        if(drive_context.height.flag_153c){
            challenge_jump_success.ended1697=1;
            unsigned voices=taxi_challenge_success_step(&challenge_jump_success,drive_context.height.flag_153d,
                0,0,audio_live&&audio_settings_state.effects_enabled,1);
            if(voices&TAXI_CHALLENGE_SUCCESS_VOICE)audio_request(76);
            if(voices&TAXI_CHALLENGE_BEST_VOICE)audio_request(68);
            if(voices&TAXI_CHALLENGE_PLACED_VOICE)audio_request(69);
            if(challenge_jump_success.outer1ea8){city_fade.interval_1771=city_fade.reload_1772=1;city_fade_tick(1,frame_delta);}
        }
        /* Diagnostic only: refreshed physical input, actual earned ended flag.
           Input service/effect callback fidelity remains unqualified. */
        if(challenge_bound_drive_preview&&!paused&&!challenge_jump_failure.outer1ea8&&!challenge_jump_success.outer1ea8){
            /* Current physical controls; scripted driving overrides stop at end.
               Keyboard/replay/audio effect ownership remains diagnostic scope. */
            uint16_t ended_keys=taxi_xbox_driving_keys(pad,(uint8_t)((native_settings[0]>>1)&3));
            sample_native_driving_buttons(ended_keys,frame_delta);
            TaxiCommand ended_probe_command={0};taxi_command_pack(&ended_probe_command,&driving_buttons);
            TaxiActionContext ended_actions={.profile=(uint8_t)((native_settings[0]>>1)&3),.vehicle_flags=car.driver.power.gears.flags_4ac};
            TaxiGearInput ended_input={.delta=gameplay_global_delta,.frame_delta=frame_delta,.settings=drive_context.driver.motion.orientation.settings,
                .action2=(uint8_t)taxi_action_query(&driving_buttons,ended_actions,2),.action3=(uint8_t)taxi_action_query(&driving_buttons,ended_actions,3),
                .action4=(uint8_t)taxi_action_query(&driving_buttons,ended_actions,4),.action5=(uint8_t)taxi_action_query(&driving_buttons,ended_actions,5)};
            for(unsigned i=0;i<10;i++)ended_input.held[i]=driving_buttons.held[i];
            if(pad.connected){uint16_t analog=(uint16_t)taxi_xbox_vehicle_steering_command(pad.steering,(uint8_t)car.driver.power.gears.phase_e6a,drive_context.driver.gear.state_2630);ended_probe_command.bytes[12]=(uint8_t)analog;ended_probe_command.bytes[13]=(uint8_t)(analog>>8);}
            TaxiDriveWorldEffects ended_effects={0};previous_position=car.world.height.position;
            if(taxi_drive_world_step(&car,&drive_context,&ended_probe_command,&ended_input,0,&runtime.trig,&runtime.traction,&runtime.normals,route_obstacle_query,taxi_world_asset_ground,&world,&ended_effects))simulation_error_at(__LINE__);
            native_drive_sound_requests(&ended_effects);
            if(!simulation_error)native_drive_effect_tail(ended_effects,0,paused,0,ended_input.action2,ended_actions);
            previous_camera_heading=camera_heading;previous_camera_pitch=camera_pitch;
            previous_camera_yaw_offset=camera_yaw_offset;previous_camera_offsets=camera_offsets;
            camera_heading=taxi_camera_yaw_step(camera_heading,car.world.heading_20,gameplay_global_delta,0,&runtime.trig);
            camera_pitch=taxi_camera_pitch_step(camera_pitch,(int16_t)car.world.angle_18,gameplay_global_delta,&runtime.trig);
            camera_yaw_offset=taxi_camera_yaw_offset_step(camera_yaw_offset,camera_yaw_target,frame_delta);
            if(bound_failure_check&&car.world.height.position.y<=25)printf("ENDED BOUND PROBE: y%d ended%u latch%d\n",car.world.height.position.y,drive_context.height.flag_1697,live_bound_state.failure_latch165e);
        }
        taxi_menu_input_step(&challenge_jump_end_input,mask,1);
        return;
    }
    if(attract_session_check&&!attract_session.active)return;
    if(attract_session_check&&taxi_attract_flow_step(&attract_session.flow,1,
        attract_exit_fade.active_177b,0,0)==1)taxi_palette_fade_begin(&attract_exit_fade,0);
    if(endgame_diagnostic&&result_live_tick(pad,frame_delta))return;
    /* Originalafb2 material blink precedes afb6 music and ba04 passengers.
       Ordinary host gameplay is unfrozen; paused/result flows are separate. */
    if(taxi_material_blink_update(&native_materials,frame_delta,paused)<0) {
        simulation_error_at(__LINE__);return;
    }
    /* Sourceafb6/10b54 precedes passenger clocks/expiry in ba04.
       Native result subflows return above; their original service ownership remains pending. */
    music_fade_factor=taxi_music_fade_step(music_fade_factor,frame_delta,
        passenger_scene.session.ended_1697,passenger_scene.session.outer_state_1ea8);
    if(recovered_engine_audio&&audio_live&&!live_attract){
        /* Original1696 is the recovered opening sequence active flag;1690
           is the ordinary driver selection copied at startup handoff. */
        TaxiActionContext prior_actions={.profile=(uint8_t)((native_settings[0]>>1)&3),
            .vehicle_flags=car.driver.power.gears.flags_4ac};
        live_sound_input=(TaxiGameAudioControllerInput){.delta=frame_delta,
            .speed=car.world.speed_544,.max_speed=car.speed_divisor_548,
            .scale=car.world.height.scale_e84,.rpm=car.driver.power.gears.rpm_538,
            .muted=passenger_scene.session.audio_muted_1534,
            .ended=passenger_scene.session.ended_1697,.outer=passenger_scene.session.outer_state_1ea8,
            .intro=live_game_start.active,.frozen=(uint8_t)paused,.fade=city_fade.active_177b,
            .effects=(uint8_t)audio_settings_state.effects_enabled,.settings=native_settings[0],
            .driver=(int8_t)native_game_driver,
            .action2=(uint8_t)taxi_action_query(&driving_buttons,prior_actions,2),
            .action3=(uint8_t)taxi_action_query(&driving_buttons,prior_actions,3)};
        TaxiGameAudioController shared={0};
        if(recovered_game_audio&&(!passenger_scene_initialized||(native_settings[0]&224)==128))fail("Full recovered audio diagnostic requires initialized ordinary passengers.");
        TaxiGameAudioController *owner=recovered_game_audio?&live_sound_state:&shared;
        if(passenger_scene_initialized&&taxi_game_audio_frame_import(owner,&live_sound_input,&car,&drive_context,
            &passenger_scene,&traffic_globals,traffic_timer))fail("Recovered audio shared state invalid.");
        if(recovered_game_audio){
            if(live_sound_state.pending3){
                ++recovered_game_near_miss;
                if(live_sound_input.gate00e4||live_sound_state.reward_cooldown)++recovered_game_reward_gated;
            }
            unsigned count;
            if(taxi_game_audio_controller_dispatch_shared(&live_sound_state,&live_sound_input,
                live_game_sound_dispatch,NULL,live_game_sound_selection,&count))fail("Recovered full game sound controller failed.");
            taxi_game_audio_frame_commit(&live_sound_state,&drive_context,&passenger_scene,&traffic_globals,&traffic_timer);
            live_engine_audio.handle[0]=live_sound_state.handles[0];live_engine_audio.handle[1]=live_sound_state.handles[1];
            ++recovered_game_frames;
        }else{
        TaxiGameAudioLevelsInput input={live_sound_input.speed,live_sound_input.max_speed,
            live_sound_input.scale,live_engine_cooldown,live_sound_input.delta,live_sound_input.rpm,
            live_engine_factor,live_sound_input.muted,live_sound_input.ended,live_sound_input.outer};
        TaxiGameAudioLevels levels;if(taxi_game_audio_levels(&input,&levels))fail("Recovered engine speed divisor invalid.");
        live_engine_cooldown=levels.cooldown;live_engine_factor=levels.factor;
        taxi_game_audio_engine_dispatch(&live_engine_audio,levels.low,levels.high,
            drive_context.driver.power.state_25dc,levels.pitch,live_engine_dispatch,NULL);
        drive_context.driver.power.state_25dc=taxi_game_audio_amplitude_step(drive_context.driver.power.state_25dc,frame_delta);
        }
        ++recovered_engine_frames;
    }
    memcpy(previous_traffic_actors,traffic_actors,sizeof traffic_actors);
    memcpy(previous_customers,passenger_scene.customers,sizeof previous_customers);
    TaxiCommand command={0};
    int keyboard_enabled=!gameplay_step_replay&&!startup_game_audio_smoke&&!live_attract;
    if(drift_raw_replay_path&&scripted){
        if(drift_xbox_replay){pad=live_game_start.active?taxi_xbox_decode(0,0,0,255,0):drift_raw_index<drift_raw_count?drift_xbox_samples[drift_raw_index++]:taxi_xbox_decode(0,0,0,0,0);drift_raw_held=(unsigned)(pad.active_low_keys^1023);}
        else drift_raw_held=live_game_start.active?1:drift_raw_index<drift_raw_count?drift_raw_replay[drift_raw_index++]:0;
    }
    uint16_t driving_keys=taxi_xbox_driving_keys(pad,(uint8_t)((native_settings[0]>>1)&3));
    sample_native_driving_buttons(drift_raw_replay_path&&scripted?(uint16_t)(1023^drift_raw_held):driving_keys,frame_delta);
    TaxiActionContext actions={.profile=(uint8_t)((native_settings[0]>>1)&3),
        .vehicle_flags=car.driver.power.gears.flags_4ac};
    int gas=route_probe_active?!route_probe_brake:scripted||taxi_action_query(&driving_buttons,actions,2)||pad.accelerator>30||(keyboard_enabled&&(GetAsyncKeyState(VK_UP)&0x8000));
    int brake=route_probe_active?route_probe_brake:!scripted&&(taxi_action_query(&driving_buttons,actions,3)||pad.brake>30||(keyboard_enabled&&(GetAsyncKeyState(VK_DOWN)&0x8000)));
    if(challenge_jump_retry_check==3&&scripted&&!live_game_start.active)gas=(gear_input_tick%60)>=5;
    int left=scripted?script_steer<0:(taxi_action_query(&driving_buttons,actions,0)||(keyboard_enabled&&(GetAsyncKeyState(VK_LEFT)&0x8000)));
    int right=scripted?script_steer>0:(taxi_action_query(&driving_buttons,actions,1)||(keyboard_enabled&&(GetAsyncKeyState(VK_RIGHT)&0x8000)));
    if(flag_input_replay_path&&scripted){
        flag_input_mask=(!live_game_start.active&&flag_input_index<flag_input_count)?flag_input_replay[flag_input_index++]:0;
        gas=!!(flag_input_mask&4);brake=!!(flag_input_mask&8);left=!!(flag_input_mask&1);right=!!(flag_input_mask&2);
    }
    if(turn_input_replay_path&&scripted&&!live_game_start.active){
        int analog=0;turn_input_mask=8;
        if(turn_input_index<turn_input_count){turn_input_mask=turn_input_masks[turn_input_index];analog=turn_input_analog[turn_input_index++];}
        gas=!!(turn_input_mask&4);brake=!!(turn_input_mask&8);left=!!(turn_input_mask&1);right=!!(turn_input_mask&2);
        uint16_t raw=(uint16_t)analog;command.bytes[12]=(uint8_t)raw;command.bytes[13]=(uint8_t)(raw>>8);
    }
    if(ball_input_replay_path&&scripted){
        if(!live_game_start.active&&ball_input_index<ball_input_count){ball_input_mask=ball_input_masks[ball_input_index];ball_input_steering=ball_input_analog[ball_input_index++];}
        else {ball_input_mask=0;ball_input_steering=0;}
        gas=!!(ball_input_mask&4);brake=!!(ball_input_mask&8);left=!!(ball_input_mask&1);right=!!(ball_input_mask&2);
        uint16_t raw=(uint16_t)ball_input_steering;command.bytes[12]=(uint8_t)raw;command.bytes[13]=(uint8_t)(raw>>8);
    }
    if(drift_raw_replay_path&&scripted){gas=taxi_action_query(&driving_buttons,actions,2)||(drift_xbox_replay&&pad.accelerator>30);brake=taxi_action_query(&driving_buttons,actions,3)||(drift_xbox_replay&&pad.brake>30);left=taxi_action_query(&driving_buttons,actions,0);right=taxi_action_query(&driving_buttons,actions,1);}
    command.bytes[4]=(uint8_t)((gas?4:0)|(brake?8:0)|(left?1:0)|(right?2:0));
    if((!scripted||challenge_analog_trace||drift_xbox_replay)&&pad.connected){
        uint16_t analog=(uint16_t)taxi_xbox_vehicle_steering_command(pad.steering,(uint8_t)car.driver.power.gears.phase_e6a,drive_context.driver.gear.state_2630);
        command.bytes[12]=(uint8_t)analog;command.bytes[13]=(uint8_t)(analog>>8);
    }
    if(route_probe_active&&route_probe_speed_steering){uint16_t analog=(uint16_t)route_probe_analog_command;command.bytes[12]=(uint8_t)analog;command.bytes[13]=(uint8_t)(analog>>8);}
    if(source_attract_ai){
        /* Source total1/human0 selects5554c for actor0. Player phase equals
           this actor's phase, so source separation relocation is inactive. */
        uint32_t frame_counter=0;
        if(taxi_ai_world_prepare(&car,&car,&command,&frame_counter,&attract_ai_path)<0){simulation_error_at(__LINE__);return;}
        actions.override_enabled=1;actions.override_flags=command.bytes[4];
        gas=taxi_action_query(&driving_buttons,actions,2);
        brake=taxi_action_query(&driving_buttons,actions,3);
    }
    uint16_t held=turn_input_replay_path&&scripted&&!live_game_start.active&&turn_input_index?(uint16_t)turn_input_held[turn_input_index-1]:drift_raw_replay_path&&scripted?(uint16_t)drift_raw_held:pad.connected?(uint16_t)(driving_keys^0x3ff):0;
    TaxiGearInput input={.delta=gameplay_global_delta,.frame_delta=frame_delta,.settings=challenge_jump_drive_preview?drive_context.driver.motion.orientation.settings:source_attract_ai?(uint8_t)(((native_settings_loaded?native_settings[0]:0)&31)|0x60):0,.action2=(uint8_t)gas,.action3=(uint8_t)brake,
        .action4=(uint8_t)(!scripted&&(taxi_action_query(&driving_buttons,actions,4)||(keyboard_enabled&&(GetAsyncKeyState('D')&0x8000)))),
        .action5=(uint8_t)(!scripted&&(taxi_action_query(&driving_buttons,actions,5)||(keyboard_enabled&&(GetAsyncKeyState('S')&0x8000))))};
    for(unsigned i=0;i<10;i++) input.held[i]=(uint8_t)((held>>i)&1);
    if(source_attract_ai){input.action4=input.action5=0;}
    if(gear_input_sequence){
        if(!scripted||source_attract_ai)fail("Gear input sequence requires ordinary scripted driving.");
        unsigned tick=++gear_input_tick;
        input.action5=(uint8_t)(tick>=60&&tick<70);
        input.action4=(uint8_t)(tick>=70&&tick<80);
    }
    if(challenge_jump_drive_preview&&scripted&&!live_game_start.active)
        input.action4=(uint8_t)(challenge_jump_retry_check==3?gear_input_tick++%60==4:gear_input_tick++==0);
    if(turn_input_replay_path&&scripted&&!live_game_start.active){input.action4=!!(turn_input_mask&16);input.action5=!!(turn_input_mask&32);}
    if(flag_input_replay_path&&scripted){input.action4=!!(flag_input_mask&16);input.action5=!!(flag_input_mask&32);}
    if(ball_input_replay_path&&scripted){input.action4=!!(ball_input_mask&16);input.action5=!!(ball_input_mask&32);}
    if(drift_raw_replay_path&&scripted){input.action4=(uint8_t)taxi_action_query(&driving_buttons,actions,4);input.action5=(uint8_t)taxi_action_query(&driving_buttons,actions,5);}

    if(reward_drive_running){
        unsigned t=reward_drive_tick++%180;
        input.action5=(uint8_t)(t>=60&&t<70);input.action4=(uint8_t)(t>=70&&t<80);
    }
    int start_active=live_game_start.active&&!live_attract;
    int normal_reward_frame=passenger_diagnostic&&!challenge_jump_drive_preview&&!live_attract;
    if(normal_reward_frame&&!paused)native_tip_frame=(uint16_t)((native_tip_frame+32u*(uint32_t)frame_delta)&4095);
    TaxiPassengerFrameEffects early_passenger_effects={0};
    if(normal_reward_frame&&!start_active){
        TaxiCameraView before=taxi_camera_normal_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,&runtime.trig);
        TaxiPassengerFrameInput early={.taxi=car.world.height.position,.speed=car.world.speed_544,.delta=frame_delta,.material=car.world.material_e60,.camera_word_10=before.yaw,.freeze=paused,.select_pressed=(uint8_t)taxi_action_query(&driving_buttons,actions,6),.cash_glyphs=&cash_glyphs};
        if(taxi_passenger_scene_clock_service_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,&early,&early_passenger_effects,native_destination_selected,NULL)){simulation_error_at(__LINE__);return;}
        if(early_passenger_effects.expiry_fade)city_fade_tick(1,frame_delta);
        if(passenger_scene.session.ended_1697||passenger_scene.session.outer_state_1ea8){taxi_cash_glyphs_targets(&cash_glyphs,&passenger_scene.session.fare);taxi_cash_glyphs_step(&cash_glyphs);return;}
        if(taxi_reward_update_allowed(paused,(uint8_t)start_active,passenger_scene.session.ended_1697,0)){
            TaxiRewardUpdateEffects reward_effects;
            if(taxi_reward_session_update(&passenger_scene.session,&cash_glyphs,frame_delta,(uint8_t)audio_settings_state.effects_enabled,&reward_effects)){simulation_error_at(__LINE__);return;}
            for(unsigned j=0;j<reward_effects.sound_requests;j++)audio_request(50);
        }
    }
    /* Original BA44 copies frame delta only during phase-1 intro.
       The normal B00A caller copies it again before movement. */
    if(gameplay_step_replay||native_elapsed_time)gameplay_global_delta=taxi_frame_time_intro_delta(gameplay_global_delta,frame_delta,live_game_start.phase,(uint8_t)start_active,city_fade.active_177b,passenger_scene.session.outer_state_1ea8);
    live_game_start_events=start_active?(challenge_jump_drive_preview?
        taxi_game_start_challenge_step(&live_game_start,frame_delta,240,city_fade.active_177b,0,audio_settings_state.effects_enabled):
        taxi_game_start_step(&live_game_start,frame_delta,240,city_fade.active_177b,passenger_scene.session.outer_state_1ea8,audio_settings_state.effects_enabled)):0;
    for(unsigned i=0;i<3;i++)if(live_game_start_events&(1u<<i)){live_game_start_sound_counts[i]++;audio_request(51+i);}
    /* Original AFD0 calls BA04 before AFD4 reads the intro flag. */
    start_active=live_game_start.active&&!live_attract;
    TaxiDriveWorldEffects effects={0};previous_position=car.world.height.position;
    previous_vehicle_heading=car.world.heading_1c;previous_motion_heading=car.world.heading_20;
    previous_body_roll=car.world.angle_1a;
    if(normal_reward_frame){
        drive_context.driver.motion.orientation.flag_1694=passenger_scene.session.trip.active_1694;
        drive_context.driver.motion.orientation.flag_1697=passenger_scene.session.ended_1697;
        drive_context.driver.motion.orientation.flag_1ea8=passenger_scene.session.outer_state_1ea8;
        drive_context.driver.motion.orientation.flag_177b=city_fade.active_177b;
        drive_context.driver.motion.traction.flag_1696=(uint8_t)live_game_start.active;
    }
    if(gameplay_step_replay||native_elapsed_time)gameplay_global_delta=taxi_frame_time_normal_delta(gameplay_global_delta,frame_delta,(uint8_t)start_active,paused,passenger_scene.session.ended_1697,(uint8_t)live_attract);
    input.delta=gameplay_global_delta;
    int movement_blocked=normal_reward_frame&&!start_active&&!taxi_normal_movement_allowed(passenger_scene.session.gate_1700,passenger_scene.pickup.pending_1704);
    int step_result=paused?0:movement_blocked?taxi_drive_world_gear_step(&car,&drive_context,&input,&effects):start_active?
        taxi_drive_world_start_step(&car,&drive_context,&input,0,&runtime.trig,&runtime.normals,
            route_obstacle_query,taxi_world_asset_ground,&world,&effects):
        taxi_drive_world_step(&car,&drive_context,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,
            route_obstacle_query,taxi_world_asset_ground,&world,&effects);
    if(challenge_jump_drive_preview&&!challenge_flag_drive_preview&&!challenge_ball_drive_preview&&!challenge_drift_drive_preview&&!challenge_turn_drive_preview&&!challenge_bound_drive_preview&&effects.world.height.completion_call){
        if(taxi_challenge_distance_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,&drive_context.height,&effects.world.height)<0)fail("Jump earned score update failed.");
        printf("JUMP LIVE LANDING: distance%d ended%u success%u position%d,%d,%d camera%d heading%d sprite%u. Native physics, no result-loop claim.\n",
            drive_context.height.distance_1540,drive_context.height.flag_1697,drive_context.height.flag_153c,
            car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,
            camera_heading,car.world.heading_1c,taxi_sprite_frame((int16_t)(camera_heading/4096),car.world.heading_1c));
    }
    if(movement_blocked&&!step_result){
        if(car.world.height.position.x!=previous_position.x||car.world.height.position.y!=previous_position.y||car.world.height.position.z!=previous_position.z)fail("Original movement gate changed taxi position.");
        native_gate_holds++;if(passenger_scene.pickup.pending_1704)native_gate_boarding_holds++;if(passenger_scene.session.gate_1700)native_gate_delivery_holds++;
    }
    if(step_result) simulation_error_at(__LINE__);
    else if(route_probe_active||challenge_camera_trace||flag_input_replay_path)route_probe_movement=effects.world.movement;
    if(attract_session_check&&!simulation_error){
        attract_probe_ticks++;
        if(effects.world.movement.collided)attract_probe_collisions++;
        int first_miss=!effects.world.ground_hit&&!attract_probe_ground_misses;
        if(!effects.world.ground_hit)attract_probe_ground_misses++;
        if(first_miss||attract_probe_ticks%300==0)
            printf("ATTRACT PATH TRACE: tick%u position%d,%d,%d ground%d hit%d phase%u heading%d speed%d flags%u misses%u.\n",
                attract_probe_ticks,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,
                car.world.ground_36c,effects.world.ground_hit,car.world.contact_378,car.world.heading_1c,
                car.world.speed_544,command.bytes[4],attract_probe_ground_misses);
        if(attract_probe_ticks>=7500&&attract_probe_ticks%300==0){
            int32_t target[3]={0};
            if(taxi_ai_path_target(attract_ai_path.points,attract_ai_path.count,(car.world.contact_378>>8)+300,target))fail("Attract target diagnostic failed.");
            printf("ATTRACT COLLISION TRACE: tick%u target%d,%d displacement%d,%d collided%d queries%u normal%d,%d,%d corrected%d,%d collisions%u.\n",
                attract_probe_ticks,target[0],target[2],car.world.displacement_x,car.world.displacement_z,
                effects.world.movement.collided,effects.world.movement.queries,effects.world.movement.normal.x,
                effects.world.movement.normal.y,effects.world.movement.normal.z,effects.world.movement.position.x,
                effects.world.movement.position.z,attract_probe_collisions);
        }
    }
    native_drive_sound_requests(&effects);
    /* Source126DC orientation request precedes14394 traction, then world.
       These scalar requests cannot change motion math; consume in source RNG order. */
    if(normal_reward_frame&&!simulation_error&&!start_active){
        const TaxiOrientationEffects *requests[2]={&effects.driver.motion.orientation,&effects.driver.motion.traction};
        for(unsigned j=0;j<2;j++)if(requests[j]->score_requested){
            TaxiRewardInput request={.kind=(uint8_t)requests[j]->score_a,.active_1694=passenger_scene.session.trip.active_1694,.settings_0=passenger_scene.session.settings.bytes[0],.sound_enabled=(uint8_t)audio_settings_state.effects_enabled,.whole=(int32_t)requests[j]->score_b,.fraction=(int32_t)requests[j]->score_c};TaxiRewardEffects reward_effects;
            if(taxi_reward_session_request(&passenger_scene.session,&passenger_scene.random_state,&request,&reward_effects))simulation_error_at(__LINE__);
            else{passenger_scene.pickup.voice_25d6=passenger_scene.session.trip.voice_25d6;if(reward_effects.sound_table_index>=0)audio_request((unsigned)reward_effects.sound_table_index);}
        }
    }
    taxi_random_trace_label("world-impact-voice");
    if(passenger_diagnostic&&!simulation_error){
        uint32_t voice_rng_before=passenger_scene.random_state;
        uint8_t voice_before=passenger_scene.session.trip.voice_25d6;
        int voice_requested=taxi_impact_voice(effects.world.movement.collided,
            passenger_scene.session.trip.active_1694,&passenger_scene.random_state,
            &passenger_scene.session.trip.voice_25d6);
        if(voice_requested)passenger_scene.pickup.voice_25d6=passenger_scene.session.trip.voice_25d6;
        if(native_impact_voice_trace&&effects.world.movement.collided)
            fprintf(native_impact_voice_trace,"%u %u %u %u %u %d\n",
                (unsigned)passenger_scene.session.trip.active_1694,voice_rng_before,
                (unsigned)voice_before,passenger_scene.random_state,
                (unsigned)passenger_scene.session.trip.voice_25d6,voice_requested);
    }
    taxi_random_trace_label(NULL);
    if(normal_reward_frame&&!simulation_error&&effects.world.height.airborne_call){
        TaxiRewardInput request={.kind=0,.active_1694=passenger_scene.session.trip.active_1694,.settings_0=passenger_scene.session.settings.bytes[0],.sound_enabled=(uint8_t)audio_settings_state.effects_enabled,.whole=effects.world.height.airborne_arg1,.fraction=effects.world.height.airborne_arg2};
        TaxiRewardEffects reward_effects;
        if(taxi_reward_session_request(&passenger_scene.session,&passenger_scene.random_state,&request,&reward_effects))simulation_error_at(__LINE__);
        else{passenger_scene.pickup.voice_25d6=passenger_scene.session.trip.voice_25d6;if(reward_effects.sound_table_index>=0)audio_request((unsigned)reward_effects.sound_table_index);}
    }
    native_drive_effect_tail(effects,start_active,paused,movement_blocked,gas,actions);
    if(normal_reward_frame&&!simulation_error&&!start_active){
        TaxiPassengerFrameInput pickup={.taxi=car.world.height.position,.speed=car.world.speed_544,
            .delta=frame_delta,.material=car.world.material_e60,.cash_glyphs=&cash_glyphs};
        taxi_random_trace_label("passenger-pickup-before-camera");
        if(taxi_passenger_scene_pickup_transition_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,
            &pickup,&early_passenger_effects,native_destination_selected,NULL))simulation_error_at(__LINE__);
        taxi_random_trace_label(NULL);
    }
    previous_camera_mode_64c=live_camera_mode_64c;
    previous_camera_heading=camera_heading;previous_camera_yaw_offset=camera_yaw_offset;
    previous_camera_pitch=camera_pitch;
    previous_camera_offsets=camera_offsets;
    uint8_t camera_flags_before=car.world.camera_flags_29;
    if(native_camera_state_trace&&!simulation_error){
        fprintf(native_camera_state_trace,"%ld %ld %ld %d %ld %ld %ld %ld %u %ld %u %ld %ld %ld %ld %ld %ld %u %u",
            (long)camera_heading,(long)car.world.heading_20,(long)camera_pitch,(int16_t)car.world.angle_18,
            (long)camera_yaw_offset,(long)camera_yaw_target,(long)camera_offsets.zoom_2628,
            (long)car.world.height.scale_e84,car.driver.power.gears.flags_4ac,(long)frame_delta,(unsigned)paused,
            (long)(live_game_start.axes[1]?live_game_start.axes[1]:4096),
            (long)(live_game_start.axes[2]?live_game_start.axes[2]:12288),(long)camera_base,
            (long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z,
            live_game_start.active,live_camera_mode_64c);
    }
    if(!simulation_error) {
        camera_heading=taxi_camera_yaw_step(camera_heading,car.world.heading_20,gameplay_global_delta,(int16_t)live_game_start.active,&runtime.trig);
        camera_pitch=taxi_camera_pitch_step(camera_pitch,(int16_t)car.world.angle_18,gameplay_global_delta,&runtime.trig);
        camera_yaw_offset=taxi_camera_yaw_offset_step(camera_yaw_offset,camera_yaw_target,frame_delta);
        /* This native session contains one player actor. Original ordinary
           AF60/AF62 sets total/human1; demo selector forces human1 at14DF6.
           Player11734 clears command5 camera bits; AI retains its own command.
           Other recovered modes require full view-consumer integration. */
        if(!scripted&&pad.connected&&!source_attract_ai)command.bytes[5]=taxi_xbox_camera_command(command.bytes[5],pad.look_back);
        if(camera_xbox_lookback_check){unsigned tick=camera_lookback_tick++;TaxiXboxSample sample=taxi_xbox_decode((tick>=20&&tick<80)?0x80:0,0,0,0,0);command.bytes[5]=taxi_xbox_camera_command(command.bytes[5],sample.look_back);}
        if(camera_lookback_check){unsigned tick=camera_lookback_tick++;command.bytes[5]=(uint8_t)((command.bytes[5]&~1u)|((tick>=20&&tick<80)?1u:0u));}
        taxi_camera_command(&live_camera_mode_64c,command.bytes[5]);
        int selected_camera=taxi_camera_select_mode(&live_camera_mode_64c,1,live_attract);
        if(selected_camera!=2){
            simulation_error_at(__LINE__);
            fputs("Native camera view consumer does not yet support selected original mode.\n",stderr);
        }else taxi_camera_mode2_offsets(&camera_offsets,car.world.height.scale_e84,car.driver.power.gears.flags_4ac,frame_delta,paused,
            live_game_start.axes[1]?live_game_start.axes[1]:4096,
            live_game_start.axes[2]?live_game_start.axes[2]:12288);
    }
    TaxiCameraView frame_camera=taxi_camera_mode2_view(car.world.height.position,
        camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,car.camera_offset_690,live_camera_mode_64c,&car.world.camera_flags_29,&runtime.trig);
    if(native_camera_state_trace&&!simulation_error){
        fprintf(native_camera_state_trace," %ld %ld %ld %ld %ld %ld %ld %ld %ld %d %d %d",
            (long)camera_heading,(long)camera_pitch,(long)camera_yaw_offset,(long)camera_offsets.zoom_2628,
            (long)camera_offsets.height_76c,(long)camera_offsets.distance_770,
            (long)frame_camera.position.x,(long)frame_camera.position.y,(long)frame_camera.position.z,
            frame_camera.pitch,frame_camera.yaw,frame_camera.roll);
        if(camera_owner_trace)fprintf(native_camera_state_trace," %u %u %ld %u %u %ld",command.bytes[5],live_camera_mode_64c,(long)car.camera_offset_690,camera_flags_before,car.world.camera_flags_29,(long)car.world.heading_1c);
        if(camera_distinct_headings)fprintf(native_camera_state_trace," %ld",(long)car.world.heading_1c);
        if(camera_time_owner_trace)fprintf(native_camera_state_trace," %ld",(long)gameplay_global_delta);
        fputc('\n',native_camera_state_trace);
        if(ferror(native_camera_state_trace))fail("Camera state trace write failed.");
    }
    if(start_active)city_fade_tick(0,frame_delta);
    if(passenger_diagnostic && !simulation_error && !start_active) {
        taxi_random_trace_label("passenger-frame");
        /* Ordinary yaw includes recovered offset2618; target261C starts at0.
           PC mouse orbit is a presentation control, separate from this field. */
        int32_t transform_yaw=frame_camera.yaw;
        /* Normal mode2 offsets use the live original drivetrain flags.
           Special camera branches remain pending. */
        TaxiPoint guidance_camera=frame_camera.position;
        TaxiPassengerFrameInput frame={.taxi=car.world.height.position,.speed=car.world.speed_544,
            .delta=frame_delta,.freeze=paused,.material=car.world.material_e60,.camera_word_10=transform_yaw,.random_result=0,
            .select_pressed=(uint8_t)taxi_action_query(&driving_buttons,actions,6),.cash_glyphs=&cash_glyphs};
        TaxiPassengerFrameEffects passenger_effects=normal_reward_frame?early_passenger_effects:(TaxiPassengerFrameEffects){0};
        if(challenge_bound_drive_preview){
            uint8_t request=0;frame.freeze=(uint8_t)live_game_start.active;
            if(taxi_passenger_scene_bound_frame_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,&frame,&passenger_effects,&request,native_destination_selected,NULL))simulation_error_at(__LINE__);
            if(request){drive_context.height.flag_1697=drive_context.height.flag_153c=drive_context.driver.motion.orientation.flag_1697=1;
                if(taxi_challenge_bound_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,live_bound_state.remaining_1660,request,&drive_context.height.flag_153d)<0)fail("Bound original time rank failed.");}
        }else if(challenge_turn_drive_preview){
            uint8_t request=0;frame.freeze=(uint8_t)live_game_start.active;
            if(taxi_passenger_scene_turn_frame_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,&frame,&passenger_effects,&request,native_destination_selected,NULL))simulation_error_at(__LINE__);
            if(request){drive_context.height.flag_1697=drive_context.height.flag_153c=drive_context.driver.motion.orientation.flag_1697=1;
                if(taxi_challenge_turn_complete(native_settings,native_current_profile,(uint8_t)native_game_driver,live_turn_state.remaining_165c,request,&drive_context.height.flag_153d)<0)fail("Turn original time rank failed.");}
        }else if(normal_reward_frame){
            if(taxi_passenger_scene_customer_service(&passenger_scene,&frame))simulation_error_at(__LINE__);
        }else if(taxi_passenger_scene_frame_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,
            &frame,&passenger_effects,native_destination_selected,NULL)) simulation_error_at(__LINE__);
        city_fade_tick(passenger_effects.expiry_fade,frame_delta);
        int32_t selected=passenger_scene.pickup.destination_index;
        if(!simulation_error&&passenger_scene.session.trip.active_1694&&
            taxi_guidance_route_angle(guidance_route.segments,guidance_route.count,selected,
                selected>=0?passenger_asset.destinations[selected]:NULL,
                car.world.height.position,guidance_camera,transform_yaw,
                &runtime.trig,&runtime.traction,&guidance_angle)) simulation_error_at(__LINE__);
        if(!passenger_scene.session.trip.active_1694) guidance_angle=-1;
        if(guidance_angle>=0&&selected>=0&&(unsigned)selected<passenger_asset.destination_count) {
            int32_t remaining=taxi_destination_distance(selected,passenger_asset.destinations[selected],
                car.world.height.position.x,car.world.height.position.z);
            passenger_scene.session.trip.scale_5310=taxi_guidance_scale_step(passenger_scene.session.trip.scale_5310,
                taxi_destination_vertical_scale(remaining),frame_delta);
        }
        if(!normal_reward_frame){taxi_cash_glyphs_targets(&cash_glyphs,&passenger_scene.session.fare);taxi_cash_glyphs_step(&cash_glyphs);}
        taxi_random_trace_label(NULL);
    }
    if(attract_session_check){
        taxi_palette_fade_step(&attract_exit_fade,1);
        if(taxi_attract_session_finish(&attract_session,attract_probe_settings,attract_exit_fade.active_177b)==1)
            puts("PASS diagnostic attract exit: natural controller request, scalar fade completion, saved city restored; title/audio/visual fade host transition pending.");
    }
    /* Source b36820a20 precedes b378 traffic. Cache once per simulation
       frame: drawing/interpolation must not consume the shared RNG again.
       Ordinary entry b6ec clears5f20; pause/result flows return above. */
    if(!simulation_error){
        native_effect_capture_camera=frame_camera;
        memcpy(native_effect_capture_records,native_actor_effects.pool.records,sizeof native_effect_capture_records);
        memcpy(native_effect_capture_generations,native_effect_generations,sizeof native_effect_generations);
        native_effect_capture_phase=car.world.speed_544;native_effect_capture_disabled=paused;
        native_effect_capture_random_before=passenger_scene.random_state;
        int32_t pitch_word=frame_camera.pitch;
        TaxiPoint effect_camera=frame_camera.position;
        taxi_random_trace_label("effect-sprite-projection");
        native_effect_request_count=taxi_actor_effects_sprites(&native_actor_effects,
            effect_camera,(int16_t)pitch_word,frame_camera.yaw,
            frame_camera.roll,&runtime.trig,192,120,80,car.world.speed_544,paused,native_effect_random,
            &passenger_scene.random_state,native_effect_requests);
        native_effect_capture_random_after=passenger_scene.random_state;
        taxi_random_trace_label(NULL);
        if(native_effect_request_count<0)simulation_error_at(__LINE__);
        else if(native_effect_request_count)native_effect_visible_frames++;
        for(int index=0;index<native_effect_request_count;index++){
            const TaxiEffectSprite *request=native_effect_requests+index;TaxiEffectFrame frame;
            if(!taxi_effect_asset_frame(&native_effect_asset,source_attract_ai!=0,
                request->group,request->frame,request->palette,&frame))simulation_error_at(__LINE__);
        }
        if(native_effect_trace){
            unsigned smoke=0;
            for(int e=0;e<native_effect_request_count;e++)if(native_effect_requests[e].flags&4)smoke++;
            fprintf(native_effect_trace,"%u,%u,%u,%u,%d,%d,%u,%d,%u\n",++native_effect_trace_tick,
                car.driver.power.gears.phase_e6a,drive_context.driver.gear.state_25d4,
                drive_context.driver.gear.state_2630,car.world.speed_544,
                (int)car.world.height.scale_e84,smoke,native_effect_request_count,native_effect_emissions);
        }
        unsigned active=0;
        for(unsigned index=0;index<9;index++)if(native_actor_effects.pool.records[index].bytes[0x34])active++;
        if(active>native_effect_max_active)native_effect_max_active=active;
    }
    if(traffic_preview&&passenger_diagnostic&&!simulation_error&&scene_index<2){
        uint8_t setting=passenger_scene.session.settings.bytes[3];
        traffic_submission_count=taxi_traffic_density(setting);
        native_effect_capture_camera=frame_camera;
        int32_t pitch_word=frame_camera.pitch;
        TaxiTrafficScene scene={.camera=frame_camera.position,
            .camera_yaw=frame_camera.yaw,.camera_pitch=pitch_word,
            .frame_delta=frame_delta,.global_delta=gameplay_global_delta,.paused=paused,.sound_enabled=1,
            .grid=&traffic_assets.grid,.roads=traffic_assets.roads,.trig=&runtime.trig,.angles=&runtime.traction,
            .globals=&traffic_globals,.timer_25d5=&traffic_timer,.selection_counter=&traffic_counter,
            .spacing=taxi_traffic_spacing(setting),.scales=traffic_scales,
            .obstacle=taxi_world_asset_obstacle,.height=taxi_world_asset_height,.terrain=&world,
            .submissions=traffic_submissions,.submission_count=50};
        taxi_random_trace_label("traffic-frame");
        int spawned=taxi_traffic_game_update(traffic_actors,traffic_submission_count,&scene,&car,&passenger_scene);
        taxi_random_trace_label(NULL);
        if(spawned<0)simulation_error_at(__LINE__);
        else traffic_spawned+=(unsigned)spawned;
        if(spawned>=0)for(unsigned i=0;i<traffic_submission_count;i++){
            TaxiTrafficActorEffects *effect=&traffic_submissions[i].effects;
            if(effect->effects.sound_table_index>=0)audio_request((unsigned)effect->effects.sound_table_index);
            for(unsigned k=0;k<effect->gears.count;k++)audio_request(effect->gears.indices[k]);
        }
        for(unsigned i=0;i<traffic_submission_count;i++)if(traffic_submissions[i].visible){++traffic_visible_frames;break;}
    }
    if(normal_reward_frame&&!simulation_error&&!start_active){
        TaxiPassengerFrameInput delivery={.taxi=car.world.height.position,.speed=car.world.speed_544,.delta=frame_delta,.material=car.world.material_e60,.camera_word_10=frame_camera.yaw,.freeze=paused,.cash_glyphs=&cash_glyphs};TaxiPassengerFrameEffects e={0};
        taxi_random_trace_label("passenger-delivery-after-traffic");
        if(taxi_passenger_scene_delivery_service_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,&delivery,&e,native_destination_selected,NULL))simulation_error_at(__LINE__);
        if(!passenger_scene.session.trip.active_1694)guidance_angle=-1;
        taxi_cash_glyphs_targets(&cash_glyphs,&passenger_scene.session.fare);taxi_cash_glyphs_step(&cash_glyphs);taxi_random_trace_label(NULL);
    }

    /* Original B470 advances engine+F4 from global3B00 before scene dispatch.
       Drawing and interpolation only read this retained simulation state. */
    if(!simulation_error&&scene_index>=0&&scene_index<2){
        TaxiGraphicsStorage *storage=&native_graphics_bootstrap.storage;
        uint32_t prior=storage->state_6904;
        storage->state_6904=taxi_material_phase_advance(prior,gameplay_global_delta,(uint8_t)paused);
        if(native_material_phase_trace)fprintf(native_material_phase_trace,"%u %ld %u %u\n",(unsigned)prior,(long)gameplay_global_delta,(unsigned)paused,(unsigned)storage->state_6904);
    }

}
/* Original opaque effects: exact cached requests and original asset/Q8 field.
   Flag4 forces8/8 BG composition only with qualified immutable capture. */
static unsigned native_effect_drawn,native_effect_blend_pending,native_effect_blended;
static void observe_effect_spawn(void *context,const TaxiEffectPool *pool,unsigned slot){
 (void)context;(void)pool;if(slot>=9||native_effect_generations[slot]==UINT64_MAX)fail("Effect presentation generation overflow.");native_effect_generations[slot]++;
}
static void drive_tick(TaxiXboxSample pad,int scripted){
 if(timestamped_presentation)taxi_effect_emit_observer(observe_effect_spawn,NULL);
 drive_tick_inner(pad,scripted);
 if(timestamped_presentation)taxi_effect_emit_observer(NULL,NULL);
}
static int diagnostic_hide_effect_pixels;
static void draw_native_effect(unsigned index,int32_t origin_x,int32_t origin_y,double pixel){
    if(diagnostic_hide_effect_pixels)return;
    const TaxiEffectSprite *request=shown_effect_requests()+index;
    if((request->flags&4)&&(!native_effect_background_valid||!native_effect_background_coverage_valid)){native_effect_blend_pending++;return;}
    TaxiEffectFrame frame={0};TaxiObjField field;int16_t matrix[4];
    if(taxi_owned_graphics&&!source_attract_ai){
        if(!effect_owned_visible[index])return;
        field=effect_owned_fields[index];memcpy(matrix,effect_owned_matrices[index],sizeof matrix);
        unsigned divisor=field.double_size?2:1;frame.width=(uint16_t)(field.width/divisor);frame.height=(uint16_t)(field.height/divisor);frame.bytes=(size_t)frame.width*frame.height*4;frame.rgba=effect_owned_pixels[index];
    }else{
        if(!taxi_effect_asset_frame(&native_effect_asset,source_attract_ai!=0,request->group,request->frame,request->palette,&frame))fail("Effect draw asset missing.");
        if(taxi_obj_field(request->position.x,request->position.y,frame.width,frame.height,request->scale,0,&field)||taxi_obj_matrix_values(0,256,request->scale,matrix))fail("Effect affine field invalid.");
    }
    uint8_t pixels[8192];
    if(frame.bytes>sizeof pixels)fail("Effect texture exceeds source frame bound.");
    taxi_palette_fade_cap_rgba(frame.rgba,pixels,(size_t)frame.width*frame.height,(int16_t)city_fade_cap);
    GLuint texture;glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,frame.width,frame.height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    if(!taxi_obj_shader_frame_begin(matrix,field.double_size,frame.width,frame.height,
        frame.width,frame.height,0,0,0))fail("Effect affine shader unavailable.");
    if((request->flags&4)&&!taxi_obj_shader_background(native_effect_background,
        (unsigned)native_effect_background_width,(unsigned)native_effect_background_height,0x1f4f,0x0808))
        fail("Original smoke BG composition unavailable.");
    double left=(field.left-origin_x)*pixel,top=(field.top-origin_y)*pixel;
    double right=left+field.width*pixel,bottom=top+field.height*pixel;
    if(jump_effect_render_check){
        static unsigned observations;
        if(observations++<4){
            GLdouble model[16],projection[16];GLint viewport[4],unit;
            glGetDoublev(GL_MODELVIEW_MATRIX,model);glGetDoublev(GL_PROJECTION_MATRIX,projection);glGetIntegerv(GL_VIEWPORT,viewport);glGetIntegerv(0x84e0,&unit);
            printf("EFFECT GPU TRANSFORM: index%u origin%d,%d pixel%.3f field%d,%d %ux%u double%u matrix%d,%d,%d,%d quad%.2f,%.2f..%.2f,%.2f modeldiag%.3f,%.3f translate%.3f,%.3f projection%.6f,%.6f viewport%d,%d,%d,%d activeunit%x.\n",index,origin_x,origin_y,pixel,field.left,field.top,field.width,field.height,field.double_size,matrix[0],matrix[1],matrix[2],matrix[3],left,top,right,bottom,model[0],model[5],model[12],model[13],projection[0],projection[5],viewport[0],viewport[1],viewport[2],viewport[3],(unsigned)unit);
        }
    }
    glBegin(GL_QUADS);
    glTexCoord2i(0,0);glVertex2d(left,top);
    glTexCoord2i(field.width,0);glVertex2d(right,top);
    glTexCoord2i(field.width,field.height);glVertex2d(right,bottom);
    glTexCoord2i(0,field.height);glVertex2d(left,bottom);
    glEnd();taxi_obj_shader_end();glDeleteTextures(1,&texture);if(request->flags&4)native_effect_blended++;else native_effect_drawn++;
}
static TaxiCameraView presented_camera_view(double alpha) {
    if(presented_pose_valid)return (TaxiCameraView){presented_pose.camera,(int16_t)(presented_pose.camera_angles.x/4096),(int16_t)(presented_pose.camera_angles.y/4096),(int16_t)(presented_pose.camera_angles.z/4096)};
    TaxiCameraOffsets previous=presentation_offsets(previous_camera_offsets),current=presentation_offsets(camera_offsets);
    if((previous_camera_mode_64c|live_camera_mode_64c)&16){
        uint8_t flags=car.world.camera_flags_29;
        TaxiCameraView before=taxi_camera_mode2_view(previous_position,previous_camera_pitch,previous_camera_heading,previous_camera_yaw_offset,&previous,camera_base,car.camera_offset_690,previous_camera_mode_64c,&flags,&runtime.trig);
        TaxiCameraView after=taxi_camera_mode2_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&current,camera_base,car.camera_offset_690,live_camera_mode_64c,&flags,&runtime.trig);
        TaxiCameraView view=after;view.position=taxi_render_point(before.position,after.position,alpha);
        view.pitch=(int16_t)(taxi_render_angle((int32_t)before.pitch*4096,(int32_t)after.pitch*4096,alpha)/4096);
        view.yaw=(int16_t)(taxi_render_angle((int32_t)before.yaw*4096,(int32_t)after.yaw*4096,alpha)/4096);
        return view;
    }
    TaxiCameraView before=taxi_camera_normal_view(previous_position,previous_camera_pitch,
        previous_camera_heading,previous_camera_yaw_offset,&previous,camera_base,&runtime.trig);
    TaxiCameraView after=taxi_camera_normal_view(car.world.height.position,camera_pitch,
        camera_heading,camera_yaw_offset,&current,camera_base,&runtime.trig);
    int32_t pitch=taxi_render_angle(previous_camera_pitch,camera_pitch,alpha);
    int32_t heading=taxi_render_angle(previous_camera_heading,camera_heading,alpha);
    int32_t offset=taxi_render_angle(previous_camera_yaw_offset*4096,camera_yaw_offset*4096,alpha)/4096;
    TaxiCameraView view=taxi_camera_normal_view(car.world.height.position,pitch,heading,
        offset,&current,camera_base,&runtime.trig);
    view.position=taxi_render_point(before.position,after.position,alpha);
    return view;
}
static void prepare_timestamped_pose(int focus){
 presented_pose_valid=0;if(!timestamped_presentation)return;
 if(camera_orbit!=0||camera_pitch_orbit!=0||distance!=3072*raw_scale){taxi_render_pose_clear(&presentation_history);return;}
 if(presentation_seen_revision!=host_reset_revision||presentation_seen_focus!=focus||presentation_seen_mode!=live_camera_mode_64c){taxi_render_pose_clear(&presentation_history);presentation_seen_updates=~0u;presentation_seen_revision=host_reset_revision;presentation_seen_focus=focus;presentation_seen_mode=live_camera_mode_64c;}
 uint64_t now=(uint64_t)live_host_time.counter*4389000000000ull/262144ull;
 if(!presentation_history.time.count||presentation_seen_updates!=live_host_updates){
  uint8_t flags=car.world.camera_flags_29;TaxiCameraView view=taxi_camera_mode2_view(car.world.height.position,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,car.camera_offset_690,live_camera_mode_64c,&flags,&runtime.trig);
  TaxiRenderPose pose={car.world.height.position,view.position,{(int16_t)car.world.angle_18*4096,car.world.heading_1c,(int16_t)car.world.angle_1a*4096},{view.pitch*4096,view.yaw*4096,view.roll*4096}};
  if(presentation_history.time.count){unsigned last=(presentation_history.time.begin+presentation_history.time.count-1)%TAXI_RENDER_HISTORY_CAPACITY;if(now<=presentation_history.time.stamps[last].time_ns)taxi_render_pose_clear(&presentation_history);}
  unsigned slot=(presentation_history.time.begin+presentation_history.time.count)%TAXI_RENDER_HISTORY_CAPACITY;presentation_offset_history[slot]=camera_offsets;
  memcpy(presentation_traffic_history[slot],traffic_actors,sizeof traffic_actors);memcpy(presentation_traffic_submissions[slot],traffic_submissions,sizeof traffic_submissions);presentation_traffic_counts[slot]=traffic_submission_count;
  presentation_passenger_history[slot]=passenger_scene;presentation_motion_headings[slot]=car.world.heading_20;
  presentation_cash_history[slot]=cash_glyphs;presentation_tip_frames[slot]=native_tip_frame;
  memcpy(presentation_panel_pixels[slot],native_panel_pixels,sizeof native_panel_pixels);presentation_panel_generations[slot]=native_panel_generation;
  presentation_guidance_angles[slot]=guidance_angle;presentation_turn_guidance_frames[slot]=turn_guidance_frame;
  if(native_effect_request_count<0||taxi_effect_presentation_slots(native_effect_capture_records,native_effect_capture_camera.position,native_effect_capture_camera.pitch,native_effect_capture_camera.yaw,native_effect_capture_camera.roll,&runtime.trig,native_effect_capture_phase,native_effect_requests,(unsigned)native_effect_request_count,presentation_effect_history[slot])<0)fail("Presentation effect cache association failed.");
  memcpy(presentation_effect_generations[slot],native_effect_capture_generations,sizeof native_effect_capture_generations);
  if(taxi_render_pose_push(&presentation_history,now,&pose))fail("Presentation pose history rejected timestamp.");presentation_seen_updates=live_host_updates;
 }
 uint64_t render_now=now+live_host_time.clock.phase/262144ull;
 uint64_t target=render_now>presentation_delay_ns?render_now-presentation_delay_ns:0;
 TaxiRenderBracket bracket;if(taxi_render_pose_sample(&presentation_history,target,&presented_pose,&bracket))fail("Presentation pose history sample failed.");presented_pose_valid=1;presented_bracket=bracket;presented_effect_count=0;
 /* Source frame, jitter and visibility remain discrete recorded choices.
    Never replay effect projection against the shared RNG during drawing. */
 for(unsigned n=0;n<9;n++){const TaxiEffectPresentationSlot *effect=presentation_effect_history[bracket.after_tag]+n;if(effect->visible){int blended=0;presented_effect_requests[presented_effect_count++]=taxi_effect_presentation_interpolate(presentation_effect_history[bracket.before_tag]+n,effect,presentation_effect_generations[bracket.before_tag][n],presentation_effect_generations[bracket.after_tag][n],bracket.alpha,&blended);if(bracket.before_tag!=bracket.after_tag){if(blended)timestamped_effect_blends++;else timestamped_effect_snaps++;}}}
 timestamped_effect_requests+=presented_effect_count;presented_motion_heading=taxi_render_angle(presentation_motion_headings[bracket.before_tag],presentation_motion_headings[bracket.after_tag],bracket.alpha);
 if(presentation_time_trace){uint64_t bt=presentation_history.time.stamps[bracket.before_tag].time_ns,at=presentation_history.time.stamps[bracket.after_tag].time_ns;/* tags are pose slots; find matching timestamp slots after ring wrap */
  for(unsigned n=0;n<presentation_history.time.count;n++){TaxiRenderStamp stamp=presentation_history.time.stamps[(presentation_history.time.begin+n)%TAXI_RENDER_HISTORY_CAPACITY];if(stamp.tag==bracket.before_tag)bt=stamp.time_ns;if(stamp.tag==bracket.after_tag)at=stamp.time_ns;}
  fprintf(presentation_time_trace,"%llu %llu %llu %llu %.17g %u %d %d %d %d %d %d %u %u\n",(unsigned long long)render_now,(unsigned long long)target,(unsigned long long)bt,(unsigned long long)at,bracket.alpha,bracket.held,presented_pose.taxi.x,presented_pose.taxi.y,presented_pose.taxi.z,presented_pose.camera.x,presented_pose.camera.y,presented_pose.camera.z,live_camera_mode_64c,live_host_updates);fflush(presentation_time_trace);}
}
/* Bounded offline-reference integration; expected poses validate native
   arithmetic and never supply a post-update taxi position. */
static void install_reference_driving(void){
 previous_position=car.world.height.position;previous_camera_pitch=camera_pitch;previous_camera_heading=camera_heading;previous_camera_yaw_offset=camera_yaw_offset;previous_camera_offsets=camera_offsets;
 car=reference_driving.car;drive_context=reference_driving.context;camera_pitch=reference_driving.pitch;camera_heading=reference_driving.heading;camera_yaw_offset=reference_driving.offset;camera_offsets=reference_driving.offsets;camera_base=reference_driving.base;live_camera_mode_64c=car.initial_camera_mode_64c;
}
static void commit_reference_driving(int focus){
 uint32_t observed=live_host_time.counter;
 while(reference_driving.next<reference_driving.count&&reference_driving.steps[reference_driving.next].tick<=observed){
  if(memcmp(&car,&reference_driving.car,sizeof car))fail("Reference fixture taxi was changed outside native driving.");
  if(taxi_reference_driving_step(&reference_driving,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world))fail("Native reference driving diverged or failed.");
  install_reference_driving();live_host_updates++;live_host_time.counter=(uint32_t)reference_driving.steps[reference_driving.next-1].tick;prepare_timestamped_pose(focus);
  unsigned slot=(presentation_history.time.begin+presentation_history.time.count-1)%TAXI_RENDER_HISTORY_CAPACITY;TaxiCameraView view=taxi_reference_driving_view(&reference_driving,&runtime.trig);TaxiRenderPose stored=presentation_history.poses[slot];
  if(memcmp(&stored.taxi,&car.world.height.position,sizeof stored.taxi)||memcmp(&stored.camera,&view.position,sizeof stored.camera)||stored.camera_angles.x!=view.pitch*4096||stored.camera_angles.y!=view.yaw*4096||stored.camera_angles.z!=view.roll*4096)fail("Reference camera/pose history association differs.");
  printf("REFERENCE DRIVE UPDATE: %u %u %d %d %d %d %d %d %d %d %d %d %d.\n",reference_driving.next,live_host_time.counter,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.heading_1c,car.world.speed_544,view.position.x,view.position.y,view.position.z,view.pitch,view.yaw,view.roll);
 }
 live_host_time.counter=observed;live_host_ready=0;
}
/* Silent diagnostic: failed voice channel is explicitly absent. Original
   HUD visibility hold/phase gates and source group0 labels retained. */
static void draw_challenge_jump_failure(int width,int height){
    if(!challenge_jump_drive_preview||!drive_context.height.flag_1697)return;
    if(!drive_context.height.flag_153c&&(challenge_jump_failure.phase16bc<=0||challenge_jump_failure.hold16be||challenge_failed_voice_busy()))return;
    if(!challenge_jump_end_font.rgba){
        char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
        char *tail=strrchr(path,92);if(!tail)fail("Jump font path failed.");
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/loading/font0-metrics.ctf");strcpy(metrics,path);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/loading/font0.rgba");
        if(taxi_font_asset_load(&challenge_jump_end_font,metrics,path,4,32,1))fail("Jump result font invalid.");
        glGenTextures(1,&challenge_jump_end_texture);
    }
    uint8_t pixels[153600]={0};const char *labels[]={"Failed!","Try Again?","Yes","No"};
    if(drive_context.height.flag_153c){
        TaxiChallengeJumpStrings strings;
        if(challenge_bound_drive_preview){if(taxi_challenge_bound_strings(&strings,native_settings,native_current_profile,live_bound_state.remaining_1660,"--"))fail("Bound result strings failed.");}
        else if(challenge_turn_drive_preview){if(taxi_challenge_turn_strings(&strings,native_settings,native_current_profile,live_turn_state.remaining_165c,"--"))fail("Turn result strings failed.");}
        else if(challenge_drift_drive_preview){if(taxi_challenge_drift_strings(&strings,native_settings,native_current_profile,(int16_t)drive_context.driver.motion.orientation.counter_164c,"--"))fail("Drift result strings failed.");}
        else if(challenge_ball_drive_preview){if(taxi_challenge_ball_strings(&strings,native_settings,native_current_profile,live_ball_state.remaining_1558,"--"))fail("Ball result strings failed.");}
        else if(challenge_flag_drive_preview){if(taxi_challenge_flag_strings(&strings,native_settings,native_current_profile,live_flag_timer.remaining_1552,"--"))fail("Flag result strings failed.");}
        else if(taxi_challenge_jump_strings(&strings,native_settings,native_current_profile,drive_context.height.distance_1540,"--"))fail("Jump result strings failed.");
        const char *headings[]={"Cleared!","Your Name","Result","Ranking","Record","Name"};
        const char *records[]={strings.records[0],strings.records[1],strings.records[2]},*names[]={strings.names[0],strings.names[1],strings.names[2]};
        if(taxi_challenge_success_text(pixels,&challenge_jump_end_font,headings,strings.name,strings.result,records,names,(unsigned)city_fade_cap))fail("Jump success text failed.");
    }else{
    if(taxi_challenge_failure_text(pixels,&challenge_jump_end_font,labels,challenge_jump_failure.selection153b,(unsigned)city_fade_cap))fail("Jump failure text failed.");
    }
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_TEXTURE_BIT|GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D,challenge_jump_end_texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    double scale=(double)height/160,left=(width-240*scale)/2;
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glColor4f(1,1,1,1);
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2d(left,0);glTexCoord2i(1,0);glVertex2d(left+240*scale,0);glTexCoord2i(1,1);glVertex2d(left+240*scale,height);glTexCoord2i(0,1);glVertex2d(left,height);glEnd();
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static GLuint taxi_owned_frame_texture(unsigned frame,const int32_t centers[2][2],int32_t depth,int32_t scale,uint16_t roll){
 if(!taxi_owned_package.bytes){char path[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,path,MAX_PATH);if(!n||n>=MAX_PATH)fail("Taxi package path failed.");char *tail=strrchr(path,92);if(!tail)fail("Taxi package path failed.");snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/taxis/graphics.ctg");if(taxi_graphics_package_load(&taxi_owned_package,path))fail("Taxi graphics package invalid.");live_taxi_registry.flags=4;live_taxi_owner=(TaxiOrdinaryGraphicsOwners){&live_taxi_assets,&live_taxi_registry,&live_taxi_banks,&live_taxi_fonts,&live_taxi_queue,&live_taxi_transport,live_taxi_base,live_taxi_matrices,taxi_owned_package.bytes,taxi_owned_package.size,0x06014000};if(taxi_ordinary_graphics_owned(&live_taxi_owner,1))fail("Ordinary taxi owner initialization failed.");}
 if(!taxi_owned_texture){glGenTextures(1,&taxi_owned_texture);live_taxi_cache=UINT32_MAX;}
 if(live_taxi_cached_driver!=native_game_driver){live_taxi_cache=UINT32_MAX;live_taxi_cached_driver=native_game_driver;}
 TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy dma={0};TaxiPaletteTransfer palette_dma={0};
 live_taxi_queue.total=0;live_taxi_queue.count[0]=live_taxi_queue.count[1]=live_taxi_queue.count[2]=0;live_taxi_transport.matrix_count=0;memset(live_taxi_transport.matrices,0,sizeof live_taxi_transport.matrices);memset(taxi_owned_visible,0,sizeof taxi_owned_visible);
 uint32_t left[3]={(uint32_t)centers[0][0],(uint32_t)centers[0][1],(uint32_t)depth},right[3]={(uint32_t)centers[1][0],(uint32_t)centers[1][1],(uint32_t)depth};unsigned angle=(unsigned)(0u-roll)&4095;
 if(taxi_render_owned(&live_taxi_owner,&copies,&dma,&palette_dma,&live_taxi_cache,frame,(uint8_t)native_game_driver,1,0,left,right,(uint32_t)scale,roll,runtime.trig.sine[angle],runtime.trig.cosine[angle],240,160,0))fail("Owned taxi rendering failed.");
 uint16_t emitted_attributes[11][3]={{0}};shared_owned_count=0;memset(effect_owned_visible,0,sizeof effect_owned_visible);
 unsigned tile=(live_taxi_assets.bases[1]-0x06010000u)/32;
 for(unsigned i=0;i<live_taxi_queue.count[2];i++){const uint16_t *a=live_taxi_queue.attributes[2][i];unsigned source_tile=a[2]&1023;if(source_tile<tile||(source_tile-tile)%64||(source_tile-tile)/64>1)fail("Owned taxi tile selection invalid.");unsigned half=(source_tile-tile)/64;int affine=!!(a[0]&256),doubled=affine&&!!(a[0]&512);taxi_owned_fields[half]=(TaxiObjField){(int16_t)live_taxi_queue.depth[i][0],(int16_t)live_taxi_queue.depth[i][1],doubled?128u:64u,doubled?128u:64u,doubled};taxi_owned_visible[half]=1;
  TaxiObjRequest *entry=shared_owned_requests+shared_owned_count++;memcpy(entry->anchor,live_taxi_queue.depth[i],sizeof entry->anchor);memset(entry->attributes,0,sizeof entry->attributes);entry->attributes[2]=(uint16_t)half;memcpy(emitted_attributes[half],a,6);
  if(affine){unsigned slot=(a[1]>>9)&31;memcpy(taxi_owned_matrices[half],live_taxi_transport.matrices[slot].values,sizeof taxi_owned_matrices[half]);}
  else{taxi_owned_matrices[half][0]=taxi_owned_matrices[half][3]=256;taxi_owned_matrices[half][1]=taxi_owned_matrices[half][2]=0;}
 }

 if(!source_attract_ai)for(unsigned e=0;e<shown_effect_count();e++){
  TaxiStartupObjQueued placed;int status=taxi_effect_render_owned(&live_taxi_owner,shown_effect_requests()+e,240,160,0,&placed);if(status<0)fail("Owned effect placement failed.");if(!status)continue;if(placed.bucket!=2)fail("Owned effect bucket unsupported.");
  unsigned divisor=placed.field.double_size?2:1;
  if(taxi_obj_tiles_rgba(effect_owned_pixels[e],sizeof effect_owned_pixels[e],&live_taxi_assets,&live_taxi_banks,live_taxi_owner.destination,live_taxi_queue.attributes[2][placed.index][2],placed.field.width/divisor,placed.field.height/divisor))fail("Owned effect pixel decode failed.");
  effect_owned_fields[e]=placed.field;effect_owned_visible[e]=1;const uint16_t *a=live_taxi_queue.attributes[2][placed.index];
  if(a[0]&256){unsigned slot=(a[1]>>9)&31;memcpy(effect_owned_matrices[e],live_taxi_transport.matrices[slot].values,sizeof effect_owned_matrices[e]);}else{effect_owned_matrices[e][0]=effect_owned_matrices[e][3]=256;effect_owned_matrices[e][1]=effect_owned_matrices[e][2]=0;}
  TaxiObjRequest *entry=shared_owned_requests+shared_owned_count++;memcpy(entry->anchor,live_taxi_queue.depth[placed.index],sizeof entry->anchor);memset(entry->attributes,0,sizeof entry->attributes);entry->attributes[2]=(uint16_t)(e+2);memcpy(emitted_attributes[e+2],a,6);
 }
 if(taxi_obj_sort(shared_owned_requests,shared_owned_count)||taxi_ordinary_objects_publish(&live_taxi_owner))fail("Owned object publication failed.");
 if(live_taxi_transport.previous_total!=shared_owned_count)fail("Owned published object count invalid.");
 for(unsigned i=0;i<shared_owned_count;i++){
  unsigned identity=shared_owned_requests[i].attributes[2];const uint16_t *a=live_taxi_transport.output[i];
  if(identity>=11||memcmp(a,emitted_attributes[identity],6))fail("Owned publication identity mismatch.");
  int16_t *matrix=identity<2?taxi_owned_matrices[identity]:effect_owned_matrices[identity-2];
  if(a[0]&256){unsigned slot=(a[1]>>9)&31;for(unsigned j=0;j<4;j++)matrix[j]=(int16_t)live_taxi_transport.output[slot*4+j][3];}
  else{matrix[0]=matrix[3]=256;matrix[1]=matrix[2]=0;}
 }
 if(taxi_source_capture_path){
  uint16_t taxi_oam[128][4];uint8_t layer[240*160*4];
  memcpy(taxi_oam,live_taxi_transport.output,sizeof taxi_oam);
  /* Keep matrix attr3 values, but exclude every non-taxi published object. */
  for(unsigned i=0;i<128;i++)if(i>=shared_owned_count||shared_owned_requests[i].attributes[2]>=2)taxi_oam[i][0]&=(uint16_t)~256u;
  if(taxi_oam_layer(layer,240,160,taxi_oam,&live_taxi_assets,&live_taxi_banks,live_taxi_owner.destination))fail("Native taxi OAM layer failed.");
  char path[MAX_PATH];FILE *capture;
  if(snprintf(path,sizeof path,"%s.oam-input.bin",taxi_source_capture_path)>=(int)sizeof path)fail("Taxi OAM capture path too long.");
  capture=fopen(path,"wb");if(!capture)fail("Taxi OAM input capture unavailable.");
  if(fwrite(taxi_oam,1,sizeof taxi_oam,capture)!=sizeof taxi_oam||fwrite(live_taxi_assets.objects,1,sizeof live_taxi_assets.objects,capture)!=sizeof live_taxi_assets.objects||fwrite(live_taxi_banks.objects,1,sizeof live_taxi_banks.objects,capture)!=sizeof live_taxi_banks.objects||fclose(capture))fail("Taxi OAM input capture failed.");
  if(snprintf(path,sizeof path,"%s.oam-layer.rgba",taxi_source_capture_path)>=(int)sizeof path)fail("Taxi layer capture path too long.");
  capture=fopen(path,"wb");if(!capture)fail("Taxi layer capture unavailable.");
  if(fwrite(layer,1,sizeof layer,capture)!=sizeof layer||fclose(capture))fail("Taxi layer capture failed.");
  if(snprintf(path,sizeof path,"%s.oam-layer.json",taxi_source_capture_path)>=(int)sizeof path)fail("Taxi layer metadata path too long.");
  capture=fopen(path,"wb");if(!capture)fail("Taxi layer metadata unavailable.");
  fprintf(capture,"{\"driver\":%u,\"frame\":%u,\"scale\":%d,\"depth\":%d,\"roll\":%u,\"sine\":%d,\"cosine\":%d,\"centers\":[[%d,%d],[%d,%d]],\"scope\":\"Native live published opaque affine taxi CPU layer, source240 diagnostic; scenery/shadows/other actors/HUD/priority/full original frame unverified\"}\n",native_game_driver,frame,scale,depth,(unsigned)roll,runtime.trig.sine[angle],runtime.trig.cosine[angle],centers[0][0],centers[0][1],centers[1][0],centers[1][1]);
  if(fclose(capture))fail("Taxi layer metadata close failed.");
  if(source_frame_cpu_requested){
   if(!source_frame_cpu_valid)fail("Combined CPU taxi frame has no scenery.");uint8_t combined[153600];
   for(unsigned i=0;i<38400;i++){uint16_t c=source_frame_cpu_palette[source_frame_cpu_indexed[i]];for(unsigned j=0;j<3;j++)combined[i*4+j]=(uint8_t)(((c>>(j*5))&31)*255/31);combined[i*4+3]=255;if(layer[i*4+3])memcpy(combined+i*4,layer+i*4,4);}
   if(snprintf(path,sizeof path,"%s.scene-shadow-taxi.rgba",taxi_source_capture_path)>=(int)sizeof path)fail("Combined CPU frame path too long.");capture=fopen(path,"wb");if(!capture)fail("Combined CPU frame unavailable.");if(fwrite(combined,1,sizeof combined,capture)!=sizeof combined||fclose(capture))fail("Combined CPU frame capture failed.");
  }

 }
 uint8_t pixels[32768];if(taxi_tiles_rgba(pixels,&live_taxi_assets,&live_taxi_banks,live_taxi_owner.destination,4))fail("Owned taxi pixels failed.");
 glBindTexture(GL_TEXTURE_2D,taxi_owned_texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);upload_source_sprite_bank(128,64,pixels,16);
 return taxi_owned_texture;
}
static void check_live_taxi_oam_pixels(void){
 if(!taxi_owned_graphics||!taxi_owned_texture||city_fade_cap!=31)fail("Live taxi pixel check requires rendered owned taxi and full palette.");
 uint16_t oam[128][4];uint8_t expected[240*160*4],actual[240*160*4];
 memcpy(oam,live_taxi_transport.output,sizeof oam);
 for(unsigned i=0;i<128;i++)if(i>=shared_owned_count||shared_owned_requests[i].attributes[2]>=2)oam[i][0]&=(uint16_t)~256u;
 if(taxi_oam_layer(expected,240,160,oam,&live_taxi_assets,&live_taxi_banks,live_taxi_owner.destination))fail("Live taxi CPU oracle failed.");
 GLint old_mode;glGetIntegerv(GL_MATRIX_MODE,&old_mode);glPushAttrib(GL_ALL_ATTRIB_BITS);
 glViewport(0,0,240,160);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_DITHER);glDisable(GL_SCISSOR_TEST);glDisable(GL_CULL_FACE);glDisable(GL_FOG);
 glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);glClearColor(17.0f/255,33.0f/255,49.0f/255,0);glClear(GL_COLOR_BUFFER_BIT);
 glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);glBindTexture(GL_TEXTURE_2D,taxi_owned_texture);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,240,160,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 for(int slot=(int)shared_owned_count-1;slot>=0;slot--){
  unsigned half=shared_owned_requests[slot].attributes[2];if(half>=2||!taxi_owned_visible[half])continue;
  TaxiObjField field=taxi_owned_fields[half];
  /* Production shader and production field/matrix/texture, source-size viewport. */
  if(!taxi_obj_shader_begin(taxi_owned_matrices[half],field.double_size,half,0))fail("Live taxi GPU shader failed.");
  glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(field.left,field.top);
  glTexCoord2i((int)field.width,0);glVertex2i(field.left+(int)field.width,field.top);
  glTexCoord2i((int)field.width,(int)field.height);glVertex2i(field.left+(int)field.width,field.top+(int)field.height);
  glTexCoord2i(0,(int)field.height);glVertex2i(field.left,field.top+(int)field.height);glEnd();taxi_obj_shader_end();
 }
 glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,actual);
 unsigned opaque=0;
 for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
  const uint8_t *want=expected+(y*240+x)*4,*got=actual+((159-y)*240+x)*4;const uint8_t backdrop[3]={17,33,49};
  if(want[3])opaque++;
  if(memcmp(got,want[3]?want:backdrop,3)){fprintf(stderr,"Live taxi pixel mismatch at%u,%u: GPU%u/%u/%u CPU%u/%u/%u coverage%u\n",x,y,got[0],got[1],got[2],want[0],want[1],want[2],want[3]);fail("Live taxi GPU/OAM CPU mismatch.");}
 }
 if(glGetError()!=GL_NO_ERROR||!opaque)fail("Live taxi GPU check invalid.");
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(old_mode);glPopAttrib();
 printf("PASS live taxi GPU/OAM CPU: all38400 RGB pixels exact, %u opaque; actualpublished owners/production shader, source240 isolatedtaxi, no shadows/scenepriority/fullframe/continuous4K proof.\n",opaque);
}

static void check_combined_source_gpu(void){
 if(!source_scene_gpu_requested)return;
 if(!source_scene_gpu_ready||!source_frame_cpu_valid||!taxi_owned_texture)fail("Combined GPU frame missing source owners.");
 uint16_t oam[128][4];uint8_t layer[153600],actual[153600];memcpy(oam,live_taxi_transport.output,sizeof oam);
 for(unsigned i=0;i<128;i++)if(i>=shared_owned_count||shared_owned_requests[i].attributes[2]>=2)oam[i][0]&=(uint16_t)~256u;
 if(taxi_oam_layer(layer,240,160,oam,&live_taxi_assets,&live_taxi_banks,live_taxi_owner.destination))fail("Combined GPU CPU taxi layer failed.");
 TaxiGenObjectsFn gen;TaxiDeleteObjectsFn del;TaxiBindObjectFn bind;TaxiAttachTextureFn attach;TaxiFramebufferStatusFn status;
 typedef void (APIENTRY *UseProgramFn)(GLuint);UseProgramFn use;
 TARGET_LOAD(gen,"glGenFramebuffers");TARGET_LOAD(del,"glDeleteFramebuffers");TARGET_LOAD(bind,"glBindFramebuffer");TARGET_LOAD(attach,"glFramebufferTexture2D");TARGET_LOAD(status,"glCheckFramebufferStatus");TARGET_LOAD(use,"glUseProgram");
 GLint old_fbo,old_mode,old_program;glGetIntegerv(0x8ca6,&old_fbo);glGetIntegerv(GL_MATRIX_MODE,&old_mode);glGetIntegerv(0x8b8d,&old_program);glPushAttrib(GL_ALL_ATTRIB_BITS);use(0);
 GLuint fbo,color,background;gen(1,&fbo);bind(0x8d40,fbo);glGenTextures(1,&color);glBindTexture(GL_TEXTURE_2D,color);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);attach(0x8d40,0x8ce0,GL_TEXTURE_2D,color,0);glDrawBuffer(0x8ce0);glReadBuffer(0x8ce0);if(status(0x8d40)!=0x8cd5)fail("Combined GPU target incomplete.");
 glViewport(0,0,240,160);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_DITHER);glDisable(GL_SCISSOR_TEST);glDisable(GL_CULL_FACE);glDisable(GL_FOG);glDisable(GL_ALPHA_TEST);glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,240,160,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 glGenTextures(1,&background);glBindTexture(GL_TEXTURE_2D,background);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,source_scene_gpu_pixels);
 /* Saved glReadPixels bytes are bottom-up. Upload and invert V exactly. */
 glBegin(GL_QUADS);glTexCoord2i(0,1);glVertex2i(0,0);glTexCoord2i(1,1);glVertex2i(240,0);glTexCoord2i(1,0);glVertex2i(240,160);glTexCoord2i(0,0);glVertex2i(0,160);glEnd();
 glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,.5f);glBindTexture(GL_TEXTURE_2D,taxi_owned_texture);
 for(int slot=(int)shared_owned_count-1;slot>=0;slot--){
  unsigned half=shared_owned_requests[slot].attributes[2];if(half>=2||!taxi_owned_visible[half])continue;
  TaxiObjField field=taxi_owned_fields[half];
  /* Production shader and production field/matrix/texture, source-size viewport. */
  if(!taxi_obj_shader_begin(taxi_owned_matrices[half],field.double_size,half,0))fail("Live taxi GPU shader failed.");
  glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(field.left,field.top);
  glTexCoord2i((int)field.width,0);glVertex2i(field.left+(int)field.width,field.top);
  glTexCoord2i((int)field.width,(int)field.height);glVertex2i(field.left+(int)field.width,field.top+(int)field.height);
  glTexCoord2i(0,(int)field.height);glVertex2i(field.left,field.top+(int)field.height);glEnd();taxi_obj_shader_end();
 }

 glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,actual);
 unsigned mismatch8=0,mismatch5=0,opaque=0;
 for(unsigned i=0;i<38400;i++){unsigned x=i%240,y=i/240;const uint8_t *a=actual+4*((159-y)*240+x);uint8_t want[3];uint16_t c=source_frame_cpu_palette[source_frame_cpu_indexed[i]];
  for(unsigned k=0;k<3;k++)want[k]=layer[i*4+3]?layer[i*4+k]:(uint8_t)(((c>>(5*k))&31)*255/31);
  opaque+=layer[i*4+3]!=0;int bad=0,bad5=0;for(unsigned k=0;k<3;k++){bad|=a[k]!=want[k];bad5|=(a[k]*31+127)/255!=(want[k]*31+127)/255;}mismatch8+=bad;mismatch5+=bad5;
 }
 char path[MAX_PATH];if(snprintf(path,sizeof path,"%s.combined-gpu.json",taxi_source_capture_path)>=(int)sizeof path)fail("Combined GPU report path failed.");FILE *out=fopen(path,"wb");if(!out)fail("Combined GPU report unavailable.");fprintf(out,"{\"status\":\"%s\",\"rgb8_mismatch_pixels\":%u,\"rgb555_mismatch_pixels\":%u,\"opaque_taxi_pixels\":%u,\"scope\":\"Actual production GPU background scenery shadow saved losslessly then production live taxi shader fields matrices texture; source240 only, other actors HUD priority continuous4K cadence unverified\"}\n",mismatch5?"MISMATCH_FOUND":"PASS_RGB555",mismatch8,mismatch5,opaque);if(fclose(out))fail("Combined GPU report close failed.");
 if(snprintf(path,sizeof path,"%s.combined-gpu.rgba",taxi_source_capture_path)>=(int)sizeof path)fail("Combined GPU pixels path failed.");out=fopen(path,"wb");if(!out||fwrite(actual,1,sizeof actual,out)!=sizeof actual||fclose(out))fail("Combined GPU pixels write failed.");
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(old_mode);bind(0x8d40,(GLuint)old_fbo);glDeleteTextures(1,&background);glDeleteTextures(1,&color);del(1,&fbo);glPopAttrib();use((GLuint)old_program);if(glGetError()!=GL_NO_ERROR)fail("Combined GPU restoration error.");printf("Combined GPU RGB555 mismatches%u opaque%u\n",mismatch5,opaque);
}

static void check_owned_sprite_lifecycle(void){
 if(!taxi_owned_graphics||!taxi_owned_texture)fail("Owned sprite lifecycle check requires rendered owned taxi.");
 unsigned saved_driver=native_game_driver,saved_count=sprite_source_count;
 int saved_requests=native_effect_request_count;native_effect_request_count=0;
 unsigned char expected[32768],actual[32768];FILE *files[4]={0};
 char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);
 if(!tail)fail("Owned sprite lifecycle asset path failed.");
 for(unsigned d=0;d<4;d++){snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/taxis/taxi-%u.rgba",d);files[d]=fopen(path,"rb");if(!files[d])fail("Owned sprite lifecycle source unavailable.");}
 const int32_t centers[2][2]={{106,102},{133,102}};
 for(unsigned i=0;i<2048;i++){
  /* First64 switch drivers with the SAME cached frame, then exercise all64 frames. */
  unsigned d=i<64?i%4:(i/64)%4,frame=i<64?0:i%64;
  native_game_driver=d;GLuint id=taxi_owned_frame_texture(frame,centers,3537,109,0);
  if(sprite_source_count!=saved_count)fail("Owned sprite upload grew source registry.");
  if(fseek(files[d],(long)frame*32768,SEEK_SET)||fread(expected,1,32768,files[d])!=32768)fail("Owned sprite lifecycle source read failed.");
  glBindTexture(GL_TEXTURE_2D,id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,actual);
  if(memcmp(expected,actual,32768))fail("Owned sprite cache driver/frame pixels mismatch.");
  unsigned matches=0;
  for(unsigned j=0;j<sprite_source_count;j++)if(sprite_sources[j].id==id){
   Texture *t=sprite_sources+j;matches++;
   if(t->width!=128||t->height!=64||t->pause_bank!=16||memcmp(t->pixels,expected,32768))fail("Owned sprite retained source mismatch.");
  }
  if(matches!=1)fail("Owned sprite has duplicate source records.");
 }
 for(unsigned d=0;d<4;d++)if(fclose(files[d]))fail("Owned sprite lifecycle source close failed.");
 native_game_driver=saved_driver;native_effect_request_count=saved_requests;
 if(glGetError()!=GL_NO_ERROR)fail("Owned sprite lifecycle GPU failure.");
 printf("PASS owned sprite lifecycle: 2048 uploads, stable%u retained textures, all4 driver source pixels and same-frame driver changes exact.\n",saved_count);
}
static int draw_source_taxi(int width,int height) {
    if(!recovered_camera||camera_orbit!=0||camera_pitch_orbit!=0||distance!=3072*raw_scale) return 0;
    double alpha=gameplay_render_alpha();
    TaxiPoint anchors[2]={previous_position,car.world.height.position};
    TaxiCameraOffsets offsets[2]={previous_camera_offsets,camera_offsets};
    if(presented_pose_valid){offsets[0]=presentation_offset_history[presented_bracket.before_tag];offsets[1]=presentation_offset_history[presented_bracket.after_tag];alpha=presented_bracket.alpha;}
    double scales[2];
    for(unsigned i=0;i<2;i++) {
        TaxiHalfPlan source_plan;
        if(taxi_half_plan(&source_plan,0,0,192,
            presentation_offsets(offsets[i]).distance_770,camera_base,0,256))return 1;
        scales[i]=source_plan.scale;
    }
    TaxiPoint anchor=presented_pose_valid?presented_pose.taxi:taxi_render_point(anchors[0],anchors[1],alpha);
    anchor.y-=192;
    TaxiCameraView presented_view=presented_camera_view(alpha);
    TaxiPoint eye=presented_view.position,projected;
    if(!taxi_camera_project(&projected,anchor,eye,presented_view.pitch,
        presented_view.yaw,presented_view.roll,192,&runtime.trig)||projected.z<=0)return 1;
    double pixel=(double)height/160;
    double x=width*0.5+(projected.x-120)*pixel;
    double y=projected.y*pixel;
    /* Source15586 reads camera498+e, written by154f8..5502. Reconstructing
       this integer angle from display yaw loses source ASR12 boundaries. */
    int32_t presented_vehicle_heading=presented_pose_valid?presented_pose.vehicle_angles.y:taxi_render_angle(previous_vehicle_heading,car.world.heading_1c,alpha);
    unsigned frame=taxi_sprite_frame(presented_view.yaw,presented_vehicle_heading);
    GLuint selected_taxi_texture=taxi_textures[frame];
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glTranslated(x,y,0);
    int32_t presented_roll=presented_pose_valid?presented_pose.vehicle_angles.z:taxi_render_angle((previous_body_roll&4095)*4096,
        (car.world.angle_1a&4095)*4096,alpha);
    /* Original two-entry equal-depth order. The continuous 60fps geometry is
       still a host adaptation; use recovered quantized centers for ordering. */
    int32_t obj_centers[2][2];
    unsigned obj_angle=(unsigned)(-(presented_roll/4096))&4095u;
    int32_t obj_scale=(int32_t)(scales[0]+(scales[1]-scales[0])*alpha);
    taxi_obj_half_centers(projected.x,projected.y,obj_scale,1,
        runtime.trig.sine[obj_angle],runtime.trig.cosine[obj_angle],obj_centers);
    if(taxi_source_capture_path){
        FILE *f=fopen(taxi_source_capture_path,"w");if(!f)fail("Taxi source capture unavailable.");
        fprintf(f,"{\"viewport\":[%d,%d],\"recovered_camera\":true,\"owned_graphics\":%s,\"alpha\":%.17g,\"vehicle\":[%ld,%ld,%ld,%ld,%ld],\"camera\":[%ld,%ld,%ld,%d,%d,%d],\"distance\":%ld,\"base\":%ld,\"frame\":%u,\"scale\":%ld,\"depth\":%ld,\"centers\":[[%ld,%ld],[%ld,%ld]]}",width,height,taxi_owned_graphics?"true":"false",alpha,(long)anchor.x,(long)(anchor.y+192),(long)anchor.z,(long)presented_vehicle_heading,(long)(presented_roll/4096),(long)eye.x,(long)eye.y,(long)eye.z,presented_view.pitch,presented_view.yaw,presented_view.roll,(long)(presentation_offsets(offsets[0]).distance_770/4096),(long)camera_base,frame,(long)obj_scale,(long)projected.z,(long)obj_centers[0][0],(long)obj_centers[0][1],(long)obj_centers[1][0],(long)obj_centers[1][1]);
        if(fclose(f))fail("Taxi source capture write failed.");
        taxi_source_capture_written=1;
    }
    if(taxi_owned_graphics)selected_taxi_texture=taxi_owned_frame_texture(frame,obj_centers,projected.z,obj_scale,(uint16_t)(presented_roll/4096));
    TaxiObjRequest half_requests[11]={0};
    for(unsigned h=0;h<2;h++){
        half_requests[h].anchor[0]=(int16_t)obj_centers[h][0];
        half_requests[h].anchor[2]=(int16_t)projected.z;
        half_requests[h].attributes[2]=(uint16_t)h;
    }
        /* B364 taxi then B368 effects, same source bucket2 and halfword anchors. */
    unsigned request_count=2;
    for(unsigned e=0;e<shown_effect_count();e++){
        const TaxiEffectSprite *effect=shown_effect_requests()+e;
        if(effect->bucket!=2)fail("Effect source bucket unsupported.");
        TaxiObjRequest *entry=half_requests+request_count++;
        entry->anchor[0]=(int16_t)effect->position.x;
        entry->anchor[1]=(int16_t)effect->position.y;
        entry->anchor[2]=(int16_t)effect->position.z;
        entry->attributes[2]=(uint16_t)(e+2);
    }
    if(taxi_owned_graphics&&!source_attract_ai){request_count=shared_owned_count;memcpy(half_requests,shared_owned_requests,request_count*sizeof *half_requests);}
    if(!(taxi_owned_graphics&&!source_attract_ai)&&taxi_obj_sort(half_requests,request_count))fail("Mixed OBJ sort invalid.");
    if(jump_effect_render_check){
        static unsigned order_observations;
        if(order_observations++<3){
            printf("LIVE OBJ ORDER: count%u reverse-draw IDs",request_count);
            for(int i=(int)request_count-1;i>=0;i--)printf(" %u",half_requests[i].attributes[2]);
            putchar('\n');
        }
    }
    glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);
    int16_t obj_matrix[4];
    taxi_obj_matrix_values(runtime.trig.sine[obj_angle],runtime.trig.cosine[obj_angle],(int16_t)obj_scale,obj_matrix);
    glBindTexture(GL_TEXTURE_2D,selected_taxi_texture);glColor4f(1,1,1,1);
    for(int slot=(int)request_count-1;slot>=0;slot--) {
                unsigned half=half_requests[slot].attributes[2];
        if(half>=2){draw_native_effect(half-2,projected.x,projected.y,pixel);continue;}
        glBindTexture(GL_TEXTURE_2D,selected_taxi_texture);
        if(!(taxi_diagnostic_half_mask&(1u<<half)))continue;
        TaxiObjField field;
        if(taxi_obj_field(obj_centers[half][0],obj_centers[half][1],64,64,
            (int16_t)obj_scale,(int16_t)obj_angle,&field))fail("Source taxi field invalid.");
        if(taxi_owned_graphics){if(!taxi_owned_visible[half])continue;field=taxi_owned_fields[half];memcpy(obj_matrix,taxi_owned_matrices[half],sizeof obj_matrix);}
        if(!taxi_obj_shader_begin(obj_matrix,field.double_size,half,0))fail("Native taxi affine shader unavailable.");
        double left=(field.left-projected.x)*pixel,top=(field.top-projected.y)*pixel;
        double right=left+field.width*pixel,bottom=top+field.height*pixel;
        glBegin(GL_QUADS);
        glTexCoord2i(0,0);glVertex2d(left,top);
        glTexCoord2i((int)field.width,0);glVertex2d(right,top);
        glTexCoord2i((int)field.width,(int)field.height);glVertex2d(right,bottom);
        glTexCoord2i(0,(int)field.height);glVertex2d(left,bottom);
        glEnd();taxi_obj_shader_end();
    }
    glEnable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_TEXTURE_2D);
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
    return 1;
}
static void draw_taxi(int width,int height) {
    if(draw_source_taxi(width,height)) return;
    float a=(float)gameplay_render_alpha(),point[3];
    int32_t prev[3]={previous_position.x,previous_position.y,previous_position.z};
    int32_t next[3]={car.world.height.position.x,car.world.height.position.y,car.world.height.position.z};
    for(int i=0;i<3;i++) point[i]=((float)((double)prev[i]+((double)next[i]-prev[i])*a)-raw_center[i])*raw_scale;
    point[0]=(float)taxi_view_x(point[0]);
    /* Original two affine half centres, adapted to native perspective.
       Pixel-scale quantization and exact OAM sampling remain unported. */
    unsigned frame=taxi_sprite_frame(taxi_view_sprite_angle(yaw),car.world.heading_1c);
    float sprite_scale=1;
    if(recovered_camera) {
        /* 80156d8..5700 uses target distance770 ASR12 minus base680,
           not projected taxi depth. Compensate the perspective billboard. */
        double denominator=floor((double)presentation_offsets(camera_offsets).distance_770/4096)-camera_base;
        GLdouble model[16];glGetDoublev(GL_MODELVIEW_MATRIX,model);
        double anchor_y=point[1]-192*raw_scale;
        double depth=-(model[2]*point[0]+model[6]*anchor_y+model[10]*point[2]+model[14]);
        if(denominator>0 && depth>0) {
            int32_t scale=(int32_t)(192*2048/denominator);
            if(scale==256) scale=257;
            sprite_scale=(float)(scale*depth/(192*2048*raw_scale));
        }
    }
    glPushMatrix();glTranslatef(point[0],point[1]-192*raw_scale,point[2]);
    if(recovered_camera) {glRotatef(-pitch,1,0,0);glRotatef(-yaw,0,1,0);}
    else {glRotatef(-yaw,0,1,0);glRotatef(-pitch,1,0,0);}
    /* Original screen-down rotation is -angle1a; OpenGL screen-up reverses it. */
    glRotatef((float)(car.world.angle_1a&4095)*360/4096,0,0,1);
    glScalef(sprite_scale,sprite_scale,1);
    glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    /* Player taxi is an OBJ sprite over the software-rendered world in the
       original. A 3D depth test incorrectly cuts off its below-ground anchor.
       Full original OBJ/BG priority and occlusion rules remain pending. */
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D,taxi_textures[frame]);glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
    for(unsigned half=0;half<2;half++) {
        TaxiSpriteBounds b=taxi_sprite_half_bounds(half);float u=half*0.5f;
        glTexCoord2f(u,1);glVertex3f(b.left*raw_scale,b.bottom*raw_scale,0);
        glTexCoord2f(u+0.5f,1);glVertex3f(b.right*raw_scale,b.bottom*raw_scale,0);
        glTexCoord2f(u+0.5f,0);glVertex3f(b.right*raw_scale,b.top*raw_scale,0);
        glTexCoord2f(u,0);glVertex3f(b.left*raw_scale,b.top*raw_scale,0);
    }
    glEnd();glEnable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_TEXTURE_2D);glPopMatrix();
}

static void draw_passengers(void) {
    static const unsigned starts[4]={0,1,5,9},counts[4]={1,4,4,9};
    glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    const TaxiPassengerScene *shown=presented_pose_valid?&presentation_passenger_history[presented_bracket.after_tag]:&passenger_scene;
    for(unsigned i=0;i<shown->count;i++) {
        uint8_t record[44];memcpy(record,shown->customers[i].bytes,44);
        if(presented_pose_valid){const uint8_t *old=presentation_passenger_history[presented_bracket.before_tag].customers[i].bytes;if(old[0]==record[0]&&old[1]==record[1]&&old[10]==record[10]){TaxiPoint a,b;memcpy(&a,old+16,12);memcpy(&b,record+16,12);TaxiPoint position=taxi_render_point(a,b,presented_bracket.alpha);memcpy(record+16,&position,12);}}
        const uint8_t *p=record;
        unsigned kind=p[1],stage=p[10],frame=p[8];
        if((p[0]&3)!=1||kind>=8||stage>=4||frame>=counts[stage]) continue;
        int32_t pos[3];memcpy(pos,p+16,12);
        glPushMatrix();glTranslatef((float)taxi_view_x((pos[0]-raw_center[0])*raw_scale),
            (pos[1]-raw_center[1])*raw_scale,(pos[2]-raw_center[2])*raw_scale);
        if(recovered_camera) {glRotatef(-pitch,1,0,0);glRotatef(-yaw,0,1,0);}
        else {glRotatef(-yaw,0,1,0);glRotatef(-pitch,1,0,0);}
        glBindTexture(GL_TEXTURE_2D,passenger_textures[kind*18+starts[stage]+frame]);
        float w=TAXI_PASSENGER_HALF_WIDTH*raw_scale,h=TAXI_PASSENGER_HEIGHT*raw_scale;
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);glVertex3f(-w,0,0);glTexCoord2f(1,1);glVertex3f(w,0,0);
        glTexCoord2f(1,0);glVertex3f(w,h,0);glTexCoord2f(0,0);glVertex3f(-w,h,0);
        glEnd();glPopMatrix();if(presented_pose_valid)timestamped_fallback_passenger_draws++;
    }
    glDisable(GL_ALPHA_TEST);glDisable(GL_TEXTURE_2D);
}

static int compare_faces(const void *a,const void *b) {
    return (int)(((const Face *)a)->flags&255)-(int)(((const Face *)b)->flags&255);
}
static void draw_tip_bitmap(unsigned kind,unsigned frame,int32_t image_x,int32_t image_y,double scale,double offset){
    if(kind>=3||frame>=16)fail("Invalid source tip bitmap address.");uint8_t rgba[1024];
        for(unsigned j=0;j<256;j++){uint8_t index=native_tip_indices[kind*4096+frame*256+j];uint16_t color=taxi_palette_fade_cap_rgb555(pause_scene_palette()[index],(int16_t)city_fade_cap);for(unsigned c=0;c<3;c++)rgba[j*4+c]=(uint8_t)(((color>>(5*c))&31)*255/31);rgba[j*4+3]=index?255:0;}
        glBindTexture(GL_TEXTURE_2D,native_tip_texture);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        double x=offset+image_x*scale,y=image_y*scale,w=16*scale;
        glBegin(GL_QUADS);glTexCoord2f(0,0);glVertex2d(x,y);glTexCoord2f(1,0);glVertex2d(x+w,y);glTexCoord2f(1,1);glVertex2d(x+w,y+w);glTexCoord2f(0,1);glVertex2d(x,y+w);glEnd();
}
static void draw_tip_images(int width,int height){
    if(!passenger_diagnostic||!native_tip_texture)return;
    TaxiTipHudRequest requests[10];int count=taxi_tip_hud_requests(shown_fare_session()->tips,presented_pose_valid?presentation_tip_frames[presented_bracket.after_tag]:native_tip_frame,0,requests);
    if(count<0)fail("Invalid source tip HUD class.");if(presented_pose_valid)timestamped_tip_requests+=(unsigned)count;
    const uint32_t bases[3]={0x080ea67c,0x080e9474,0x080e826c};double scale=height/160.0,offset=width/2.0-120*scale;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    for(int i=0;i<count;i++){
        unsigned kind=3,frame=0;for(unsigned j=0;j<3;j++)if(requests[i].image_address>=bases[j]&&requests[i].image_address<bases[j]+4096){kind=j;frame=(requests[i].image_address-bases[j])/256;break;}
        draw_tip_bitmap(kind,frame,requests[i].image_x,requests[i].image_y,scale,offset);double w=16*scale;
        uint32_t dimensions=taxi_text_measure(&font_metrics[0][0],224,requests[i].text,&font_kerning[0],1);
        int32_t text_x=requests[i].text_x*256-(int32_t)(dimensions>>16)*128,text_y=requests[i].text_y*256-(int32_t)(dimensions&65535)*128;
        for(unsigned j=0;requests[i].text[j];j++){
            unsigned glyph=(unsigned char)requests[i].text[j]-32,next=requests[i].text[j+1]?(unsigned char)requests[i].text[j+1]-32:0;if(glyph>=224)continue;
            const uint16_t *m=font_metrics[glyph];double left=offset+taxi_text_glyph_coordinate(text_x,(int16_t)m[2])*scale,top=taxi_text_glyph_coordinate(text_y,(int16_t)m[3])*scale;
            glBindTexture(GL_TEXTURE_2D,font_textures[requests[i].color*224+glyph]);glBegin(GL_QUADS);glTexCoord2f(0,0);glVertex2d(left,top);glTexCoord2f(1,0);glVertex2d(left+w,top);glTexCoord2f(1,1);glVertex2d(left+w,top+w);glTexCoord2f(0,1);glVertex2d(left,top+w);glEnd();
            text_x+=(int32_t)m[0]*8*256-taxi_text_correction_q8(&font_metrics[0][0],224,(uint16_t)glyph,(uint16_t)next,&font_kerning[0],1,256);
        }

    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

static void check_tip_bitmap_gpu(void){
 FILE *f=fopen("outputs/crazy-taxi-native/reports/tip-bitmap-oracles.bin","rb");if(!f)fail("Tip bitmap oracle missing.");uint32_t count;if(fread(&count,4,1,f)!=1||count!=104)fail("Invalid tip oracle cases.");
 uint8_t oracle[38400],pixels[38400*4];int saved_cap=city_fade_cap;city_fade_cap=31;
 glPushAttrib(GL_ALL_ATTRIB_BITS);glViewport(0,0,240,160);glDisable(GL_DEPTH_TEST);glDisable(GL_FOG);glDisable(GL_CULL_FACE);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glDisable(GL_DITHER);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,240,160,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 uint16_t bg=native_scene_palette[111];glClearColor(((bg&31)*255/31)/255.0f,(((bg>>5)&31)*255/31)/255.0f,(((bg>>10)&31)*255/31)/255.0f,1);
 for(unsigned n=0;n<count;n++){
  int32_t a[4];if(fread(a,4,4,f)!=4||fread(oracle,1,sizeof oracle,f)!=sizeof oracle)fail("Truncated tip bitmap oracle.");glClear(GL_COLOR_BUFFER_BIT);draw_tip_bitmap((unsigned)a[2],(unsigned)a[3],a[0],a[1],1,0);glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
   uint16_t color=native_scene_palette[oracle[y*240+x]];
   for(unsigned c=0;c<3;c++)if(abs((int)pixels[((159-y)*240+x)*4+c]-(int)(((color>>(5*c))&31)*255/31))>1)fail("Tip bitmap GPU/original pixel mismatch.");
  }
 }
 if(fgetc(f)!=EOF||ferror(f)||fclose(f)||glGetError()!=GL_NO_ERROR)fail("Tip bitmap oracle/read/render error.");city_fade_cap=saved_cap;glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
 printf("PASS tip bitmap GPU: %u original31EF8 indexed references / %u pixels; shared production bitmap draw, all48 class/frame assets, horizontal/bottom clipping and zero transparency; explicit current native palette, full HUD/font/order unverified.\n",count,count*38400);
}

static void draw_destination_panel(int width,int height) {
    int32_t x,y;
    const TaxiPassengerScene *shown=presented_pose_valid?&presentation_passenger_history[presented_bracket.after_tag]:&passenger_scene;
    const uint8_t *pixels=presented_pose_valid?presentation_panel_pixels[presented_bracket.after_tag]:native_panel_pixels;
    unsigned generation=presented_pose_valid?presentation_panel_generations[presented_bracket.after_tag]:native_panel_generation;
    if(!passenger_diagnostic || !generation ||
       !taxi_distance_display_position(&shown->session.distance_display,
           &shown->session.trip,&x,&y))return;
    uint8_t rgba[4096*4];
    for(unsigned i=0;i<4096;i++) {
        uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[pixels[i]],(int16_t)city_fade_cap);
        for(unsigned c=0;c<3;c++)rgba[i*4+c]=(uint8_t)(((color>>(c*5))&31)*255/31);
        rgba[i*4+3]=255; /* Source31ef8 caller chooses opaque bitmap branch. */
    }
    float scale=(float)height/160,offset=(float)width/2-120*scale;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT|GL_SCISSOR_BIT|GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glEnable(GL_TEXTURE_2D);
    if(!native_panel_texture)glGenTextures(1,&native_panel_texture);
    glBindTexture(GL_TEXTURE_2D,native_panel_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
    glEnable(GL_SCISSOR_TEST);glScissor((GLint)offset,0,(GLsizei)(240*scale),height);
    glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2f(offset+x*scale,y*scale);
    glTexCoord2f(1,0);glVertex2f(offset+(x+64)*scale,y*scale);
    glTexCoord2f(1,1);glVertex2f(offset+(x+64)*scale,(y+64)*scale);
    glTexCoord2f(0,1);glVertex2f(offset+x*scale,(y+64)*scale);
    glEnd();if(presented_pose_valid)timestamped_panel_draws++;glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void draw_guidance(int width,int height) {
    /* OriginalAFC6 skips1F060->206BC guidance whileattract1708enabled. */
    if(live_attract)return;
    const TaxiPassengerScene *shown=presented_pose_valid?&presentation_passenger_history[presented_bracket.after_tag]:&passenger_scene;
    int32_t shown_angle=presented_pose_valid?presentation_guidance_angles[presented_bracket.after_tag]:guidance_angle;
    unsigned shown_turn_frame=presented_pose_valid?presentation_turn_guidance_frames[presented_bracket.after_tag]:turn_guidance_frame;
    TaxiPoint shown_taxi=presented_pose_valid?presented_pose.taxi:car.world.height.position;
    if(!passenger_diagnostic||!shown->session.trip.active_1694||shown_angle<0)return;
    int32_t selected=shown->pickup.destination_index;
    if(selected<0||(unsigned)selected>=passenger_asset.destination_count)return;
    int32_t remaining=taxi_destination_distance(selected,passenger_asset.destinations[selected],
        shown_taxi.x,shown_taxi.z);
    unsigned palette=taxi_destination_palette(remaining),frame=(challenge_turn_drive_preview||challenge_bound_drive_preview)?shown_turn_frame:((unsigned)shown_angle>>7)&31;
    /* Original frame64x32 centered at viewportWidth/2, y=scale5310/16+1.
       Turn uses retained source frame stepping in phase0; ordinary target-frame
       presentation remains diagnostic. Current scale5310 is preserved here. */
    float scale=(float)height/160,zoom=(float)shown->session.trip.scale_5310/256;
    float cx=(float)width/2,cy=((int16_t)shown->session.trip.scale_5310/16+1)*scale;
    float hw=32*scale*zoom,hh=16*scale*zoom;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glColor4f(1,1,1,1);glBindTexture(GL_TEXTURE_2D,guidance_textures[palette*32+frame]);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2f(cx-hw,cy-hh);glTexCoord2f(1,0);glVertex2f(cx+hw,cy-hh);
    glTexCoord2f(1,1);glVertex2f(cx+hw,cy+hh);glTexCoord2f(0,1);glVertex2f(cx-hw,cy+hh);
    glEnd();if(presented_pose_valid)timestamped_guidance_draws++;glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

static void draw_time_expiry(int width,int height) {
    const TaxiFareSession *s=shown_fare_session();
    if(!passenger_diagnostic||!s->ended_1697||(s->settings.bytes[0]&224)==128)return;
    /* Original0801fd40..fdd2: three32x32 group7 centres spaced
       trunc(scale/8), sourcey80. English replacement table variant0.
       GL expands source pixels; original OAM inverse-matrix sampling pending. */
    int q=s->expiry.scale_16c0;
    if(q<=0)return;if(presented_pose_valid)timestamped_expiry_draws++;
    float pixel=(float)height/160,zoom=(float)q/256,half=16*pixel*zoom;
    float centre=(float)width/2,cy=80*pixel;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    for(int i=0;i<3;i++) {
        float cx=centre+(i-1)*(q/8)*pixel;
        glBindTexture(GL_TEXTURE_2D,expiry_textures[i]);glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(cx-half,cy-half);glTexCoord2f(1,0);glVertex2f(cx+half,cy-half);
        glTexCoord2f(1,1);glVertex2f(cx+half,cy+half);glTexCoord2f(0,1);glVertex2f(cx-half,cy+half);
        glEnd();
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void draw_flag_clock(int width,int height){
    if(!challenge_flag_drive_preview&&!challenge_ball_drive_preview&&!challenge_drift_drive_preview&&!challenge_turn_drive_preview&&!challenge_bound_drive_preview)return;
    static TaxiFontAsset label_font;
    static TaxiFontAsset font;static uint8_t positions[60],indicator[1440],warning_rgba[53248];static GLuint texture;static int checked;
    if(!font.rgba){
        char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);if(!tail)fail("Flag clock path failed.");
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/challenge-flag/flag-clock-metrics.ctf");strcpy(metrics,path);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/hud/clock.rgba");
        if(taxi_font_asset_load(&font,metrics,path,2,16,0))fail("Flag original clock font failed.");
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/challenge-flag/flag-hud-positions.bin");
        FILE *f=fopen(path,"rb");if(!f||fread(positions,1,60,f)!=60||fgetc(f)!=EOF)fail("Flag clock positions failed.");fclose(f);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/challenge-flag/flag-indicator.indices");
        f=fopen(path,"rb");if(!f||fread(indicator,1,1440,f)!=1440||fgetc(f)!=EOF)fail("Flag indicator source invalid.");fclose(f);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/challenge-flag/flag-clock-warning.rgba");
        f=fopen(path,"rb");if(!f||fread(warning_rgba,1,sizeof warning_rgba,f)!=sizeof warning_rgba||fgetc(f)!=EOF)fail("Flag warning glyph source invalid.");fclose(f);glGenTextures(1,&texture);
    }
    TaxiFlagHudPlan plan;uint8_t pixels[153600];TaxiFontAsset active_font=font;
    if(live_flag_warning_palette)active_font.rgba=warning_rgba;
    if(challenge_ball_drive_preview||challenge_drift_drive_preview){
        if(!label_font.rgba){
            char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);if(!tail)fail("Ball label path failed.");
            snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/loading/font0-metrics.ctf");strcpy(metrics,path);
            snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/loading/font0.rgba");
            if(taxi_font_asset_load(&label_font,metrics,path,4,32,1))fail("Ball label originalfont failed.");
        }
        TaxiBallHudPlan ball;
        int planned=challenge_drift_drive_preview?taxi_challenge_drift_hud_plan(&ball,live_drift_state.remaining_164e,(int16_t)drive_context.driver.motion.orientation.counter_164c,240,160,car.driver.power.gears.flags_4ac,0,positions,60):taxi_challenge_ball_hud_plan(&ball,live_ball_state.remaining_1558,live_ball_state.popped_1556,240,160,car.driver.power.gears.flags_4ac,0,positions,60);
        if(planned||
           taxi_challenge_ball_hud_pixels(pixels,&ball,&active_font)||taxi_challenge_ball_label_pixels(pixels,&ball,&label_font,challenge_drift_drive_preview?"Combos":"Balloons"))fail("Ball HUD failed.");
        plan=ball.clock;
    }else if(taxi_challenge_flag_hud_plan(&plan,challenge_bound_drive_preview?live_bound_state.remaining_1660:challenge_turn_drive_preview?live_turn_state.remaining_165c:live_flag_timer.remaining_1552,160,car.driver.power.gears.flags_4ac)||taxi_challenge_flag_hud_positions(&plan,positions,60)||taxi_challenge_flag_hud_pixels(pixels,&plan,&active_font))fail("Flag original countdown composition failed.");
    if(taxi_challenge_flag_indicator_pixels(pixels,&plan,indicator,sizeof indicator,native_scene_palette))fail("Flag indicator copy failed.");
    taxi_palette_fade_cap_rgba(pixels,pixels,38400,(int16_t)city_fade_cap);
    glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT|GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    if((challenge_ball_drive_preview||challenge_drift_drive_preview)&&!checked){
        uint8_t actual[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,actual);
        if(memcmp(actual,pixels,sizeof pixels))fail("Ball HUD GPU upload differs.");checked=1;
        if(challenge_drift_drive_preview)printf("PASS Drift HUD GPU upload: original clock/count15/Combos font0 label/gear; no independent GBA screen oracle.\n");
        else printf("PASS Ball HUD GPU upload: originalclock/counter/Balloons font0 label/opaquegear indicator, remaining%d popped%d warning%d. No independent GBA screen oracle.\n",live_ball_state.remaining_1558,live_ball_state.popped_1556,live_flag_warning_palette);
    }
    if(flag_scene_render_check&&!checked){uint8_t actual[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,actual);if(memcmp(actual,pixels,sizeof pixels))fail("Flag HUD GPU upload differs.");checked=1;printf("PASS Flag HUD GPU upload originalgroup3 countdown %s/remaining%d; sourceglyph positions and inverse scale128, warning%d, originalopaque indicator/gates integrated.\n",plan.time,live_flag_timer.remaining_1552,live_flag_warning_palette);}
    float scale=(float)height/160,left=(float)width/2-120*scale;
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glColor4f(1,1,1,1);
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2f(left,0);glTexCoord2i(1,0);glVertex2f(left+240*scale,0);glTexCoord2i(1,1);glVertex2f(left+240*scale,(float)height);glTexCoord2i(0,1);glVertex2f(left,(float)height);glEnd();
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void draw_main_clock(int width,int height) {
    if(!passenger_diagnostic)return;if(presented_pose_valid)timestamped_clock_draws++;
    /* Normal mode0802084a: decimal remaining ticks/60. 0801f3be..1f430
       places source group3 bank0 at x8+12*digit,y1. Other mode layouts pending. */
    char digits[16];
    snprintf(digits,sizeof digits,"%d",shown_fare_session()->clock.remaining_16a8/60);
    float scale=(float)height/160;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    for(unsigned i=0;digits[i];i++) {
        if(digits[i]<'0'||digits[i]>'9')continue;
        glBindTexture(GL_TEXTURE_2D,clock_textures[(unsigned char)digits[i]-32]);
        float x=(8+12*i)*scale,y=scale,w=16*scale;
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(x,y);glTexCoord2f(1,0);glVertex2f(x+w,y);
        glTexCoord2f(1,1);glVertex2f(x+w,y+w);glTexCoord2f(0,1);glVertex2f(x,y+w);
        glEnd();
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

static const GLuint *cash_owned_textures(const TaxiCashGlyphs *glyphs){
 static TaxiGraphicsPackage package;static TaxiTitleAssets assets;static TaxiPaletteRegistration registry;static TaxiPaletteBanks banks;static TaxiFontSetup fonts;static TaxiStartupObjQueue queue;static TaxiStartupObjTransport transport;static uint16_t base[128][4];static TaxiObjMatrixSlot matrices[32];static TaxiOrdinaryGraphicsOwners owner;static TaxiSpriteCopies copies;static TaxiSpriteImmediateCopy dma;
 if(!package.bytes){char path[MAX_PATH];DWORD size=GetModuleFileNameA(NULL,path,MAX_PATH);if(!size||size>=MAX_PATH)fail("Cash package path failed.");char *tail=strrchr(path,92);if(!tail)fail("Cash package path failed.");snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/taxis/graphics.ctg");if(taxi_graphics_package_load(&package,path))fail("Cash package invalid.");registry.flags=4;owner=(TaxiOrdinaryGraphicsOwners){&assets,&registry,&banks,&fonts,&queue,&transport,base,matrices,package.bytes,package.size,0x06014000};if(taxi_ordinary_graphics_owned(&owner,1)||taxi_hud_cash_palette_sources(&registry)||taxi_hud_cash_palette_init(&owner))fail("Cash owners invalid.");glGenTextures(19,cash_owned_slot_textures);printf("NATIVE CASH TILES: original queued group5/6 updates and blank clears,19 decoded slots; full frame service remains pending.\n");}
 /* Historical presentation has its own graphics copy state. Production
    frame-service publication must not overwrite the selected history slot. */
 if(copies.count||taxi_hud_cash_enqueue(&owner,&copies,&dma,glyphs))fail("Cash presentation enqueue failed.");
 uint8_t scratch[64]={0};TaxiSpriteCopySource source={0x03005400,scratch,sizeof scratch};
 TaxiSpriteCopyFlushOwner flush={owner.rom,owner.rom_size,assets.objects,sizeof assets.objects,owner.destination,&copies,&dma};
 if(taxi_sprite_copy_flush_sources_owned(&flush,&source,1))fail("Cash presentation publication failed.");
 uint8_t pixels[19][512];if(taxi_hud_cash_decode(pixels,&owner))fail("Native cash tile decoding failed.");
 for(unsigned slot=0;slot<19;slot++){glBindTexture(GL_TEXTURE_2D,cash_owned_slot_textures[slot]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);upload_source_sprite_bank(8,16,pixels[slot],slot<10?9:10);if(city_fade_cap!=31||pause_scene_palette()!=native_scene_palette){uint8_t displayed[512];if(pause_scene_palette()!=native_scene_palette){if(taxi_palette_gameplay_alternate_rgba(pixels[slot],displayed,128))fail("Cash alternate palette invalid.");taxi_palette_fade_cap_rgba(displayed,displayed,128,(int16_t)city_fade_cap);}else taxi_palette_fade_cap_rgba(pixels[slot],displayed,128,(int16_t)city_fade_cap);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,8,16,GL_RGBA,GL_UNSIGNED_BYTE,displayed);}}
 return cash_owned_slot_textures;
}
static void draw_cash(int width,int height) {
    if(!passenger_diagnostic)return;
    /* Source08010830 fixed-width grouping;0801f68e/762 source positions.
       Native current rows advance through original transition artwork. */
    const TaxiFareSession *shown=shown_fare_session();const TaxiFareState *fare=&shown->fare;
    const TaxiCashGlyphs *glyphs=presented_pose_valid?&presentation_cash_history[presented_bracket.after_tag]:&cash_glyphs;if(presented_pose_valid)timestamped_cash_draws++;
    const GLuint *owned_cash=cash_owned_textures(glyphs);
    float scale=(float)height/160;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    for(unsigned bank=0;bank<2;bank++) {
        unsigned slots=bank?9:10;
        const uint16_t *rows=bank?glyphs->pending:glyphs->total;
        /* Original English bitmap variant changes with fare active1694.
           Labels are opaque source blits; do not color-key bitmap index0. */
        unsigned active=shown->trip.active_1694?1:0;
        float lx=width-(bank?82:90)*scale,ly=bank?height-32*scale:2*scale;
        float lw=(bank?80:88)*scale,lh=(bank?32:24)*scale;
        glBindTexture(GL_TEXTURE_2D,cash_label_textures[bank*2+active]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(lx,ly);glTexCoord2f(1,0);glVertex2f(lx+lw,ly);
        glTexCoord2f(1,1);glVertex2f(lx+lw,ly+lh);glTexCoord2f(0,1);glVertex2f(lx,ly+lh);
        glEnd();
        float x=width-(bank?73:77)*scale,y=bank?height-20*scale:12*scale;
        for(unsigned i=0;i<slots;i++) {
            int frame=-1;
            if(i==(bank?6u:7u))frame=(bank?80:60)+i;
            else if(i==(bank?2u:3u)) {
                if((bank?fare->pending_whole_16a0:fare->total_whole_1698)>999)frame=(bank?80:60)+i;
            } else if(rows[i]<(bank?80:60))frame=rows[i];
            if(frame>=0) {
                glBindTexture(GL_TEXTURE_2D,owned_cash[(bank?10:0)+i]);
                glBegin(GL_QUADS);
                glTexCoord2f(0,0);glVertex2f(x,y);glTexCoord2f(1,0);glVertex2f(x+8*scale,y);
                glTexCoord2f(1,1);glVertex2f(x+8*scale,y+16*scale);glTexCoord2f(0,1);glVertex2f(x,y+16*scale);
                glEnd();
            }
            x+=((i==(bank?2:3)||i==(bank?6:7))?4:8)*scale;
        }
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

static void draw_trip_timer(int width,int height) {
    const TaxiTripStartState *trip=&shown_fare_session()->trip;
    if(!passenger_diagnostic||!trip->active_1694||!trip->time_16ac)return;
    /* Original801f196..34c projects the unshifted taxi position, scales its
       offset by projected depth, and draws font0 at256. */
    double alpha=gameplay_render_alpha(),pixel_scale=(double)height/160,x,y;
    if(presented_pose_valid){
        TaxiCameraView view=presented_camera_view(alpha);TaxiPoint screen;
        if(!taxi_camera_project(&screen,presented_pose.taxi,view.position,view.pitch,view.yaw,view.roll,192,&runtime.trig)||screen.z<=0)return;
        int32_t fixed=192*2048/screen.z;if(fixed==256)fixed=257;
        TaxiPoint offset=taxi_rotate_point((TaxiPoint){24*fixed,-32*fixed,0},0,0,-(presented_pose.vehicle_angles.z/4096),&runtime.trig);
        x=width*0.5+(screen.x+floor(offset.x/256.0)-120)*pixel_scale;
        y=(screen.y+floor(offset.y/256.0))*pixel_scale;
        timestamped_timer_draws++;
    }else if(recovered_camera&&camera_orbit==0&&camera_pitch_orbit==0&&distance==3072*raw_scale) {
        TaxiPoint positions[2]={previous_position,car.world.height.position};
        int32_t pitches[2]={previous_camera_pitch,camera_pitch};
        int32_t headings[2]={previous_camera_heading,camera_heading};
        TaxiCameraOffsets offsets[2]={previous_camera_offsets,camera_offsets};
        double xs[2],ys[2];
        for(unsigned i=0;i<2;i++) {
            TaxiCameraView view=taxi_camera_normal_view(positions[i],pitches[i],headings[i],
                i?camera_yaw_offset:previous_camera_yaw_offset,&offsets[i],camera_base,&runtime.trig);
            TaxiPoint screen;
            if(!taxi_camera_project(&screen,positions[i],view.position,
                view.pitch,view.yaw,view.roll,192,&runtime.trig)||screen.z<=0)return;
            int32_t fixed=192*2048/screen.z;if(fixed==256)fixed=257;
            TaxiPoint offset=taxi_rotate_point((TaxiPoint){24*fixed,-32*fixed,0},0,0,
                -(int32_t)car.world.angle_1a,&runtime.trig);
            xs[i]=screen.x+floor(offset.x/256.0);
            ys[i]=screen.y+floor(offset.y/256.0);
        }
        x=width*0.5+(xs[0]+(xs[1]-xs[0])*alpha-120)*pixel_scale;
        y=(ys[0]+(ys[1]-ys[0])*alpha)*pixel_scale;
    } else {
    double model[16],projection[16],position[4],eye[4]={0},clip[4]={0};
    glGetDoublev(GL_MODELVIEW_MATRIX,model);glGetDoublev(GL_PROJECTION_MATRIX,projection);
    position[0]=taxi_view_x((previous_position.x+((double)car.world.height.position.x-previous_position.x)*alpha-raw_center[0])*raw_scale);
    position[1]=(previous_position.y+((double)car.world.height.position.y-previous_position.y)*alpha-raw_center[1])*raw_scale;
    position[2]=(previous_position.z+((double)car.world.height.position.z-previous_position.z)*alpha-raw_center[2])*raw_scale;
    position[3]=1;
    for(unsigned r=0;r<4;r++)for(unsigned c=0;c<4;c++)eye[r]+=model[c*4+r]*position[c];
    for(unsigned r=0;r<4;r++)for(unsigned c=0;c<4;c++)clip[r]+=projection[c*4+r]*eye[c];
    if(clip[3]<=0||eye[2]>=0)return;
    double focal=recovered_camera?192:80/tan(25*3.141592653589793/180);
    int32_t fixed=(int32_t)(focal*2048/(-eye[2]/raw_scale));
    if(fixed==256)fixed=257;
    TaxiPoint offset=taxi_rotate_point((TaxiPoint){24*fixed,-32*fixed,0},0,0,-(int32_t)car.world.angle_1a,&runtime.trig);
    x=(clip[0]/clip[3]+1)*width/2+floor(offset.x/256.0)*pixel_scale;
    y=(1-clip[1]/clip[3])*height/2+floor(offset.y/256.0)*pixel_scale;
    }
    unsigned bank=trip->time_16ac>599?1:2;char text[16];snprintf(text,sizeof text,"%d",trip->time_16ac/60);
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    for(unsigned i=0;text[i];i++) {
        unsigned glyph=(unsigned char)text[i]-32,next=text[i+1]?(unsigned char)text[i+1]-32:0;
        if(glyph>=224)continue;
        const uint16_t *m=font_metrics[glyph];
        double left=x-(int16_t)m[2]*pixel_scale,top=y-(int16_t)m[3]*pixel_scale,w=16*pixel_scale;
        glBindTexture(GL_TEXTURE_2D,font_textures[bank*224+glyph]);glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2d(left,top);glTexCoord2f(1,0);glVertex2d(left+w,top);
        glTexCoord2f(1,1);glVertex2d(left+w,top+w);glTexCoord2f(0,1);glVertex2d(left,top+w);glEnd();
        x+=((int)m[0]*8-(next<224?(int)font_kerning[glyph*224+next]:0)+1)*pixel_scale;
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

static void pause_title_load(void){
    if(pause_title_textures[0])return;
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,92);if(!tail)fail("Pause asset path failed.");tail++;
    const char *names[3]={"main","restart","quit"};
    glGenTextures(9,pause_title_textures);
    for(unsigned mode=0;mode<3;mode++){
        snprintf(tail,(size_t)(path+MAX_PATH-tail),"../assets/pause/%s.rgba",names[mode]);
        FILE *f=fopen(path,"rb");unsigned char pixels[3*32*32*4];
        if(!f||fread(pixels,1,sizeof pixels,f)!=sizeof pixels)fail("Original pause art unavailable.");
        fclose(f);
        for(unsigned frame=0;frame<3;frame++){
            glBindTexture(GL_TEXTURE_2D,pause_title_textures[mode*3+frame]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels+frame*4096);
        }
    }
}
static const char *pause_label(unsigned id){
    switch(id){case 451:return "Resume";case 452:return "Restart";case 453:return "Quit";case 457:return "Yes";case 458:return "No";default:fail("Unknown original pause label.");return "";}
}
static void draw_pause_overlay(int width,int height){
    if(!native_pause_frame.draw_count)return;
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    pause_title_load();glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    double scale=height/160.0,offset=width/2.0-120*scale;
    for(unsigned i=0;i<native_pause_frame.draw_count;i++){
        const TaxiPauseRenderRequest *q=native_pause_frame.draw+i;
        if(!q->type){
            if(q->id>=3||native_pause.menu.dialog>2)fail("Pause title frame invalid.");
            double x=offset+q->x*scale,y=q->y*scale,w=32*scale;
            glBindTexture(GL_TEXTURE_2D,pause_title_textures[native_pause.menu.dialog*3+q->id]);
            glBegin(GL_QUADS);glTexCoord2f(0,0);glVertex2d(x,y);glTexCoord2f(1,0);glVertex2d(x+w,y);glTexCoord2f(1,1);glVertex2d(x+w,y+w);glTexCoord2f(0,1);glVertex2d(x,y+w);glEnd();
        }else{
            const char *text=pause_label(q->id);unsigned bank=q->group_or_palette;
            if(bank!=0&&bank!=2)fail("Pause glyph palette invalid.");
            uint32_t d=taxi_text_measure(&font_metrics[0][0],224,text,font_kerning,1);
            int32_t x=q->x*256-(int32_t)(d>>16)*128,y=q->y*256;
            for(unsigned j=0;text[j];j++){
                unsigned g=(unsigned char)text[j]-32,n=text[j+1]?(unsigned char)text[j+1]-32:0;
                const uint16_t *m=font_metrics[g];double left=offset+taxi_text_glyph_coordinate(x,(int16_t)m[2])*scale,top=taxi_text_glyph_coordinate(y,(int16_t)m[3])*scale,w=16*scale;
                glBindTexture(GL_TEXTURE_2D,font_textures[bank*224+g]);
                glBegin(GL_QUADS);glTexCoord2f(0,0);glVertex2d(left,top);glTexCoord2f(1,0);glVertex2d(left+w,top);glTexCoord2f(1,1);glVertex2d(left+w,top+w);glTexCoord2f(0,1);glVertex2d(left,top+w);glEnd();
                x+=(int32_t)m[0]*8*256-taxi_text_correction_q8(&font_metrics[0][0],224,(uint16_t)g,(uint16_t)n,font_kerning,1,256);
            }
        }
        pause_overlay_draws++;
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void pause_overlay_fixture(void){
    memset(&native_pause,0,sizeof native_pause);
    if(taxi_pause_runtime_frame(&native_pause,8,0,0,0,240,&native_pause_frame))fail("Pause preview begin failed.");
    for(unsigned i=0;i<pause_overlay_dialog;i++){
        if(taxi_pause_runtime_frame(&native_pause,0,0,0,0,240,&native_pause_frame)||taxi_pause_runtime_frame(&native_pause,128,0,0,0,240,&native_pause_frame))fail("Pause preview navigation failed.");
    }
    if(pause_overlay_dialog){
        if(taxi_pause_runtime_frame(&native_pause,0,0,0,0,240,&native_pause_frame)||taxi_pause_runtime_frame(&native_pause,1,0,0,0,240,&native_pause_frame))fail("Pause confirmation preview failed.");
    }
}

static void pause_consume_audio(const TaxiPauseEvents *events){
    if(events->pause_audio){taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_PAUSE);taxi_audio_settings_disable_effects(&audio_settings_state);}
    if(events->resume_audio){taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_RESUME);taxi_audio_settings_restore_effects(&audio_settings_state);}
}
static void check_pause_gameplay_freeze(void){
    if(!drive_mode||live_attract||challenge_jump_drive_preview||!passenger_diagnostic||!traffic_preview)fail("Pause check requires normal city gameplay with traffic.");
    for(unsigned i=0;i<400&&(live_game_start.active||city_fade.active_177b);i++)drive_tick((TaxiXboxSample){0},1);
    if(live_game_start.active||city_fade.active_177b||simulation_error)fail("Pause check startup gates did not clear.");
    memset(&native_pause,0,sizeof native_pause);
    if(taxi_pause_runtime_frame(&native_pause,8,city_fade.active_177b,(uint8_t)live_game_start.active,passenger_scene.session.ended_1697,240,&native_pause_frame)||!native_pause.menu.frozen)fail("Pause check Start did not pause.");
    pause_consume_audio(&native_pause_frame.events);
    if(music_tracker.state.playing||audio_settings_state.effects_enabled)fail("Pause audio owner flags remained enabled.");
    uint32_t before_material_phase=native_graphics_bootstrap.storage.state_6904;
    TaxiMusicState before_music=music_tracker.state;
    TaxiDriveWorld before=car;TaxiActorEffects before_effects=native_actor_effects;
    int32_t before_clock=passenger_scene.session.clock.remaining_16a8;
    TaxiPassengerMarkerPhase phase=passenger_scene.marker_phase;
    TaxiTrafficActor before_traffic[50];memcpy(before_traffic,traffic_actors,sizeof before_traffic);
    for(unsigned tick=0;tick<120;tick++){
        if(taxi_pause_runtime_frame(&native_pause,0,0,0,0,240,&native_pause_frame))fail("Paused input frame failed.");
        drive_tick((TaxiXboxSample){0},1);audio_frame();
        if(native_graphics_bootstrap.storage.state_6904!=before_material_phase)fail("Paused scenery animation phase changed.");
        if(memcmp(&before_music,&music_tracker.state,sizeof before_music))fail("Paused music tracker position changed.");
        if(simulation_error||memcmp(&car,&before,sizeof car)||memcmp(&native_actor_effects,&before_effects,sizeof before_effects)||passenger_scene.session.clock.remaining_16a8!=before_clock||memcmp(&phase,&passenger_scene.marker_phase,sizeof phase))fail("Paused taxi/clock/effect/marker state changed.");
        for(unsigned i=0;i<50;i++)if(traffic_actors[i].x!=before_traffic[i].x||traffic_actors[i].z!=before_traffic[i].z||traffic_actors[i].countdown!=before_traffic[i].countdown)fail("Paused traffic movement/spawn countdown changed.");
    }
    if(taxi_pause_runtime_frame(&native_pause,8,0,0,0,240,&native_pause_frame)||native_pause.menu.frozen||native_pause.alternate)fail("Pause resume did not restore source state.");
    pause_consume_audio(&native_pause_frame.events);
    if(!music_tracker.state.playing)fail("Resume music owner flag remained disabled.");
    drive_tick((TaxiXboxSample){0},1);
    if(simulation_error||passenger_scene.session.clock.remaining_16a8!=before_clock-1||!memcmp(&car,&before,sizeof car))fail("Resumed native gameplay did not advance.");
    puts("PASS native pause freeze: 120 frames preserve full taxi/effect state, clock, marker phase and traffic X/Z/spawn countdown; Start resumes clock/driving; music playback owner disables/resumes and tracker state stays fixed (snapshot audio device inactive). Diagnostic source-runtime inputs; full frame/palette/audio/camera and actual device flow unverified.");
}

static void detail_view(void) {
    for (uint32_t i=0;i<face_count;++i) {
        if(materials[faces[i].flags&255].flags&9) continue;
        if (materials[faces[i].flags&255].texture==255) continue;
        focus[0]=focus[1]=focus[2]=0;
        for (unsigned j=0;j<faces[i].count;++j) {
            const Vertex *v=&vertices[faces[i].index[j]];
            focus[0]+=v->x/faces[i].count;focus[1]+=v->y/faces[i].count;focus[2]+=v->z/faces[i].count;
        }
        distance=0.22f;pitch=32;yaw=-32;return;
    }
}

static void fail(const char *message) {
    fflush(stdout);fprintf(stderr, "%s\n", message);
    exit(1);
}

static void load_scene(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) fail("Cannot open extracted scene. Run tools/extract_scene.py first.");
    char magic[4];
    if (fread(magic, 1, 4, file) != 4 || (memcmp(magic, "CTM2", 4) && memcmp(magic,"CTM3",4) && memcmp(magic,"CTM4",4) && memcmp(magic,"CTM5",4) && memcmp(magic,"CTM6",4)) ||
        fread(&vertex_count, 4, 1, file) != 1 || fread(&face_count, 4, 1, file) != 1 ||
        fread(&texture_count,4,1,file)!=1 || fread(&material_count,4,1,file)!=1 ||
        !vertex_count || vertex_count > 65535 || !face_count || face_count > 12288 ||
        !texture_count || texture_count>256 || !material_count || material_count>512)
        fail("Invalid scene header.");
    if ((!memcmp(magic,"CTM4",4)||!memcmp(magic,"CTM5",4)||!memcmp(magic,"CTM6",4)) &&
        (fread(scene_clear,1,4,file)!=4 || scene_clear[3]!=255))
        fail("Invalid scene clear color.");
    if ((!memcmp(magic,"CTM5",4)||!memcmp(magic,"CTM6",4)) &&
        (fread(&sky_texture,4,1,file)!=1 || sky_texture>=texture_count))
        fail("Invalid sky texture reference.");
    if (!memcmp(magic,"CTM6",4) &&
        (fread(sky_fill,1,4,file)!=4 || sky_fill[3]!=255))
        fail("Invalid sky lower fill.");
    vertices = calloc(vertex_count, sizeof(*vertices));
    faces = calloc(face_count, sizeof(*faces));
    if (!vertices || !faces || fread(vertices, sizeof(*vertices), vertex_count, file) != vertex_count ||
        fread(faces, sizeof(*faces), face_count, file) != face_count)
        fail("Invalid scene payload.");
    materials=calloc(material_count,sizeof(*materials));textures=calloc(texture_count,sizeof(*textures));
    if (!materials || !textures) fail("Material allocation failed.");
    size_t material_bytes=memcmp(magic,"CTM2",4)?12:8;
    for(uint32_t i=0;i<material_count;i++)
        if(fread(&materials[i],1,material_bytes,file)!=material_bytes) fail("Invalid material payload.");
    for (uint32_t i=0;i<material_count;++i)
        if (materials[i].texture!=255 && materials[i].texture>=texture_count) fail("Invalid texture reference.");
    for (uint32_t i=0;i<texture_count;++i) {
        Texture *t=&textures[i];
        if (fread(&t->width,4,1,file)!=1 || fread(&t->height,4,1,file)!=1 ||
            !t->width || !t->height || t->width>256 || t->height>256) fail("Invalid texture dimensions.");
        size_t bytes=(size_t)t->width*t->height*4;
        t->pixels=malloc(bytes);
        if (!t->pixels || fread(t->pixels,1,bytes,file)!=bytes) fail("Invalid texture pixels.");
    }
    if (fgetc(file)!=EOF) fail("Trailing scene data.");
    fclose(file);
    for (uint32_t i=0; i<vertex_count; ++i)
        if (!isfinite(vertices[i].x) || !isfinite(vertices[i].y) || !isfinite(vertices[i].z)) fail("Invalid vertex.");
    for (uint32_t i=0; i<face_count; ++i) {
        if (faces[i].count != 3 && faces[i].count != 4) fail("Invalid polygon size.");
        if ((faces[i].flags&255)>=material_count) fail("Invalid material reference.");
        for (unsigned j=0;j<8;++j) if (!isfinite(faces[i].uv[j])) fail("Invalid texture coordinate.");
        for (unsigned j=0; j<faces[i].count; ++j)
            if (faces[i].index[j] >= vertex_count) fail("Invalid vertex reference.");
    }
    qsort(faces,face_count,sizeof(*faces),compare_faces);
}

static void fullscreen_toggle(HWND window) {
    if (!fullscreen) {
        GetWindowRect(window, &saved_rect);
        MONITORINFO monitor = {0}; monitor.cbSize = sizeof(monitor);
        GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor);
        SetWindowLongPtrW(window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(window, HWND_TOP, monitor.rcMonitor.left, monitor.rcMonitor.top,
                     monitor.rcMonitor.right-monitor.rcMonitor.left,
                     monitor.rcMonitor.bottom-monitor.rcMonitor.top, SWP_FRAMECHANGED);
    } else {
        SetWindowLongPtrW(window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPos(window, NULL, saved_rect.left, saved_rect.top,
                     saved_rect.right-saved_rect.left, saved_rect.bottom-saved_rect.top,
                     SWP_FRAMECHANGED | SWP_NOZORDER);
    }
    fullscreen = !fullscreen;
}

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w, LPARAM l) {
    switch (message) {
    case WM_CLOSE: PostQuitMessage(0); return 0;
    case WM_KEYDOWN:
        if (w == VK_ESCAPE) PostQuitMessage(0);
        if (w == VK_F11) fullscreen_toggle(window);
        /* Initialization observers must not recursively reset partially built
           driving state while servicing the native window. */
        if(loading_work_active)return 0;
        if (!drive_mode && w == 'W') wire_only = !wire_only;
        if (w == 'T') textured=!textured;
        if (!drive_mode && w == 'F') detail_view();
        if (drive_mode && w == 'R') {reset_drive();return 0;}
        if (drive_mode && w == 'C') {reset_camera_view();return 0;}
        if (drive_mode && w == 'V' && !(l&(1L<<30))) {recovered_camera=!recovered_camera;reset_camera_view();return 0;}
        if (w == 'R') { yaw=-32; pitch=62; distance=3.1f;focus[0]=focus[1]=focus[2]=0; }
        return 0;
    case WM_LBUTTONDOWN:
        dragging=1; last_x=GET_X_LPARAM(l); last_y=GET_Y_LPARAM(l); SetCapture(window); return 0;
    case WM_LBUTTONUP: dragging=0; ReleaseCapture(); return 0;
    case WM_RBUTTONDOWN: pan_drag=1;last_x=GET_X_LPARAM(l);last_y=GET_Y_LPARAM(l);SetCapture(window);return 0;
    case WM_RBUTTONUP: pan_drag=0;ReleaseCapture();return 0;
    case WM_CAPTURECHANGED: dragging=pan_drag=0; return 0;
    case WM_MOUSEMOVE:
        if (dragging) {
            if(drive_mode) camera_orbit += (GET_X_LPARAM(l)-last_x)*0.4f;
            else yaw += (GET_X_LPARAM(l)-last_x)*0.4f;
            if(drive_mode) {
                camera_pitch_orbit+=(GET_Y_LPARAM(l)-last_y)*0.4f;
                if(camera_pitch_orbit<-60) camera_pitch_orbit=-60;
                if(camera_pitch_orbit>60) camera_pitch_orbit=60;
            } else pitch += (GET_Y_LPARAM(l)-last_y)*0.4f;
            last_x=GET_X_LPARAM(l); last_y=GET_Y_LPARAM(l);
        }
        if (pan_drag) {
            float dx=(GET_X_LPARAM(l)-last_x)*distance*0.001f;
            float dy=(GET_Y_LPARAM(l)-last_y)*distance*0.001f;
            float angle=yaw*0.01745329252f;
            focus[0]-=dx*cosf(angle)+dy*sinf(angle);focus[2]-=dx*sinf(angle)-dy*cosf(angle);
            last_x=GET_X_LPARAM(l);last_y=GET_Y_LPARAM(l);
        }
        return 0;
    case WM_MOUSEWHEEL: distance *= GET_WHEEL_DELTA_WPARAM(w)>0 ? 0.9f : 1.1f; return 0;
    default: return DefWindowProcW(window, message, w, l);
    }
}

static void emit(uint16_t index) {
    const Vertex *v=&vertices[index];
    glVertex3f(drive_mode?(float)taxi_view_x(v->x):v->x,v->y,v->z);
}
static void textured_vertex(const Face *face,unsigned corner) {
    glTexCoord2fv(&face->uv[corner*2]);emit(face->index[corner]);
}


static int results_preview,results_ticks,results_flow_ticks=-1;
static int results_palette_cap=31,results_loaded,result_live_active,result_uploaded_cap=31;
static TaxiResultsFlow result_live_flow;
static Texture result_sources[1529];
static uint8_t result_customers,result_rank=99,result_class=8;
static TaxiResultsState result_state;
static char result_strings[440][32];
static GLuint result_sprite_ids[14];
static uint16_t result_sprite_metrics[14][9];
static TaxiFontAsset result_fonts[3];
static GLuint result_font_ids[3][672],result_background;
static void result_quad(GLuint id,double x,double y,double w,double h) {
    glBindTexture(GL_TEXTURE_2D,id);glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2d(x,y);glTexCoord2f(1,0);glVertex2d(x+w,y);
    glTexCoord2f(1,1);glVertex2d(x+w,y+h);glTexCoord2f(0,1);glVertex2d(x,y+h);glEnd();
}
static void asset_text(const TaxiFontAsset *f,const GLuint *ids,unsigned bank,unsigned align,const char *text,double x,double y,double pixel) {
    if(bank>=f->palettes)fail("Invalid results font palette.");
    double measure=0;
    for(unsigned i=0;text[i];i++) {
        unsigned g=(uint8_t)text[i]-32,next=text[i+1]?(uint8_t)text[i+1]-32:0;
        if(g<f->count)measure+=(int)f->metrics[g*9]*8-taxi_text_correction_q8(f->metrics,f->count,(uint16_t)g,(uint16_t)next,f->pairs,f->spacing,256)/256.0;
    }
    if(!(align&1)) {if(align&2)x-=measure*pixel;else if(align&4)x-=measure*pixel/2;}
    for(unsigned i=0;text[i];i++) {
        unsigned g=(uint8_t)text[i]-32,next=text[i+1]?(uint8_t)text[i+1]-32:0;
        if(g>=f->count)continue;
        const uint16_t *m=f->metrics+g*9;
        result_quad(ids[bank*f->count+g],x-(int16_t)m[2]*pixel,y-(int16_t)m[3]*pixel,32*pixel,32*pixel);
        x+=((int)m[0]*8-taxi_text_correction_q8(f->metrics,f->count,(uint16_t)g,(uint16_t)next,f->pairs,f->spacing,256)/256.0)*pixel;
    }
}

static int loading_preview=-1,loading_cap=31,loading_clear_index,loading_cache_check,loading_backdrop_explicit;
static uint8_t loading_image_source[4096],loading_sprite_source[20480],loading_animation_source[40960];
static uint16_t loading_palette_source[256];
static GLuint loading_ids[901];
static TaxiLoadingFlow startup_loading_flow;
static int loading_loaded,startup_loading_visible,startup_loading_snapshot,startup_clock_reset;
static HWND loading_window;static HDC loading_dc;
static unsigned startup_loading_rendered;
static TaxiFontAsset loading_font;
static void loading_read(const char *name,void *out,size_t size) {
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Loading asset path failed.");tail++;
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/loading/%s",name);
    FILE *f=fopen(path,"rb");
    if(!f||fread(out,1,size,f)!=size||fgetc(f)!=EOF||ferror(f))fail("Loading source asset invalid.");fclose(f);
}
static void loading_upload(GLuint id,unsigned w,unsigned h,const uint8_t *pixels) {
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
}
static void loading_cap_upload(int cap) {
    uint8_t background[153600],scratch[8192];
    taxi_loading_background(background,loading_image_source,loading_palette_source,(uint8_t)loading_clear_index,(int16_t)cap);
    loading_upload(loading_ids[0],240,160,background);
    for(unsigned i=0;i<3;i++) {
        taxi_palette_fade_cap_rgba(loading_sprite_source+i*4096,scratch,1024,(int16_t)cap);
        loading_upload(loading_ids[1+i],32,32,scratch);
    }
    taxi_palette_fade_cap_rgba(loading_animation_source+loading_preview*8192,scratch,2048,(int16_t)cap);
    loading_upload(loading_ids[4],64,32,scratch);
    for(unsigned i=0;i<896;i++) {
        taxi_palette_fade_cap_rgba(loading_font.rgba+i*4096,scratch,1024,(int16_t)cap);
        loading_upload(loading_ids[5+i],32,32,scratch);
    }
}
static void load_loading_preview(void) {
    if(loading_loaded)return;loading_loaded=1;
    uint8_t image[4096],background[153600],sprites[20480],animation[40960];uint16_t palette[256];
    if(!loading_backdrop_explicit) {uint8_t index;loading_read("backdrop-index.u8",&index,1);loading_clear_index=index;}
    loading_read("image.indices",image,sizeof image);loading_read("city-0-palette.rgb555",palette,sizeof palette);
    taxi_loading_background(background,image,palette,(uint8_t)loading_clear_index,(int16_t)loading_cap);
    loading_read("sprites.rgba",sprites,sizeof sprites);loading_read("startup-sprites.rgba",sprites,12288);loading_read("animation.rgba",animation,sizeof animation);
    memcpy(loading_image_source,image,sizeof image);memcpy(loading_palette_source,palette,sizeof palette);
    memcpy(loading_sprite_source,sprites,sizeof sprites);memcpy(loading_animation_source,animation,sizeof animation);
    taxi_palette_fade_cap_rgba(sprites,sprites,5120,(int16_t)loading_cap);
    taxi_palette_fade_cap_rgba(animation,animation,10240,(int16_t)loading_cap);
    glGenTextures(901,loading_ids);loading_upload(loading_ids[0],240,160,background);
    for(unsigned i=0;i<3;i++)loading_upload(loading_ids[1+i],32,32,sprites+i*4096);
    loading_upload(loading_ids[4],64,32,animation+loading_preview*8192);
    char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Loading font path failed.");tail++;
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/loading/font0-metrics.ctf");strcpy(metrics,path);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/loading/font0.rgba");
    if(taxi_font_asset_load(&loading_font,metrics,path,4,32,1)||loading_font.count!=224)fail("Loading original font invalid.");
    uint8_t scratch[4096];
    for(unsigned i=0;i<896;i++) {
        taxi_palette_fade_cap_rgba(loading_font.rgba+i*4096,scratch,1024,(int16_t)loading_cap);
        loading_upload(loading_ids[5+i],32,32,scratch);
    }
    if(loading_cache_check) {
        int caps[4]={0,15,31,31};uint8_t expected[153600],readback[153600];
        for(unsigned phase=0;phase<4;phase++) {
            loading_cap_upload(caps[phase]);
            for(unsigned i=0;i<901;i++) {
                size_t pixels=i==0?38400:i==4?2048:1024;
                if(!i)taxi_loading_background(expected,loading_image_source,loading_palette_source,(uint8_t)loading_clear_index,(int16_t)caps[phase]);
                else {
                    const uint8_t *source=i<4?loading_sprite_source+(i-1)*4096:i==4?loading_animation_source+loading_preview*8192:loading_font.rgba+(i-5)*4096;
                    taxi_palette_fade_cap_rgba(source,expected,pixels,(int16_t)caps[phase]);
                }
                glBindTexture(GL_TEXTURE_2D,loading_ids[i]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
                if(memcmp(expected,readback,pixels*4))fail("Loading texture source/fade restoration failed.");
            }
        }
        loading_cap_upload(loading_cap);
        puts("PASS loading cache: all901 GPU textures match immutable source at0/15/31/repeated31, requested cap restored.");
    }
    printf("LOADING PREVIEW: original image/font/sprites, bank%d cap%d explicit backdrop%d; diagnostic composition, startup work cadence pending.\n",loading_preview,loading_cap,loading_clear_index);
}
static void draw_loading_preview(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(loading_ids[0],left,0,240*pixel,height);
    asset_text(&loading_font,loading_ids+5,0,4,"Loading...",left+120*pixel,71*pixel,pixel);
    asset_text(&loading_font,loading_ids+5,0,4,"Please Wait",left+120*pixel,81*pixel,pixel);
    for(unsigned i=0;i<3;i++)result_quad(loading_ids[1+i],left+(72+32*i)*pixel,120*pixel,32*pixel,32*pixel);
    result_quad(loading_ids[4],left+87*pixel,7*pixel,64*pixel,32*pixel);
}

#include "normal_menu_view.inc"
#include "challenge_briefing_view.inc"
#include "game_start_view.inc"
static void result_text(unsigned slot,unsigned bank,unsigned align,const char *text,double x,double y,double pixel) {
    asset_text(result_fonts+slot,result_font_ids[slot],bank,align,text,x,y,pixel);
}
/* Source affine branch centers the image around the anchor. Scale256
   delegates to metric-offset non-affine drawing. GL sampling remains an
   approximation of the original inverse-matrix quantization. */
static void result_sprite(unsigned index,double x,double y,double pixel,int scale) {
    const uint16_t *m=result_sprite_metrics[index];double q=pixel*scale/256.0;
    double ox=scale==256?(int16_t)m[2]*pixel:m[0]*4*q;
    double oy=scale==256?(int16_t)m[3]*pixel:m[1]*4*q;
    result_quad(result_sprite_ids[index],x-ox,y-oy,m[0]*8*q,m[1]*8*q);
}
static void draw_results_preview(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(results_palette_cap*255/31/255.0f,results_palette_cap*255/31/255.0f,results_palette_cap*255/31/255.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(result_background,left,0,240*pixel,height);
    /* Source fresh-session fixture, not a completed driving-session result. */
    result_text(0,1,0,result_strings[331],left+4*pixel,6*pixel,pixel);
    result_text(2,0,0,result_strings[333],left+4*pixel,48*pixel,pixel);
    result_text(2,0,0,result_strings[334],left+16*pixel,95*pixel,pixel);
    result_text(1,1,4,result_strings[result_customers==1?439:332],left+180*pixel,14*pixel,pixel);
    char count[16];snprintf(count,sizeof(count),"%u",result_state.customers);
    result_text(1,1,4,count,left+180*pixel,6*pixel,pixel);
    result_sprite(native_game_driver,left+198*pixel,34*pixel,pixel,256);
    result_text(1,2,4,result_strings[176+6*native_game_driver],left+214*pixel,66*pixel,pixel);
    result_text(0,0,0,"$",left+20*pixel,72*pixel,pixel);
    const char *cash=result_state.cash;while(cash<result_state.cash+31&&!*cash)cash++;
    result_text(0,0,2,cash,left+180*pixel,72*pixel,pixel);
    result_text(2,0,2,result_strings[335],left+224*pixel,95*pixel,pixel);
    result_text(1,2,0,result_strings[336],left+52*pixel,143*pixel,pixel);
    if(result_state.phase>2) {
        int wide=result_class==0||result_class>8;
        result_sprite(4+result_class,left+(wide?64:32)*pixel,(wide?129:135)*pixel,pixel,result_state.class_scale);
    }
    if(result_state.phase>4)result_sprite(13,left+176*pixel,135*pixel,pixel,result_state.rank_scale);
    if(result_state.phase>5)result_text(0,2,4,result_strings[337+result_state.rank],left+176*pixel,128*pixel,pixel);
}
/* Immutable source copies allow fades and subsequent resets without
   compounding the RGB555 cap or reading a previously faded GPU texture. */
static void result_snapshot_cap(int cap) {
    if(cap==result_uploaded_cap) {results_palette_cap=cap;return;}
    unsigned char *pixels=malloc(240*160*4);if(!pixels)fail("Results fade allocation failed.");
    for(unsigned i=0;i<1529;i++) {
        Texture *t=result_sources+i;
        taxi_palette_fade_cap_rgba(t->pixels,pixels,t->width*t->height,(int16_t)cap);
        glBindTexture(GL_TEXTURE_2D,t->id);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,t->width,t->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    }
    free(pixels);results_palette_cap=result_uploaded_cap=cap;
}
static void result_live_reset(void) {
    profile_live_reset();
    result_live_active=0;game_over_active=0;name_live_active=0;
    if(endgame_diagnostic)results_preview=0;
    memset(&result_live_flow,0,sizeof result_live_flow);
}
static void load_results_preview(void) {
    if(results_loaded)return;
    char path[MAX_PATH],metric[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Results asset path failed.");tail++;
    for(unsigned slot=0;slot<3;slot++) {
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/font%u-metrics.ctf",slot);strcpy(metric,path);
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/font%u.rgba",slot);
        if(taxi_font_asset_load(result_fonts+slot,metric,path,slot==2?1:3,32,slot==1))fail("Results font loading failed.");
        TaxiFontAsset *f=result_fonts+slot;glGenTextures(f->count*f->palettes,result_font_ids[slot]);
        for(unsigned b=0;b<f->palettes;b++)for(unsigned g=0;g<f->count;g++) {
            glBindTexture(GL_TEXTURE_2D,result_font_ids[slot][b*f->count+g]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,taxi_font_asset_glyph(f,b,g));
        }
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/background.rgba");FILE *f=fopen(path,"rb");
    unsigned char *rgba=malloc(240*160*4);if(!f||!rgba)fail("Results background loading failed.");
    if(fread(rgba,1,240*160*4,f)!=240*160*4||fgetc(f)!=EOF||ferror(f))fail("Results background length invalid.");fclose(f);
    glGenTextures(1,&result_background);glBindTexture(GL_TEXTURE_2D,result_background);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);free(rgba);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/sprites.crs");f=fopen(path,"rb");
    unsigned char header[8],metrics[252];
    if(!f||fread(header,1,8,f)!=8||memcmp(header,"CRS1",4)||header[4]!=14||header[5]||header[6]||header[7]||fread(metrics,1,252,f)!=252||fgetc(f)!=EOF||ferror(f))fail("Results sprite metrics invalid.");fclose(f);
    for(unsigned i=0;i<14;i++)for(unsigned j=0;j<9;j++)result_sprite_metrics[i][j]=(uint16_t)(metrics[i*18+j*2]|metrics[i*18+j*2+1]<<8);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/sprites.rgba");f=fopen(path,"rb");if(!f)fail("Results sprites missing.");
    glGenTextures(14,result_sprite_ids);
    for(unsigned i=0;i<14;i++) {
        unsigned w=result_sprite_metrics[i][0]*8,h=result_sprite_metrics[i][1]*8;
        if(!w||!h||w>64||h>32)fail("Results sprite dimensions invalid.");
        rgba=malloc(w*h*4);if(!rgba||fread(rgba,1,w*h*4,f)!=w*h*4)fail("Results sprite pixels invalid.");
        glBindTexture(GL_TEXTURE_2D,result_sprite_ids[i]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);free(rgba);
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Results sprite length invalid.");fclose(f);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/text.crt");f=fopen(path,"rb");
    if(!f||fread(header,1,8,f)!=8||memcmp(header,"CRT1",4)||header[4]!=111||header[5]||header[6]||header[7])fail("Results text header invalid.");
    for(unsigned i=0;i<111;i++) {
        unsigned char row[4];if(fread(row,1,4,f)!=4)fail("Results text truncated.");
        unsigned id=row[0]|row[1]<<8,n=row[2]|row[3]<<8;
        if(id>=440||!n||n>=32||result_strings[id][0])fail("Results text record invalid.");
        if(fread(result_strings[id],1,n,f)!=n)fail("Results text truncated.");
        for(unsigned j=0;j<n;j++)if((uint8_t)result_strings[id][j]<32)fail("Results text byte invalid.");
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Results text length invalid.");fclose(f);
    for(unsigned i=331;i<=436;i++)if(!result_strings[i][0])fail("Missing results ordinal or label.");
    if(!result_strings[176][0]||!result_strings[182][0]||!result_strings[188][0]||!result_strings[194][0]||!result_strings[439][0])fail("Missing results driver or singular label.");
    for(unsigned i=0;i<1529;i++) {
        Texture *t=result_sources+i;t->width=t->height=32;
        if(i<1514) {
            unsigned slot=i<672?0:(i<1344?1:2),g=i-(slot==0?0:(slot==1?672:1344));
            t->id=result_font_ids[slot][g];
        } else if(i<1528) {
            unsigned index=i-1514;t->id=result_sprite_ids[index];t->width=result_sprite_metrics[index][0]*8;t->height=result_sprite_metrics[index][1]*8;
        } else {t->id=result_background;t->width=240;t->height=160;}
        t->pixels=malloc(t->width*t->height*4);if(!t->pixels)fail("Results source allocation failed.");
        glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,t->pixels);
    }
    results_loaded=1;
    if(taxi_results_state_begin_money(&result_state,0,0))fail("Results animation initialization failed.");
    for(int i=0;i<results_ticks;i++)taxi_results_state_step(&result_state,1,(int8_t)result_customers,(int8_t)result_rank,0);
    if(results_flow_ticks>=0) {
        TaxiResultsFlow flow;
        if(taxi_results_flow_begin(&flow,0,0,0,99,8,0,0))fail("Results flow initialization failed.");
        for(int i=0;i<results_flow_ticks;i++)taxi_results_flow_step(&flow,1,0);
        result_state=flow.animation;result_snapshot_cap(flow.palette_cap);
        printf("RESULTS FLOW FIXTURE: frame%d phase%u timer%d cap%d fade%u complete%u; retained interval explicitly0, post-results screens pending.\n",results_flow_ticks,result_state.phase,result_state.timer,flow.palette_cap,flow.fade.active_177b,flow.complete);
    }
    printf("RESULTS DIAGNOSTIC: original fresh-session zero earnings, customers0/class8/rank99; tick%d phase%u. GL affine approximation; gameplay transition pending.\n",results_ticks,result_state.phase);
}

static int game_over_preview=-1,game_over_loaded,game_over_cap=31,game_over_uploaded=31,game_over_complete;
static TaxiGameOver game_over_state;
static TaxiPaletteFade game_over_fade;
static TaxiFontAsset game_over_font;
static GLuint game_over_ids[96],game_over_background;
static unsigned char *game_over_bg;
static char game_over_message[17];
static void game_over_upload_cap(int cap) {
    game_over_cap=cap;if(game_over_uploaded==cap)return;
    unsigned char *pixels=malloc(256*256*4);if(!pixels)fail("Game Over fade allocation failed.");
    for(unsigned g=0;g<96;g++) {
        taxi_palette_fade_cap_rgba(taxi_font_asset_glyph(&game_over_font,0,g),pixels,32*32,(int16_t)cap);
        glBindTexture(GL_TEXTURE_2D,game_over_ids[g]);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,32,32,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    }
    taxi_palette_fade_cap_rgba(game_over_bg,pixels,256*256,(int16_t)cap);
    glBindTexture(GL_TEXTURE_2D,game_over_background);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,256,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    free(pixels);game_over_uploaded=cap;
}
static void game_over_begin_flow(uint8_t interval,uint8_t reload) {
    if(!game_over_loaded) {
        char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
        char *tail=strrchr(path,'\\');if(!tail)fail("Game Over asset path failed.");tail++;
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/ranking/font0-metrics.ctf");strcpy(metrics,path);
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/ranking/font0.rgba");
        if(taxi_font_asset_load(&game_over_font,metrics,path,1,32,0)||game_over_font.count!=96||game_over_font.spacing!=-2)fail("Game Over font invalid.");
        glGenTextures(96,game_over_ids);
        for(unsigned g=0;g<96;g++) {
            glBindTexture(GL_TEXTURE_2D,game_over_ids[g]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,taxi_font_asset_glyph(&game_over_font,0,g));
        }
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/ranking/background.rgba");FILE *f=fopen(path,"rb");
        game_over_bg=malloc(256*256*4);if(!f||!game_over_bg||fread(game_over_bg,1,256*256*4,f)!=256*256*4||fgetc(f)!=EOF||ferror(f))fail("Game Over background invalid.");fclose(f);
        glGenTextures(1,&game_over_background);glBindTexture(GL_TEXTURE_2D,game_over_background);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,game_over_bg);
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/ranking/game-over.got");f=fopen(path,"rb");unsigned char header[5];
        if(!f||fread(header,1,5,f)!=5||memcmp(header,"GOT1",4)||!header[4]||header[4]>16||fread(game_over_message,1,header[4],f)!=header[4]||fgetc(f)!=EOF||ferror(f))fail("Game Over text invalid.");fclose(f);
        game_over_loaded=1;
    }
    uint16_t widths[16]={0};unsigned count=(unsigned)strlen(game_over_message);
    for(unsigned start=0;start<count;start++) {
        int32_t width=0;
        for(unsigned i=start;i<count;i++) {
            unsigned g=(uint8_t)game_over_message[i]-32,next=i+1<count?(uint8_t)game_over_message[i+1]-32:0;
            if(g>=96)fail("Game Over glyph outside source font.");
            width+=(int)game_over_font.metrics[g*9]*8-taxi_text_correction_q8(game_over_font.metrics,96,(uint16_t)g,(uint16_t)next,NULL,game_over_font.spacing,256)/256;
        }
        widths[start]=(uint16_t)width;
    }
    if(taxi_game_over_begin(&game_over_state,count,240,widths))fail("Game Over animation initialization failed.");
    memset(&game_over_fade,0,sizeof game_over_fade);game_over_fade.interval_1771=interval;game_over_fade.reload_1772=reload;
    taxi_palette_fade_begin(&game_over_fade,1);game_over_complete=0;game_over_active=1;game_over_upload_cap(0);
}
static void game_over_tick_flow(void) {
    if(game_over_complete)return;
    if(taxi_game_over_step(&game_over_state,1,game_over_fade.active_177b))taxi_palette_fade_begin(&game_over_fade,0);
    game_over_upload_cap(game_over_fade.progress_178a);taxi_palette_fade_step(&game_over_fade,1);
    if(game_over_state.exiting&&!game_over_fade.active_177b) {
        game_over_complete=1;
        if(endgame_diagnostic&&native_settings_loaded)profile_live_begin(game_over_fade.interval_1771,game_over_fade.reload_1772);
    }
}
static void draw_game_over(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    unsigned char clear[4];taxi_palette_fade_cap_rgba(game_over_bg,clear,1,(int16_t)game_over_cap);
    glViewport(0,0,width,height);glClearColor(clear[0]/255.0f,clear[1]/255.0f,clear[2]/255.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D,game_over_background);glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2d(left,0);glTexCoord2f(240.0f/256,0);glVertex2d(left+240*pixel,0);
    glTexCoord2f(240.0f/256,160.0f/256);glVertex2d(left+240*pixel,height);glTexCoord2f(0,160.0f/256);glVertex2d(left,height);glEnd();
    for(unsigned i=0;i<game_over_state.count;i++) {
        if(i>game_over_state.glyph)break;
        int current=i==game_over_state.glyph;if(current&&game_over_state.scale<=0)continue;
        unsigned g=(uint8_t)game_over_message[i]-32;const uint16_t *m=game_over_font.metrics+g*9;
        double q=pixel*(current?game_over_state.scale:256)/256.0;
        double x=left+(current?game_over_state.current_x:game_over_state.position[i])*pixel/256;
        double y=(current?80:64)*pixel;
        double ox=current?m[0]*4*q:(int16_t)m[2]*pixel,oy=current?m[1]*4*q:(int16_t)m[3]*pixel;
        result_quad(game_over_ids[g],x-ox,y-oy,32*q,32*q);
    }
}
static int name_ring_preview=-1,name_ring_loaded,name_flight_preview=-1,name_flow_preview=-1,name_cap=31,name_uploaded_cap=31;
static Texture name_sources[1455];
static int name_interactive,name_input_check,post_results_check,post_register_check;
static uint8_t name_display_rank=1;
static TaxiNameFlow name_live_flow;
static TaxiMenuInput name_live_input;
static TaxiNameEntry name_preview_state;
static TaxiNameLayout name_ring_layout;
static TaxiFontAsset name_ring_font;
static uint8_t name_ring_characters[43];
static GLuint name_ring_ids[96],name_ring_bg;
static TaxiFontAsset name_label_fonts[2];
static GLuint name_label_ids[2][672],name_panel_ids[14];
static uint16_t name_panel_metrics[14][9];
static char name_labels[438][32];
static void name_upload_cap(int cap) {
    name_cap=cap;if(name_uploaded_cap==cap)return;
    unsigned char *pixels=malloc(240*160*4);if(!pixels)fail("Name fade allocation failed.");
    for(unsigned i=0;i<1455;i++) {
        Texture *t=name_sources+i;taxi_palette_fade_cap_rgba(t->pixels,pixels,t->width*t->height,(int16_t)cap);
        glBindTexture(GL_TEXTURE_2D,t->id);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,t->width,t->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    }
    free(pixels);name_uploaded_cap=cap;
}
static void load_name_ring_preview(void) {
    if(name_ring_loaded)return;
    char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Name ring asset path failed.");tail++;
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/layout.cnl");if(taxi_name_layout_load(&name_ring_layout,path))fail("Name ring layout invalid.");
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/name-characters.ctn");if(taxi_name_characters_load(name_ring_characters,path))fail("Name ring character table invalid.");
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/font2-metrics.ctf");strcpy(metrics,path);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/font2.rgba");
    if(taxi_font_asset_load(&name_ring_font,metrics,path,1,32,0)||name_ring_font.count!=96)fail("Name ring glyph art invalid.");
    glGenTextures(96,name_ring_ids);
    for(unsigned g=0;g<96;g++) {
        glBindTexture(GL_TEXTURE_2D,name_ring_ids[g]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,taxi_font_asset_glyph(&name_ring_font,0,g));
    }
    for(unsigned slot=0;slot<2;slot++) {
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/font%u-metrics.ctf",slot);strcpy(metrics,path);
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/font%u.rgba",slot);
        if(taxi_font_asset_load(name_label_fonts+slot,metrics,path,3,32,1)||name_label_fonts[slot].count!=224)fail("Name label font invalid.");
        glGenTextures(672,name_label_ids[slot]);
        for(unsigned b=0;b<3;b++)for(unsigned g=0;g<224;g++) {
            glBindTexture(GL_TEXTURE_2D,name_label_ids[slot][b*224+g]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,taxi_font_asset_glyph(name_label_fonts+slot,b,g));
        }
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/background.rgba");FILE *f=fopen(path,"rb");
    unsigned char *pixels=malloc(240*160*4);if(!f||!pixels||fread(pixels,1,240*160*4,f)!=240*160*4||fgetc(f)!=EOF||ferror(f))fail("Name ring background invalid.");fclose(f);
    glGenTextures(1,&name_ring_bg);glBindTexture(GL_TEXTURE_2D,name_ring_bg);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);free(pixels);name_ring_loaded=1;
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/sprites.crs");f=fopen(path,"rb");
    unsigned char header[8],sprite_bytes[252];
    if(!f||fread(header,1,8,f)!=8||memcmp(header,"CRS1",4)||header[4]!=14||header[5]||header[6]||header[7]||fread(sprite_bytes,1,252,f)!=252||fgetc(f)!=EOF||ferror(f))fail("Results sprite metrics invalid.");fclose(f);
    for(unsigned i=0;i<14;i++)for(unsigned j=0;j<9;j++)name_panel_metrics[i][j]=(uint16_t)(sprite_bytes[i*18+j*2]|sprite_bytes[i*18+j*2+1]<<8);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/sprites.rgba");f=fopen(path,"rb");if(!f)fail("Results sprites missing.");
    glGenTextures(14,name_panel_ids);
    for(unsigned i=0;i<14;i++) {
        unsigned w=name_panel_metrics[i][0]*8,h=name_panel_metrics[i][1]*8;
        if(!w||!h||w>64||h>32)fail("Results sprite dimensions invalid.");
        unsigned char *rgba=malloc(w*h*4);if(!rgba||fread(rgba,1,w*h*4,f)!=w*h*4)fail("Results sprite pixels invalid.");
        glBindTexture(GL_TEXTURE_2D,name_panel_ids[i]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);free(rgba);
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Results sprite length invalid.");fclose(f);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/name-entry/text.crt");f=fopen(path,"rb");
    if(!f||fread(header,1,8,f)!=8||memcmp(header,"CRT1",4)||header[4]!=105||header[5]||header[6]||header[7])fail("Name label header invalid.");
    for(unsigned i=0;i<105;i++) {
        unsigned char row[4];if(fread(row,1,4,f)!=4)fail("Name labels truncated.");
        unsigned id=row[0]|row[1]<<8,n=row[2]|row[3]<<8;
        if(id>=438||!n||n>=32||name_labels[id][0])fail("Name label record invalid.");
        if(fread(name_labels[id],1,n,f)!=n)fail("Name label truncated.");
        for(unsigned j=0;j<n;j++)if((uint8_t)name_labels[id][j]<32)fail("Name label byte invalid.");
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Name label length invalid.");fclose(f);
    for(unsigned i=337;i<=437;i++)if(!name_labels[i][0])fail("Missing name-entry ordinal/prompt.");
    if(!name_labels[176][0]||!name_labels[182][0]||!name_labels[188][0]||!name_labels[194][0])fail("Missing name-entry driver.");
    for(unsigned i=0;i<1455;i++) {
        Texture *t=name_sources+i;t->width=t->height=32;
        if(i<96)t->id=name_ring_ids[i];
        else if(i<1440)t->id=name_label_ids[(i-96)/672][(i-96)%672];
        else if(i<1454) {unsigned g=i-1440;t->id=name_panel_ids[g];t->width=name_panel_metrics[g][0]*8;t->height=name_panel_metrics[g][1]*8;}
        else {t->id=name_ring_bg;t->width=240;t->height=160;}
        t->pixels=malloc(t->width*t->height*4);if(!t->pixels)fail("Name original texture allocation failed.");
        glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,t->pixels);
    }
    puts("NAME RING DIAGNOSTIC: original background/layout/characters, selection/flight with original labels and panels; input/save screen pending. GL affine approximation.");
}
static void draw_name_ring(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(name_cap/31.0f,name_cap/31.0f,name_cap/31.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(name_ring_bg,left,0,240*pixel,height);
    /* Driver remains explicit Axel; live flow supplies the captured rank. */
    asset_text(name_label_fonts,name_label_ids[0],1,4,name_labels[437],left+120*pixel,6*pixel,pixel);
    asset_text(name_label_fonts+1,name_label_ids[1],2,4,name_labels[176],left+214*pixel,66*pixel,pixel);
    const unsigned panels[2]={0,13};const int anchors[2][2]={{198,34},{52,51}};
    for(unsigned i=0;i<2;i++) {
        unsigned index=panels[i];const uint16_t *m=name_panel_metrics[index];
        result_quad(name_panel_ids[index],left+(anchors[i][0]-(int16_t)m[2])*pixel,(anchors[i][1]-(int16_t)m[3])*pixel,m[0]*8*pixel,m[1]*8*pixel);
    }
    asset_text(name_label_fonts,name_label_ids[0],2,4,name_labels[337+name_display_rank],left+52*pixel,44*pixel,pixel);
    /* Original offscreen glyphs clip at the logical240-pixel viewport. */
    glEnable(GL_SCISSOR_TEST);glScissor((int)left,0,(int)(240*pixel),height);
    for(unsigned slot=0;slot<16;slot++) {
        TaxiNameRingGlyph ring;if(taxi_name_ring_glyph(&name_ring_layout,name_preview_state.selection,name_preview_state.angle,slot,160,&ring))fail("Name ring projection failed.");
        if(!ring.visible)continue;
        unsigned g=name_ring_characters[ring.character]-32;if(g>=96)fail("Name ring glyph outside source art.");
        const uint16_t *m=name_ring_font.metrics+g*9;double q=pixel*ring.scale/256.0;
        double ox=ring.affine?m[0]*4*q:(int16_t)m[2]*pixel,oy=ring.affine?m[1]*4*q:(int16_t)m[3]*pixel;
        result_quad(name_ring_ids[g],left+ring.x*pixel-ox,ring.y*pixel-oy,32*q,32*q);
    }
    /* Original accepted letters and the moving letter use font2 at scale512.
       Q8 positions come from the native source controller, not interpolation. */
    for(unsigned i=0;i<name_preview_state.length+(name_preview_state.flying?1u:0u);i++) {
        int flight=i==name_preview_state.length;
        unsigned ch=flight?name_ring_characters[name_preview_state.selection]:(uint8_t)name_preview_state.name[i];
        unsigned g=ch-32;if(g>=96)fail("Name flight glyph outside source art.");
        const uint16_t *m=name_ring_font.metrics+g*9;double q=pixel*2;
        double x=flight?name_preview_state.flight_x/256.0:name_preview_state.slots[i][0];
        double y=flight?name_preview_state.flight_y/256.0:name_preview_state.slots[i][1];
        result_quad(name_ring_ids[g],left+x*pixel-m[0]*4*q,y*pixel-m[1]*4*q,32*q,32*q);
    }
    glDisable(GL_SCISSOR_TEST);
}
static uint16_t name_device_mask(TaxiXboxSample pad) {
    uint16_t mask=pad.connected?(uint16_t)(~pad.active_low_keys&1023):0;
    if(GetAsyncKeyState('Z')&0x8000)mask|=1;
    if(GetAsyncKeyState('X')&0x8000)mask|=2;
    if(GetAsyncKeyState(VK_RETURN)&0x8000)mask|=8;
    if(GetAsyncKeyState(VK_RIGHT)&0x8000)mask|=16;
    if(GetAsyncKeyState(VK_LEFT)&0x8000)mask|=32;
    return mask;
}
static void name_interactive_tick(uint16_t mask) {
    if(game_over_active) {game_over_tick_flow();return;}
    taxi_name_flow_step(&name_live_flow,name_ring_characters,1,
        name_live_input.held[4],name_live_input.held[5],
        name_live_input.pressed[0]||name_live_input.pressed[3],name_live_input.pressed[1]);
    name_preview_state=name_live_flow.entry;name_upload_cap(name_live_flow.palette_cap);
    /* Source26318 samples the next request bank after consuming this one. */
    taxi_menu_input_step(&name_live_input,mask,1);
    if(name_live_flow.complete) {
        printf("NAME INTERACTIVE: accepted %u initials, moving to original Game Over; profile/save pending.\n",name_live_flow.entry.length);
        game_over_begin_flow(name_live_flow.fade.interval_1771,name_live_flow.fade.reload_1772);
    }
}
/* Native normal-mode endgame orchestration; ranking and complete original
   screen/audio scheduling remain unresolved. */
static int result_live_tick(TaxiXboxSample pad,int32_t frame_delta) {
    TaxiFareSession *session=&passenger_scene.session;
    if(profile_live_active) {
        uint16_t mask=startup_return_audio_smoke?return_profile_mask:name_device_mask(pad);
        if(!startup_return_audio_smoke&&(GetAsyncKeyState(VK_UP)&0x8000))mask|=64;
        if(!startup_return_audio_smoke&&(GetAsyncKeyState(VK_DOWN)&0x8000))mask|=128;
        if(pad.connected)mask|=(uint16_t)(~pad.active_low_keys)&192;
        profile_tick(mask);return 1;
    }
    if(game_over_active) {game_over_tick_flow();return 1;}
    if(name_live_active) {name_interactive_tick(name_device_mask(pad));return 1;}
    if(!result_live_active) {
        if(session->outer_state_1ea8!=2)return 0;
        if(city_fade.active_177b) {city_fade_tick(0,frame_delta);return 1;}
        /* Normal parent b5b0..b5cc stops music, clears outer/ended,
           sets main mode BEFORE results2392c, not after profile return. */
        music_tracker.state.playing=0;
        session->outer_state_1ea8=0;session->ended_1697=0;
        music_context.main_mode=1;
        load_results_preview();
        result_customers=session->end.completed_1692;result_rank=session->rank_16d4;result_class=session->fare.rating_1693;
        if(result_class>8||result_rank>99)fail("Live results source class/rank invalid.");
        if(taxi_results_flow_begin(&result_live_flow,session->fare.total_whole_1698,session->fare.total_fraction_169c,
            (int8_t)result_customers,(int8_t)result_rank,(int8_t)result_class,city_fade.interval_1771,city_fade.reload_1772))fail("Live results initialization failed.");
        result_state=result_live_flow.animation;result_snapshot_cap(0);results_preview=1;result_live_active=1;
        printf("LIVE RESULTS DIAGNOSTIC: captured native session cash%d.%02d customers%u class%u rank%u driver%u; recovered endgame flow, complete gameplay pending.\n",session->fare.total_whole_1698,session->fare.total_fraction_169c,result_customers,result_class,result_rank,native_game_driver);
        return 1;
    }
    taxi_results_flow_step(&result_live_flow,frame_delta,session->settings.bytes[7]);
    result_state=result_live_flow.animation;result_snapshot_cap(result_live_flow.palette_cap);
    if(result_live_flow.unlock_city)session->settings.bytes[7]=1;
    if(result_live_flow.complete) {
        if(taxi_name_entry_eligible(session->settings.bytes[0],result_rank)) {
            load_name_ring_preview();name_display_rank=result_rank;
            if(taxi_name_flow_begin(&name_live_flow,&name_ring_layout,160,result_live_flow.fade.interval_1771,result_live_flow.fade.reload_1772))fail("Live name entry begin failed.");
            memset(&name_live_input,0,sizeof name_live_input);name_preview_state=name_live_flow.entry;name_upload_cap(0);name_live_active=1;
            printf("POST-RESULTS DIAGNOSTIC: source eligible rank%u enters native name entry; selected driver retained, profile/save pending.\n",result_rank);
        } else {
            game_over_begin_flow(result_live_flow.fade.interval_1771,result_live_flow.fade.reload_1772);
            puts("POST-RESULTS DIAGNOSTIC: source name-entry eligibility skipped; native Game Over started. Profile/save screen pending.");
        }
    }
    return 1;
}
static int profile_preview=-1,profile_option,profile_loaded,profile_edit_column=-1;
static TaxiFontAsset profile_fonts[2];
static GLuint profile_font_ids[2][896],profile_background;
static char profile_labels[4][32],profile_prompts[2][32];
static GLuint profile_prompt_icons[2];
static int profile_interactive,profile_input_check,profile_commit_done,profile_persist_requested;
static uint8_t profile_extra_commit;
static TaxiProfileFlow profile_flow;
static TaxiMenuInput profile_input;
static uint8_t profile_names[2][3]={{'A','A','A'},{'A','A','A'}},profile_chars[43];
static Texture profile_sources[1571];
static unsigned profile_source_count;
static int profile_uploaded_cap=31;
static uint8_t *profile_background_phases;
static void profile_retain(GLuint id,const uint8_t *bytes,unsigned w,unsigned h) {
    if(profile_source_count>=1571)fail("Profile source cache overflow.");
    Texture *t=profile_sources+profile_source_count++;t->id=id;t->width=w;t->height=h;
    t->pixels=malloc((size_t)w*h*4);if(!t->pixels)fail("Profile source allocation failed.");
    memcpy(t->pixels,bytes,(size_t)w*h*4);
}
static void profile_cap(int cap) {
    if(cap==profile_uploaded_cap)return;
    for(unsigned i=0;i<profile_source_count;i++) {
        Texture *t=profile_sources+i;size_t n=(size_t)t->width*t->height;
        uint8_t *pixels=malloc(n*4);if(!pixels)fail("Profile fade allocation failed.");
        if(t->id==profile_background)memcpy(pixels,profile_background_phases+(size_t)cap*n*4,n*4);
        else taxi_palette_fade_cap_rgba(t->pixels,pixels,n,(int16_t)cap);
        glBindTexture(GL_TEXTURE_2D,t->id);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,t->width,t->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);free(pixels);
    }
    profile_uploaded_cap=cap;
}
static uint16_t challenge_profile_check_mask(void){
    static unsigned tick;
    if(!(tick++&1))return 0;
    if(challenge_jump_cross_profile_check&&!profile_flow.choice.editing&&profile_flow.choice.option!=1)return 128;
    if(challenge_jump_new_profile_check&&profile_flow.choice.editing&&profile_names[profile_flow.choice.current][0]=='A')return 64;
    if(challenge_jump_discard_check&&profile_flow.choice.option!=taxi_profile_option_count(profile_preview&1,profile_preview&2)-1)
        return 128;
    return 1;
}
static void profile_tick(uint16_t mask) {
    taxi_profile_flow_step(&profile_flow,profile_names,profile_chars,profile_preview&1,profile_preview&2,&profile_input,1);
    if(profile_live_active)native_current_profile=profile_flow.choice.current;
    profile_option=profile_flow.choice.option;
    profile_edit_column=profile_flow.choice.editing?profile_flow.choice.edit.column:-1;
    profile_cap(profile_flow.palette_cap);
    if(profile_live_active&&!profile_commit_done) {
        for(unsigned i=0;i<2;i++)memcpy(native_settings+8+i*208,profile_names[i],3);
        if(profile_flow.complete) {
            const TaxiFareSession *session=&passenger_scene.session;
            int status=profile_challenge_owner==6?taxi_challenge_profile_commit_bound(native_settings,session_settings_snapshot,&profile_flow.choice,drive_context.height.flag_153d,native_game_driver,live_bound_state.remaining_1660):profile_challenge_owner==5?taxi_challenge_profile_commit_turn(native_settings,session_settings_snapshot,&profile_flow.choice,drive_context.height.flag_153d,native_game_driver,live_turn_state.remaining_165c):profile_challenge_owner==4?taxi_challenge_profile_commit_drift(native_settings,session_settings_snapshot,
                &profile_flow.choice,drive_context.height.flag_153d,native_game_driver,(int16_t)drive_context.driver.motion.orientation.counter_164c):
                profile_challenge_owner==3?taxi_challenge_profile_commit_ball(native_settings,session_settings_snapshot,
                &profile_flow.choice,drive_context.height.flag_153d,native_game_driver,live_ball_state.remaining_1558):
                profile_challenge_owner==2?taxi_challenge_profile_commit_flag(native_settings,session_settings_snapshot,
                &profile_flow.choice,drive_context.height.flag_153d,native_game_driver,live_flag_timer.remaining_1552):
                profile_challenge_owner?taxi_challenge_profile_commit_jump(native_settings,session_settings_snapshot,
                &profile_flow.choice,drive_context.height.flag_153d,native_game_driver,drive_context.height.distance_1540):
                taxi_profile_commit_normal(native_settings,session_settings_snapshot,native_score_rows,
                &profile_flow.choice,result_rank,native_game_driver,result_class,profile_extra_commit,
                (const uint8_t *)name_live_flow.entry.name,session->fare.total_whole_1698,session->fare.total_fraction_169c);
            if(profile_challenge_owner&&status<0)fail("Challenge profile commit ownership invalid.");
            profile_commit_done=1;profile_persist_requested=status>0;
            native_save_write_status=0;
            if(profile_persist_requested&&native_save_path) {
                native_save_write_status=taxi_save_file_write(native_save_path,native_settings);
                if(native_save_write_status)fprintf(stderr,"PC SAVE: write failed; previous disk save retained.\n");
                else printf("PC SAVE: profile completion saved512 bytes.\n");
            }
            printf("LIVE PROFILE: commit status%d (selected driver retained), persistence request%d, configured path%d, write status%d.\n",status,profile_persist_requested,native_save_path!=NULL,native_save_write_status);
            if(status>=0)for(unsigned i=0;i<100;i++) {
                const uint8_t *p=native_score_rows+i*8+4;
                default_ranking_scores[i]=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
            }
        }
    }
    taxi_menu_input_step(&profile_input,mask,1);
    if(profile_challenge_owner&&profile_flow.complete&&profile_commit_done){profile_live_reset();profile_challenge_owner=0;challenge_jump_return_grid();}
}
static void load_profile_preview(void) {
    if(profile_loaded)return;
    char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Profile asset path failed.");tail++;
    for(unsigned slot=0;slot<2;slot++) {
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/font%u-metrics.ctf",slot);strcpy(metrics,path);
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/font%u.rgba",slot);
        TaxiFontAsset *font=profile_fonts+slot;
        if(taxi_font_asset_load(font,metrics,path,slot?3:4,32,1)||font->count!=224)fail("Profile font invalid.");
        glGenTextures(font->count*font->palettes,profile_font_ids[slot]);
        for(unsigned b=0;b<font->palettes;b++)for(unsigned g=0;g<font->count;g++) {
            glBindTexture(GL_TEXTURE_2D,profile_font_ids[slot][b*font->count+g]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,taxi_font_asset_glyph(font,b,g));
            profile_retain(profile_font_ids[slot][b*font->count+g],taxi_font_asset_glyph(font,b,g),32,32);
        }
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/background-prompts.rgba");FILE *f=fopen(path,"rb");
    unsigned char *pixels=malloc(240*160*4);if(!f||!pixels||fread(pixels,1,240*160*4,f)!=240*160*4||fgetc(f)!=EOF||ferror(f))fail("Profile background invalid.");fclose(f);
    glGenTextures(1,&profile_background);glBindTexture(GL_TEXTURE_2D,profile_background);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);profile_retain(profile_background,pixels,240,160);free(pixels);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/background-fade.rgba");f=fopen(path,"rb");
    size_t phase_bytes=32*240*160*4;profile_background_phases=malloc(phase_bytes);
    if(!f||!profile_background_phases||fread(profile_background_phases,1,phase_bytes,f)!=phase_bytes||fgetc(f)!=EOF||ferror(f))fail("Profile background phases invalid.");fclose(f);
    if(memcmp(profile_background_phases+31*240*160*4,profile_sources[1568].pixels,240*160*4))fail("Profile full-cap phase mismatch.");
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/text.cpr");f=fopen(path,"rb");unsigned char magic[4];
    if(!f||fread(magic,1,4,f)!=4||memcmp(magic,"CPR1",4))fail("Profile text header invalid.");
    for(unsigned i=0;i<4;i++) {
        unsigned char row[4];if(fread(row,1,4,f)!=4)fail("Profile text truncated.");
        unsigned id=row[0]|row[1]<<8,n=row[2]|row[3]<<8;
        if(id!=440+i||!n||n>=32||fread(profile_labels[i],1,n,f)!=n)fail("Profile text record invalid.");
        for(unsigned j=0;j<n;j++)if((uint8_t)profile_labels[i][j]<32)fail("Profile text byte invalid.");
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Profile text length invalid.");fclose(f);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/prompts.cpr");f=fopen(path,"rb");
    if(!f||fread(magic,1,4,f)!=4||memcmp(magic,"CPP1",4))fail("Profile prompts header invalid.");
    for(unsigned i=0;i<2;i++) {
        unsigned char row[4];if(fread(row,1,4,f)!=4)fail("Profile prompts truncated.");
        unsigned id=row[0]|row[1]<<8,n=row[2]|row[3]<<8;
        if(id!=(i?8:0)||!n||n>=32||fread(profile_prompts[i],1,n,f)!=n)fail("Profile prompt record invalid.");
        for(unsigned j=0;j<n;j++)if((uint8_t)profile_prompts[i][j]<32)fail("Profile prompt byte invalid.");
    }
    if(fgetc(f)!=EOF||ferror(f))fail("Profile prompts length invalid.");fclose(f);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/profile/prompts.rgba");f=fopen(path,"rb");
    unsigned char icons[4608];if(!f||fread(icons,1,sizeof icons,f)!=sizeof icons||fgetc(f)!=EOF||ferror(f))fail("Profile icons invalid.");fclose(f);
    glGenTextures(2,profile_prompt_icons);
    for(unsigned i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D,profile_prompt_icons[i]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,icons+i*1024);profile_retain(profile_prompt_icons[i],icons+i*1024,16,16);
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/results/name-characters.ctn");
    if(taxi_name_characters_load(profile_chars,path))fail("Profile characters invalid.");
    profile_loaded=1;
    printf("PROFILE DIAGNOSTIC: original BG/font/labels, existence flags%u option%d, explicit source-default AAA names; original Confirm prompt/normal-callback strip; edit/fade/live commit pending.\n",profile_preview,profile_option);
}
static void profile_live_begin(uint8_t interval,uint8_t reload) {
    if(profile_challenge_owner){
        /* Original24728..24738 registration entry restores menu order5. */
        music_context.main_mode=1;music_context.sound_active=0;music_test_start(4);
    }
    if(!profile_challenge_owner)memcpy(native_settings,passenger_scene.session.settings.bytes,8);
    profile_commit_done=0;profile_persist_requested=0;profile_extra_commit=0; /* source normal startupacae */
    profile_preview=(native_settings[11]?1:0)|(native_settings[219]?2:0);
    load_profile_preview();
    for(unsigned i=0;i<2;i++)memcpy(profile_names[i],native_settings+8+i*208,3);
    taxi_profile_flow_begin(&profile_flow,profile_preview&1,profile_preview&2,native_current_profile,interval,reload);
    memset(&profile_input,0,sizeof profile_input);profile_option=profile_flow.choice.option;profile_edit_column=-1;
    profile_live_active=1;profile_cap(0);
    printf("POST-GAME-OVER DIAGNOSTIC: original names/flags and retained current profile%u; initial startup choice remains provisional.\n",native_current_profile);
}
static void profile_live_reset(void) {
    if(profile_live_active) {profile_preview=-1;profile_live_active=0;profile_edit_column=-1;memset(&profile_input,0,sizeof profile_input);}
}
static void draw_profile_preview(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor((profile_interactive||profile_live_active)?profile_uploaded_cap/31.0f:1,(profile_interactive||profile_live_active)?profile_uploaded_cap/31.0f:1,(profile_interactive||profile_live_active)?profile_uploaded_cap/31.0f:1,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(profile_background,left,0,240*pixel,height);
    asset_text(profile_fonts,profile_font_ids[0],0,0,profile_labels[0],left+4*pixel,6*pixel,pixel);
    unsigned count=profile_preview?3:2;
    for(unsigned i=0;i<count;i++) {
        char label[64];
        if(i==count-1)snprintf(label,sizeof label,"%s",profile_labels[3]);
        else if((profile_preview==3)||((profile_preview!=0)&&i==0))snprintf(label,sizeof label,"%s %.3s",profile_labels[1],profile_names[profile_preview==3?i:((profile_preview&1)?0:1)]);
        else snprintf(label,sizeof label,"%s",profile_labels[2]);
        asset_text(profile_fonts+1,profile_font_ids[1],i==(unsigned)profile_option?1:0,4,label,left+120*pixel,(48+i*12)*pixel,pixel);
    }
    if(profile_edit_column>=0)for(unsigned i=0;i<3;i++) {
        /* Source24b0a..24c1e uses group0 glyph A-32, fixed16px
           anchors96/112/128,126; selected palette3 otherwise2. */
        const TaxiFontAsset *f=profile_fonts;unsigned glyph=(profile_interactive||profile_input_check||profile_live_active)?profile_names[profile_flow.choice.current][i]-32:'A'-32;
        const uint16_t *m=f->metrics+glyph*9;unsigned bank=i==(unsigned)profile_edit_column?3:2;
        result_quad(profile_font_ids[0][bank*f->count+glyph],left+(96+i*16-(int16_t)m[2])*pixel,(126-(int16_t)m[3])*pixel,32*pixel,32*pixel);
    }
    /* Source1ae50: group2 frame4 at(4,149), metric offsetY5;
       measured full tile width16 then text at20,148, font1 bank0. */
    result_quad(profile_prompt_icons[0],left+4*pixel,144*pixel,16*pixel,16*pixel);
    asset_text(profile_fonts+1,profile_font_ids[1],0,1,profile_prompts[0],left+20*pixel,148*pixel,pixel);
    if(profile_edit_column>=0) {
        const TaxiFontAsset *f=profile_fonts+1;int32_t advance_q8=0;
        for(unsigned i=0;profile_prompts[0][i];i++) {
            unsigned g=(uint8_t)profile_prompts[0][i]-32;
            unsigned next=profile_prompts[0][i+1]?(uint8_t)profile_prompts[0][i+1]-32:0;
            advance_q8+=(int32_t)f->metrics[g*9]*8*256-taxi_text_correction_q8(f->metrics,f->count,(uint16_t)g,(uint16_t)next,f->pairs,f->spacing,256);
        }
        /* fb ac packed measurement truncates Q8 before next icon anchor. */
        double x=20+advance_q8/256;
        result_quad(profile_prompt_icons[1],left+x*pixel,144*pixel,16*pixel,16*pixel);
        asset_text(f,profile_font_ids[1],0,1,profile_prompts[1],left+(x+16)*pixel,148*pixel,pixel);
    }
}
static int startup_mode,startup_check,startup_stage,startup_gameplay_check,startup_second_city,startup_expiry_check,startup_profile_check;
static int startup_warm_reload_check,startup_retry_input_check;
static int startup_driver_selection=-1;
static int startup_idle_check;
static int startup_parent_check;
enum { TAXI_STARTUP_TITLE_STAGE=13 };
static int startup_title_check,startup_title_interactive,startup_attract_check;
static void startup_attract_begin(void);
static TaxiTitleRuntime live_title;
static void title_live_begin(void);
static void title_live_upload(const TaxiTitleRuntimeOutput *output);
static unsigned startup_timed_check;
static void startup_drive_begin(void);
static void startup_loading_begin(void);
static int driver_preview=-1;
static int driver_preview_cap=31;
static GLuint driver_preview_ids[11];
static uint8_t driver_portrait_source[32768],driver_icon_source[2048];
static uint8_t driver_bg_indices[262144],driver_strip_indices[640],driver_bg_pixels[153600];
static uint16_t driver_bg_palette[16],driver_strip_palette[16];
static unsigned driver_scroll_x,driver_scroll_y;
static int driver_cache_check,driver_interactive,driver_input_check;
static TaxiDriverFlow driver_flow;
static TaxiMenuInput driver_input;
static void driver_cap_upload(int cap) {
    if(cap<0||cap>31)fail("Driver palette cap invalid.");
    if(taxi_driver_background(driver_bg_pixels,driver_bg_indices,driver_bg_palette,driver_strip_indices,driver_strip_palette,driver_scroll_x,driver_scroll_y,(unsigned)cap))fail("Driver native background composition failed.");
    profile_cap(cap);driver_preview_cap=cap;
    glBindTexture(GL_TEXTURE_2D,driver_preview_ids[0]);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,driver_bg_pixels);
    uint8_t pixels[4096];
    for(unsigned i=0;i<10;i++) {
        unsigned side=i<8?32:16;const uint8_t *source=i<8?driver_portrait_source+i*4096:driver_icon_source+(i-8)*1024;
        taxi_palette_fade_cap_rgba(source,pixels,side*side,cap);
        glBindTexture(GL_TEXTURE_2D,driver_preview_ids[i+1]);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,side,side,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    }
}
static void driver_tick(uint16_t mask) {
    taxi_driver_flow_step(&driver_flow,&driver_input,1);
    driver_preview=driver_flow.choice.driver;
    driver_scroll_x=(unsigned)(driver_flow.scroll_x_q8/256);
    driver_scroll_y=(unsigned)(driver_flow.scroll_y_q8/256);
    driver_cap_upload(driver_flow.palette_cap);
    taxi_menu_input_step(&driver_input,mask,1);
}
static void load_driver_preview(void) {
    if(!startup_mode)load_profile_preview(); /* Startup loads shared fonts through city first. */
    profile_cap(driver_preview_cap);
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Driver preview asset path failed.");tail++;
    const char *files[4]={"background.indices","background.rgb555","footer.indices","footer.rgb555"};
    void *dest[4]={driver_bg_indices,driver_bg_palette,driver_strip_indices,driver_strip_palette};
    const size_t sizes[4]={262144,32,640,32};FILE *f;
    for(unsigned i=0;i<4;i++) {
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/driver-selection/%s",files[i]);f=fopen(path,"rb");
        if(!f||fread(dest[i],1,sizes[i],f)!=sizes[i]||fgetc(f)!=EOF||ferror(f))fail("Driver source background asset invalid.");fclose(f);
    }
    driver_scroll_x=(driver_preview&1)*256;driver_scroll_y=(driver_preview/2)*256;
    if(taxi_driver_background(driver_bg_pixels,driver_bg_indices,driver_bg_palette,driver_strip_indices,driver_strip_palette,driver_scroll_x,driver_scroll_y,(unsigned)driver_preview_cap))fail("Driver background invalid.");
    glGenTextures(11,driver_preview_ids);glBindTexture(GL_TEXTURE_2D,driver_preview_ids[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,driver_bg_pixels);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/driver-selection/labels.rgba");f=fopen(path,"rb");uint8_t portraits[32768];
    if(!f||fread(portraits,1,sizeof portraits,f)!=sizeof portraits||fgetc(f)!=EOF||ferror(f))fail("Driver portraits invalid.");fclose(f);
    memcpy(driver_portrait_source,portraits,sizeof portraits);
    taxi_palette_fade_cap_rgba(portraits,portraits,8192,driver_preview_cap);
    for(unsigned i=0;i<8;i++) {
        glBindTexture(GL_TEXTURE_2D,driver_preview_ids[i+1]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,portraits+i*4096);
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/driver-selection/prompts.rgba");f=fopen(path,"rb");uint8_t icons[2048];
    if(!f||fread(icons,1,sizeof icons,f)!=sizeof icons||fgetc(f)!=EOF||ferror(f))fail("Driver prompt icons invalid.");fclose(f);
    memcpy(driver_icon_source,icons,sizeof icons);
    taxi_palette_fade_cap_rgba(icons,icons,512,driver_preview_cap);
    for(unsigned i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D,driver_preview_ids[9+i]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,icons+i*1024);
    }
    if(driver_cache_check) {
        int original=driver_preview_cap;driver_cap_upload(0);driver_cap_upload(15);driver_cap_upload(31);
        uint8_t *readback=malloc(153600);if(!readback)fail("Driver cache check allocation failed.");
        for(unsigned i=0;i<11;i++) {
            size_t bytes=i==0?153600:i<=8?4096:1024;
            const uint8_t *expected=i==0?driver_bg_pixels:i<=8?driver_portrait_source+(i-1)*4096:driver_icon_source+(i-9)*1024;
            glBindTexture(GL_TEXTURE_2D,driver_preview_ids[i]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
            if(memcmp(readback,expected,bytes))fail("Driver full-cap texture restoration failed.");
        }
        free(readback);driver_cap_upload(original);
        puts("PASS native driver cache:0/15/31 fade cycle restores all11 GPU textures byte-exact; source pixels retained.");
    }
    puts("DRIVER PREVIEW: original settled BG0/portraits/name/ConfirmBack and source RGB555 footer; fade/input and gameplay connection pending.");
}
static void draw_driver_preview(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(driver_preview_cap/31.0f,driver_preview_cap/31.0f,driver_preview_cap/31.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(driver_preview_ids[0],left,0,240*pixel,height);
    const char *names[4]={"Axel","B.D.Joe","Gena","Gus"};
    asset_text(profile_fonts,profile_font_ids[0],0,0,names[driver_preview],left+4*pixel,6*pixel,pixel);
    for(unsigned i=0;i<4;i++)result_quad(driver_preview_ids[1+i*2+(i!=(unsigned)driver_preview)],left+(104+34*i)*pixel,2*pixel,32*pixel,32*pixel);
    result_quad(driver_preview_ids[9],left+4*pixel,144*pixel,16*pixel,16*pixel);
    asset_text(profile_fonts+1,profile_font_ids[1],0,1,profile_prompts[0],left+20*pixel,148*pixel,pixel);
    const TaxiFontAsset *font=profile_fonts+1;int32_t advance=0;
    for(unsigned i=0;profile_prompts[0][i];i++) {
        unsigned glyph=(uint8_t)profile_prompts[0][i]-32,next=profile_prompts[0][i+1]?(uint8_t)profile_prompts[0][i+1]-32:0;
        advance+=(int32_t)font->metrics[glyph*9]*8*256-taxi_text_correction_q8(font->metrics,font->count,(uint16_t)glyph,(uint16_t)next,font->pairs,font->spacing,256);
    }
    double x=20+advance/256;
    result_quad(driver_preview_ids[10],left+x*pixel,144*pixel,16*pixel,16*pixel);
    asset_text(font,profile_font_ids[1],0,1,profile_prompts[1],left+(x+16)*pixel,148*pixel,pixel);
}
static int city_preview=-1;
static int city_preview_cap=31;
static GLuint city_preview_ids[8];
static uint8_t city_portrait_source[20480],city_icon_source[2048];
static uint8_t city_bg_indices[262144],city_strip_indices[640],city_bg_pixels[153600];
static uint16_t city_bg_palette[256],city_strip_palette[16];
static unsigned city_scroll_x,city_scroll_y;
static int city_cache_check,city_locked,city_interactive,city_input_check;
static TaxiCityFlow city_flow;
static TaxiMenuInput city_input;
static void city_cap_upload(int cap) {
    if(cap<0||cap>31)fail("City palette cap invalid.");
    if(taxi_city_background(city_bg_pixels,city_bg_indices,city_bg_palette,city_strip_indices,city_strip_palette,city_scroll_x,city_scroll_y,(unsigned)cap))fail("City native background composition failed.");
    profile_cap(cap);city_preview_cap=cap;
    glBindTexture(GL_TEXTURE_2D,city_preview_ids[0]);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,city_bg_pixels);
    uint8_t pixels[4096];
    for(unsigned i=0;i<7;i++) {
        unsigned side=i<5?32:16;const uint8_t *source=i<5?city_portrait_source+i*4096:city_icon_source+(i-5)*1024;
        taxi_palette_fade_cap_rgba(source,pixels,side*side,cap);
        glBindTexture(GL_TEXTURE_2D,city_preview_ids[i+1]);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,side,side,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    }
}
static void city_tick(uint16_t mask) {
    taxi_city_flow_step(&city_flow,native_settings,&city_input,1);
    city_preview=native_settings[4];city_locked=!native_settings[7];
    city_scroll_x=(unsigned)(city_flow.scroll_x_q8/256);city_scroll_y=(unsigned)(city_flow.scroll_y_q8/256);
    city_cap_upload(city_flow.palette_cap);taxi_menu_input_step(&city_input,mask,1);
}
static void city_begin(void) {
    taxi_city_flow_begin(&city_flow,native_settings);memset(&city_input,0,sizeof city_input);
    city_preview=0;city_locked=!native_settings[7];city_scroll_x=16;city_scroll_y=0;city_cap_upload(0);
}
static void load_city_preview(void) {
    load_profile_preview(); /* Startup loads shared fonts through city first. */
    profile_cap(city_preview_cap);
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("City preview asset path failed.");tail++;
    const char *files[4]={"background.indices","background.rgb555","footer.indices","footer.rgb555"};
    void *dest[4]={city_bg_indices,city_bg_palette,city_strip_indices,city_strip_palette};
    const size_t sizes[4]={262144,512,640,32};FILE *f;
    for(unsigned i=0;i<4;i++) {
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/city-selection/%s",files[i]);f=fopen(path,"rb");
        if(!f||fread(dest[i],1,sizes[i],f)!=sizes[i]||fgetc(f)!=EOF||ferror(f))fail("City source background asset invalid.");fclose(f);
    }
    city_scroll_x=city_preview==1&&!city_locked?256:0;city_scroll_y=city_preview==1&&city_locked?256:0;
    if(taxi_city_background(city_bg_pixels,city_bg_indices,city_bg_palette,city_strip_indices,city_strip_palette,city_scroll_x,city_scroll_y,(unsigned)city_preview_cap))fail("City background invalid.");
    glGenTextures(8,city_preview_ids);glBindTexture(GL_TEXTURE_2D,city_preview_ids[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,city_bg_pixels);
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/city-selection/labels.rgba");f=fopen(path,"rb");uint8_t portraits[20480];
    if(!f||fread(portraits,1,sizeof portraits,f)!=sizeof portraits||fgetc(f)!=EOF||ferror(f))fail("City portraits invalid.");fclose(f);
    memcpy(city_portrait_source,portraits,sizeof portraits);
    taxi_palette_fade_cap_rgba(portraits,portraits,5120,city_preview_cap);
    for(unsigned i=0;i<5;i++) {
        glBindTexture(GL_TEXTURE_2D,city_preview_ids[i+1]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,portraits+i*4096);
    }
    snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/city-selection/prompts.rgba");f=fopen(path,"rb");uint8_t icons[2048];
    if(!f||fread(icons,1,sizeof icons,f)!=sizeof icons||fgetc(f)!=EOF||ferror(f))fail("City prompt icons invalid.");fclose(f);
    memcpy(city_icon_source,icons,sizeof icons);
    taxi_palette_fade_cap_rgba(icons,icons,512,city_preview_cap);
    for(unsigned i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D,city_preview_ids[6+i]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,icons+i*1024);
    }
    if(city_cache_check) {
        int original=city_preview_cap;city_cap_upload(0);city_cap_upload(15);city_cap_upload(31);
        uint8_t *readback=malloc(153600);if(!readback)fail("City cache check allocation failed.");
        for(unsigned i=0;i<8;i++) {
            size_t bytes=i==0?153600:i<=5?4096:1024;
            const uint8_t *expected=i==0?city_bg_pixels:i<=5?city_portrait_source+(i-1)*4096:city_icon_source+(i-6)*1024;
            glBindTexture(GL_TEXTURE_2D,city_preview_ids[i]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
            if(memcmp(readback,expected,bytes))fail("City full-cap texture restoration failed.");
        }
        free(readback);city_cap_upload(original);
        puts("PASS native city cache:0/15/31 fade cycle restores all8 GPU textures byte-exact; source pixels retained.");
    }
    puts("CITY PREVIEW: original settled BG0/portraits/name/ConfirmBack and source RGB555 footer; fade/input and gameplay connection pending.");
}
static void draw_city_preview(int width,int height) {
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(city_preview_cap/31.0f,city_preview_cap/31.0f,city_preview_cap/31.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    result_quad(city_preview_ids[0],left,0,240*pixel,height);
    const char *names[2]={"City 1","City 2"};
    asset_text(profile_fonts,profile_font_ids[0],0,0,names[city_preview],left+4*pixel,6*pixel,pixel);
    for(unsigned i=0;i<2;i++)result_quad(city_preview_ids[i==1&&city_locked?5:1+i*2+(i!=(unsigned)city_preview)],left+(172+34*i)*pixel,2*pixel,32*pixel,32*pixel);
    result_quad(city_preview_ids[6],left+4*pixel,144*pixel,16*pixel,16*pixel);
    asset_text(profile_fonts+1,profile_font_ids[1],0,1,profile_prompts[0],left+20*pixel,148*pixel,pixel);
    const TaxiFontAsset *font=profile_fonts+1;int32_t advance=0;
    for(unsigned i=0;profile_prompts[0][i];i++) {
        unsigned glyph=(uint8_t)profile_prompts[0][i]-32,next=profile_prompts[0][i+1]?(uint8_t)profile_prompts[0][i+1]-32:0;
        advance+=(int32_t)font->metrics[glyph*9]*8*256-taxi_text_correction_q8(font->metrics,font->count,(uint16_t)glyph,(uint16_t)next,font->pairs,font->spacing,256);
    }
    double x=20+advance/256;
    result_quad(city_preview_ids[7],left+x*pixel,144*pixel,16*pixel,16*pixel);
    asset_text(font,profile_font_ids[1],0,1,profile_prompts[1],left+(x+16)*pixel,148*pixel,pixel);
}
static void startup_parent_begin(int main,int reinitialize){
    uint8_t interval=normal_live_flow.fade.interval_1771,reload=normal_live_flow.fade.reload_1772;
    taxi_normal_menu_flow_begin(&normal_live_flow,main,main?normal_live_main_selection:normal_live_mode_selection,reinitialize,interval,reload);
    /* Source174dc..17588: music is reset only on main reinitialization
       (source3897 nonzero), not each retained submenu return. */
    if(main&&reinitialize){music_context.sound_active=0;music_test_start(4);}
    startup_stage=main?9:8;
    normal_menu_update_motion((main?0:5)+normal_live_flow.choice.selection,taxi_normal_menu_flow_cap(&normal_live_flow),taxi_normal_menu_flow_slide(&normal_live_flow),startup_parent_check);
}
static void credits_live_begin(int parent_entry){
    if(startup_stage!=3||(!parent_entry&&!startup_credits_audio_smoke)||
       (parent_entry&&(!startup_quit_complete||startup_quit_parent.phase!=3||!startup_quit_parent.qualified)))
        fail("Credits entry owner invalid.");
    if(taxi_credits_asset_load(&credits_live_asset,"outputs/crazy-taxi-native/assets/credits"))fail("Native credits assets unavailable.");
    /* Parent entry already consumed original10AAC before B5E6. Direct
       diagnostic entry cleans up live channels explicitly, without fare edits. */
    if(!parent_entry)taxi_game_audio_cleanup_dispatch(live_sound_state.handles,live_game_sound_dispatch,NULL);
    if(taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_STOP))fail("Credits entry music stop failed.");
    music_context.main_mode=1;music_context.sound_active=0;
    music_test_start(4);credits_music_frames_before=music_frames;credits_music_nonzero_before=music_nonzero_samples;
    taxi_credits_runtime_begin(&credits_live_runtime,city_fade.interval_1771,city_fade.reload_1772);
    glGenTextures(1,&credits_live_texture);glBindTexture(GL_TEXTURE_2D,credits_live_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,credits_live_rgba);
    credits_live_active=1;startup_stage=15;
    puts(parent_entry?"LIVE CREDITS: actual qualified Quit parent entry; original assets/music/fade runtime, whole-frame fidelity unverified.":
        "LIVE CREDITS DIAGNOSTIC: original assets/music order5/fade runtime; explicit screen entry, no earned qualification or original whole-frame claim.");
}
static void credits_live_tick(uint16_t mask){
    if(credits_live_runtime.complete)return;
    TaxiCreditsRuntimeFrame f;unsigned line=credits_live_runtime.flow.line;
    int32_t sine=runtime.trig.sine[credits_live_runtime.flow.angle];
    if(taxi_credits_runtime_frame(&credits_live_runtime,1,sine,line<104?credits_live_asset.lines[line]:NULL,
        audio_settings_state.effects_enabled,(mask&1)!=0,(mask&8)!=0,&f))fail("Native credits frame failed.");
    if(f.running&&taxi_credits_row_write(&credits_live_asset,f.row,&f.frame))fail("Native credits row failed.");
    if(f.frame.sound29)audio_request(29);
    if(f.stop_music&&taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_STOP))fail("Credits music stop failed.");
    ++credits_live_ticks;
}
static void credits_live_draw(int width,int height){
    uint16_t pixels[240*160];
    if(taxi_credits_compose(&credits_live_asset,credits_live_runtime.scroll,pixels))fail("Native credits composition failed.");
    for(unsigned i=0;i<240*160;i++){
        uint16_t color=taxi_palette_fade_darken_rgb555(pixels[i],credits_live_runtime.bldy);
        for(unsigned c=0;c<3;c++)credits_live_rgba[i*4+c]=(uint8_t)(((color>>(5*c))&31)*255/31);
        credits_live_rgba[i*4+3]=255;
    }
    glViewport(0,0,width,height);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
    glBindTexture(GL_TEXTURE_2D,credits_live_texture);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,credits_live_rgba);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    double scale=height/160.0,left=(width-240*scale)/2;
    result_quad(credits_live_texture,left,0,240*scale,height);++credits_live_draws;
}
/* Source projected billboard requests, widened around screen center. Whole
   world/actor software queue occlusion is still a separate renderer dependency. */
static void draw_traffic(int width,int height){
    if(!traffic_submission_count||!recovered_camera||camera_orbit||camera_pitch_orbit)return;
    float scale=(float)height/160.0f,offset=(float)width/2-120*scale;
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glColor4f(1,1,1,1);
    /* Original queue consumes descending buckets, newest insertion first. */
    for(int bucket=127;bucket>=0;bucket--)for(unsigned reverse=traffic_submission_count;reverse;reverse--){
        const TaxiTrafficSubmission *s=traffic_submissions+reverse-1;
        if(!s->visible||s->depth_bucket!=(unsigned)bucket||s->type>=3||s->view.frame>=16)continue;
        float left=offset+s->screen.left*scale,right=offset+s->screen.right*scale;
        float top=s->screen.top*scale,bottom=s->screen.bottom*scale;
        glBindTexture(GL_TEXTURE_2D,traffic_textures[s->type*16+s->view.frame]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(left,top);glTexCoord2f(1,0);glVertex2f(right,top);
        glTexCoord2f(1,1);glVertex2f(right,bottom);glTexCoord2f(0,1);glVertex2f(left,bottom);
        glEnd();
    }
    glDisable(GL_ALPHA_TEST);glDisable(GL_TEXTURE_2D);glEnable(GL_DEPTH_TEST);
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
}
static TaxiWorldFramePacket *queued_world;
static TaxiRenderPacket *queued_packets;
static uint8_t *queued_normal_seen,*queued_special_seen;
static size_t queued_capacity;
static TaxiPassengerRender queued_passengers[128],queued_passenger_markers[256];
static unsigned queued_passenger_marker_count;
static int native_marker_fixture,passenger_marker_mask;
static unsigned queued_passenger_count;
enum { TAXI_RENDER_PASSENGER=3,TAXI_RENDER_SHADOW=4,TAXI_RENDER_FLAG=5,TAXI_RENDER_BALL=6,TAXI_RENDER_PASSENGER_MARKER=7 };
static TaxiChallengeFlagProjection queued_flag;
static int flag_gpu_check,ball_gpu_check;
static TaxiBallProjection queued_balls[20];
/* Source indexed pixels use caller-retained BG colors, not embedded bitmap
   colors. Caller must establish original registration and fade ownership. */
static void flag_palette_upload(const uint8_t indices[2048],const uint16_t registered_palette[256],int16_t cap){
    uint8_t rgba[8192];
    for(unsigned i=0;i<2048;i++){
        unsigned index=indices[i];uint16_t color=taxi_palette_fade_cap_rgb555(registered_palette[index],cap);
        for(unsigned c=0;c<3;c++)rgba[i*4+c]=(uint8_t)(((color>>(c*5))&31)*255/31);
        rgba[i*4+3]=index?255:0;
    }
    glBindTexture(GL_TEXTURE_2D,flag_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,64,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
}
static void ball_palette_upload(const uint8_t indices[8192],const uint16_t registered[256],int16_t cap){
    uint8_t rgba[32768];
    if(taxi_challenge_ball_palette_rgba(rgba,indices,registered,cap))fail("Ball registered palette invalid.");
    for(unsigned frame=0;frame<8;frame++){
        glBindTexture(GL_TEXTURE_2D,ball_textures[frame]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba+frame*4096);
    }
}
static TaxiShadowRender queued_shadow;
static unsigned queued_shadow_frame;
static int queue_occlusion_check,render_state_check;
static const char *source_column_work_report_path;static int source_column_work_report_done;
typedef struct {TaxiWorldFrame *frame;float scale,offset;const TaxiTrafficSubmission *traffic;int depth_valid;uint32_t far_depth,shade_depth;int bitmap_coverage;} QueueDraw;
static int draw_queue_packet(void *context,unsigned kind,size_t payload){
    QueueDraw *draw=context;
    if(kind==TAXI_RENDER_BALL){
        if(payload>=20||draw->scale<=0)return -1;
        const TaxiBallProjection *p=queued_balls+payload;
        if((p->packet_flags!=8&&p->packet_flags!=12)||p->bitmap_identity<0x0849b0c9u)return -1;
        unsigned offset=p->bitmap_identity-0x0849b0c9u;
        if(offset>=8192||(offset&1023)||!ball_textures[offset/1024])return -1;
        TaxiIndexedSpan spans[160];int left=(int)floor(-draw->offset/draw->scale/2)*2;
        int right=(int)ceil((240+draw->offset/draw->scale)/2)*2;
        if(native_axis_snapped_check){left=0;right=240;}
        int raster_left=p->bounds.left<left?left:p->bounds.left;
        int raster_right=p->bounds.right>right?right:p->bounds.right;
        if((raster_right&~1)<=(raster_left&~1)||p->bounds.bottom<=0||p->bounds.top>=160||p->bounds.bottom<=p->bounds.top)return 0;
        if(!taxi_indexed_billboard_rows_bounds(spans,p->bounds.left,p->bounds.top,p->bounds.right,p->bounds.bottom,32,32,left,right,(p->packet_flags&4)!=0,0))return -1;
        int x0=right,x1=left,y0=160,y1=0;
        for(int y=0;y<160;y++)if(spans[y].pairs){
            int end=spans[y].x+(int)spans[y].pairs*2;
            if(spans[y].x<x0)x0=spans[y].x;if(end>x1)x1=end;
            if(y<y0)y0=y;y1=y+1;
        }
        if(x1<=x0||y1<=y0)return 0;
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,ball_textures[offset/1024]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        if(!taxi_axis_shader_begin(spans,160,0,left,32,32,native_axis_snapped_check))fail("Ball indexed span shader unavailable.");
        glColor4f(1,1,1,1);glBegin(GL_QUADS);
        glTexCoord2i(x0,y0);glVertex2f(draw->offset+x0*draw->scale,y0*draw->scale);
        glTexCoord2i(x1,y0);glVertex2f(draw->offset+x1*draw->scale,y0*draw->scale);
        glTexCoord2i(x1,y1);glVertex2f(draw->offset+x1*draw->scale,y1*draw->scale);
        glTexCoord2i(x0,y1);glVertex2f(draw->offset+x0*draw->scale,y1*draw->scale);
        glEnd();taxi_axis_shader_end();return 0;
    }
    if(kind==TAXI_RENDER_FLAG){
        if(payload||!flag_texture)return -1;
        TaxiIndexedSpan spans[160];TaxiIndexedVertex vertices[4];
        int left=(int)floor(-draw->offset/draw->scale/2)*2;
        int right=(int)ceil((240+draw->offset/draw->scale)/2)*2;
        if(native_axis_snapped_check){left=0;right=240;}
        if(taxi_challenge_flag_vertices(&queued_flag,vertices)||taxi_challenge_flag_rows(spans,&queued_flag,left,right)!=1)return -1;
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,flag_texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        if(!taxi_axis_shader_begin(spans,160,0,left,32,64,native_axis_snapped_check))fail("Native Flag span shader failed.");
        glColor4f(1,1,1,1);glBegin(GL_QUADS);
        for(unsigned i=0;i<4;i++){glTexCoord2i(vertices[i].x,vertices[i].y);glVertex2f(draw->offset+vertices[i].x*draw->scale,vertices[i].y*draw->scale);}
        glEnd();taxi_axis_shader_end();return 0;
    }
    if(kind==TAXI_RENDER_PASSENGER||kind==TAXI_RENDER_PASSENGER_MARKER){
        int marker=kind==TAXI_RENDER_PASSENGER_MARKER;
        if(marker&&passenger_marker_mask)return 0;
        if(marker?(payload>=queued_passenger_marker_count):(payload>=passenger_scene.count||payload>=128))return -1;
        const TaxiPassengerRender *p=marker?queued_passenger_markers+payload:queued_passengers+payload;
        if(p->texture>=(marker?10u:144u))return -1;
        int bitmap_width=marker?64:32;
        const int32_t x[4]={p->screen.left,p->screen.left,p->screen.right,p->screen.right};
        const int32_t y[4]={p->screen.bottom,p->screen.top,p->screen.top,p->screen.bottom};
        TaxiIndexedSpan spans[160];
        int left=(int)floor(-draw->offset/draw->scale/2)*2;
        int right=(int)ceil((240+draw->offset/draw->scale)/2)*2;
        if(native_axis_snapped_check){left=0;right=240;}
        /* Source231f4/23232/2324c caller flags0 or horizontal flip1,
           angle0; flags1 changes to four-corner affine UV raster owner. */
        uint16_t packet_flags=taxi_indexed_billboard_packet_flags(0,p->flip?1:0,1);
        int planned;
        if(packet_flags&8){
            planned=taxi_indexed_billboard_rows_bounds(spans,p->screen.left,p->screen.top,
                p->screen.right,p->screen.bottom,bitmap_width,64,left,right,1,0);
        }else{
            TaxiIndexedVertex vertices[4];
            for(unsigned i=0;i<4;i++)vertices[i]=(TaxiIndexedVertex){x[i],y[i],
                (uint32_t)p->u[i],(uint32_t)p->v[i],(uint32_t)p->billboard.top_left.z};
            planned=taxi_indexed_affine_rows_bounds(spans,vertices,4,left,right,1);
        }
        if(!planned)return -1;
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,marker?passenger_marker_textures[p->texture]:passenger_textures[p->texture]);
        /* Native U/V bit masks wrap, including negative flipped gradients. */
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        if(!taxi_axis_shader_begin(spans,160,0,left,bitmap_width,64,native_axis_snapped_check))
            fail("Original passenger bitmap sampling setup failed.");
        glColor4f(1,1,1,1);glBegin(GL_QUADS);
        for(unsigned i=0;i<4;i++){
            glTexCoord2i(x[i],y[i]);
            glVertex2f(draw->offset+x[i]*draw->scale,y[i]*draw->scale);
        }
        glEnd();taxi_axis_shader_end();return 0;
    }
    if(kind==TAXI_RENDER_TRAFFIC||kind==TAXI_RENDER_SHADOW){
        TaxiTrafficScreenBounds screen;GLuint texture;
        if(kind==TAXI_RENDER_SHADOW){
            if(payload||queued_shadow_frame>=64)return -1;
            screen=queued_shadow.screen;texture=shadow_textures[queued_shadow_frame];
        }else{
            if(payload>=traffic_submission_count)return -1;
            const TaxiTrafficSubmission *s=(draw->traffic?draw->traffic:traffic_submissions)+payload;
            screen=s->screen;texture=traffic_textures[s->type*16+s->view.frame];
        }
        /* Source68be ordinary traffic flags0; shadow15636 flags0 on
           normal branch155ee==0. Both are keyed software BG rectangles. */
        unsigned tw=kind==TAXI_RENDER_SHADOW?64:128;
        unsigned th=kind==TAXI_RENDER_SHADOW?32:64;
        TaxiIndexedSpan spans[160];
        int left=(int)floor(-draw->offset/draw->scale/2)*2;
        int right=(int)ceil((240+draw->offset/draw->scale)/2)*2;
        if(native_axis_snapped_check){left=0;right=240;}
        /* Frustum submissions may clip to no raster pair (including width1).
           Source rectangle callback writes nothing in that case. */
        int raster_left=screen.left<left?left:screen.left;
        int raster_right=screen.right>right?right:screen.right;
        if((raster_right&~1)<=(raster_left&~1)||screen.bottom<=0||screen.top>=160||screen.bottom<=screen.top)return 0;
        /* Qualified original shadow callback6a28/ARM4a94 retains divider
           remainder-register U for a wholly visible rectangle. Clipped4a14
           starts U at zero. Traffic qualification remains separate. */
        int shadow_unclipped=kind==TAXI_RENDER_SHADOW&&screen.left>=left&&screen.right<=right&&screen.top>=0&&screen.bottom<=160;
        if(!taxi_indexed_billboard_rows_bounds(spans,screen.left,screen.top,
            screen.right,screen.bottom,tw,th,left,right,!shadow_unclipped,0))return -1;
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        if(!taxi_axis_shader_begin(spans,160,0,left,tw,th,native_axis_snapped_check))
            fail("Original traffic/shadow bitmap sampling setup failed.");
        /* Original paired stores align rectangle X down before drawing.
           The source-width shader must be allowed to cover that first pair. */
        if(native_axis_snapped_check){screen.left&=~1;screen.right&=~1;}
        glColor4f(1,1,1,1);glBegin(GL_QUADS);
        glTexCoord2i(screen.left,screen.top);glVertex2f(draw->offset+screen.left*draw->scale,screen.top*draw->scale);
        glTexCoord2i(screen.right,screen.top);glVertex2f(draw->offset+screen.right*draw->scale,screen.top*draw->scale);
        glTexCoord2i(screen.right,screen.bottom);glVertex2f(draw->offset+screen.right*draw->scale,screen.bottom*draw->scale);
        glTexCoord2i(screen.left,screen.bottom);glVertex2f(draw->offset+screen.left*draw->scale,screen.bottom*draw->scale);
        glEnd();taxi_axis_shader_end();return 0;
    }
    if(payload>=draw->frame->count)return -1;
    const TaxiWorldFramePacket *p=draw->frame->packets+payload;
    if(p->draw_culled)return 0;
    uint32_t callback=taxi_world_indexed_callback(p,draw->frame->world);
    int original_flat=kind==TAXI_RENDER_WORLD &&
        (callback==0x03006950 || callback==0x0300690c);
    int attribute_flat=kind==TAXI_RENDER_WORLD && callback==0x03006918;
    int axis_rows=kind==TAXI_RENDER_WORLD&&(callback==0x03006934||callback==0x03006944);
    int axis_columns=kind==TAXI_RENDER_WORLD&&(callback==0x030069ac||callback==0x030069b8);
    int affine_clipped=kind==TAXI_RENDER_WORLD&&callback==0x03006914;
    int affine_unclipped=kind==TAXI_RENDER_WORLD&&callback==0x03006948;
    int shaded=kind==TAXI_RENDER_WORLD&&callback==0x0300693c;
    int flat_shaded=kind==TAXI_RENDER_WORLD&&(callback==0x0300694c||original_flat);
    TaxiTrafficScreenBounds clipped_quad={0};int clipped_quad_valid=0;
    int affine=affine_clipped||affine_unclipped||shaded||flat_shaded;
    int special_axis=kind==TAXI_RENDER_SPECIAL&&(callback==0x030069a4||callback==0x03006a28||callback==0x03006a38);
    int special_shaded=special_axis&&callback==0x03006a38;
    int axis=axis_rows||axis_columns||affine||special_axis;
    if(axis) {
        TaxiIndexedVertex vertices[8];TaxiIndexedSpan spans[1024];
        if(!special_axis&&(p->polygon.count<3||p->polygon.count>8))return -1;
        for(unsigned i=0;i<p->polygon.count;i++) {
            const TaxiWorldClipVertex *v=p->polygon.vertices+i;
            vertices[i]=(TaxiIndexedVertex){v->screen_x,v->screen_y,(uint32_t)v->u,(uint32_t)v->v,(uint32_t)v->z};
        }
        int left=(int)floor(-draw->offset/draw->scale/2)*2;
        int right=(int)ceil((240+draw->offset/draw->scale)/2)*2;
        if(native_axis_snapped_check){left=0;right=240;}
        unsigned count=axis_columns?(unsigned)(right-left)/2:160;
        uint32_t far_depth=draw->depth_valid?draw->far_depth:(uint32_t)draw->frame->projection.far_plane;
        uint32_t shade_depth=draw->depth_valid?draw->shade_depth:far_depth;
        unsigned special_bank=0;
        if(special_shaded&&!taxi_indexed_billboard_bank(&special_bank,(uint32_t)p->special.depth,far_depth,shade_depth))return -1;
        TaxiTrafficScreenBounds bounds=p->special.screen;
        /* Clipped source rectangles may survive submission but contain no
           two-pixel raster pair. Match the traffic/shadow no-write case. */
        if(special_axis && callback!=0x03006a28) {
            int raster_left=bounds.left<left?left:bounds.left;
            int raster_right=bounds.right>right?right:bounds.right;
            if((raster_right&~1)<=(raster_left&~1)||bounds.bottom<=0||
               bounds.top>=160||bounds.bottom<=bounds.top)return 0;
        }
        int planned=special_axis?taxi_indexed_billboard_rows_bounds(spans,bounds.left,bounds.top,bounds.right,bounds.bottom,
            textures[p->texture].width,textures[p->texture].height,left,right,callback!=0x03006a28,special_bank):
            original_flat?taxi_indexed_flat_rows_bounds(spans,vertices,p->polygon.count,left,right):
            flat_shaded?taxi_indexed_shaded_flat_rows_bounds(spans,vertices,p->polygon.count,left,right,far_depth,shade_depth):
            shaded?taxi_indexed_shaded_rows_bounds(spans,vertices,p->polygon.count,left,right,far_depth,shade_depth):
            affine_clipped?taxi_indexed_unshaded_clipped_rows_bounds(spans,vertices,p->polygon.count,left,right):
            affine?taxi_indexed_affine_rows_bounds(spans,vertices,p->polygon.count,left,right,affine_clipped):
            axis_columns?taxi_indexed_perspective_columns_bounds(spans,1024,vertices,p->polygon.count,left,right):
            taxi_indexed_perspective_rows_bounds(spans,vertices,p->polygon.count,left,right);
        if(!planned) {
            fprintf(stderr,"WORLD SPAN FAILURE: kind%u callback%08x count%u texture%u clip%d..%d\n",kind,callback,p->polygon.count,p->texture,left,right);
            fprintf(stderr,"special bounds%d,%d..%d,%d texture dimensions%u,%u bank%u\n",bounds.left,bounds.top,bounds.right,bounds.bottom,textures[p->texture].width,textures[p->texture].height,special_bank);
            for(unsigned i=0;i<p->polygon.count;i++)fprintf(stderr,"vertex%u x%ld y%ld u%ld v%ld depth%ld\n",i,(long)vertices[i].x,(long)vertices[i].y,(long)vertices[i].u,(long)vertices[i].v,(long)vertices[i].depth);
            return -1;
        }
        if(kind==TAXI_RENDER_WORLD&&axis_columns){
            clipped_quad=(TaxiTrafficScreenBounds){right,160,left,0};
            for(unsigned column=0;column<count;column++)if(spans[column].pairs){
                int x=left+(int)column*2,end=spans[column].x+(int)spans[column].pairs;
                if(x<clipped_quad.left)clipped_quad.left=x;
                if(x+2>clipped_quad.right)clipped_quad.right=x+2;
                if(spans[column].x<clipped_quad.top)clipped_quad.top=spans[column].x;
                if(end>clipped_quad.bottom)clipped_quad.bottom=end;
                clipped_quad_valid=1;
            }
            if(!clipped_quad_valid)return 0;
        }
        if(kind==TAXI_RENDER_WORLD&&!axis_columns){
            clipped_quad=(TaxiTrafficScreenBounds){right,160,left,0};
            for(unsigned row=0;row<160;row++)if(spans[row].pairs){int end=spans[row].x+(int)spans[row].pairs*2;if(spans[row].x<clipped_quad.left)clipped_quad.left=spans[row].x;if(end>clipped_quad.right)clipped_quad.right=end;if((int)row<clipped_quad.top)clipped_quad.top=(int)row;clipped_quad.bottom=(int)row+1;clipped_quad_valid=1;}
            if(!clipped_quad_valid)return 0;
        }
        if(source_scene_gpu_requested&&native_axis_snapped_check&&!axis_columns&&!flat_shaded){
            const int audit_x[3]={136,220,180},audit_y[3]={61,70,66};
            for(unsigned a=0;a<3;a++){TaxiIndexedSpan sp=spans[audit_y[a]];int pair_x=audit_x[a]&~1;
                if(sp.pairs&&pair_x>=sp.x&&pair_x<sp.x+(int)sp.pairs*2){
                    unsigned pairs=(unsigned)(pair_x-sp.x)/2;uint32_t u=sp.u+sp.du*pairs,v=sp.v+sp.dv*pairs;
                    unsigned tw=textures[p->texture].width,th=textures[p->texture].height;
                    printf("UV_AUDIT source%u callback%08x pixel%d,%d span%d pairs%u u%u v%u du%u dv%u sample%u,%u dimensions%u,%u\n",(unsigned)p->source_index,callback,audit_x[a],audit_y[a],sp.x,(unsigned)sp.pairs,sp.u,sp.v,sp.du,sp.dv,(u>>14)&(tw-1),(v>>14)&(th-1),tw,th);
                }
            }
        }
        unsigned texture_width=flat_shaded?2:textures[p->texture].width,texture_height=flat_shaded?2:textures[p->texture].height;
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,flat_shaded?0:textures[p->texture].id);
        if(!taxi_axis_shader_begin(spans,count,axis_columns,left,texture_width,texture_height,native_axis_snapped_check))fail("Native perspective axis shader failed.");
        if(shaded||flat_shaded||special_shaded) {
            static uint8_t flat_identity[2560];static int flat_identity_ready;
            if(original_flat&&!flat_identity_ready){for(unsigned i=0;i<2560;i++)flat_identity[i]=(uint8_t)i;flat_identity_ready=1;}
            uint8_t dummy=0;TaxiIndexedTexture indexed={1,1,0,0,&dummy,1};
            if((!flat_shaded&&taxi_indexed_asset_texture(&native_indexed_scene,p->texture,&indexed))||
               !taxi_axis_shader_lookup(&indexed,original_flat?flat_identity:(native_shade_lookup_check?native_shade_lookup_check:native_scene_lookup),
                    pause_scene_palette(),(int16_t)city_fade_cap))fail("Native shaded texture lookup failed.");
            if(flat_shaded&&!taxi_axis_shader_flat(draw->frame->world->materials[p->material*28+14],draw->frame->world->materials[p->material*28+15]))fail("Native shaded flat lookup failed.");
            if(special_shaded&&!taxi_axis_shader_special())fail("Native shaded special lookup failed.");
            if(draw->bitmap_coverage&&!taxi_axis_shader_bitmap_coverage(1))fail("Shaded BG coverage setup failed.");
        }
        native_axis_draws++;if(original_flat)native_flat_pattern_draws++;
    }
    else if(attribute_flat) {
        if(!taxi_material_shader_begin(draw->frame->world->materials+p->material*28,
            pause_scene_palette(),(int16_t)city_fade_cap))fail("Native attribute material shader failed.");
        if(draw->bitmap_coverage&&!taxi_material_shader_bitmap_coverage(1))fail("Attribute BG coverage setup failed.");
        native_attribute_pattern_draws++;
    }
    else if(original_flat) {
        const uint8_t *pattern=draw->frame->world->materials+p->material*28+14;
        uint8_t rgba[16];
        for(unsigned texel=0;texel<4;texel++) {
            unsigned index=pattern[(texel^(texel>>1))&1];
            uint16_t color=taxi_palette_fade_cap_rgb555(pause_scene_palette()[index],(int16_t)city_fade_cap);
            for(unsigned channel=0;channel<3;channel++)rgba[texel*4+channel]=(uint8_t)(((color>>(channel*5))&31)*255/31);
            rgba[texel*4+3]=(uint8_t)(draw->bitmap_coverage&&!index?0:255); /* Index0 writes raster byte, then mode4 backdrop coverage. */
        }
        if(!native_flat_pattern_texture)glGenTextures(1,&native_flat_pattern_texture);
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,native_flat_pattern_texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        glColor4f(1,1,1,1);native_flat_pattern_draws++;
    }
    else if(p->texture!=255){glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,textures[p->texture].id);glColor4f(1,1,1,1);}
    else{
        if(draw->bitmap_coverage)native_effect_background_coverage_valid=0; /* Unsupported callback cannot establish index coverage. */
        /* Existing flat-color preview; source shade rasterization remains pending. */
        unsigned char color[4];taxi_palette_fade_cap_rgba(materials[p->material].color,color,1,(int16_t)city_fade_cap);
        glDisable(GL_TEXTURE_2D);glColor4ubv(color);
    }
    GLboolean alpha_before=glIsEnabled(GL_ALPHA_TEST);
    int retain_zero=draw->bitmap_coverage&&(original_flat||attribute_flat||shaded||flat_shaded||special_shaded);
    if(retain_zero)glDisable(GL_ALPHA_TEST);
    if(kind==TAXI_RENDER_SPECIAL){
        TaxiTrafficScreenBounds s=p->special.screen;
        if(axis&&native_axis_snapped_check){s.left&=~1;s.right&=~1;}
        glBegin(GL_QUADS);
        if(axis)glTexCoord2i(s.left,s.top);else glTexCoord2f(0,0);glVertex2f(draw->offset+s.left*draw->scale,s.top*draw->scale);
        if(axis)glTexCoord2i(s.right,s.top);else glTexCoord2f(1,0);glVertex2f(draw->offset+s.right*draw->scale,s.top*draw->scale);
        if(axis)glTexCoord2i(s.right,s.bottom);else glTexCoord2f(1,1);glVertex2f(draw->offset+s.right*draw->scale,s.bottom*draw->scale);
        if(axis)glTexCoord2i(s.left,s.bottom);else glTexCoord2f(0,1);glVertex2f(draw->offset+s.left*draw->scale,s.bottom*draw->scale);glEnd();
    }else if(kind==TAXI_RENDER_WORLD&&clipped_quad_valid){
        TaxiTrafficScreenBounds b=clipped_quad;glBegin(GL_QUADS);
        glTexCoord2i(b.left,b.top);glVertex2f(draw->offset+b.left*draw->scale,b.top*draw->scale);
        glTexCoord2i(b.right,b.top);glVertex2f(draw->offset+b.right*draw->scale,b.top*draw->scale);
        glTexCoord2i(b.right,b.bottom);glVertex2f(draw->offset+b.right*draw->scale,b.bottom*draw->scale);
        glTexCoord2i(b.left,b.bottom);glVertex2f(draw->offset+b.left*draw->scale,b.bottom*draw->scale);glEnd();
    }else if(kind==TAXI_RENDER_WORLD){
        glBegin(GL_TRIANGLE_FAN);
        for(unsigned i=0;i<p->polygon.count;i++){
            const TaxiWorldClipVertex *v=p->polygon.vertices+i;
            if(axis)glTexCoord2d(v->screen_x,v->screen_y);
            else if(attribute_flat)glTexCoord3d(v->screen_x,v->screen_y,(double)v->attribute/16777216.0);
            else if(original_flat)glTexCoord2d((double)v->screen_x/2,(double)v->screen_y/2);
            else if(p->texture!=255)glTexCoord2d((double)v->u/(16384.0*textures[p->texture].width),
                (double)v->v/(16384.0*textures[p->texture].height));
            glVertex2f(draw->offset+v->screen_x*draw->scale,v->screen_y*draw->scale);
        }
        glEnd();
    }else return -1;
    if(attribute_flat)taxi_material_shader_end();
    if(axis)taxi_axis_shader_end();
    if(retain_zero&&alpha_before)glEnable(GL_ALPHA_TEST);
    return 0;
}

static void draw_native_gpu_background(int width,int height,double view_pitch,double view_yaw){
        /* ARM08002fe0 opaque bitmap copy before world polygons. Widescreen
           extends the original 240-pixel repeating sky about its centre. */
        int source_pitch=(int)floor(view_pitch*4096/360.0);
        int half_pitch=source_pitch/2;
        if(half_pitch>142)half_pitch=142;
        if(half_pitch<-142)half_pitch=-142;
        int row=64-(int)floor(half_pitch/4.0);
        if(row<0)row=0;if(row>160)row=160;
        int source_yaw=(int)floor((180-view_yaw)*4096/360.0);
        int scroll=((((uint32_t)-source_yaw*4u)&4095u)*60u>>12)<<2;
        double span=(double)width/height*160;
        double u0=(scroll+(240-span)/2)/240.0;
        glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_ALPHA_TEST);
        glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,textures[sky_texture].id);
        glColor4f(1,1,1,1);
        glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,span,160,0,-1,1);
        glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
        glBegin(GL_QUADS);
        glTexCoord2d(u0,row/160.0);glVertex2d(0,0);
        glTexCoord2d(u0+span/240.0,row/160.0);glVertex2d(span,0);
        glTexCoord2d(u0+span/240.0,1);glVertex2d(span,160-row);
        glTexCoord2d(u0,1);glVertex2d(0,160-row);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        unsigned char fill_color[4];if(pause_scene_palette()!=native_scene_palette){taxi_palette_gameplay_alternate_rgba(sky_fill,fill_color,1);taxi_palette_fade_cap_rgba(fill_color,fill_color,1,(int16_t)city_fade_cap);}else taxi_palette_fade_cap_rgba(sky_fill,fill_color,1,(int16_t)city_fade_cap);
        glColor4ubv(fill_color);glBegin(GL_QUADS);
        glVertex2d(0,160-row);glVertex2d(span,160-row);
        glVertex2d(span,160);glVertex2d(0,160);glEnd();
        glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();
        glMatrixMode(GL_MODELVIEW);glPopAttrib();
        /* Source0802b4e0 repeats the sky's bottom-left palette index. */
}

/* Private source-width GPU evidence: verified CPU background, production scenery/shadow. */
static void check_source_scene_gpu(TaxiWorldFrame *frame,const TaxiShadowRender *shadow,int visible){
    TaxiGenObjectsFn gen;TaxiDeleteObjectsFn del;TaxiBindObjectFn bind;TaxiAttachTextureFn attach;TaxiFramebufferStatusFn status;
    typedef void (APIENTRY *UseProgramFn)(GLuint);UseProgramFn use;
    TARGET_LOAD(gen,"glGenFramebuffers");TARGET_LOAD(del,"glDeleteFramebuffers");TARGET_LOAD(bind,"glBindFramebuffer");TARGET_LOAD(attach,"glFramebufferTexture2D");TARGET_LOAD(status,"glCheckFramebufferStatus");TARGET_LOAD(use,"glUseProgram");
    GLint old_fbo,old_mode,old_program;glGetIntegerv(0x8ca6,&old_fbo);glGetIntegerv(GL_MATRIX_MODE,&old_mode);glGetIntegerv(0x8b8d,&old_program);
    glPushAttrib(GL_ALL_ATTRIB_BITS);use(0);GLuint fbo,color;gen(1,&fbo);bind(0x8d40,fbo);glGenTextures(1,&color);glBindTexture(GL_TEXTURE_2D,color);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);attach(0x8d40,0x8ce0,GL_TEXTURE_2D,color,0);
    glDrawBuffer(0x8ce0);glReadBuffer(0x8ce0);if(status(0x8d40)!=0x8cd5)fail("Source GPU framebuffer incomplete.");
    uint8_t pixels[153600];
    glViewport(0,0,240,160);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_FOG);glDisable(GL_DITHER);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,.5f);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,240,160,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    draw_native_gpu_background(240,160,pitch,yaw);
    glEnable(GL_ALPHA_TEST);
    TaxiRenderQueue *q=frame->queue;taxi_render_queue_init(q,q->packets,q->capacity);
    if(visible&&taxi_render_queue_push(q,shadow->bucket,TAXI_RENDER_SHADOW,0))fail("Source GPU shadow queue failed.");
    for(size_t i=0;i<frame->count;i++)if(taxi_render_queue_push(q,frame->packets[i].bucket,frame->packets[i].kind,i))fail("Source GPU world queue failed.");
    TaxiShadowRender old_shadow=queued_shadow;queued_shadow=*shadow;int old_snap=native_axis_snapped_check;native_axis_snapped_check=1;
    QueueDraw draw={.frame=frame,.scale=1,.offset=0,.bitmap_coverage=1};if(taxi_render_queue_consume(q,draw_queue_packet,&draw))fail("Source GPU dispatch failed.");glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    memcpy(source_scene_gpu_pixels,pixels,sizeof pixels);source_scene_gpu_ready=1;
    unsigned mismatches=0,mismatches5=0;
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){unsigned i=y*240+x;uint16_t c=source_frame_cpu_palette[source_frame_cpu_indexed[i]];const uint8_t *a=pixels+4*((159-y)*240+x);int bad=0,bad5=0;for(unsigned j=0;j<3;j++){unsigned v=(c>>(5*j))&31;bad|=a[j]!=v*255/31;bad5|=(a[j]*31+127)/255!=v;}mismatches+=bad;mismatches5+=bad5;}
    char path[MAX_PATH];if(snprintf(path,sizeof path,"%s.gpu-scene.json",source_column_work_report_path)>=(int)sizeof path)fail("GPU report path failed.");FILE *out=fopen(path,"wb");if(!out)fail("GPU report unavailable.");fprintf(out,"{\"status\":\"%s\",\"rgb8_mismatch_pixels\":%u,\"rgb555_mismatch_pixels\":%u,\"scope\":\"Production GPU background scenery and shadow at source240; no taxi or whole-frame claim\"}\n",mismatches5?"MISMATCH_FOUND":"PASS_RGB555",mismatches,mismatches5);if(fclose(out))fail("GPU report close failed.");
    if(snprintf(path,sizeof path,"%s.gpu-scene.rgba",source_column_work_report_path)>=(int)sizeof path)fail("GPU pixels path failed.");out=fopen(path,"wb");if(!out||fwrite(pixels,1,sizeof pixels,out)!=sizeof pixels||fclose(out))fail("GPU pixels write failed.");
    queued_shadow=old_shadow;native_axis_snapped_check=old_snap;glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(old_mode);bind(0x8d40,(GLuint)old_fbo);glDeleteTextures(1,&color);del(1,&fbo);glPopAttrib();use((GLuint)old_program);
    if(glGetError()!=GL_NO_ERROR)fail("Source GPU state restoration error.");printf("Source GPU scene: RGB8 mismatches%u RGB555 mismatches%u\n",mismatches,mismatches5);
}

static void draw_shared_queue(int width,int height){
    queued_passenger_count=0;queued_passenger_marker_count=0;
    size_t capacity=(size_t)world.np+world.special_count+50+128+1;
    if(capacity>queued_capacity){
        free(queued_world);free(queued_packets);free(queued_normal_seen);free(queued_special_seen);
        queued_world=calloc(capacity,sizeof *queued_world);queued_packets=calloc(capacity,sizeof *queued_packets);
        queued_normal_seen=malloc(capacity);queued_special_seen=malloc(capacity);
        if(!queued_world||!queued_packets||!queued_normal_seen||!queued_special_seen)fail("Shared scenery queue allocation failed.");
        queued_capacity=capacity;
    }
    TaxiRenderQueue queue;taxi_render_queue_init(&queue,queued_packets,queued_capacity);
    double alpha=gameplay_render_alpha();
    TaxiPoint presented_taxi=presented_pose_valid?presented_pose.taxi:taxi_render_point(previous_position,car.world.height.position,alpha);
    TaxiCameraView presented_view=presented_camera_view(alpha);
    if(flag_scene_render_check){
        /* Explicit source-course rendering fixture; never changes simulation. */
        memcpy(&presented_taxi,live_flag_actor.bytes+16,12);
        presented_view.position=(TaxiPoint){presented_taxi.x,presented_taxi.y,presented_taxi.z-4000};
        presented_view.yaw=presented_view.pitch=0;
    }
    int32_t pitch_word=presented_view.pitch;
    int32_t left=(int32_t)floor(120.0-(double)width/height*80.0);
    /* Extend original viewport symmetrically; retain source center/focal/depth. */
    left-=((left%4)+4)%4;int32_t right=240-left;
    TaxiWorldFrame frame={.world=&world,
        .camera=presented_view.position,
        .yaw=presented_view.yaw,.pitch=pitch_word,.trig=&runtime.trig,
        .projection=native_scene_projection(),.left=left,.top=0,.right=right,.bottom=160,
        .texture_count=texture_count,.textures=&native_indexed_scene,.material_cache=native_materials.cache,
        .material_cache_size=512,.shade_depth=0,
        .queue=&queue,.packets=queued_world,.capacity=queued_capacity,
        .normal_seen=queued_normal_seen,.normal_seen_size=queued_capacity,
        .special_seen=queued_special_seen,.special_seen_size=queued_capacity};
    /* Normal preview: source155ee zero gate. Recorded/demo layer is separate. */
    int32_t shadow_ground;
    /* Original155C2 reads retained actor+E8C, initialized by template load.
       After the query155D8 retains ground minus192 for the next draw. */
    int32_t shadow_hint;memcpy(&shadow_hint,native_vehicle_start_owner.actor+0xe8c,4);
    TaxiPoint shadow_query={presented_taxi.x,shadow_hint,presented_taxi.z};
    if(taxi_world_asset_height(&world,shadow_query,&shadow_ground)<0)fail("Taxi shadow ground query failed.");
    uint32_t shadow_hint_word=(uint32_t)shadow_ground-192u;
    memcpy(native_vehicle_start_owner.actor+0xe88,&presented_taxi.x,4);
    memcpy(native_vehicle_start_owner.actor+0xe8c,&shadow_hint_word,4);
    memcpy(native_vehicle_start_owner.actor+0xe90,&presented_taxi.z,4);
    TaxiWorldRenderFrustum shadow_frustum=taxi_world_render_frustum(left,0,right,160);
    int shadow_rc=passenger_scene.session.audio_muted_1534?0:taxi_shadow_render(presented_taxi,shadow_ground,frame.camera,
        frame.yaw,frame.pitch,&runtime.trig,shadow_frustum.horizontal,shadow_frustum.vertical,&queued_shadow);
    if(shadow_rc<0)fail("Taxi shadow projection failed.");
    queued_shadow_frame=taxi_sprite_frame((int16_t)frame.yaw,
        presented_pose_valid?presented_pose.vehicle_angles.y:taxi_render_angle(previous_vehicle_heading,car.world.heading_1c,alpha));
    if(presented_pose_valid&&shadow_rc)timestamped_shadow_views++;
    if(shadow_rc&&shadow_textures[queued_shadow_frame]&&taxi_render_queue_push(&queue,queued_shadow.bucket,TAXI_RENDER_SHADOW,0))fail("Taxi shadow queue overflow.");
    if(passenger_diagnostic){
        TaxiWorldRenderFrustum frustum=taxi_world_render_frustum(left,0,right,160);
        if(passenger_scene.count>128)fail("Passenger queue count invalid.");
        const TaxiPassengerScene *presentation_passengers=presented_pose_valid?&presentation_passenger_history[presented_bracket.after_tag]:&passenger_scene;
        int retained_entries=presentation_passengers->render_entries_valid&&!live_attract&&
            !challenge_jump_drive_preview&&!challenge_bound_drive_preview&&!challenge_turn_drive_preview&&
            !challenge_ball_drive_preview&&!challenge_flag_drive_preview;
        for(unsigned i=0;i<presentation_passengers->count;i++){
            if(presented_pose_valid)timestamped_passenger_records++;
            if(retained_entries&&!presentation_passengers->render_eligible[i]){if(presented_pose_valid)timestamped_passenger_skips++;continue;}
            uint8_t record[44];memcpy(record,presentation_passengers->customers[i].bytes,44);
            const uint8_t *old=presented_pose_valid?presentation_passenger_history[presented_bracket.before_tag].customers[i].bytes:previous_customers[i].bytes;
            if(old[0]==record[0]&&old[1]==record[1]&&old[10]==record[10]){
                TaxiPoint a,b;memcpy(&a,old+16,12);memcpy(&b,record+16,12);
                TaxiPoint p=taxi_render_point(a,b,presented_pose_valid?presented_bracket.alpha:alpha);memcpy(record+16,&p,12);
            }
            int rc=(retained_entries?taxi_passenger_render_qualified_frustum:taxi_passenger_render_frustum)(record,
                presented_taxi,(int32_t)(((uint32_t)(presented_pose_valid?presented_motion_heading:taxi_render_angle(previous_motion_heading,car.world.heading_20,alpha))>>12)&4095),
                frame.camera,frame.yaw,frame.pitch,0,&runtime.trig,
                frustum.horizontal,frustum.vertical,queued_passengers+i);
            if(presented_pose_valid){timestamped_passenger_attempts++;if(!rc)timestamped_passenger_culled++;}
            if(passenger_presentation_trace){for(unsigned b=0;b<44;b++)fprintf(passenger_presentation_trace,"%02x",record[b]);fprintf(passenger_presentation_trace," %d %d %d %d %d %d %d %d %d %d %d %d %d\n",retained_entries,presented_taxi.x,presented_taxi.y,presented_taxi.z,(int32_t)(((uint32_t)(presented_pose_valid?presented_motion_heading:car.world.heading_20)>>12)&4095),frame.camera.x,frame.camera.y,frame.camera.z,frame.yaw,frame.pitch,frustum.horizontal,frustum.vertical,rc);}
            if(rc<0)fail("Original passenger frame preparation failed.");
            if(rc&&taxi_render_queue_push(&queue,queued_passengers[i].bucket,TAXI_RENDER_PASSENGER,i))
                fail("Shared passenger queue overflow.");
            if(rc){queued_passenger_count++;if(presented_pose_valid)timestamped_passenger_views++;}
            if(!challenge_jump_drive_preview&&!live_attract&&!challenge_bound_drive_preview&&
                !challenge_turn_drive_preview&&!challenge_ball_drive_preview&&!challenge_flag_drive_preview&&
                (record[0]&3)==1&&record[1]<8){
                TaxiPoint customer;memcpy(&customer,record+16,12);
                int64_t dx=(int64_t)presented_taxi.x-customer.x,dz=(int64_t)presented_taxi.z-customer.z;
                if(retained_entries||(dx>=-12799&&dx<=12799&&dz>=-12799&&dz<=12799)){
                    TaxiPassengerState actor;memcpy(actor.bytes,record,44);
                    TaxiPassengerMarkerRequest requests[2];unsigned count;
                    if(taxi_passenger_marker_requests(&actor,presentation_passengers->pickup.pending_1704,
                        presentation_passengers->session.trip.active_1694,presentation_passengers->marker_phase.frame_5f29,requests,&count))fail("Passenger marker requests failed.");
                    for(unsigned j=0;j<count;j++){
                        if(queued_passenger_marker_count>=256)fail("Passenger marker storage overflow.");
                        unsigned index=queued_passenger_marker_count;
                        int visible=taxi_passenger_marker_render_frustum(requests+j,frame.camera,frame.yaw,frame.pitch,
                            &runtime.trig,frustum.horizontal,frustum.vertical,queued_passenger_markers+index);
                        if(visible<0)fail("Passenger marker projection failed.");
                        if(visible){queued_passenger_marker_count++;
                            if(taxi_render_queue_push(&queue,queued_passenger_markers[index].bucket,TAXI_RENDER_PASSENGER_MARKER,index))fail("Passenger marker queue overflow.");}
                    }
                }
            }
        }
    }
    if(challenge_ball_drive_preview){
        TaxiPassengerState presented[20];memcpy(presented,live_balls,sizeof presented);
        for(unsigned i=0;i<20;i++)if(previous_balls[i].bytes[10]==presented[i].bytes[10]){
            TaxiPoint old,current;memcpy(&old,previous_balls[i].bytes+16,12);memcpy(&current,presented[i].bytes+16,12);
            current=taxi_render_point(old,current,alpha);memcpy(presented[i].bytes+16,&current,12);
        }
        TaxiWorldRenderFrustum frustum=taxi_world_render_frustum(left,0,right,160);uint32_t visible;
        if(taxi_challenge_ball_course_render(&live_ball_course,presented,presented_taxi,frame.camera,frame.yaw,frame.pitch,frustum.horizontal,frustum.vertical,1,0,0,&runtime.trig,queued_balls,&visible))fail("Ball liveframe failed.");
        ball_palette_upload(live_ball_course.balloon_indices,native_scene_palette,city_fade_cap);
        for(unsigned i=0;i<20;i++)if(visible&(1u<<i)){
            if(taxi_render_queue_push(&queue,queued_balls[i].bucket,TAXI_RENDER_BALL,i))fail("Ball livequeue overflow.");live_ball_submissions++;
        }
    }
    if(challenge_flag_drive_preview&&!flag_scene_mask){
        int32_t x,z,ground_y;memcpy(&x,live_flag_actor.bytes+16,4);memcpy(&z,live_flag_actor.bytes+24,4);
        TaxiPoint query={x,0,z};memcpy(&query.y,live_flag_actor.bytes+20,4);
        int ground=taxi_world_asset_height(&world,query,&ground_y);
        if(ground<0)fail("Flag renderer ground query failed.");
        TaxiChallengeFlagRenderRequest request;TaxiWorldRenderFrustum frustum=taxi_world_render_frustum(left,0,right,160);
        int rc=ground?taxi_challenge_flag_render_request(live_flag_actor.bytes,presented_taxi.x,presented_taxi.z,ground_y,live_flag_timer.scale_1554,0,&request):0;
        if(rc<0)fail("Flag render request failed.");
        if(rc)rc=taxi_challenge_flag_project(&request,frame.camera,frame.yaw,frame.pitch,frustum.horizontal,frustum.vertical,&runtime.trig,&queued_flag);
        if(rc<0)fail("Flag scene projection failed.");
        if(rc){
            flag_palette_upload(live_flag_indices,native_scene_palette,flag_scene_render_check?31:city_fade_cap);
            if(taxi_render_queue_push(&queue,queued_flag.bucket,TAXI_RENDER_FLAG,0))fail("Flag scene queue overflow.");
            flag_scene_submissions++;
        }
    }
    const TaxiTrafficActor *traffic_before=previous_traffic_actors,*traffic_after=traffic_actors;
    const TaxiTrafficSubmission *presentation_submissions=traffic_submissions;unsigned presentation_traffic_count=traffic_submission_count;double traffic_alpha=alpha;
    if(presented_pose_valid){traffic_before=presentation_traffic_history[presented_bracket.before_tag];traffic_after=presentation_traffic_history[presented_bracket.after_tag];presentation_submissions=presentation_traffic_submissions[presented_bracket.after_tag];presentation_traffic_count=presentation_traffic_counts[presented_bracket.after_tag];traffic_alpha=presented_bracket.alpha;}
    TaxiTrafficSubmission render_traffic[50];memcpy(render_traffic,presentation_submissions,sizeof render_traffic);
    for(unsigned i=0;i<presentation_traffic_count;i++)if(presentation_submissions[i].prepared){
        TaxiTrafficActor actor=taxi_render_traffic_actor(traffic_before+i,traffic_after+i,traffic_alpha);
        if(presented_pose_valid)timestamped_traffic_views++;
        TaxiTrafficSubmission *r=render_traffic+i;
        if(taxi_traffic_view(&actor,frame.camera,frame.yaw,frame.pitch,&runtime.trig,&runtime.traction,&r->view))fail("Traffic presentation view failed.");
        r->billboard=taxi_traffic_billboard(r->view.camera_space,r->scale);
        int rc=taxi_traffic_project(&r->billboard,&frame.projection,&r->screen);
        if(rc<0)fail("Traffic presentation projection failed.");
        if(!rc)continue;
        if(taxi_traffic_depth_bucket(r->view.camera_space.z,4608,frame.projection.far_plane,&runtime.trig,&r->depth_bucket))fail("Traffic presentation bucket failed.");
        if(taxi_render_queue_push(&queue,r->depth_bucket,TAXI_RENDER_TRAFFIC,i))fail("Shared traffic queue overflow.");
    }
    native_world_cache_bind(&frame,1);
    if(native_cache_preview_pending&&frame.generation!=native_cache_preview_generation)
        fail("Main frame did not reuse restored preview generation.");
    if(taxi_world_frame_build(&frame))fail("Original scenery frame preparation failed.");
    if(native_axis_source_check&&!native_flat_source_valid){
        for(size_t i=0;i<frame.count;i++){uint8_t scratch[38400]={0};TaxiFlatRowWork work;
            if(taxi_world_indexed_flat_packet_work(scratch,frame.packets+i,&world,&work)==1&&(work.framebuffer_halfword_writes||work.framebuffer_word_writes)){native_flat_source_packet=frame.packets[i];native_flat_source_valid=1;break;}
        }
    }
    if(source_column_work_report_path&&!source_column_work_report_done){
        uint8_t source_fb[38400]={0};uint64_t reads=0,writes=0;unsigned packets=0,visible=0,row_packets=0,row_skipped=0,clipped_packets=0,perspective_packets=0,flat_packets=0;uint64_t flat_half=0,flat_word=0;
        uint32_t callbacks[32]={0};unsigned callback_counts[32]={0},culled_counts[32]={0},callback_count=0;

        for(size_t i=0;i<frame.count;i++){
            const TaxiWorldFramePacket *p=frame.packets+i;uint32_t callback=taxi_world_indexed_callback(p,&world);
            unsigned bucket=0;while(bucket<callback_count&&callbacks[bucket]!=callback)bucket++;
            if(bucket==callback_count){if(callback_count>=32)fail("Source callback workload capacity exceeded.");callbacks[callback_count++]=callback;}
            callback_counts[bucket]++;culled_counts[bucket]+=(p->draw_culled!=0);
            if(callback==0x03006950){TaxiFlatRowWork flat;
                if(taxi_world_indexed_flat_packet_work(source_fb,p,&world,&flat)!=1){fprintf(stderr,"Flat work packet%zu kind%u flags%u vertices%u material%u callback%08x\n",i,(unsigned)p->kind,(unsigned)p->raster_flags,(unsigned)p->polygon.count,(unsigned)p->material,callback);for(unsigned v=0;v<p->polygon.count;v++)fprintf(stderr,"flat vertex%u %ld,%ld\n",v,(long)p->polygon.vertices[v].screen_x,(long)p->polygon.vertices[v].screen_y);fail("Source flat packet workload failed.");}
                flat_packets++;if(flat.framebuffer_halfword_writes||flat.framebuffer_word_writes)visible++;
                flat_half+=flat.framebuffer_halfword_writes;flat_word+=flat.framebuffer_word_writes;continue;
            }
            int row_family=callback==0x03006948;
            int clipped_family=callback==0x03006914;
            int perspective_family=callback==0x03006934||callback==0x03006944;
            if(!row_family&&!clipped_family&&!perspective_family&&callback!=0x030069ac&&callback!=0x030069b8)continue;
            if(row_family&&!p->draw_culled){int inside=1;for(unsigned v=0;v<p->polygon.count;v++){const TaxiWorldClipVertex *vertex=p->polygon.vertices+v;if(vertex->screen_x<0||vertex->screen_x>240||vertex->screen_y<0||vertex->screen_y>=160)inside=0;}if(!inside){row_skipped++;continue;}}

            TaxiPairColumnWork work;
            if(taxi_world_indexed_packet_work(source_fb,p,&world,&native_indexed_scene,native_scene_lookup,0,0,&work)!=1)fail("Source column packet workload failed.");
            if(row_family)row_packets++;else if(clipped_family)clipped_packets++;else if(perspective_family)perspective_packets++;else packets++;if(work.texture_byte_reads)visible++;reads+=work.texture_byte_reads;writes+=work.framebuffer_halfword_writes;
        }
        FILE *report=fopen(source_column_work_report_path,"wb");if(!report)fail("Source column workload report cannot open.");
        fprintf(report,"{\"scope\":\"Actual native prepared scene packets clipped to240x160; column, perspective row, clipped flat, clipped row and source-inbounds ordinary row families only, not original full-frame or CPU cycle proof\",\"scene_packets\":%zu,\"column_packets\":%u,\"measured_row_packets\":%u,\"clipped_row_packets\":%u,\"perspective_row_packets\":%u,\"flat_packets\":%u,\"flat_halfword_writes\":%llu,\"flat_word_writes\":%llu,\"source_row_bounds_skipped\":%u,\"visible_measured_packets\":%u,\"texture_byte_reads\":%llu,\"framebuffer_halfword_writes\":%llu,\"callbacks\":[",frame.count,packets,row_packets,clipped_packets,perspective_packets,flat_packets,(unsigned long long)flat_half,(unsigned long long)flat_word,row_skipped,visible,(unsigned long long)reads,(unsigned long long)writes);
        for(unsigned bucket=0;bucket<callback_count;bucket++)fprintf(report,"%s{\"callback\":\"%08x\",\"packets\":%u,\"draw_culled\":%u}",bucket?",":"",callbacks[bucket],callback_counts[bucket],culled_counts[bucket]);
        fputs("],\"special_inputs\":[",report);unsigned special_records=0,special_qualified=0,special_unqualified=0,special_visible=0;uint64_t special_reads=0,special_writes=0;
        for(size_t i=0;i<frame.count;i++){const TaxiWorldFramePacket *p=frame.packets+i;uint32_t cb=taxi_world_indexed_callback(p,&world);
            if(p->kind!=TAXI_RENDER_SPECIAL||(cb!=0x030069a4&&cb!=0x03006a28))continue;
            TaxiIndexedTexture texture;if(taxi_indexed_asset_texture(&native_indexed_scene,p->texture,&texture))fail("Special workload input texture unavailable.");
            uint8_t scratch[38400]={0};TaxiPairColumnWork work={0};int result=taxi_world_indexed_packet_work(scratch,p,&world,&native_indexed_scene,native_scene_lookup,0,0,&work);
            if(result==1){special_qualified++;special_visible+=(work.texture_byte_reads!=0);special_reads+=work.texture_byte_reads;special_writes+=work.framebuffer_halfword_writes;}else special_unqualified++;
            fprintf(report,"%s{\"packet\":%zu,\"callback\":\"%08x\",\"texture\":%u,\"width\":%u,\"height\":%u,\"log_width\":%u,\"rect\":[%ld,%ld,%ld,%ld],\"draw_culled\":%u,\"native_source_result\":%d,\"texture_byte_reads\":%u,\"framebuffer_halfword_writes\":%u}",special_records++?",":"",i,cb,(unsigned)p->texture,texture.width,texture.height,texture.log_width,(long)p->special.screen.left,(long)p->special.screen.top,(long)p->special.screen.right,(long)p->special.screen.bottom,(unsigned)p->draw_culled,result,work.texture_byte_reads,work.framebuffer_halfword_writes);
        }
        fprintf(report,"],\"qualified_special_packets\":%u,\"unqualified_special_packets\":%u,\"qualified_special_visible_packets\":%u,\"special_texture_byte_reads\":%llu,\"special_framebuffer_halfword_writes\":%llu,\"combined_qualified_texture_byte_reads\":%llu,\"combined_qualified_framebuffer_halfword_writes\":%llu}\n",special_qualified,special_unqualified,special_visible,(unsigned long long)special_reads,(unsigned long long)special_writes,(unsigned long long)(reads+special_reads),(unsigned long long)(writes+special_writes));
        char source_path[MAX_PATH];if(snprintf(source_path,sizeof source_path,"%s.source240.json",source_column_work_report_path)>=(int)sizeof source_path)fail("Source240 capture path too long.");
        TaxiWorldFrame source_frame=frame;
        source_frame.left=0;source_frame.right=240;source_frame.top=0;source_frame.bottom=160;
        source_frame.bounds_corners=NULL;source_frame.normal_stamps=NULL;source_frame.special_stamps=NULL;
        source_frame.packets=calloc(frame.capacity,sizeof *source_frame.packets);
        source_frame.normal_seen=calloc(frame.normal_seen_size,1);source_frame.special_seen=calloc(frame.special_seen_size,1);
        TaxiRenderPacket *source_storage=calloc(frame.capacity,sizeof *source_storage);TaxiRenderQueue source_queue;
        if(!source_frame.packets||!source_frame.normal_seen||!source_frame.special_seen||!source_storage)fail("Source240 diagnostic allocation failed.");
        taxi_render_queue_init(&source_queue,source_storage,frame.capacity);source_frame.queue=&source_queue;
        TaxiShadowRender cpu_shadow_view={0};int cpu_shadow_visible=0;
        if(source_frame_cpu_requested){
            cpu_shadow_visible=passenger_scene.session.audio_muted_1534?0:taxi_shadow_render(presented_taxi,shadow_ground,source_frame.camera,source_frame.yaw,source_frame.pitch,&runtime.trig,160,106,&cpu_shadow_view);
            if(cpu_shadow_visible<0)fail("Source240 CPU shadow projection failed.");
            if(cpu_shadow_visible&&taxi_render_queue_push(&source_queue,cpu_shadow_view.bucket,TAXI_RENDER_SHADOW,0))fail("Source240 CPU shadow queue failed.");
        }
        if(taxi_world_frame_build(&source_frame))fail("Source240 diagnostic preparation failed.");
        FILE *source_report=fopen(source_path,"wb");if(!source_report)fail("Source240 capture cannot open.");
        fprintf(source_report,"{\"scope\":\"Isolated native original-width scenery preparation with source viewport and computed bounds; same presented camera/material phase, fresh seen sets; not original full producer or cache lifecycle proof\",\"camera\":[%ld,%ld,%ld],\"yaw\":%ld,\"pitch\":%ld,\"phase\":%u,\"packets\":[",(long)source_frame.camera.x,(long)source_frame.camera.y,(long)source_frame.camera.z,(long)source_frame.yaw,(long)source_frame.pitch,(unsigned)source_frame.phase);
        for(size_t si=0;si<source_frame.count;si++){const TaxiWorldFramePacket *sp=source_frame.packets+si;
            fprintf(source_report,"%s{\"source_index\":%u,\"kind\":%u,\"callback\":\"%08x\",\"raster_flags\":%u,\"draw_culled\":%u,\"material\":%u,\"texture\":%u,\"bucket\":%u,\"rect\":[%ld,%ld,%ld,%ld]",si?",":"",(unsigned)sp->source_index,(unsigned)sp->kind,taxi_world_indexed_callback(sp,&world),(unsigned)sp->raster_flags,(unsigned)sp->draw_culled,(unsigned)sp->material,(unsigned)sp->texture,(unsigned)sp->bucket,(long)sp->special.screen.left,(long)sp->special.screen.top,(long)sp->special.screen.right,(long)sp->special.screen.bottom);
            fputs(",\"vertices\":[",source_report);
            for(unsigned vi=0;vi<sp->polygon.count;vi++){const TaxiWorldClipVertex *v=sp->polygon.vertices+vi;
                fprintf(source_report,"%s[%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld]",vi?",":"",(long)v->screen_x,(long)v->screen_y,(long)v->x,(long)v->y,(long)v->z,(long)v->u,(long)v->v,(long)v->attribute);
            }
            uint8_t packet_fb[38400]={0};TaxiPairColumnWork textured_work={0};TaxiFlatRowWork flat_work={0};
            int work_result=taxi_world_indexed_callback(sp,&world)==0x03006950?
                taxi_world_indexed_flat_packet_work(packet_fb,sp,&world,&flat_work):
                taxi_world_indexed_packet_work(packet_fb,sp,&world,&native_indexed_scene,native_scene_lookup,0,0,&textured_work);
            fprintf(source_report,"],\"work_result\":%d,\"texture_byte_reads\":%u,\"textured_halfword_writes\":%u,\"flat_halfword_writes\":%u,\"flat_word_writes\":%u}",work_result,textured_work.texture_byte_reads,textured_work.framebuffer_halfword_writes,flat_work.framebuffer_halfword_writes,flat_work.framebuffer_word_writes);
        }
        fputs("]}\n",source_report);if(fclose(source_report))fail("Source240 capture close failed.");
        if(source_frame_cpu_requested){
            TaxiShadow *cpu_shadow=malloc(sizeof *cpu_shadow);if(!cpu_shadow)fail("Source240 CPU shadow allocation failed.");
            char asset[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,asset,MAX_PATH);if(!n||n>=MAX_PATH)fail("Source240 CPU asset path failed.");char *tail=strrchr(asset,92);if(!tail)fail("Source240 CPU asset path failed.");
            snprintf(tail+1,MAX_PATH-(size_t)(tail+1-asset),"../assets/taxi-software/taxi-software.ctx");if(taxi_shadow_load(cpu_shadow,asset))fail("Source240 CPU shadow bank failed.");
            TaxiWorldCpuWork work;
            if(taxi_world_cpu_scene(source_frame_cpu_indexed,&source_frame,native_scene_registration.background,native_scene_lookup,cpu_shadow,cpu_shadow_visible?&cpu_shadow_view:NULL,queued_shadow_frame,TAXI_RENDER_SHADOW,&work))fail("Source240 CPU frame failed.");
            free(cpu_shadow);memcpy(source_frame_cpu_palette,native_scene_palette,sizeof source_frame_cpu_palette);source_frame_cpu_valid=1;
            if(source_scene_gpu_requested)check_source_scene_gpu(&source_frame,&cpu_shadow_view,cpu_shadow_visible);
            char path[MAX_PATH];if(snprintf(path,sizeof path,"%s.cpu-frame-indexed.bin",source_column_work_report_path)>=(int)sizeof path)fail("Source240 CPU output path failed.");FILE *out=fopen(path,"wb");if(!out)fail("Source240 CPU output unavailable.");if(fwrite(source_frame_cpu_indexed,1,sizeof source_frame_cpu_indexed,out)!=sizeof source_frame_cpu_indexed||fclose(out))fail("Source240 CPU output failed.");
            if(snprintf(path,sizeof path,"%s.cpu-frame.json",source_column_work_report_path)>=(int)sizeof path)fail("Source240 CPU metadata path failed.");out=fopen(path,"wb");if(!out)fail("Source240 CPU metadata unavailable.");
            fprintf(out,"{\"camera\":[%ld,%ld,%ld],\"yaw\":%ld,\"pitch\":%ld,\"shadow_ground\":%ld,\"shadow_frame\":%u,\"packets\":%u,\"work\":[%u,%u,%u,%u],\"scope\":\"Native live source240 CPU scenery and shadow reference; other actors/HUD/hardware priority/full original frame and cadence unverified\"}\n",(long)source_frame.camera.x,(long)source_frame.camera.y,(long)source_frame.camera.z,(long)source_frame.yaw,(long)source_frame.pitch,(long)shadow_ground,queued_shadow_frame,work.packets,work.textured.texture_byte_reads,work.textured.framebuffer_halfword_writes,work.flat.framebuffer_halfword_writes,work.flat.framebuffer_word_writes);if(fclose(out))fail("Source240 CPU metadata close failed.");
        }
        free(source_frame.packets);free(source_frame.normal_seen);free(source_frame.special_seen);free(source_storage);
        if(fclose(report))fail("Source column workload report cannot close.");source_column_work_report_done=1;
        printf("NATIVE SOURCE RASTER WORK: %u measured packets (%u with source240 coverage), %llu texture reads, %llu halfword stores; diagnostic native packet workload, not CPU cycles.\n",packets+row_packets+clipped_packets+perspective_packets+flat_packets,visible,(unsigned long long)reads,(unsigned long long)writes);
    }
    if(native_cache_preview_pending){printf("CACHE MAIN: reused preview generation%u; prepared%zu normal/special packets\n",frame.generation,frame.count);native_cache_preview_pending=0;}
    QueueDraw context={.frame=&frame,.scale=(float)height/160,.offset=(float)width/2-120*(float)height/160,.traffic=render_traffic,.bitmap_coverage=1};
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    if(taxi_render_queue_consume(&queue,draw_queue_packet,&context))fail("Shared scenery draw failed.");
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
/* Diagnostic geometry only: verifies the production queue dispatch on GPU,
   not source city geometry or complete gameplay occlusion fidelity. */
static void check_queue_occlusion(int width,int height){
    if(!queued_world||!traffic_textures[0]||city_fade_cap!=31)fail("Queue GPU check requires loaded normal city preview.");
    Texture *source=NULL;
    for(unsigned i=0;i<sprite_source_count;i++)if(sprite_sources[i].id==traffic_textures[0])source=sprite_sources+i;
    if(!source)fail("Traffic GPU source missing for queue check.");
    size_t samples[2]={SIZE_MAX,SIZE_MAX};
    for(size_t i=0;i<(size_t)source->width*source->height;i++){
        unsigned category=source->pixels[4*i+3]?1:0;
        if(samples[category]==SIZE_MAX)samples[category]=i;
    }
    if(samples[0]==SIZE_MAX||samples[1]==SIZE_MAX)fail("Traffic alpha fixture lacks both sample types.");
    unsigned char old_color[4];memcpy(old_color,materials[0].color,4);
    const unsigned char color[4]={223,31,97,255};memcpy(materials[0].color,color,4);
    unsigned old_count=traffic_submission_count;TaxiTrafficSubmission old=traffic_submissions[0];
    traffic_submission_count=1;memset(traffic_submissions,0,sizeof *traffic_submissions);
    traffic_submissions[0].screen=(TaxiTrafficScreenBounds){0,0,240,160};
    TaxiWorldFramePacket saved_packet=queued_world[0];queued_world[0]=(TaxiWorldFramePacket){.material=0,.texture=255,.kind=TAXI_RENDER_WORLD};
    queued_world[0].polygon.count=4;
    int32_t x[4]={0,240,240,0},y[4]={0,0,160,160};
    for(unsigned i=0;i<4;i++){queued_world[0].polygon.vertices[i].screen_x=x[i];queued_world[0].polygon.vertices[i].screen_y=y[i];}
    TaxiWorldFrame frame={.world=&world,.packets=queued_world,.count=1};QueueDraw context={.frame=&frame,.scale=(float)height/160,.offset=(float)width/2-120*(float)height/160};
    glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    int saved_snapped=native_axis_snapped_check;native_axis_snapped_check=1;
    TaxiPassengerRender saved_passenger=queued_passengers[0];
    unsigned saved_customer_count=passenger_scene.count;
    TaxiShadowRender saved_shadow=queued_shadow;unsigned saved_shadow_frame=queued_shadow_frame;
    for(unsigned sprite_mode=0;sprite_mode<4;sprite_mode++){
        if(!passenger_diagnostic&&(sprite_mode==1||sprite_mode==2))continue;
        unsigned sprite_kind=sprite_mode==3?TAXI_RENDER_SHADOW:sprite_mode?TAXI_RENDER_PASSENGER:TAXI_RENDER_TRAFFIC;
        if(sprite_mode){
            source=NULL;
            GLuint id=sprite_mode==3?shadow_textures[0]:passenger_textures[0];
            for(unsigned i=0;i<sprite_source_count;i++)if(sprite_sources[i].id==id)source=sprite_sources+i;
            if(!source)fail("Passenger/shadow GPU source missing.");
            samples[0]=samples[1]=SIZE_MAX;
            for(size_t i=0;i<(size_t)source->width*source->height;i++){
                unsigned category=source->pixels[4*i+3]?1:0;
                if(samples[category]==SIZE_MAX)samples[category]=i;
            }
            if(samples[0]==SIZE_MAX||samples[1]==SIZE_MAX)fail("Passenger/shadow alpha fixture lacks samples.");
            if(sprite_mode==3){queued_shadow.screen=(TaxiTrafficScreenBounds){0,0,240,160};queued_shadow_frame=0;}
            passenger_scene.count=1;
            queued_passengers[0]=(TaxiPassengerRender){.screen={0,0,240,160},.flip=sprite_mode==2};
            for(unsigned i=0;i<4;i++){
                queued_passengers[0].u[i]=((i<2)^(sprite_mode==2))?0:(32<<14);
                queued_passengers[0].v[i]=(i==0||i==3)?(64<<14):0;
            }
        }
    for(unsigned mode=0;mode<4;mode++){
        TaxiRenderPacket storage[2];TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,2);
        unsigned world_bucket=mode==0?20:80,traffic_bucket=mode==1?20:80;
        if(mode==3){taxi_render_queue_push(&queue,traffic_bucket,sprite_kind,0);taxi_render_queue_push(&queue,world_bucket,TAXI_RENDER_WORLD,0);}
        else {taxi_render_queue_push(&queue,world_bucket,TAXI_RENDER_WORLD,0);taxi_render_queue_push(&queue,traffic_bucket,sprite_kind,0);}
        if(taxi_render_queue_consume(&queue,draw_queue_packet,&context))fail("Queue GPU fixture dispatch failed.");
        glFinish();
        for(unsigned category=0;category<2;category++){
            size_t pixel=samples[category];
            int px=(int)floor(context.offset+((sprite_mode==2?source->width-1-pixel%source->width:pixel%source->width)+0.5)*240/source->width*context.scale);
            int py=(int)floor(((pixel/source->width)+0.5)*160/source->height*context.scale);
            unsigned char actual[4];glReadPixels(px,height-1-py,1,1,GL_RGBA,GL_UNSIGNED_BYTE,actual);
            const unsigned char *sample=source->pixels+4*pixel;
            {
                TaxiIndexedSpan oracle[160];TaxiPassengerRender *p=queued_passengers;
                if(sprite_mode==2){
                    TaxiIndexedVertex v[4]={{0,160,(uint32_t)p->u[0],(uint32_t)p->v[0],0},
                        {0,0,(uint32_t)p->u[1],(uint32_t)p->v[1],0},
                        {240,0,(uint32_t)p->u[2],(uint32_t)p->v[2],0},
                        {240,160,(uint32_t)p->u[3],(uint32_t)p->v[3],0}};
                    if(!taxi_indexed_affine_rows_bounds(oracle,v,4,0,240,1))fail("Passenger affine oracle failed.");
                }else if(!taxi_indexed_billboard_rows_bounds(oracle,0,0,240,160,source->width,source->height,0,240,1,0))
                    fail("Passenger rectangle oracle failed.");
                int sy=(int)((py+0.5)/context.scale);
                int sx=(int)((px+0.5-context.offset)/context.scale)&~1;
                TaxiIndexedSpan row=oracle[sy];unsigned pairs=(unsigned)(sx-row.x)/2;
                uint32_t u=row.u+pairs*row.du,v=row.v+pairs*row.dv;
                unsigned index=((v>>14)&(source->height-1))*source->width+((u>>14)&(source->width-1));
                sample=source->pixels+4*index;
            }
            const unsigned char *expected=(sample[3]&&(mode==1||mode==3))?sample:color;
            if(memcmp(actual,expected,3)){fprintf(stderr,"Queue mismatch sprite%u order%u category%u xy%d,%d actual%u,%u,%u expected%u,%u,%u\n",sprite_mode,mode,category,px,py,actual[0],actual[1],actual[2],expected[0],expected[1],expected[2]);fail("Shared queue GPU occlusion/alpha/order mismatch.");}
        }
    }
    }
    native_axis_snapped_check=saved_snapped;queued_passengers[0]=saved_passenger;passenger_scene.count=saved_customer_count;
    queued_shadow=saved_shadow;queued_shadow_frame=saved_shadow_frame;
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    queued_world[0]=saved_packet;traffic_submissions[0]=old;traffic_submission_count=old_count;memcpy(materials[0].color,old_color,4);
    puts(passenger_diagnostic?
        "QUEUE GPU: traffic, shadow and both passenger facings: farther/nearer, equal-bucket insertion orders and transparent texels pixel-exact PASS; diagnostic geometry, not whole-game fidelity.":
        "QUEUE GPU: traffic and shadow farther/nearer, equal-bucket insertion orders and transparent texels pixel-exact PASS; diagnostic geometry, not whole-game fidelity.");
}
/* Diagnostic test-driver inputs only; recovered physics/passenger state
   remains untouched. Same steering/brake navigation as snapshot pickup test. */
/* Input-only live test driver; route and vehicle/session state are read-only. */
static void live_delivery_input(void){
 int32_t destination=passenger_scene.pickup.destination_index;if(destination<0||(unsigned)destination>=passenger_asset.destination_count)fail("Live delivery destination missing.");const uint8_t *record=passenger_asset.destinations[destination];
 if(!live_delivery_route){int selected=passenger_scene.pickup.selected_index;if(selected<0||(unsigned)selected>=passenger_scene.count)fail("Live delivery selected customer missing.");TaxiRoutePoint origin=passenger_scene.spawns[selected];
  for(unsigned i=0;i<passenger_asset.route_count;i++){const TaxiPassengerRouteAsset *candidate=passenger_asset.routes+i;if(!candidate->count||candidate->points[0].x!=origin.x||candidate->points[0].z!=origin.z)continue;TaxiRoutePoint endpoint=candidate->points[candidate->count-1];TaxiGroundHit hit={0};int found=taxi_world_asset_ground(&world,(TaxiPoint){endpoint.x,0,endpoint.z},&hit);if(found<0)fail("Live delivery endpoint query failed.");if(found&&taxi_destination_accepts(destination,record,hit.material)){live_delivery_route=candidate;break;}}
  if(!live_delivery_route)fail("Live delivery original route missing.");live_delivery_waypoint=live_delivery_route->count>1?1:0;printf("LIVE DELIVERY ROUTE: source%u points%u destination%ld; input-only navigation.\n",live_delivery_route->source_index,live_delivery_route->count,(long)destination);
 }
 TaxiRoutePoint point=live_delivery_route->points[live_delivery_waypoint];int64_t dx=(int64_t)point.x-car.world.height.position.x,dz=(int64_t)point.z-car.world.height.position.z;
 while(live_delivery_waypoint+1<live_delivery_route->count){int reached=dx*dx+dz*dz<1024*1024;
  if(!reached&&live_delivery_waypoint){TaxiRoutePoint previous=live_delivery_route->points[live_delivery_waypoint-1];double sx=(double)point.x-previous.x,sz=(double)point.z-previous.z,px=(double)car.world.height.position.x-previous.x,pz=(double)car.world.height.position.z-previous.z,length=sx*sx+sz*sz,cross=px*sz-pz*sx;reached=length>0&&px*sx+pz*sz>=length&&cross*cross<=4096.0*4096*length;}
  if(!reached)break;point=live_delivery_route->points[++live_delivery_waypoint];dx=(int64_t)point.x-car.world.height.position.x;dz=(int64_t)point.z-car.world.height.position.z;
 }
 /* Diagnostic navigation only: steer toward the current route segment,
    not a distant endpoint that can cut across recovered raised terrain. */
 int64_t endpoint_distance_squared=dx*dx+dz*dz;double route_lateral=0;
 if(live_delivery_waypoint){TaxiRoutePoint previous=live_delivery_route->points[live_delivery_waypoint-1];double sx=(double)point.x-previous.x,sz=(double)point.z-previous.z,length=sqrt(sx*sx+sz*sz);
  if(length>0){double px=(double)car.world.height.position.x-previous.x,pz=(double)car.world.height.position.z-previous.z;double along=(px*sx+pz*sz)/length;route_lateral=(px*sz-pz*sx)/length;
   double lookahead=1024.0+fmax(0,car.world.speed_544)/32.0;if(lookahead>4096)lookahead=4096;along=fmax(0,fmin(length,along+lookahead));
   dx=(int64_t)llround(previous.x+sx*along/length)-car.world.height.position.x;dz=(int64_t)llround(previous.z+sz*along/length)-car.world.height.position.z;
  }
 }
 int desired=0;int64_t alignment=INT64_MIN;for(int angle=0;angle<4096;angle++){int64_t dot=-(int64_t)runtime.trig.sine[angle]*dx+(int64_t)runtime.trig.cosine[angle]*dz;if(dot>alignment){alignment=dot;desired=angle;}}
 int current=(int)((uint32_t)car.world.heading_20>>12)&4095,difference=((desired-current+2048)&4095)-2048;
 int approaching_bend=0;
 if(live_delivery_waypoint+1<live_delivery_route->count&&endpoint_distance_squared<(int64_t)8192*8192){TaxiRoutePoint next=live_delivery_route->points[live_delivery_waypoint+1];int64_t nx=(int64_t)next.x-point.x,nz=(int64_t)next.z-point.z;int next_angle=0;int64_t best_dot=INT64_MIN;for(int angle=0;angle<4096;angle++){int64_t dot=-(int64_t)runtime.trig.sine[angle]*nx+(int64_t)runtime.trig.cosine[angle]*nz;if(dot>best_dot){best_dot=dot;next_angle=angle;}}int next_difference=((next_angle-current+2048)&4095)-2048;approaching_bend=abs(next_difference)>256;}
 int destination_brake=taxi_destination_accepts(destination,record,(uint32_t)(int32_t)(int16_t)car.world.material_e60);route_probe_brake=destination_brake||((abs(difference)>256||approaching_bend||fabs(route_lateral)>1024)&&car.world.speed_544>40000);script_steer=destination_brake?0:difference>32?-1:difference < -32?1:0;route_probe_analog_command=0;
 if(!destination_brake&&abs(difference)>32){int magnitude=abs(difference)*192/512;if(magnitude>192)magnitude=192;int speed=car.world.speed_544;if(speed<0)speed=0;if(speed>60000)speed=60000;magnitude=(int)((int64_t)magnitude*speed/60000);if(magnitude<8)magnitude=8;route_probe_analog_command=difference>0?magnitude:-magnitude;}
 route_probe_obstacle_tick=live_host_time.counter;route_probe_obstacle_waypoint=live_delivery_waypoint;
 if(startup_pickup_audio_driving%120==0)printf("LIVE DELIVERY PROGRESS: sourcecounter%u waypoint%u/%u remaining%ld trip%ld position%ld,%ld speed%ld heading_difference%d brake%d.\n",live_host_time.counter,live_delivery_waypoint,live_delivery_route->count,(long)passenger_scene.session.clock.remaining_16a8,(long)passenger_scene.session.trip.time_16ac,(long)car.world.height.position.x,(long)car.world.height.position.z,(long)car.world.speed_544,difference,route_probe_brake);
 if(startup_pickup_audio_driving%120==0)printf("LIVE DELIVERY CORRIDOR: waypoint%u lateral%.3f target_delta%lld,%lld endpoint_distance_squared%lld.\n",live_delivery_waypoint,route_lateral,(long long)dx,(long long)dz,(long long)endpoint_distance_squared);
}
static void startup_pickup_audio_input(void){
    ++startup_pickup_audio_driving;
    if(live_delivery_smoke&&passenger_scene.session.end.completed_1692){route_probe_brake=1;script_steer=route_probe_analog_command=0;return;}
    if(passenger_scene.session.trip.active_1694){
        if(live_delivery_smoke){++startup_pickup_audio_hold;live_delivery_input();return;}
        ++startup_pickup_audio_hold;
        route_probe_brake=!startup_tip_audio_smoke||startup_pickup_audio_hold<=180||startup_pickup_audio_hold>780;
        script_steer=0;
        if(startup_tip_audio_smoke&&!route_probe_brake&&!recovered_game_rewards){
            /* Test-driver targets an existing road actor,720 units to one
               side and4000 along its actual segment. Only commands change. */
            int target=startup_tip_target;
            if(target<0||!(traffic_actors[target].flags&1)){
                target=-1;double best=1e100;
                for(unsigned n=0;n<50;n++)if(traffic_actors[n].flags&1){
                    double dx=(double)traffic_actors[n].x-car.world.height.position.x;
                    double dz=(double)traffic_actors[n].z-car.world.height.position.z;
                    double d=dx*dx+dz*dz;if(d<best){best=d;target=(int)n;}
                }
                startup_tip_target=target;startup_tip_side=0;
            }
            if(target>=0){
                const TaxiTrafficActor *actor=traffic_actors+target;
                const TaxiAiPathAsset *road=traffic_assets.roads+actor->route;
                size_t segment=actor->path.segment;
                if(segment+1<road->count){
                    double vx=(double)road->points[segment+1].x-road->points[segment].x;
                    double vz=(double)road->points[segment+1].z-road->points[segment].z;
                    double length=sqrt(vx*vx+vz*vz);
                    if(length>0){
                        double px=(double)car.world.height.position.x-actor->x;
                        double pz=(double)car.world.height.position.z-actor->z;
                        if(!startup_tip_side)startup_tip_side=px*vz-pz*vx>=0?720.0:-720.0;
                        double side=startup_tip_side;
                        double dx=actor->x+(vx*4000+vz*side)/length-car.world.height.position.x;
                        double dz=actor->z+(vz*4000-vx*side)/length-car.world.height.position.z;
                        int desired=0;double alignment=-1e100;
                        for(int angle=0;angle<4096;angle++){
                            double dot=-runtime.trig.sine[angle]*dx+runtime.trig.cosine[angle]*dz;
                            if(dot>alignment){alignment=dot;desired=angle;}
                        }
                        int current=(int)((uint32_t)car.world.heading_20>>12)&4095;
                        int difference=((desired-current+2048)&4095)-2048;
                        script_steer=difference>32?-1:difference < -32?1:0;
                    }
                }
            }
        }
        if(startup_tip_audio_smoke&&recovered_game_rewards)route_probe_brake=1;
        return;
    }
    int nearest=-1;int64_t best=INT64_MAX;
    for(unsigned i=0;i<passenger_scene.count;i++)if((passenger_scene.customers[i].bytes[0]&1)&&!(passenger_scene.customers[i].bytes[0]&64)){
        int64_t dx=(int64_t)car.world.height.position.x-passenger_scene.spawns[i].x;
        int64_t dz=(int64_t)car.world.height.position.z-passenger_scene.spawns[i].z;
        int64_t d=dx*dx+dz*dz;if(d<best){best=d;nearest=(int)i;}
    }
    route_probe_brake=nearest>=0&&(passenger_scene.customers[nearest].bytes[0]&4);script_steer=0;
    if(nearest>=0&&!route_probe_brake){
        int64_t dx=(int64_t)passenger_scene.spawns[nearest].x-car.world.height.position.x;
        int64_t dz=(int64_t)passenger_scene.spawns[nearest].z-car.world.height.position.z;
        int desired=0;int64_t best_dot=INT64_MIN;
        for(int angle=0;angle<4096;angle++){
            int64_t dot=-(int64_t)runtime.trig.sine[angle]*dx+(int64_t)runtime.trig.cosine[angle]*dz;
            if(dot>best_dot){best_dot=dot;desired=angle;}
        }
        int current=(int)((uint32_t)car.world.heading_20>>12)&4095;
        int difference=((desired-current+2048)&4095)-2048;
        script_steer=difference>32?-1:difference < -32?1:0;
    }
}
static void startup_tick_control(uint16_t mask) {
    native_presentation_counter++; /* Source26400..08 nonpaused delta1; boot epoch remains qualified. */
    if(startup_stage==15&&credits_live_active){credits_live_tick(mask);return;}
    if(startup_stage==TAXI_STARTUP_TITLE_STAGE){
        TaxiTitleRuntimeOutput output;
        if(taxi_title_runtime_frame(&live_title,&passenger_scene.random_state,mask,
            startup_title_check?0:audio_settings_state.effects_enabled,&output))fail("Native title frame failed.");
        if(output.callback.sound_index>=0)audio_request((unsigned)output.callback.sound_index);
        if(output.confirm_sound>=0)audio_request((unsigned)output.confirm_sound);
        title_live_upload(&output);
        if(output.parent==1){
            normal_live_input=live_title.input;normal_live_flow.fade=live_title.fade;
            startup_parent_begin(1,1);
        }else if(output.parent==2)startup_attract_begin();
        return;
    }
    if(startup_stage==14) {
        if(taxi_attract_flow_step(&attract_session.flow,1,city_fade.active_177b,
            (mask&1)!=0,(mask&8)!=0)==1)taxi_palette_fade_begin(&city_fade,0);
        drive_tick((TaxiXboxSample){0},0);
        if(taxi_attract_session_finish(&attract_session,native_settings,city_fade.active_177b)==1) {
            live_attract=source_attract_ai=drive_mode=0;
            music_tracker.state.playing=0;music_context.main_mode=1;
            title_live_begin();
            puts("LIVE ATTRACT: original controller/fade completed, saved city restored, title restarted; full trajectory/audio scheduling fidelity remains unverified.");
        }
        return;
    }
    music_context.sound_active=(uint8_t)(startup_stage==12);
    if(startup_stage==3&&profile_live_active&&profile_flow.complete&&profile_commit_done){
        normal_live_input=profile_input;
        normal_live_flow.fade=profile_flow.fade;
        result_live_reset();results_preview=0;
        /* b5ca storesr4=1; b602 profile clears3897; b606 restoresr4
           into retainedr5=3897 before b6c4 dispatches MAIN. */
        startup_parent_begin(1,1);
        return;
    }
    if(startup_stage==10){
        control_update(startup_parent_check);
        TaxiControlMenuEffect effect;
        taxi_control_menu_step(&control_live,native_settings,&normal_live_input,control_fade.active_177b,
            audio_settings_state.effects_enabled,1,&effect);
        if(effect.sound>=0)audio_request((unsigned)effect.sound);
        if(effect.fade_flags)taxi_palette_fade_begin(&control_fade,0);
        taxi_palette_fade_step(&control_fade,1);
        taxi_menu_input_step(&normal_live_input,mask,1);
        taxi_control_menu_presented(&control_live,1,0);
        if(control_live.exiting&&!control_fade.active_177b){
            taxi_options_menu_flow_begin(&options_live_flow,options_live_selection,control_fade.interval_1771,control_fade.reload_1772);startup_stage=11;
        }
        return;
    }
    if(startup_stage==11||startup_stage==12){
        int audio=startup_stage==12;
        uint8_t selected=audio?audio_live_flow.control.row:options_live_flow.control.row;
        submenu_update(audio,selected,audio?audio_live_flow.motion.position[0]:options_live_flow.motion.position[0],startup_parent_check);
        TaxiGameSettings settings;memcpy(settings.bytes,native_settings,sizeof settings.bytes);
        if(audio){
            TaxiAudioMenuControlEffect effect;
            if(taxi_audio_menu_flow_frame(&audio_live_flow,&settings,&normal_live_input,native_recorded_input_gate,audio_settings_state.effects_enabled,mask,1,&effect))fail("Sound flow failed.");
            for(unsigned n=0;n<effect.volume_after_sound;n++)audio_request((unsigned)effect.sounds[n]);
            if(effect.volume_changed){
                if(effect.volume_setting==1)audio_settings_state.music_volume=effect.current_volume;
                else {audio_settings_state.effects_volume=effect.current_volume;audio_global_volume=effect.current_volume;}
            }
            for(unsigned n=effect.volume_after_sound;n<effect.count;n++)audio_request((unsigned)effect.sounds[n]);
            if(effect.test_row_confirmed==2&&effect.test_index>=0&&audio_live){
                taxi_audio_channels_stop(audio_channels);
                audio_test_stops++;
                if(audio_settings_state.effects_enabled)audio_request((unsigned)effect.test_index);
            }
            audio_live_selection=audio_live_flow.control.row;
            music_context.sound_active=1;music_context.sound_row=audio_live_flow.control.row;
            music_context.music_selection=(uint8_t)audio_live_flow.control.music_test;
            if(effect.test_row_confirmed==3&&effect.test_index>=0)music_test_start((unsigned)effect.test_index);
            if(audio_live_flow.finished){
                /* Source17b7a..17b90 restores menu music order table entry4. */
                music_test_start(4);music_context.sound_active=0;
                taxi_options_menu_flow_begin(&options_live_flow,options_live_selection,audio_live_flow.fade.interval_1771,audio_live_flow.fade.reload_1772);startup_stage=11;
            }
        }else{
            TaxiOptionsMenuControlEffect effect;
            if(taxi_options_menu_flow_frame(&options_live_flow,&settings,&normal_live_input,native_recorded_input_gate,audio_settings_state.effects_enabled,mask,1,&effect))fail("Options flow failed.");
            for(unsigned n=0;n<effect.count;n++)audio_request((unsigned)effect.sounds[n]);
            options_live_selection=options_live_flow.control.row;
            if(options_live_flow.finished){
                if(options_live_flow.route.callback==0x080174dd){normal_live_flow.fade=options_live_flow.fade;startup_parent_begin(1,0);}
                else if(options_live_flow.route.callback==0x08017a25){taxi_audio_menu_flow_begin(&audio_live_flow,audio_live_selection,options_live_flow.fade.interval_1771,options_live_flow.fade.reload_1772);startup_stage=12;}
                else {
                    memset(&control_live,0,sizeof control_live);memset(&control_fade,0,sizeof control_fade);
                    control_fade.interval_1771=options_live_flow.fade.interval_1771;control_fade.reload_1772=options_live_flow.fade.reload_1772;
                    taxi_palette_fade_begin(&control_fade,1);startup_stage=10;control_update(startup_parent_check);
                    puts("Native Control: original diagram/text/layout cycle; background/footer and complete palette priority remain pending.");
                }
            }
        }
        memcpy(native_settings,settings.bytes,sizeof settings.bytes);submenu_blink++;return;
    }
    if(startup_stage==15||startup_stage==16){
        challenge_live_update();
        if(taxi_challenge_menu_flow_frame(&challenge_live_flow,&normal_live_input,
            native_settings,&native_current_profile,&challenge_live_driver,
            &challenge_live_submode,mask,1)==-2)fail("Native challenge callback frame failed.");
        if(!challenge_live_flow.is_briefing)
            challenge_live_selection=challenge_live_flow.grid.action.selection3886;
        if(challenge_live_flow.finished){
            normal_live_flow.fade=challenge_live_flow.fade;
            if(challenge_live_flow.target==TAXI_MENU_CHALLENGE_BRIEFING)startup_stage=challenge_live_begin(1,0);
            else if(challenge_live_flow.target==TAXI_MENU_CHALLENGE)startup_stage=challenge_live_begin(0,0);
            else if(challenge_live_flow.target==TAXI_MENU_MAIN)startup_parent_begin(1,0);
            else if(challenge_live_flow.target==TAXI_MENU_SESSION&&challenge_live_submode<=5)challenge_course_begin_live();
            else fail("This challenge session is not yet ported.");
        }
        return;
    }
    if(startup_stage==8||startup_stage==9){
        int main=startup_stage==9;
        normal_menu_update_motion((main?0:5)+normal_live_flow.choice.selection,taxi_normal_menu_flow_cap(&normal_live_flow),taxi_normal_menu_flow_slide(&normal_live_flow),startup_parent_check);
        taxi_normal_menu_flow_frame(&normal_live_flow,&normal_live_input,native_recorded_input_gate,mask,1);
        if(main)normal_live_main_selection=normal_live_flow.choice.selection;
        else normal_live_mode_selection=normal_live_flow.choice.selection;
        if(normal_live_flow.finished){
            TaxiNormalMenuTarget target;
            if(main)target=taxi_normal_main_route(normal_live_flow.choice.selection,normal_live_flow.choice.exiting);
            else if(taxi_normal_mode_route(normal_live_flow.choice.selection,normal_live_flow.choice.exiting,native_settings,&target))fail("Native mode route invalid.");
            if(target==TAXI_MENU_MODE)startup_parent_begin(0,0);
            else if(target==TAXI_MENU_CHALLENGE)startup_stage=challenge_live_begin(0,0);
            else if(target==TAXI_MENU_OPTIONS){taxi_options_menu_flow_begin(&options_live_flow,options_live_selection,normal_live_flow.fade.interval_1771,normal_live_flow.fade.reload_1772);startup_stage=11;}
            else if(target==TAXI_MENU_MAIN)startup_parent_begin(1,0);
            else if(!main&&target==TAXI_MENU_SESSION){city_begin();startup_stage=1;}
            else {startup_stage=10;puts("Original target selected; its native implementation remains pending.");}
        }
        return;
    }
    if(startup_stage==1) {
        city_tick(mask);
        if(city_flow.complete) {
            if(city_flow.choice.exiting==1) {
                taxi_driver_flow_begin(&driver_flow);memset(&driver_input,0,sizeof driver_input);
                driver_preview=0;driver_scroll_x=16;driver_scroll_y=0;driver_cap_upload(0);startup_stage=2;
            } else {normal_live_input=city_input;normal_live_flow.fade=city_flow.fade;startup_parent_begin(0,1);}
        }
    } else if(startup_stage==2) {
        driver_tick(mask);
        if(driver_flow.complete) {
            if(driver_flow.choice.exiting==2) {city_begin();startup_stage=1;}
            else startup_loading_begin();
        }
    } else if(startup_stage==5||startup_stage==7) {
        taxi_loading_flow_present(&startup_loading_flow,1);
        loading_cap=startup_loading_flow.palette_cap;loading_cap_upload(loading_cap);
        if(startup_stage==5&&startup_loading_flow.phase==TAXI_LOADING_WORK) {
            startup_stage=6;loading_preview=0;startup_drive_begin();
            if(!taxi_loading_flow_finish(&startup_loading_flow))fail("Startup loading work did not finish.");
            startup_stage=7;startup_clock_reset=1;
        } else if(startup_stage==7&&startup_loading_flow.phase==TAXI_LOADING_READY) {
            startup_loading_visible=0;startup_stage=live_attract?14:3;
            if(!live_attract&&!(native_settings[0]&128))taxi_game_start_begin_ordinary(&live_game_start,240);
            city_fade.interval_1771=city_fade.reload_1772=1;taxi_palette_fade_begin(&city_fade,1);city_fade_cap=0;
            printf("STARTUP LOADING DISPLAY: source fadeIN/work/fadeOUT complete, %u actual work renders; city fadeIN begins with ordinary gameplay.\n",startup_loading_rendered);
        }
    } else if(startup_stage==3){
        if(startup_credits_audio_smoke&&startup_audio_tick>=900&&!live_game_start.active&&!city_fade.active_177b){credits_live_begin(0);return;}
        if(startup_expiry_audio_smoke&&!expiry_audio_armed){
            /* Diagnostic trigger only: ordinary recovered countdown/expiry follows. */
            if(natural_expiry_smoke){natural_expiry_initial_clock=passenger_scene.session.clock.remaining_16a8;natural_expiry_initial_counter=live_host_time.counter;if(natural_expiry_initial_clock!=taxi_game_initial_clock(&passenger_scene.session.settings,0)||natural_expiry_initial_clock<=1)fail("Natural expiry did not start from original session clock.");printf("NATURAL EXPIRY START: clock%ld sourcecounter%u; no clock edit.\n",(long)natural_expiry_initial_clock,natural_expiry_initial_counter);}else passenger_scene.session.clock.remaining_16a8=1;expiry_audio_armed=1;
        }
        if(startup_audio_cleanup_smoke>=3&&startup_pickup_audio_hold>=180){
            static const uint16_t keys[]={8,0,128,0,128,0,1,0,128,0,1};
            if(!startup_quit_input_tick){
                startup_quit_frozen_car=car;startup_quit_frozen_clock=passenger_scene.session.clock.remaining_16a8;
                pause_host_frames=1;
            }
            mask=startup_quit_input_tick<sizeof keys/sizeof keys[0]?keys[startup_quit_input_tick]:0;
            startup_quit_input_tick++;
        }
        if(pause_host_frames){
            if(taxi_pause_runtime_control(&native_pause,mask,city_fade.active_177b,
               (uint8_t)live_game_start.active,passenger_scene.session.ended_1697,&native_pause_frame))fail("Host pause controller failed.");
            pause_consume_audio(&native_pause_frame.events);
            if(native_pause_frame.events.fade_out){
                if(startup_audio_cleanup_smoke<3||native_pause.menu.outer_state!=2||!native_pause.menu.exit_flag)
                    fail("Diagnostic host exit requires pending child/cleanup integration.");
                /* Original21C5C requests E5A8(0,32,0x201,0x201). Existing
                   cap presentation represents mask bit0; other mask effects unverified. */
                passenger_scene.session.outer_state_1ea8=native_pause.menu.outer_state;
                taxi_palette_fade_begin(&city_fade,0);
            }
        }
        if(startup_pickup_audio_smoke&&!startup_quit_input_tick&&!live_game_start.active&&!city_fade.active_177b)startup_pickup_audio_input();
        int retained_endgame=endgame_diagnostic;
        if(startup_quit_input_tick)endgame_diagnostic=0; /* Exit parent owns this diagnostic path. */
        drive_tick(startup_game_audio_smoke?(TaxiXboxSample){0}:taxi_xbox_poll_any(),startup_pickup_audio_smoke&&!live_game_start.active&&!city_fade.active_177b);
        endgame_diagnostic=retained_endgame;
        if(startup_quit_input_tick){
            if(memcmp(&car,&startup_quit_frozen_car,sizeof car)||passenger_scene.session.clock.remaining_16a8!=startup_quit_frozen_clock)
                fail("Quit confirmation/fade advanced frozen gameplay.");
            if(passenger_scene.session.outer_state_1ea8==2){
                if(city_fade.active_177b){city_fade_tick(0,native_elapsed_time?live_host_time.frame.delta_3b00:1);startup_quit_fade_ticks++;}
                if(!city_fade.active_177b&&!startup_quit_complete){
                    if(!startup_quit_fade_ticks)fail("Quit skipped original exit fade.");
                    startup_quit_complete=1;
                }
            }
        }
        if(pause_host_frames&&taxi_pause_runtime_present(&native_pause,city_fade.active_177b,240,&native_pause_frame))fail("Host pause presentation failed.");
    }
}
static void startup_tick(uint16_t mask){
    startup_tick_control(mask);
    if(native_audio_capture)audio_frame();
}
static void check_pause_host_frames(void){
    if(startup_stage!=3||live_attract||challenge_jump_drive_preview||!passenger_diagnostic||!traffic_preview)fail("Host pause check requires ordinary startup game with traffic.");
    for(unsigned i=0;i<400&&(live_game_start.active||city_fade.active_177b);i++)startup_tick(0);
    if(live_game_start.active||city_fade.active_177b)fail("Host pause intro gates did not clear.");
    memset(&native_pause,0,sizeof native_pause);pause_host_frames=1;
    startup_tick(8);
    if(!native_pause.menu.frozen||!native_pause.alternate||native_pause_frame.draw_count!=6)fail("Host Start pause did not reach overlay/palette.");
    TaxiDriveWorld saved_car=car;int32_t clock=passenger_scene.session.clock.remaining_16a8;
    for(unsigned i=0;i<120;i++){
        startup_tick(0);
        if(memcmp(&car,&saved_car,sizeof car)||clock!=passenger_scene.session.clock.remaining_16a8)fail("Host paused gameplay advanced.");
    }
    startup_tick(128);startup_tick(0);startup_tick(1);
    if(native_pause.menu.dialog!=1||!native_pause.menu.no_selected||native_pause_frame.draw_count!=5)fail("Host Restart confirmation failed.");
    startup_tick(0);startup_tick(1); /* Default No returns to menu. */
    if(native_pause.menu.dialog||!native_pause.menu.frozen)fail("Host Restart No failed.");
    startup_tick(0);startup_tick(128);startup_tick(0);startup_tick(1);
    if(native_pause.menu.dialog!=2||!native_pause.menu.no_selected)fail("Host Quit confirmation failed.");
    startup_tick(0);startup_tick(2);
    if(native_pause.menu.dialog||!native_pause.menu.frozen)fail("Host Quit cancel failed.");
    startup_tick(0);startup_tick(8);
    if(native_pause.menu.frozen||native_pause.alternate||native_pause_frame.draw_count||passenger_scene.session.clock.remaining_16a8!=clock-1)fail("Host Start resume failed.");
    pause_host_frames=0;
    puts("PASS startup host pause frames: Start, 120 frozen ticks, Restart default-No, Quit cancel, Start resume; controller-before-drive and presentation-after-drive. Actual retry/quit Yes, camera request and physical device input remain unverified.");
}
static int title_entry_check;
static GLuint title_texture;
static uint8_t title_pixels[240*160*4];
static TaxiTitleLoad live_title_loaded;
static uint8_t live_title_rom[8388608];
static uint16_t live_title_rgb555[240*160];
static int live_title_rom_ready;
static void title_live_upload(const TaxiTitleRuntimeOutput *output){
    unsigned scroll=(unsigned)live_title.callback.scroll.x_q8/256;
    if(taxi_title_requests_queue(&live_title_loaded,&output->callback,live_title_rom,sizeof live_title_rom,live_title.obj_palette_mask)||
       taxi_title_objects_publish(&live_title_loaded)||taxi_title_pixels(&live_title_loaded,scroll,live_title.bldy,live_title_rgb555))fail("Recovered native title rendering failed.");
    for(unsigned i=0;i<240*160;i++){
        uint16_t color=live_title_rgb555[i];
        for(unsigned j=0;j<3;j++)title_pixels[i*4+j]=(uint8_t)(((color>>(j*5))&31)*255/31);
        title_pixels[i*4+3]=255;
    }
    glBindTexture(GL_TEXTURE_2D,title_texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,title_pixels);
    if(glGetError()!=GL_NO_ERROR)fail("Live title texture upload failed.");
    if(startup_title_check){
        uint8_t readback[240*160*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
        if(memcmp(readback,title_pixels,sizeof readback)||glGetError()!=GL_NO_ERROR)fail("Recovered title GPU readback mismatch.");
    }
}
static void title_live_begin(void){
    if(!live_title_rom_ready){
        FILE *f=fopen("Crazy Taxi - Catch a Ride (USA).gba","rb");
        if(!f||fread(live_title_rom,1,sizeof live_title_rom,f)!=sizeof live_title_rom||fgetc(f)!=EOF)fail("Original title ROM unavailable.");fclose(f);live_title_rom_ready=1;
    }
    if(taxi_palette_registration_reset(&live_title_loaded.registry))fail("Title palette reset failed.");
    live_title_loaded.fade.fade=live_title.fade;
    if(taxi_title_load(&live_title_loaded,live_title_rom,sizeof live_title_rom,0)||taxi_title_entry(&live_title,&live_title_loaded))fail("Recovered title preparation failed.");
    startup_stage=TAXI_STARTUP_TITLE_STAGE;
    if(!title_texture)glGenTextures(1,&title_texture);glBindTexture(GL_TEXTURE_2D,title_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    TaxiTitleRuntimeOutput output={0};title_live_upload(&output);
}
static void load_native_title_check(void){
    uint8_t *background=malloc(512*256*4),prompt[128*16*4];
    if(!background)fail("Title allocation failed.");
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,92);if(!tail)fail("Title asset path failed.");tail++;
    const char *files[2]={"title/background.rgba","state5-live-strip/language-0.rgba"};
    void *dest[2]={background,prompt};size_t sizes[2]={512*256*4,sizeof prompt};
    for(unsigned i=0;i<2;i++){
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/%s",files[i]);FILE *f=fopen(path,"rb");
        if(!f||fread(dest[i],1,sizes[i],f)!=sizes[i]||fgetc(f)!=EOF)fail("Original title asset invalid.");
        fclose(f);
    }
    TaxiTitleRuntime runtime;taxi_title_runtime_begin(&runtime);
    uint32_t title_random=1;TaxiTitleRuntimeOutput title_output;
    for(unsigned frame=0;frame<50;frame++){
        if(taxi_title_runtime_frame(&runtime,&title_random,0,0,&title_output))fail("Native title frame failed.");
    }
    TaxiTitlePrompt requests[4];
    if(runtime.callback.scroll.timer!=34||runtime.callback.scroll.x_q8!=272*256||runtime.fade.active_177b||runtime.bldy||title_output.callback.request_count!=4)fail("Native title entry gates failed.");
    memcpy(requests,title_output.callback.requests,sizeof requests);
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)
        memcpy(title_pixels+4*(y*240+x),background+4*(y*512+x+272),4);
    for(unsigned half=0;half<4;half++)for(unsigned y=0;y<16;y++)for(unsigned x=0;x<32;x++){
        const uint8_t *art=prompt+4*(y*128+half*32+x);
        if(art[3])memcpy(title_pixels+4*((requests[half].y+(int)y)*240+requests[half].x+(int)x),art,4);
    }
    if(taxi_title_runtime_frame(&runtime,&title_random,1,1,&title_output)||title_output.confirm_sound!=5||!runtime.flow.menu_requested||!runtime.fade.active_177b)fail("Native title confirm gate failed.");
    unsigned exit_updates=0;
    while(!runtime.finished&&exit_updates++<40)
        if(taxi_title_runtime_frame(&runtime,&title_random,0,1,&title_output))fail("Native title exit failed.");
    if(title_output.parent!=1||runtime.bldy!=16||runtime.fade.active_177b||title_random!=1)fail("Native title parent ownership failed.");
    puts("PASS composed native title frames: natural fade exit reaches main parent request; actual host parent dispatch pending.");
    glGenTextures(1,&title_texture);glBindTexture(GL_TEXTURE_2D,title_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,title_pixels);
    uint8_t readback[240*160*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
    if(memcmp(readback,title_pixels,sizeof readback)||glGetError()!=GL_NO_ERROR)fail("Title GPU upload mismatch.");
    uint8_t darkened[240*160*4];
    for(unsigned coefficient=0;coefficient<=16;coefficient++){
        taxi_palette_fade_darken_rgba(title_pixels,darkened,240*160,(uint16_t)coefficient);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,darkened);
        glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
        if(glGetError()!=GL_NO_ERROR)fail("Title brightness GPU transfer failed.");
        for(unsigned pixel=0;pixel<240*160;pixel++)for(unsigned c=0;c<4;c++){
            unsigned original=title_pixels[4*pixel+c],expected=original;
            if(c<3){
                unsigned channel=(original*31+127)/255;
                unsigned result=(channel*(16-coefficient)+15)/16;
                expected=result==channel?original:result*255/31;
            }
            if(readback[4*pixel+c]!=expected)fail("Title brightness GPU pixel mismatch.");
        }
    }
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,title_pixels);
    if(glGetError()!=GL_NO_ERROR)fail("Title brightness diagnostic restore failed.");
    puts("PASS native title darkening: all17 hardware coefficients,652800 GPU texture pixels versus independent integer color arithmetic; live fade animation not integrated.");
    free(background);
    puts("PASS native title entry: original BG/prompt, 34-update scroll, fade gate, source repeat confirm and byte-exact GPU upload; menu parent/attract not integrated.");
}
static void draw_native_title_check(int width,int height){
    glClearColor(title_pixels[0]/255.0f,title_pixels[1]/255.0f,title_pixels[2]/255.0f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    float scale=(float)height/160,offset=width*0.5f-120*scale;
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,title_texture);glColor4f(1,1,1,1);
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2f(offset,0);glTexCoord2i(1,0);glVertex2f(offset+240*scale,0);
    glTexCoord2i(1,1);glVertex2f(offset+240*scale,height);glTexCoord2i(0,1);glVertex2f(offset,height);glEnd();
    glDisable(GL_TEXTURE_2D);glEnable(GL_DEPTH_TEST);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
}
/* Capture BG before ANY OBJ; OpenGL bottom-up rows match gl_FragCoord.
   Qualified shared bitmap path alpha preserves resolved index0/backdrop vs BG2. */
static void capture_effect_background(int width,int height,int verify){
    GLint binding,read_buffer;
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&binding);glGetIntegerv(GL_READ_BUFFER,&read_buffer);
    if(!native_effect_background)glGenTextures(1,&native_effect_background);
    glBindTexture(GL_TEXTURE_2D,native_effect_background);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glReadBuffer(target_bound?0x8ce0:GL_BACK);
    if(width!=native_effect_background_width||height!=native_effect_background_height){
        glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,width,height,0);
        native_effect_background_width=width;native_effect_background_height=height;
    }else glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,width,height);
    if(glGetError()!=GL_NO_ERROR)fail("Original BG capture failed.");
    if(verify){
        size_t bytes=(size_t)width*height*4;
        uint8_t *texture=malloc(bytes),*frame=malloc(bytes);
        if(!texture||!frame)fail("BG verification allocation failed.");
        glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,texture);
        glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,frame);
        if(glGetError()!=GL_NO_ERROR||memcmp(texture,frame,bytes))fail("Original BG capture differs from pre-OBJ pixels.");
        if(native_effect_background_coverage_valid){
            size_t bg=0,backdrop=0;
            for(size_t i=3;i<bytes;i+=4){
                if(texture[i]==255)bg++;else if(texture[i]==0)backdrop++;
                else fail("BG coverage contains fractional eligibility.");
            }
            printf("PASS captured bitmap coverage: %zu BG2 / %zu backdrop pixels; original-index alpha, RGB capture byte-exact.\n",bg,backdrop);
            native_effect_background_coverage_checks++;
        }
        free(texture);free(frame);native_effect_background_checks++;
    }
    glReadBuffer((GLenum)read_buffer);glBindTexture(GL_TEXTURE_2D,(GLuint)binding);
    native_effect_background_valid=1;native_effect_background_captures++;
}
static TaxiFontAsset attract_prompt_font;
static GLuint attract_prompt_ids[224];
static char attract_prompt_text[2][128];
static int attract_prompt_cap=-1;
static TaxiFontAsset attract_score_font;
static GLuint attract_score_ids[155];
static char attract_rank_text[10][128];
static int attract_score_cap=-1;
static GLuint attract_driver_ids[4];
static uint8_t attract_driver_rgba[4][1024];
static void draw_attract_prompt(int width,int height) {
    if(!live_attract)return;
    if(!attract_prompt_font.count) {
        char path[MAX_PATH],metric[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
        char *tail=strrchr(path,92);if(!tail)fail("Demo font path failed.");
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/font0-metrics.ctf");strcpy(metric,path);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/font0.rgba");
        if(taxi_font_asset_load(&attract_prompt_font,metric,path,2,32,1)||attract_prompt_font.count!=224||attract_prompt_font.spacing!=1)fail("Original demo font invalid.");
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/text.crt");FILE *f=fopen(path,"rb");uint8_t header[8];
        if(!f||fread(header,1,8,f)!=8||memcmp(header,"CRT1",4)||header[4]!=12||header[5]||header[6]||header[7])fail("Demo text header invalid.");
        unsigned found=0;
        for(unsigned i=0;i<12;i++) {
            uint8_t row[4];char text[128];
            if(fread(row,1,4,f)!=4)fail("Demo text record missing.");
            unsigned id=row[0]|row[1]<<8,len=row[2]|row[3]<<8;
            if(!len||len>=128||fread(text,1,len,f)!=len)fail("Demo text length invalid.");text[len]=0;
            if(id==30||id==31){strcpy(attract_prompt_text[id-30],text);found++;}
            else if(id>=320&&id<330){strcpy(attract_rank_text[id-320],text);found++;}
        }
        if(found!=12||fgetc(f)!=EOF)fail("Demo prompt records invalid.");fclose(f);
        glGenTextures(224,attract_prompt_ids);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/font1-metrics.ctf");strcpy(metric,path);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/font1.rgba");
        if(taxi_font_asset_load(&attract_score_font,metric,path,1,32,0)||attract_score_font.count!=155||attract_score_font.spacing!=-1)fail("Original demo score font invalid.");
        glGenTextures(155,attract_score_ids);
        glGenTextures(4,attract_driver_ids);
        for(unsigned i=0;i<4;i++) {
            snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/attract/driver%u.rgba",i);
            FILE *portrait=fopen(path,"rb");
            if(!portrait||fread(attract_driver_rgba[i],1,1024,portrait)!=1024||fgetc(portrait)!=EOF)fail("Original demo portrait invalid.");
            fclose(portrait);
        }
    }
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glDisable(GL_DEPTH_TEST);glColor4f(1,1,1,1);
    if(attract_prompt_cap!=city_fade_cap) {
        uint8_t rgba[4096];
        for(unsigned i=0;i<224;i++) {
            taxi_palette_fade_cap_rgba(taxi_font_asset_glyph(&attract_prompt_font,0,i),rgba,1024,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,attract_prompt_ids[i]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        }
        attract_prompt_cap=city_fade_cap;
    }
    if(attract_score_cap!=city_fade_cap) {
        uint8_t rgba[4096];
        for(unsigned i=0;i<155;i++) {
            taxi_palette_fade_cap_rgba(taxi_font_asset_glyph(&attract_score_font,0,i),rgba,1024,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,attract_score_ids[i]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        }
        for(unsigned i=0;i<4;i++) {
            taxi_palette_fade_cap_rgba(attract_driver_rgba[i],rgba,256,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,attract_driver_ids[i]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        }
        attract_score_cap=city_fade_cap;
    }
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    double pixel=height/160.0;
    if((native_presentation_counter&127)<=111) {
        asset_text(&attract_prompt_font,attract_prompt_ids,0,4,attract_prompt_text[0],width/2.0,138*pixel,pixel);
        asset_text(&attract_prompt_font,attract_prompt_ids,0,4,attract_prompt_text[1],width/2.0,148*pixel,pixel);
    }
    /* Source D934..DAC2: independently gated score reveal, not prompt blink. */
    if(attract_session.flow.blink_enabled) {
        unsigned page=attract_session.flow.phase;
        if(page>1)fail("Demo score page invalid.");
        unsigned visible=attract_session.flow.blink_frame;
        if(visible>20)visible=20;
        for(unsigned row=0;row<visible&&row<20;row++) {
            unsigned item=row%5,index=page*5+item;
            const uint8_t *record=native_settings+424+8*index;
            char text[40];double x;unsigned align;
            if(row<5){strcpy(text,attract_rank_text[index]);x=22*pixel;align=4;}
            else if(row<10){for(unsigned k=0;k<3;k++)text[k]=record[k]=='.'?'+':record[k];text[3]=0;x=50*pixel;align=0;}
            else if(row<15) {
                uint32_t word=(uint32_t)record[4]|(uint32_t)record[5]<<8|(uint32_t)record[6]<<16|(uint32_t)record[7]<<24;
                text[0]='$';if(taxi_results_cash_format(text+1,(int32_t)(word>>15),record[5]&127))fail("Demo score money invalid.");
                x=width-32*pixel;align=2;
            }
            else {
                unsigned driver=(record[4]>>5)&3;
                double px=width-20*pixel,py=(32+18*item)*pixel;
                glBindTexture(GL_TEXTURE_2D,attract_driver_ids[driver]);
                glBegin(GL_QUADS);
                glTexCoord2f(0,0);glVertex2d(px,py);glTexCoord2f(1,0);glVertex2d(px+16*pixel,py);
                glTexCoord2f(1,1);glVertex2d(px+16*pixel,py+16*pixel);glTexCoord2f(0,1);glVertex2d(px,py+16*pixel);
                glEnd();continue;
            }
            asset_text(&attract_score_font,attract_score_ids,0,align,text,x,(32+18*item)*pixel,pixel);
        }
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void draw(int width, int height) {
    if(credits_live_active){credits_live_draw(width,height);return;}
    if(title_entry_check){draw_native_title_check(width,height);return;}
    if(startup_mode&&startup_stage==TAXI_STARTUP_TITLE_STAGE){draw_native_title_check(width,height);return;}

    if(normal_menu_preview>=0){draw_normal_menu_preview(width,height);return;}
    if(startup_mode&&startup_stage>=8&&startup_stage!=14){draw_normal_menu_preview(width,height);return;}
    if(startup_loading_visible||(!startup_mode&&loading_preview>=0)) {draw_loading_preview(width,height);return;}
    if(startup_mode&&startup_stage!=3&&startup_stage!=14) {if(startup_stage==1||startup_stage==4)draw_city_preview(width,height);else draw_driver_preview(width,height);return;}
    if(city_preview>=0&&!(startup_mode&&(startup_stage==3||startup_stage==14))) {draw_city_preview(width,height);return;}
    if(driver_preview>=0&&!(startup_mode&&(startup_stage==3||startup_stage==14))) {draw_driver_preview(width,height);return;}
    if(profile_preview>=0) {draw_profile_preview(width,height);return;}
    if((name_ring_preview>=0||name_interactive||name_live_active)&&!game_over_active) {draw_name_ring(width,height);return;}
    if(game_over_active) {draw_game_over(width,height);return;}
    if(results_preview) {draw_results_preview(width,height);return;}
    if(native_pause.alternate&&!live_attract&&!challenge_jump_drive_preview){
        /* Ordinary HUD contributes16 OBJ records before the scene BG bank. */
        if(taxi_palette_gameplay_bank_transfer(native_pause_scene_palette,native_scene_palette,256,16,1)!=1)fail("Pause scene palette transfer failed.");
    }
    if(uploaded_city_cap!=city_fade_cap||uploaded_scene_alternate!=(int)(pause_scene_palette()!=native_scene_palette)) {
        for(unsigned i=0;i<sprite_source_count;i++) {
            Texture *t=sprite_sources+i;size_t count=(size_t)t->width*t->height;
            unsigned char *pixels=malloc(count*4);if(!pixels)fail("Sprite fade allocation failed.");
            if(pause_scene_palette()!=native_scene_palette&&t->pause_bank>=0&&t->pause_bank!=0&&t->pause_bank!=2&&t->pause_bank!=11){
                taxi_palette_gameplay_alternate_rgba(t->pixels,pixels,count);
                taxi_palette_fade_cap_rgba(pixels,pixels,count,(int16_t)city_fade_cap);
            }else taxi_palette_fade_cap_rgba(t->pixels,pixels,count,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,t->id);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,(GLsizei)t->width,(GLsizei)t->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
            free(pixels);
        }
        for(unsigned i=0;i<texture_count;i++) {
            Texture *t=textures+i;size_t count=(size_t)t->width*t->height;
            unsigned char *pixels=malloc(count*4);if(!pixels)fail("City fade allocation failed.");
            if(pause_scene_palette()!=native_scene_palette){
                if(taxi_palette_gameplay_alternate_rgba(t->pixels,pixels,count))fail("Scene alternate RGBA conversion failed.");
                taxi_palette_fade_cap_rgba(pixels,pixels,count,(int16_t)city_fade_cap);
            }else taxi_palette_fade_cap_rgba(t->pixels,pixels,count,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,t->id);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,(GLsizei)t->width,(GLsizei)t->height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
            free(pixels);
        }
        uploaded_city_cap=city_fade_cap;
        uploaded_scene_alternate=(pause_scene_palette()!=native_scene_palette);
    }
    if (width<1 || height<1) return;
    float minimum_distance=drive_mode?512*raw_scale:0.015f;
    if (distance<minimum_distance) distance=minimum_distance;
    if (distance>20) distance=20;
    if(drive_mode) {
        double alpha=gameplay_render_alpha();
        double difference=(double)camera_heading-previous_camera_heading;
        difference=fmod(difference,16777216.0);
        if(difference>8388608) difference-=16777216;
        if(difference<-8388608) difference+=16777216;
        yaw=(float)taxi_view_yaw(previous_camera_heading+difference*alpha+taxi_render_angle(previous_camera_yaw_offset*4096,camera_yaw_offset*4096,alpha),camera_orbit);
        difference=fmod((double)camera_pitch-previous_camera_pitch,16777216.0);
        if(difference>8388608) difference-=16777216;
        if(difference<-8388608) difference+=16777216;
        pitch=camera_pitch_orbit;
        if(recovered_camera) pitch+=(float)remainder((previous_camera_pitch+difference*alpha)*360/16777216.0,360);
        focus[0]=(float)((previous_position.x+((double)car.world.height.position.x-previous_position.x)*alpha-raw_center[0])*raw_scale);
        focus[0]=(float)taxi_view_x(focus[0]);
        focus[1]=(float)((previous_position.y+((double)car.world.height.position.y-previous_position.y)*alpha-raw_center[1])*raw_scale);
        focus[2]=(float)((previous_position.z+((double)car.world.height.position.z-previous_position.z)*alpha-raw_center[2])*raw_scale);
        /* Camera eye, not a look-at target. Recovered mode2 dynamic offsets;
           mouse zoom adds a PC override to the source distance. */
        if(recovered_camera) {
            if(camera_orbit==0 && camera_pitch_orbit==0 && distance==3072*raw_scale) {
                /* Calculate each simulation endpoint with the recovered Q8
                   matrix and ASR rounding, then interpolate presentation. */
                TaxiCameraView shown=presented_camera_view(alpha);
                TaxiPoint eye=shown.position;
                if(presented_pose_valid||((previous_camera_mode_64c|live_camera_mode_64c)&16)){yaw=(float)taxi_view_yaw((int32_t)shown.yaw*4096,0);pitch=(float)remainder((double)shown.pitch*360/4096,360);}
                focus[0]=(float)taxi_view_x((eye.x-raw_center[0])*raw_scale);
                focus[1]=(float)((eye.y-raw_center[1])*raw_scale);
                focus[2]=(float)((eye.z-raw_center[2])*raw_scale);
            } else {
            double source_height=floor((double)camera_offsets.height_76c/4096)*raw_scale;
            double source_distance=(floor((double)presentation_offsets(camera_offsets).distance_770/4096)-camera_base)*raw_scale;
            double offset[3];taxi_view_camera_offset(yaw,-pitch,source_height,
                source_distance+distance-3072*raw_scale,offset);
            for(unsigned i=0;i<3;i++) focus[i]+=(float)offset[i];
            }
        } else focus[1]+=300*raw_scale;
    }
    glViewport(0, 0, width, height);
    unsigned char clear_color[4];if(pause_scene_palette()!=native_scene_palette){taxi_palette_gameplay_alternate_rgba(scene_clear,clear_color,1);taxi_palette_fade_cap_rgba(clear_color,clear_color,1,(int16_t)city_fade_cap);}else taxi_palette_fade_cap_rgba(scene_clear,clear_color,1,(int16_t)city_fade_cap);
    glClearColor(clear_color[0]/255.0f,clear_color[1]/255.0f,clear_color[2]/255.0f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(drive_mode && sky_texture!=UINT32_MAX)draw_native_gpu_background(width,height,pitch,yaw);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    /* Original802c7fe stores camera+28 focal192, viewport160high.
       Preserve vertical field of view while widening native horizontal view. */
    double top=recovered_camera?0.0001*80/192:0.0001*tan(50.0*3.141592653589793/360.0);
    double right=top*(double)width/height;
    glFrustum(-right,right,-top,top,0.0001,100);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    if(!drive_mode || !recovered_camera) glTranslatef(0,0,-distance);
    /* Original projection Y(-yaw)*X(-pitch), converted from +Z to GL -Z. */
    if(drive_mode&&recovered_camera) {glRotatef(yaw,0,1,0);glRotatef(pitch,1,0,0);}
    else {glRotatef(pitch,1,0,0);glRotatef(yaw,0,1,0);}
    glTranslatef(-focus[0],-focus[1],-focus[2]);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    int shared_queue=drive_mode&&recovered_camera&&!camera_orbit&&!camera_pitch_orbit&&((traffic_preview&&scene_index<2)||challenge_flag_drive_preview||challenge_ball_drive_preview||challenge_drift_drive_preview||challenge_turn_drive_preview||challenge_bound_drive_preview);
    native_effect_background_coverage_valid=shared_queue;
    if(shared_queue)draw_shared_queue(width,height);
    if (!wire_only&&!shared_queue) {
        glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(1,1);
        unsigned previous=UINT32_MAX;
        for (uint32_t i=0; i<face_count; ++i) {
            unsigned material=faces[i].flags&255;
            /* Original08003248..3260: material byte20 &9 skips polygon. */
            if(materials[material].flags&9) continue;
            if (material!=previous) {
                if (previous!=UINT32_MAX) glEnd();
                Material *m=&materials[material];
                if (textured && m->texture!=255) {
                    glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,textures[m->texture].id);
                    glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glColor4f(1,1,1,1);
                } else {
                    glDisable(GL_TEXTURE_2D);glDisable(GL_ALPHA_TEST);
                    if (textured) {
                        unsigned char flat_color[4];
                        if(pause_scene_palette()!=native_scene_palette){taxi_palette_gameplay_alternate_rgba(m->color,flat_color,1);taxi_palette_fade_cap_rgba(flat_color,flat_color,1,(int16_t)city_fade_cap);}else taxi_palette_fade_cap_rgba(m->color,flat_color,1,(int16_t)city_fade_cap);
                        glColor4ubv(flat_color);
                    }
                    else glColor3f(0.28f+(material*37%97)/190.0f,0.34f+(material*53%89)/190.0f,0.4f+(material*19%83)/190.0f);
                }
                glBegin(GL_TRIANGLES);previous=material;
            }
            textured_vertex(&faces[i],0);textured_vertex(&faces[i],1);textured_vertex(&faces[i],2);
            if (faces[i].count==4) { textured_vertex(&faces[i],0);textured_vertex(&faces[i],2);textured_vertex(&faces[i],3); }
        }
        if(previous!=UINT32_MAX) glEnd();
        glDisable(GL_POLYGON_OFFSET_FILL);glDisable(GL_TEXTURE_2D);glDisable(GL_ALPHA_TEST);
    }
    native_effect_background_valid=0;
    int needs_effect_background=native_taxi_facing_check;
    for(unsigned e=0;e<shown_effect_count();e++)if(shown_effect_requests()[e].flags&4)needs_effect_background=1;
    if(passenger_diagnostic&&!shared_queue) draw_passengers();
    if(drive_mode&&!shared_queue)draw_traffic(width,height);
    /* Original31a50 software billboards belong to BG, before taxi/effect OAM. */
    if(drive_mode&&needs_effect_background)
        capture_effect_background(width,height,native_taxi_facing_check);
    if(!shared_queue)native_effect_background_coverage_valid=0;
    if(drive_mode) draw_taxi(width,height);
    /* SourceB490 demo branch skips destination bitmapB51E.
       Trip timer1F196 belongs to skipped ordinaryHUD1F0E8. */
    if(!live_attract) {
        draw_destination_panel(width,height);
        if(!challenge_turn_drive_preview&&!challenge_bound_drive_preview)draw_trip_timer(width,height);
    }
    draw_guidance(width,height);
    /* Source B570..B578 skips ordinary1F0E8 HUD while attract1708 is set. */
    if(!live_attract&&!challenge_turn_drive_preview&&!challenge_bound_drive_preview) {
        draw_main_clock(width,height);
        draw_time_expiry(width,height);
        draw_cash(width,height);draw_tip_images(width,height);
    }
    draw_attract_prompt(width,height);
    draw_game_start_art(width,height);
    draw_challenge_jump_failure(width,height);
    /* Original79BE common HUD, then course-specific callback79CA. */
    draw_flag_clock(width,height);
    draw_pause_overlay(width,height);
    if (!wire_only && textured) return;
    glColor3f(wire_only?0.5f:0.09f,wire_only?0.85f:0.15f,wire_only?1.0f:0.2f);
    glBegin(GL_LINES);
    for (uint32_t i=0; i<face_count; ++i) {
        if(materials[faces[i].flags&255].flags&9) continue;
        for (unsigned j=0; j<faces[i].count; ++j) { emit(faces[i].index[j]); emit(faces[i].index[(j+1)%faces[i].count]); }
    }
    glEnd();
}

static void snapshot(const char *path, int width, int height) {
    size_t bytes=(size_t)width*height*4;
    unsigned char *pixels=malloc(bytes);
    if (!pixels) fail("Image allocation failed.");
    glReadBuffer(target_bound?0x8ce0:GL_BACK); glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    if (glGetError()!=GL_NO_ERROR) fail("OpenGL image readback failed.");
    for (size_t i=0; i<bytes; i+=4) { unsigned char tmp=pixels[i]; pixels[i]=pixels[i+2]; pixels[i+2]=tmp; }
    BITMAPFILEHEADER file_header={0}; BITMAPINFOHEADER info={0};
    file_header.bfType=0x4d42; file_header.bfOffBits=sizeof(file_header)+sizeof(info);
    file_header.bfSize=file_header.bfOffBits+(DWORD)bytes;
    info.biSize=sizeof(info); info.biWidth=width; info.biHeight=height; info.biPlanes=1; info.biBitCount=32;
    FILE *file=fopen(path,"wb");
    if (!file) fail("Cannot write snapshot.");
    if (fwrite(&file_header,sizeof(file_header),1,file)!=1 || fwrite(&info,sizeof(info),1,file)!=1 || fwrite(pixels,1,bytes,file)!=bytes)
        fail("Snapshot write failed.");
    fclose(file); free(pixels);
}

/* Shared native CPU asset handoff. The caller retains settings and loads
   matching scene/GPU assets before exposing gameplay. */
static void load_drive_state(const char *scene,TaxiPassengerInitObserver observer,void *context) {
    /* Original2AC6C->2C790/2C784. Keep source projection owner separate
       from widened PC screen bounds; consumers share this recovered state. */
    taxi_render_viewport_set(&native_scene_viewport,0,0,240,160);
    taxi_render_viewport_depth(&native_scene_viewport,256,38400);
    native_scene_viewport_ready=1;
    native_scene_frame_ready=0;

    /* Same-city source227AC reuses each passenger runtime Y. Asset reload
       must not turn that warm session entry into cold initialization. */
    int retain_passengers=passenger_scene_initialized&&!challenge_jump_drive_preview&&
        ((scene_index==0&&!strcmp(scene,"city1"))||(scene_index==1&&!strcmp(scene,"city2")));
    taxi_challenge_ball_course_free(&live_ball_course);
    taxi_challenge_drift_course_free(&live_drift_course);
    taxi_challenge_bound_course_free(&live_bound_course);
    taxi_challenge_turn_course_free(&live_turn_course);
    if(world.vertices)taxi_world_asset_free(&world);
    if(passenger_scene_initialized) {taxi_passenger_asset_free(&passenger_asset);passenger_scene_initialized=retain_passengers;}
    char asset_path[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,asset_path,MAX_PATH);
    if(!n||n>=MAX_PATH)fail("Driving asset path too long.");
    char *slash=strrchr(asset_path,'\\');if(!slash)fail("Driving asset path failed.");
        scene_index=!strcmp(scene,"city1")?0:!strcmp(scene,"city2")?1:2;
        if(!challenge_jump_drive_preview){
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/scene-descriptors/%s.bin",scene);
            if(taxi_scene_registration_load(&native_scene_registration,asset_path))fail("Original scene descriptor registration failed.");
            if(native_scene_registration.vertices!=vertex_count||native_scene_registration.polygons!=face_count||
               native_scene_registration.materials!=material_count||native_scene_registration.textures!=texture_count||
               native_scene_registration.background!=(int32_t)sky_texture)fail("Scene descriptor disagrees with extracted renderer assets.");
        }

        if(challenge_jump_drive_preview){
            TaxiChallengeAssets course={0};
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds");
            if(challenge_flag_drive_preview){
                TaxiChallengeFlagCourse flag={0};
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets");
                if(taxi_challenge_flag_course_load(&flag,asset_path))fail("Cannot load original Flag course.");
                if(taxi_challenge_flag_course_actor(&flag,&live_flag_actor)!=1)fail("Cannot initialize original Flag actor.");
                live_flag_initial=live_flag_actor;memcpy(live_flag_indices,flag.flag_indices,sizeof live_flag_indices);course=flag.resources;memset(&flag.resources,0,sizeof flag.resources);
                taxi_challenge_flag_course_free(&flag);
            }else if(challenge_ball_drive_preview){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets");
                if(taxi_challenge_ball_course_load(&live_ball_course,asset_path)||taxi_challenge_ball_course_actors(&live_ball_course,live_balls)!=1)fail("Ball course/actors load failed.");
                memcpy(live_balls_initial,live_balls,sizeof live_balls);
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds");
                if(taxi_challenge_assets_load(&course,asset_path,2))fail("Ball sharedworld load failed.");
            }else if(challenge_bound_drive_preview){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets");
                if(taxi_challenge_bound_course_load(&live_bound_course,asset_path))fail("Turn owned course load failed.");
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds");
                if(taxi_challenge_assets_load(&course,asset_path,5))fail("Turn sharedworld load failed.");
            }else if(challenge_turn_drive_preview){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets");
                if(taxi_challenge_turn_course_load(&live_turn_course,asset_path))fail("Turn owned course load failed.");
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds");
                if(taxi_challenge_assets_load(&course,asset_path,4))fail("Turn sharedworld load failed.");
            }else if(challenge_drift_drive_preview){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets");
                if(taxi_challenge_drift_course_load(&live_drift_course,asset_path))fail("Drift owned course load failed.");
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds");
                if(taxi_challenge_assets_load(&course,asset_path,3))fail("Drift sharedworld load failed.");
            }else if(taxi_challenge_assets_load(&course,asset_path,0))fail("Cannot load original challenge course.");
            taxi_indexed_asset_free(&native_indexed_scene);
            world=course.world;native_indexed_scene=course.textures;
            live_challenge_spawn=course.spawn;
            memcpy(native_material_prefix,course.material_prefix,28);
            memcpy(native_scene_palette,course.palette,sizeof native_scene_palette);
            /* Transfer owned storage; metadata remains copied, no double free. */
            memset(&course.world,0,sizeof course.world);memset(&course.textures,0,sizeof course.textures);
            taxi_challenge_assets_free(&course);
        }else{
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/%s.ctw",scene);
        if(taxi_world_asset_load(&world,asset_path)) fail("Cannot load native world data.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/material-prefix/%s.bin",scene);
        FILE *material_prefix_file=fopen(asset_path,"rb");
        if(!material_prefix_file)fail("Cannot load original material prefix.");
        int material_prefix_ok=fread(native_material_prefix,1,28,material_prefix_file)==28&&fgetc(material_prefix_file)==EOF;
        fclose(material_prefix_file);
        if(!material_prefix_ok)fail("Invalid original material prefix.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/%s-special.cts",scene);
        if(taxi_world_asset_load_special(&world,asset_path))fail("Cannot load original special scenery data.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/indexed-scenes/%s.cti",scene);
        if(taxi_indexed_asset_load(&native_indexed_scene,asset_path))fail("Cannot load original indexed scene.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/%s-palette.rgb555",scene);
        FILE *palette_file=fopen(asset_path,"rb");uint8_t palette_bytes[512];uint16_t palette[256];
        if(!palette_file || fread(palette_bytes,1,512,palette_file)!=512)fail("Cannot load scene palette.");
        fclose(palette_file);
        for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(palette_bytes[i*2]|palette_bytes[i*2+1]<<8);
        memcpy(native_scene_palette,palette,sizeof palette);
        }
        taxi_effect_asset_free(&native_effect_asset);
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/effects/sprites.cte");
        if(taxi_effect_asset_load(&native_effect_asset,asset_path))fail("Cannot load original effect sprites.");
        if(sky_texture!=UINT32_MAX){
            uint32_t background_pattern;
            if(taxi_world_background_pattern(&native_indexed_scene,(int32_t)sky_texture,&background_pattern))
                fail("Original background index registration failed.");
            uint16_t color=native_scene_palette[background_pattern&255];
            uint8_t original_clear[4]={0,0,0,255};
            for(unsigned c=0;c<3;c++){unsigned v=(color>>(5*c))&31;original_clear[c]=(uint8_t)((v<<3)|(v>>2));}
            if(memcmp(sky_fill,original_clear,4))fail("Cached scene background disagrees with original indexed texture/palette.");
            memcpy(sky_fill,original_clear,4);
        }
        uint16_t palette[256];memcpy(palette,native_scene_palette,sizeof palette);
        if(!challenge_jump_drive_preview){
            if(taxi_palette_registration_reset(&native_scene_materials))fail("Original scene palette reset failed.");
            int material=taxi_palette_register(&native_scene_materials,native_scene_palette,0x084a5a34,32,1,&native_scene_palette_request);
            if(material!=0||native_scene_materials.count!=1)fail("Original scene palette registration failed.");
            native_scene_border=native_scene_materials.records[material][14];
            native_bootstrap_render.registered_materials=(const uint8_t *)native_scene_materials.records;
            native_bootstrap_render.registered_material_count=native_scene_materials.count;
        }else{
            uint8_t brightest;
            if(!taxi_palette_extrema(palette,&native_scene_border,&brightest))fail("Scene palette extrema failed.");
            uint8_t registered[20]={0};registered[15]=brightest;
            if(!taxi_shade_table_material(native_scene_lookup,registered,1,0))fail("Scene lookup initialization failed.");
        }

        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/driving.ctr");
        if(taxi_runtime_asset_load(&runtime,asset_path)) fail("Cannot load native driving data.");
        if(!challenge_jump_drive_preview){
            uint8_t raw_trig[16388];FILE *trig_file=fopen(asset_path,"rb");
            if(!trig_file)fail("Cannot bind original graphics math assets.");
            int trig_ok=fread(raw_trig,1,sizeof raw_trig,trig_file)==sizeof raw_trig&&!memcmp(raw_trig,"CTR1",4);
            fclose(trig_file);if(!trig_ok)fail("Invalid original graphics math assets.");
            native_bootstrap_render.frame=native_bootstrap_first;
            native_bootstrap_render.frame_size=sizeof native_bootstrap_first;
            native_bootstrap_render.alternate_frame=native_bootstrap_second;
            native_bootstrap_render.alternate_size=sizeof native_bootstrap_second;
            native_bootstrap_render.width=240;native_bootstrap_render.height=160;
            native_bootstrap_render.bitmap_mode=4;
            native_bootstrap_render.shade_lookup=native_scene_lookup;
            native_bootstrap_render.shade_lookup_size=sizeof native_scene_lookup;
            TaxiGraphicsBootstrapAssets assets={.descriptor=native_scene_registration.descriptor,.descriptor_size=104,
                .sine=raw_trig+4,.cosine=raw_trig+8196,.sine_size=8192,.cosine_size=8192,
                .textures=&native_indexed_scene,.palette=native_scene_palette};
            const int32_t bounds[4]={0,0,240,160};
            if(taxi_graphics_bootstrap_tail(&native_graphics_bootstrap,&native_bootstrap_render,
                &native_scene_registration,&assets,bounds,256,38400))fail("Recovered native graphics startup failed.");
            if(startup_warm_reload_check){
                TaxiGraphicsStorage *storage=&native_graphics_bootstrap.storage;
                if(storage->state_6844)fail("Graphics bootstrap retained old generation.");
                for(unsigned i=0;i<12288;i++)if(storage->pool_1b6ec[i])fail("Graphics bootstrap retained normal stamp.");
                for(unsigned i=0;i<4096;i++)if(storage->pool_276ec[i])fail("Graphics bootstrap retained special stamp.");
                puts("CACHE BOOTSTRAP: generation0 and both polygon stamp pools clear.");
            }
            if(memcmp(&runtime.trig,&native_graphics_bootstrap.trig,sizeof runtime.trig))fail("Graphics math disagrees with native runtime tables.");
            const uint32_t material_args[7]={(uint32_t)(native_scene_materials.count-1),0,0,0,0,0,0};
            if(taxi_restart_render_call(&native_bootstrap_render,0x0802ca94,material_args)!=1)fail("Original scene material lookup binding failed.");
            for(unsigned i=0;i<sizeof native_scene_lookup;i++)if(native_scene_lookup[i]!=native_scene_materials.records[0][15])fail("Original scene material lookup disagrees with registration.");
            printf("NATIVEMATERIAL scene=%s count=%u bg-bytes=%u darkest=%u brightest=%u; palette transfer backend separate\n",scene,(unsigned)native_scene_materials.count,(unsigned)native_scene_materials.bg_bytes,(unsigned)native_scene_materials.records[0][14],(unsigned)native_scene_materials.records[0][15]);
            runtime.trig=native_graphics_bootstrap.trig;
            native_scene_viewport=native_bootstrap_render.viewport;
            native_scene_frame=native_graphics_bootstrap.frame;
            native_scene_frame_ready=1;
            uint16_t clear=native_scene_palette[native_bootstrap_render.fill_06900&255];
            uint8_t rgba[4]={0,0,0,255};
            for(unsigned c=0;c<3;c++){unsigned v=(clear>>(5*c))&31;rgba[c]=(uint8_t)((v<<3)|(v>>2));}
            if(memcmp(scene_clear,rgba,4))fail("Recovered startup clear disagrees with original scene palette.");
            memcpy(scene_clear,rgba,4); /* Actual GPU clear consumes recovered native owner. */
            printf("NATIVEFRAME scene=%s near=%ld far=%ld corner0=%ld,%ld streams=%08x,%08x\n",scene,
                (long)native_scene_frame.corners[0][2],(long)native_scene_frame.corners[4][2],
                (long)native_scene_frame.corners[0][0],(long)native_scene_frame.corners[0][1],
                (unsigned)native_scene_frame.streams[2],(unsigned)native_scene_frame.streams[5]);
            printf("NATIVEBOOTSTRAP scene=%s original-tail=AE5A..AF26 clear-index=%u sky-pattern=%08x shader-depth=%u; GPU presentation separate\n",
                scene,(unsigned)native_bootstrap_render.fill_06900,(unsigned)native_bootstrap_render.background_pattern_6850,
                (unsigned)native_bootstrap_render.shade_depth_6854);
        }
        if(scene_index<2){
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/traffic");
            if(taxi_traffic_assets_load(&traffic_assets,asset_path,(unsigned)scene_index+1))
                fail("Cannot load original city traffic roads.");
        }else taxi_traffic_assets_free(&traffic_assets);
        if(passenger_diagnostic) {
            if(challenge_bound_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge5-actors.ctp");
            else if(challenge_turn_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge4-actors.ctp");
            else snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/%s.ctp",scene);
            if(taxi_passenger_asset_load(&passenger_asset,asset_path)) fail("Cannot load customer data.");
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/results/default-scores.cts");
            if(!native_settings_loaded)load_native_settings(asset_path);
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/guidance/%s.ctg",challenge_bound_drive_preview?"challenge5":challenge_turn_drive_preview?"challenge4":scene);
            if(taxi_guidance_route_load(&guidance_route,asset_path)) fail("Cannot load original guidance route.");
        }
        double lo[3]={1e30,1e30,1e30},hi[3]={-1e30,-1e30,-1e30},extent=0;
        for(uint32_t i=0;i<world.nv;i++) for(int k=0;k<3;k++) {
            int32_t value;memcpy(&value,world.vertices+i*16+k*4,4);
            if(value<lo[k]) lo[k]=value;if(value>hi[k]) hi[k]=value;
        }
        for(int k=0;k<3;k++) {raw_center[k]=(float)((lo[k]+hi[k])/2);if(hi[k]-lo[k]>extent) extent=hi[k]-lo[k];}
        if(extent<=0) fail("Invalid scene bounds.");raw_scale=(float)(2/extent);reset_drive_observed(observer,context);
 }
static void load_scene_graphics(void) {
    for (uint32_t i=0;i<texture_count;++i) {
        Texture *t=&textures[i];glGenTextures(1,&t->id);glBindTexture(GL_TEXTURE_2D,t->id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,(GLsizei)t->width,(GLsizei)t->height,0,GL_RGBA,GL_UNSIGNED_BYTE,t->pixels);
        /* Retain immutable source pixels for original palette transitions. */
    }
}
static void load_drive_graphics(void) {
    release_drive_graphics();
    if(challenge_ball_drive_preview){
        glGenTextures(8,ball_textures);for(unsigned i=0;i<8;i++){
            glBindTexture(GL_TEXTURE_2D,ball_textures[i]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        }
        ball_palette_upload(live_ball_course.balloon_indices,native_scene_palette,31);
    }
    if(challenge_flag_drive_preview){
        glGenTextures(1,&flag_texture);glBindTexture(GL_TEXTURE_2D,flag_texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        flag_palette_upload(live_flag_indices,native_scene_palette,city_fade_cap);
    }
    char asset_path[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,asset_path,MAX_PATH);
    if(!n||n>=MAX_PATH)fail("Graphics asset path too long.");
    char *slash=strrchr(asset_path,'\\');if(!slash)fail("Graphics asset path failed.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/taxis/taxi-%u.rgba",native_game_driver);
        FILE *sprites=fopen(asset_path,"rb");if(!sprites) fail("Cannot load original taxi sprites.");
        unsigned char pixels[128*64*4];glGenTextures(64,taxi_textures);
        for(unsigned i=0;i<64;i++) {
            if(fread(pixels,1,sizeof pixels,sprites)!=sizeof pixels) fail("Truncated taxi sprites.");
            glBindTexture(GL_TEXTURE_2D,taxi_textures[i]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            upload_source_sprite_bank(128,64,pixels,16);
            if(taxi_cache_check) {
                unsigned char readback[128*64*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
                if(memcmp(readback,pixels,sizeof pixels))fail("Selected taxi GPU pixels differ from original source.");
            }
        }
        if(fgetc(sprites)!=EOF||ferror(sprites)) fail("Trailing taxi data or read error.");fclose(sprites);
        if(taxi_cache_check)printf("TAXI GPU: driver%u all64 original frames byte-exact.\n",native_game_driver);
        if(scene_index<2){
            uint8_t palette_bytes[512];uint16_t palette[256];
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/loading/city-%u-palette.rgb555",(unsigned)scene_index);
            FILE *palette_file=fopen(asset_path,"rb");if(!palette_file)fail("Cannot load original city traffic palette.");
            if(fread(palette_bytes,1,512,palette_file)!=512||fgetc(palette_file)!=EOF)fail("Invalid city traffic palette.");
            fclose(palette_file);
            for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(palette_bytes[2*i]|(unsigned)palette_bytes[2*i+1]<<8);
            TaxiShadow *shadow=malloc(sizeof *shadow);if(!shadow)fail("Taxi shadow allocation failed.");
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/taxi-software/taxi-software.ctx");
            if(taxi_shadow_load(shadow,asset_path))fail("Cannot load original taxi shadow bank.");
            glGenTextures(64,shadow_textures);
            for(unsigned i=0;i<64;i++){
                if(taxi_shadow_rgba(shadow,i,palette,pixels))fail("Cannot decode taxi shadow.");
                glBindTexture(GL_TEXTURE_2D,shadow_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                upload_source_sprite_bank(64,32,pixels,16);
                if(taxi_cache_check){
                    unsigned char readback[64*32*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
                    if(memcmp(readback,pixels,sizeof readback))fail("Taxi shadow GPU pixels differ from source indices/city palette.");
                }
            }
            free(shadow);
            if(taxi_cache_check)puts("TAXI SHADOW GPU: all64 original frames with selected city palette byte-exact.");
            TaxiTrafficSprites *art=malloc(sizeof *art);if(!art)fail("Traffic artwork allocation failed.");
            glGenTextures(48,traffic_textures);
            for(unsigned type=0;type<3;type++){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/traffic/traffic-%u.ctb",type);
                if(taxi_traffic_sprites_load(art,asset_path))fail("Cannot load original traffic sprites.");
                for(unsigned frame=0;frame<16;frame++){
                    if(taxi_traffic_sprite_rgba(art,frame,palette,pixels))fail("Cannot decode traffic sprite.");
                    glBindTexture(GL_TEXTURE_2D,traffic_textures[type*16+frame]);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                    upload_source_sprite_bank(128,64,pixels,16);
                    if(taxi_cache_check){
                        uint8_t readback[128*64*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
                        if(memcmp(readback,pixels,sizeof pixels))fail("Traffic GPU sprite differs from original indices/city palette.");
                    }
                }
            }
            free(art);
            if(taxi_cache_check)printf("TRAFFIC GPU: all48 original frames with selected city palette byte-exact.\n");
        }
        if(passenger_diagnostic) {
            TaxiPassengerSpriteAsset art={0};
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/passengers");
            if(taxi_passenger_sprites_load(&art,asset_path)) fail("Cannot load passenger art.");
            glGenTextures(144,passenger_textures);
            for(unsigned i=0;i<144;i++) {
                glBindTexture(GL_TEXTURE_2D,passenger_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                upload_source_sprite(32,64,art.pixels+i*TAXI_PASSENGER_FRAME_BYTES);
            }
            taxi_passenger_sprites_free(&art);
            glGenTextures(10,passenger_marker_textures);
            for(unsigned i=0;i<10;i++){
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/passenger-markers/%s-%u.rgba",i<5?"ring":"class",i%5);
                FILE *marker=fopen(asset_path,"rb");if(!marker)fail("Cannot load original customer marker.");
                unsigned char bitmap[64*64*4];
                if(fread(bitmap,1,sizeof bitmap,marker)!=sizeof bitmap||fgetc(marker)!=EOF||ferror(marker))fail("Invalid customer marker asset.");
                fclose(marker);glBindTexture(GL_TEXTURE_2D,passenger_marker_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                upload_source_sprite(64,64,bitmap);
            }
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/guidance/guidance.rgba");
            FILE *guidance=fopen(asset_path,"rb");if(!guidance) fail("Cannot load original guidance art.");
            unsigned char arrow[64*32*4];glGenTextures(160,guidance_textures);
            for(unsigned i=0;i<160;i++) {
                if(fread(arrow,1,sizeof arrow,guidance)!=sizeof arrow) fail("Truncated guidance art.");
                glBindTexture(GL_TEXTURE_2D,guidance_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                upload_source_sprite_bank(64,32,arrow,5);
            }
            if(fgetc(guidance)!=EOF||ferror(guidance)) fail("Invalid guidance art length.");fclose(guidance);
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/clock.rgba");
            FILE *clock_art=fopen(asset_path,"rb");if(!clock_art) fail("Cannot load original clock art.");
            unsigned char glyph[16*16*4];glGenTextures(52,clock_textures);
            for(unsigned i=0;i<52;i++) {
                if(fread(glyph,1,sizeof glyph,clock_art)!=sizeof glyph) fail("Truncated clock art.");
                glBindTexture(GL_TEXTURE_2D,clock_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                upload_source_sprite_bank(16,16,glyph,6+(int)(i/26));
            }
            if(fgetc(clock_art)!=EOF||ferror(clock_art)) fail("Invalid clock art length.");fclose(clock_art);
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/time-expiry.rgba");
            FILE *expiry_art=fopen(asset_path,"rb");if(!expiry_art)fail("Cannot load original expiry art.");
            unsigned char expiry_frame[32*32*4];glGenTextures(15,expiry_textures);
            for(unsigned i=0;i<15;i++) {
                if(fread(expiry_frame,1,sizeof expiry_frame,expiry_art)!=sizeof expiry_frame)fail("Truncated expiry art.");
                glBindTexture(GL_TEXTURE_2D,expiry_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                upload_source_sprite_bank(32,32,expiry_frame,11);
            }
            if(fgetc(expiry_art)!=EOF||ferror(expiry_art))fail("Invalid expiry art length.");fclose(expiry_art);
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/font-metrics.ctf");
            FILE *metrics=fopen(asset_path,"rb");if(!metrics)fail("Cannot load original font metrics.");
            unsigned char header[8];
            if(fread(header,1,8,metrics)!=8||memcmp(header,"CTF1",4)||header[4]!=224||header[5]||header[6]!=1||header[7])fail("Invalid font metrics header.");
            if(fread(font_metrics,1,sizeof font_metrics,metrics)!=sizeof font_metrics||fread(font_kerning,1,sizeof font_kerning,metrics)!=sizeof font_kerning||fgetc(metrics)!=EOF||ferror(metrics))fail("Invalid font metrics length.");fclose(metrics);
            for(unsigned i=0;i<224;i++)if(font_metrics[i][0]>2||font_metrics[i][1]>2)fail("Invalid font bounds.");
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/font.rgba");
            FILE *font=fopen(asset_path,"rb");if(!font)fail("Cannot load original font art.");
            snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/tips-indexed.bin");FILE *tips=fopen(asset_path,"rb");if(!tips||fread(native_tip_indices,1,sizeof native_tip_indices,tips)!=sizeof native_tip_indices||fgetc(tips)!=EOF)fail("Cannot load source tip bitmap indices.");fclose(tips);
            glGenTextures(1,&native_tip_texture);glBindTexture(GL_TEXTURE_2D,native_tip_texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            unsigned char font_glyph[16*16*4];glGenTextures(896,font_textures);
            for(unsigned i=0;i<896;i++) {
                if(fread(font_glyph,1,sizeof font_glyph,font)!=sizeof font_glyph)fail("Truncated font art.");
                glBindTexture(GL_TEXTURE_2D,font_textures[i]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                upload_source_sprite_bank(16,16,font_glyph,(int)(i/224));
            }
            if(fgetc(font)!=EOF||ferror(font))fail("Invalid font art length.");fclose(font);
            glGenTextures(4,cash_label_textures);
            for(unsigned bank=0;bank<2;bank++) {
                snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/hud/%s-labels.rgba",bank?"pending":"total");
                FILE *labels=fopen(asset_path,"rb");if(!labels)fail("Cannot load original cash labels.");
                unsigned w=bank?80:88,h=bank?32:24;unsigned char pixels[80*32*4];size_t count=w*h*4;
                for(unsigned active=0;active<2;active++) {
                    if(fread(pixels,1,count,labels)!=count)fail("Truncated cash labels.");
                    glBindTexture(GL_TEXTURE_2D,cash_label_textures[bank*2+active]);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                    upload_source_sprite((unsigned)w,(unsigned)h,pixels);
                }
                if(fgetc(labels)!=EOF||ferror(labels))fail("Invalid cash label length.");fclose(labels);
            }
            if(challenge_turn_drive_preview)printf("TURN PASSENGER: %u challenge-initialized actor; ordinary route eligibility is bypassed by original mode4 init. RNG/startup seed and full frame order remain unverified.\n",passenger_scene.count);
            else printf("CUSTOMER/FARE DIAGNOSTIC: %u eligible; endpoint/prior Y=0, recovered yaw with offset2618=0; native customer RNG from seed0, other original consumers/startup seed unverified. Fade and endgame presentation pending.\n",passenger_scene.eligible);
        }
}
static void pump_window_messages(void) {
    MSG message;
    while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){
        if(message.message==WM_QUIT){window_quit_requested=1;continue;}
        TranslateMessage(&message);DispatchMessageW(&message);
    }
}
static int startup_loading_pacing_check,startup_loading_close_check,startup_reload_check;
typedef struct {unsigned records,presentations,bank;FramePacer pacer;} StartupLoadingWork;
static void startup_loading_record(void *context,unsigned pass,size_t index) {
    StartupLoadingWork *work=context;
    if(pass!=work->records/passenger_asset.count||index!=work->records%passenger_asset.count)fail("Startup loading record order failed.");
    work->records++;
    if(startup_loading_close_check&&work->records==1){
        /* Exercise the real window procedure while state is incomplete. */
        PostMessageW(loading_window,WM_KEYDOWN,'R',0);
        PostMessageW(loading_window,WM_CLOSE,0,0);
    }
    pump_window_messages();
    if(index%5==0){
        /* Absolute deadlines include upload/render time. Source record events
           still own animation advances; missed deadlines never replay work. */
        if(!window_quit_requested&&(!startup_loading_snapshot||startup_loading_pacing_check)){
            LARGE_INTEGER now,frequency;QueryPerformanceCounter(&now);
            if(!work->pacer.frequency){
                QueryPerformanceFrequency(&frequency);
                work->pacer=(FramePacer){now.QuadPart,frequency.QuadPart,1,0};
            }
            while(!window_quit_requested&&!frame_due(&work->pacer,now)){
                pump_window_messages();QueryPerformanceCounter(&now);
            }
        }
        work->presentations++;work->bank=(work->bank+1)%5;
        if(!taxi_loading_flow_record(&startup_loading_flow,(unsigned)index))fail("Startup loading work phase failed.");
        loading_preview=startup_loading_flow.animation_bank;
        taxi_loading_flow_present(&startup_loading_flow,1);loading_cap=startup_loading_flow.palette_cap;loading_cap_upload(loading_cap);
        if(!window_quit_requested){
            RECT client;GetClientRect(loading_window,&client);draw_loading_preview(client.right,client.bottom);glFinish();SwapBuffers(loading_dc);
            startup_loading_rendered++;
            /* Normal D898->26318 waits for frame service. Attract D794
               skips that call; this host presentation/audio pacing is provisional.
               Source IRQ16174 audio26888 timing remains independently qualified. */
            if(audio_live){
                if(audio_host&&taxi_audio_host_pump(audio_host)<0)fail("Loading audio output failed.");
                audio_frame();loading_audio_frames++;
            }
        }
    }
}
static void startup_attract_begin(void) {
    uint16_t city_random=(uint16_t)taxi_random_range(&passenger_scene.random_state,0,65535);
    if(taxi_attract_session_begin(&attract_session,native_settings,(uint8_t)native_game_driver,city_random))fail("Attract session entry failed.");
    live_attract=source_attract_ai=1;
    startup_loading_begin();
}
static void startup_loading_begin(void) {
    /* Normal sourceac66..ac98 selects music before loading/vehicle/passengers.
       Same retained diagnostic stream is used by subsequent gameplay consumers. */
    music_context.main_mode=0;music_context.sound_active=0;
    music_context.random=music_shared_random;music_context.context=&passenger_scene.random_state;
    uint8_t loaded=0,choice=0;
    if(taxi_music_session_begin(&music_tracker,&music_asset,music_bytes,181241,
        (uint8_t)(audio_live&&audio_settings_state.music_enabled),&passenger_scene.random_state,&loaded,&choice))
        fail("Original session music initialization failed.");
    music_loaded=loaded;if(loaded)music_starts[choice]++;
    startup_loading_visible=1;loading_preview=startup_loading_flow.animation_bank;loading_cap=0;load_loading_preview();
    taxi_loading_flow_begin(&startup_loading_flow,(uint8_t)loading_preview);startup_loading_rendered=0;loading_cap_upload(0);startup_stage=5;
}
static void challenge_music_begin(void){
    /* Source74EC bootstrap precedes7508 shared RNG; selection occurs even
       when26EF8/27110 music-enable gates suppress playback. */
    unsigned choice=music_shared_random(&passenger_scene.random_state)%4;
    music_context.main_mode=0;music_context.sound_active=0;
    music_context.random=music_shared_random;music_context.context=&passenger_scene.random_state;
    if(!audio_live||!music_bytes)return;
    music_tracker.state.playing=0;music_test_start(choice);
    printf("NATIVE CHALLENGE MUSIC: original order choice%u, retained native RNG; complete bootstrap RNG consumption unverified.\n",choice);
}
static void challenge_course_begin_live(void){
    if(challenge_live_submode>5)fail("Selected challenge session startup is not integrated.");
    challenge_bound_drive_preview=challenge_live_submode==5;challenge_turn_drive_preview=challenge_live_submode==4;challenge_drift_drive_preview=challenge_live_submode==3;challenge_ball_drive_preview=challenge_live_submode==2;challenge_flag_drive_preview=challenge_live_submode==1;challenge_jump_drive_preview=1;live_attract=source_attract_ai=0;
    native_game_driver=challenge_live_driver;recovered_camera=1;
    for(uint32_t i=0;i<texture_count;i++){glDeleteTextures(1,&textures[i].id);free(textures[i].pixels);}
    free(vertices);free(faces);free(materials);free(textures);
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);if(!tail)fail("Jump scene path failed.");
    snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),challenge_bound_drive_preview?"../assets/challenge-worlds/challenge5.ctm":challenge_turn_drive_preview?"../assets/challenge-worlds/challenge4.ctm":challenge_drift_drive_preview?"../assets/challenge-worlds/challenge3.ctm":challenge_ball_drive_preview?"../assets/challenge-worlds/challenge2.ctm":challenge_flag_drive_preview?"../assets/challenge-worlds/challenge1.ctm":"../assets/startup.ctm");load_scene(path);
    drive_mode=1;passenger_diagnostic=challenge_turn_drive_preview||challenge_bound_drive_preview;endgame_diagnostic=0;
    load_drive_state("startup",NULL,NULL);load_scene_graphics();load_drive_graphics();
    startup_stage=3;
    printf("NATIVE CHALLENGE ENTRY: main/Crazy Box choice -> original submode%u course, driver%u, source start; full fidelity pending.\n",challenge_live_submode,native_game_driver);
}
static void challenge_jump_return_grid(void){
    /* Original7ADC..7AEC failure restores menu; success registration already did. */
    if(!music_context.main_mode){music_context.main_mode=1;music_context.sound_active=0;music_test_start(4);}
    challenge_live_selection=challenge_bound_drive_preview?7:challenge_turn_drive_preview?6:challenge_drift_drive_preview?5:challenge_ball_drive_preview?4:challenge_flag_drive_preview?3:2;
    challenge_bound_drive_preview=0;challenge_turn_drive_preview=0;challenge_drift_drive_preview=0;challenge_ball_drive_preview=0;challenge_flag_drive_preview=0;challenge_jump_drive_preview=0;passenger_diagnostic=0;drive_mode=0;startup_mode=1;normal_live_flow.fade.interval_1771=normal_live_flow.fade.reload_1772=1;
    memset(&normal_live_input,0,sizeof normal_live_input);
    startup_stage=challenge_live_begin(0,1);
}
static void startup_drive_begin(void) {
    const char *scene=native_settings[4]?"city2":"city1";
    if(!live_attract)native_game_driver=driver_flow.choice.driver;
    for(uint32_t i=0;i<texture_count;i++) {glDeleteTextures(1,&textures[i].id);free(textures[i].pixels);}
    free(vertices);free(faces);free(materials);free(textures);
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');if(!tail)fail("Startup scene path failed.");
    snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/%s.ctm",scene);load_scene(path);
    drive_mode=1;passenger_diagnostic=1;endgame_diagnostic=!live_attract;
    StartupLoadingWork loading_work={0};loading_work_active=1;
    load_drive_state(scene,startup_loading_record,&loading_work);loading_work_active=0;
    if(loading_work.records!=2*passenger_asset.count||loading_work.presentations!=2*((passenger_asset.count+4)/5)||loading_work.bank!=loading_work.presentations%5)fail("Startup loading work observations failed.");
    printf("STARTUP LOADING WORK: %u actual records, %u source presentation events, final bank%u; %u actual work renders.\n",loading_work.records,loading_work.presentations,loading_work.bank,startup_loading_rendered);
    if(loading_work.pacer.frequency){
        LARGE_INTEGER now;QueryPerformanceCounter(&now);
        printf("STARTUP LOADING PACING: %.3f seconds, %llu skipped deadlines; CPU presentation timing, not monitor scanout.\n",(double)(now.QuadPart-loading_work.pacer.epoch)/loading_work.pacer.frequency,(unsigned long long)loading_work.pacer.skipped);
    }
    if(live_attract) {
        taxi_ai_path_free(&attract_ai_path);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/ai/%s-path.cta",scene);
        if(taxi_ai_path_load(&attract_ai_path,path))fail("Live attract path load failed.");
    }
    load_scene_graphics();load_drive_graphics();
    printf("STARTUP DRIVING: selected city%u/driver%u; recovered native driving/passenger prototype, full original startup fade/audio/traffic pending.\n",native_settings[4],native_game_driver);
}
static void check_native_retry_input(void){
    if(startup_stage!=3||audio_live||challenge_jump_drive_preview||live_attract)
        fail("Native Retry input check requires muted ordinary startup.");
    for(unsigned n=0;n<500&&(live_game_start.active||city_fade.active_177b);n++)startup_tick(0);
    if(live_game_start.active||city_fade.active_177b)fail("Retry check did not reach ordinary driving.");
    unsigned selected_city=native_settings[4],selected_driver=native_game_driver;
    uint8_t settings_before[512];memcpy(settings_before,native_settings,512);
    int32_t initial_clock=taxi_game_initial_clock(&passenger_scene.session.settings,0);
    passenger_scene.session.trip.initial_time_16b0=12345; /* Nonzero retained-owner fixture for this diagnostic only. */
    TaxiPassengerScene expected=passenger_scene;
    if(taxi_passenger_scene_reinit(&expected,&passenger_asset,&world,0))fail("Retry passenger reference failed.");
    uint32_t expected_rng=passenger_scene.random_state;
    (void)music_shared_random(&expected_rng);
    (void)taxi_random_range(&expected_rng,20,60); /* Original14D10 vehicle request before passenger passes. */
    memset(&native_pause,0,sizeof native_pause);pause_host_frames=1;
    static const uint16_t keys[]={8,0,128,0,1,0,64,0,1};
    for(unsigned i=0;i<sizeof keys/sizeof keys[0];i++){
        if(taxi_pause_runtime_frame(&native_pause,keys[i],0,0,0,240,&native_pause_frame))fail("Retry input controller failed.");
        pause_consume_audio(&native_pause_frame.events);
    }
    if(native_pause.menu.outer_state!=1||!native_pause.menu.frozen||native_pause.menu.exit_flag||!native_pause_frame.events.fade_out)
        fail("Original Restart Yes input failed.");
    passenger_scene.session.outer_state_1ea8=1;
    taxi_palette_fade_begin(&city_fade,0);unsigned fade_ticks=0;
    while(city_fade.active_177b&&fade_ticks<100){city_fade_tick(0,1);fade_ticks++;}
    if(city_fade.active_177b||fade_ticks!=33)fail("Retry original fade did not finish.");
    TaxiNormalExit transfer={.outer=1,.frozen=1,.ended=passenger_scene.session.ended_1697,
        .qualified=passenger_scene.session.fare.qualified_1691};TaxiNormalExitRequests requests;
    if(taxi_normal_exit_begin(&transfer,&requests)!=2||requests.count!=5||requests.calls[0]!=0x08010aac||requests.calls[4]!=0x0800ac70)
        fail("Retry restart transfer requests failed.");
    uint32_t stops[10];
    if(taxi_game_audio_cleanup_requests(live_sound_state.handles,stops))fail("Muted Retry unexpectedly owns sound handles.");
    music_tracker.state.playing=0;
    startup_loading_begin();
    for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
    if(startup_stage!=3||startup_loading_rendered!=52||native_settings[4]!=selected_city||native_game_driver!=selected_driver||
       memcmp(settings_before,native_settings,512)||passenger_scene.random_state!=expected_rng||
       memcmp(expected.customers,passenger_scene.customers,sizeof expected.customers)||
       passenger_scene.session.fare.total_whole_1698||passenger_scene.session.fare.total_fraction_169c||
       passenger_scene.session.clock.remaining_16a8!=initial_clock||passenger_scene.session.outer_state_1ea8||
       passenger_scene.session.trip.initial_time_16b0!=12345)
        fail("Native Retry reload lost retained selection, RNG or session reset.");
    memset(&native_pause,0,sizeof native_pause);pause_host_frames=0;
    if(!live_game_start.active)fail("Retry skipped source Ready/Set/Go intro.");
    for(unsigned n=0;n<500&&(live_game_start.active||city_fade.active_177b);n++)startup_tick(0);
    TaxiPoint before=car.world.height.position;
    for(unsigned n=0;n<60;n++)drive_tick((TaxiXboxSample){0},1);
    if(simulation_error||(car.world.height.position.x==before.x&&car.world.height.position.z==before.z))
        fail("Native Retry did not resume driving.");
    if(taxi_normal_exit_finish(&transfer,1,&requests)||requests.count)
        fail("Retry incorrectly resumed old parent cleanup.");
    startup_quit_frozen_car=car;startup_quit_frozen_clock=passenger_scene.session.clock.remaining_16a8;
    pause_host_frames=1;startup_audio_cleanup_smoke=4;startup_quit_input_tick=1;
    startup_quit_complete=0;startup_quit_fade_ticks=0;
    static const uint16_t quit_keys[]={8,0,128,0,128,0,1,0,64,0,1};
    for(unsigned i=0;i<sizeof quit_keys/sizeof quit_keys[0];i++)startup_tick(quit_keys[i]);
    for(unsigned n=0;n<100&&!startup_quit_complete;n++)startup_tick(0);
    if(!startup_quit_complete||startup_quit_fade_ticks!=33||native_pause.menu.outer_state!=2||!native_pause.menu.exit_flag)
        fail("Subsequent actual Quit Yes did not complete.");
    TaxiNormalExit quit={.outer=2,.frozen=1,.exit_flag=1};
    if(taxi_normal_exit_begin(&quit,&requests)!=1||requests.count!=4||
       taxi_normal_exit_prepare(&quit,0,&requests)!=1||requests.count||quit.callback!=0x080174dd||quit.frozen||quit.results)
        fail("Subsequent Quit parent cleanup failed.");
    native_pause.menu.outer_state=quit.outer;native_pause.menu.frozen=quit.frozen;
    passenger_scene.session.outer_state_1ea8=quit.outer;
    startup_audio_cleanup_smoke=0;startup_quit_input_tick=0;pause_host_frames=0;
    music_context.main_mode=1;startup_parent_begin(1,1);
    for(unsigned n=0;n<80;n++)startup_tick(0);
    if(startup_stage!=9||native_pause.menu.frozen||memcmp(settings_before,native_settings,512))
        fail("Subsequent Quit did not return to parent menu with retained settings.");
    puts("PASS native Restart Yes then Quit Yes diagnostic:33 source fade ticks each, AC70 transfer, retained city/driver/settings/RNG,52 loading renders, fare/clock reset, Ready/Set/Go,60 resumed driving ticks and parent menu9; full recovered-flow adapters/audio/interactive Retry pending.");
}
/* Actual source packet, captured with diagnostic cache1/depth state. */
static void check_cached_texture_flat_packet(int width,int height) {
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Flat fixture path failed.");
    snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../reports/city%u-flat-cache-packet.bin",native_settings[4]+1);
    FILE *file=fopen(path,"rb");if(!file)fail("Captured flat fixture missing.");
    uint32_t info[8];uint8_t material[28];TaxiWorldFramePacket packet={0};
    if(fread(info,sizeof info,1,file)!=1||fread(material,28,1,file)!=1||
       info[0]!=0x31505441||info[1]!=(unsigned)scene_index+1||info[3]<3||info[3]>8||
       info[4]>=world.nm||info[5]==255||memcmp(material,world.materials+info[4]*28,28))
        fail("Captured flat fixture metadata mismatch.");
    packet.material=info[4];packet.texture=info[5];packet.raster_flags=(uint16_t)info[6];
    packet.raster_flags_valid=1;packet.source_index=info[7];packet.polygon.count=info[3];
    for(unsigned i=0;i<info[3];i++) {
        int32_t values[4];if(fread(values,sizeof values,1,file)!=1)fail("Flat fixture truncated.");
        TaxiWorldClipVertex *v=packet.polygon.vertices+i;
        v->screen_x=values[0];v->screen_y=values[1];v->attribute=values[2];v->z=values[3];
    }
    if(fgetc(file)!=EOF)fail("Flat fixture trailing data.");fclose(file);
    TaxiWorldFrame frame={.world=&world,.packets=&packet,.count=1};
    uint32_t callback=taxi_world_indexed_callback(&packet,&world);
    if(callback!=0x03006950&&callback!=0x0300690c)fail("Fixture does not resolve pure flat callback.");
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    unsigned before=native_flat_pattern_draws;QueueDraw context={.frame=&frame,.scale=1,.offset=0};
    if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0)||native_flat_pattern_draws!=before+1)
        fail("Textured cache fixture missed production flat dispatch.");
    glFinish();uint8_t pixels[240*160*4];glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    unsigned checked=0;
    for(int y=1;y<159;y++)for(int x=1;x<239;x++) {
        /* Stay two source pixels from every edge; coverage rounding is a separate dependency. */
        int sign=0,inside=1;
        for(unsigned i=0;i<packet.polygon.count;i++) {
            const TaxiWorldClipVertex *a=packet.polygon.vertices+i,*b=packet.polygon.vertices+(i+1)%packet.polygon.count;
            double dx=(double)b->screen_x-a->screen_x,dy=(double)b->screen_y-a->screen_y;
            double cross=dx*(y+0.5-a->screen_y)-dy*(x+0.5-a->screen_x);
            if(cross*cross<4*(dx*dx+dy*dy)||(!dx&&!dy)){inside=0;break;}
            int edge=cross>0?1:-1;if(sign&&edge!=sign){inside=0;break;}sign=edge;
        }
        if(!inside)continue;
        uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[material[14+((x+y)&1)]],(int16_t)city_fade_cap);
        unsigned offset=((159-y)*240+x)*4;
        for(unsigned c=0;c<3;c++)if(abs((int)pixels[offset+c]-(int)(((color>>(c*5))&31)*255/31))>1)
            fail("Captured textured-to-flat GPU palette/parity mismatch.");
        if(pixels[offset+3]!=255)fail("Captured flat packet opacity mismatch.");checked++;
    }
    if(checked<8)fail("Captured flat packet lacks verified interior pixels.");
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS captured textured-to-flat production GPU: texture%u/material%u, %u interior palette/parity pixels; artificial cache1/depth fixture, coverage edges excluded.\n",packet.texture,packet.material,checked);
}
static void check_unshaded_bitmap_coverage(void){
    uint8_t rgba[16]={0},pixels[4*2*4];rgba[11]=rgba[15]=255;
    TaxiIndexedSpan spans[2]={{.x=0,.pairs=2,.du=32768},{.x=0,.pairs=2,.du=32768}};
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_VIEWPORT_BIT|GL_TEXTURE_BIT|GL_CURRENT_BIT);
    glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_DEPTH_TEST);glDisable(GL_SCISSOR_TEST);
    glViewport(0,0,4,2);glClearColor(1,0,1,0);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,4,2,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLuint texture;glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,4,1,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
    if(!taxi_axis_shader_begin(spans,2,0,0,4,1,1))fail("Unshaded BG coverage shader setup failed.");
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(4,0);glVertex2i(4,0);
    glTexCoord2i(4,2);glVertex2i(4,2);glTexCoord2i(0,2);glVertex2i(0,2);glEnd();
    taxi_axis_shader_end();glFinish();glReadPixels(0,0,4,2,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<2;y++)for(unsigned x=0;x<4;x++){
        unsigned at=(y*4+x)*4;
        if(pixels[at]!=(x<2?255:0)||pixels[at+1]!=0||pixels[at+2]!=(x<2?255:0)||
           pixels[at+3]!=(x<2?0:255))fail("Unshaded keyed zero versus black BG coverage mismatch.");
    }
    if(glGetError()!=GL_NO_ERROR)fail("Unshaded BG coverage GPU error.");
    glDeleteTextures(1,&texture);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts("PASS unshaded BG coverage: keyed index0 retains backdrop alpha0, nonzero black writes BG alpha255; paired native sampling.");
}
static void check_flat_bitmap_coverage(void){
    uint8_t material[28]={0},pixels[16*8*4];material[15]=1;
    TaxiWorldAsset fixture={.materials=material,.nm=1};
    TaxiWorldFramePacket packet={.texture=255,.raster_flags=32|4,.raster_flags_valid=1};
    packet.polygon.count=4;
    const int32_t xs[4]={0,16,16,0},ys[4]={0,0,8,8};
    for(unsigned i=0;i<4;i++){packet.polygon.vertices[i].screen_x=xs[i];packet.polygon.vertices[i].screen_y=ys[i];packet.polygon.vertices[i].z=1024;}
    TaxiWorldFrame frame={.world=&fixture,.packets=&packet,.count=1};
    QueueDraw context={.frame=&frame,.scale=1};
    uint16_t saved[2]={native_scene_palette[0],native_scene_palette[1]};
    int saved_cap=city_fade_cap;native_scene_palette[0]=native_scene_palette[1]=0x7fff;
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_VIEWPORT_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glViewport(0,0,16,8);glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,16,8,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();unsigned checked=0;
    for(unsigned cap=0;cap<=31;cap+=31)for(int coverage=0;coverage<=1;coverage++){
        city_fade_cap=(int)cap;context.bitmap_coverage=coverage;
        glEnable(GL_ALPHA_TEST);glClearColor(1,0,1,1);glClear(GL_COLOR_BUFFER_BIT);
        if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0)||!glIsEnabled(GL_ALPHA_TEST))
            fail("Flat BG coverage dispatch/alpha-test restore failed.");
        glFinish();glReadPixels(0,0,16,8,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        for(unsigned y=0;y<8;y++)for(unsigned x=0;x<16;x++){
            unsigned at=((7-y)*16+x)*4,index=(x+y)&1,alpha=coverage&&!index?0:255;
            for(unsigned c=0;c<3;c++)if(pixels[at+c]!=cap*255/31)fail("Written flat index0 was discarded or color changed.");
            if(pixels[at+3]!=alpha)fail("Flat BG coverage/parity mismatch.");checked++;
        }
    }
    native_scene_palette[0]=saved[0];native_scene_palette[1]=saved[1];city_fade_cap=saved_cap;
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS production flat BG coverage: %u GPU pixels, source index0/1 parity, identical colors, caps0/31, retained writes and alpha-test restore.\n",checked);
}
static void check_material_bitmap_coverage(int width,int height){
    (void)width;(void)height;
    uint8_t material[28]={0},pixels[16*8*4];uint16_t palette[256];
    for(unsigned i=0;i<256;i++)palette[i]=0x7fff; /* Same RGB, different indices. */
    for(unsigned i=0;i<16;i++)material[i]=(uint8_t)(i%4?i+1:0);
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_VIEWPORT_BIT|GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glViewport(0,0,16,8);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,16,8,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    unsigned checked=0;
    for(unsigned cap=0;cap<=31;cap+=31)for(int coverage=0;coverage<=1;coverage++){
        if(!taxi_material_shader_begin(material,palette,(int16_t)cap)||
           !taxi_material_shader_bitmap_coverage(coverage))fail("Material BG coverage shader setup failed.");
        if(taxi_material_shader_bitmap_coverage(2))fail("Invalid material coverage mode accepted.");
        glBegin(GL_QUADS);
        glTexCoord3i(0,0,0);glVertex2i(0,0);glTexCoord3i(16,0,0);glVertex2i(16,0);
        glTexCoord3i(16,8,8);glVertex2i(16,8);glTexCoord3i(0,8,8);glVertex2i(0,8);
        glEnd();taxi_material_shader_end();
        if(taxi_material_shader_bitmap_coverage(1))fail("Inactive material coverage accepted.");
        glFinish();
        glReadPixels(0,0,16,8,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        if(glGetError()!=GL_NO_ERROR)fail("Material BG coverage GPU readback failed.");
        for(unsigned y=0;y<8;y++)for(unsigned x=0;x<16;x++){
            unsigned index=y*2+((x+y)&1),at=((7-y)*16+x)*4;
            unsigned alpha=coverage&&!material[index]?0:255;
            for(unsigned c=0;c<3;c++)if(pixels[at+c]!=cap*255/31)
                fail("Material coverage changed equal-color RGB/fade.");
            if(pixels[at+3]!=alpha)fail("Material BG coverage confused palette color with index.");
            checked++;
        }
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS material BG coverage: %u GPU pixels, all16 attribute/parity slots, identical RGB indices, caps0/31, opaque default and invalid-mode retention.\n",checked);
}
static void check_flat_scene_packet(int width,int height){
    if(!native_flat_source_valid)fail("Actual scene flat fixture unavailable.");
    TaxiWorldFramePacket packet=native_flat_source_packet;uint8_t oracle[38400],pixels[38400*4];memset(oracle,111,sizeof oracle);TaxiFlatRowWork work;
    if(taxi_world_indexed_flat_packet_work(oracle,&packet,&world,&work)!=1)fail("Actual flat native oracle failed.");
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    uint16_t background=taxi_palette_fade_cap_rgb555(native_scene_palette[111],(int16_t)city_fade_cap);
    glClearColor((background&31)/31.0f,((background>>5)&31)/31.0f,((background>>10)&31)/31.0f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    TaxiWorldFrame frame={.world=&world,.packets=&packet,.count=1};QueueDraw context={.frame=&frame,.scale=1};
    int saved_snapped=native_axis_snapped_check;native_axis_snapped_check=1;
    if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0))fail("Actual flat production draw failed.");
    native_axis_snapped_check=saved_snapped;
    glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[y*240+x]],(int16_t)city_fade_cap);
        for(unsigned c=0;c<3;c++){unsigned expected=((color>>(5*c))&31)*255/31,actual=pixels[((159-y)*240+x)*4+c];
            if(abs((int)actual-(int)expected)>2){fprintf(stderr,"Actual flat pixel%u,%u channel%u got%u expected%u flags%u vertices%u\n",x,y,c,actual,expected,(unsigned)packet.raster_flags,(unsigned)packet.polygon.count);fail("Actual flat complete GPU coverage mismatch.");}}
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts("PASS actual scene flat GPU complete coverage:38400 source-screen pixels versus recovered native batch; native prepared packet not original full-frame proof.");
}
static void check_axis_continuous_table(int width,int height) {
    TaxiIndexedSpan spans[1024];uint8_t texture[16*16*4];
    for(unsigned i=0;i<1024;i++)spans[i]=(TaxiIndexedSpan){0,160,3*16384+4096,(i%16)*16384+4096,0,8192,0,0};
    for(unsigned y=0;y<16;y++)for(unsigned x=0;x<16;x++) {
        unsigned i=(y*16+x)*4;texture[i]=(uint8_t)(x*16);texture[i+1]=(uint8_t)(y*16);texture[i+2]=77;texture[i+3]=255;
    }
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLuint image;glGenTextures(1,&image);glBindTexture(GL_TEXTURE_2D,image);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,16,16,0,GL_RGBA,GL_UNSIGNED_BYTE,texture);
    GLint active_before,binding_before;glGetIntegerv(0x84e0,&active_before);glGetIntegerv(GL_TEXTURE_BINDING_2D,&binding_before);
    if(!taxi_axis_shader_begin(spans,1024,1,0,16,16,0))fail("Continuous1024 span shader failed.");
    for(unsigned cell=0;cell<2;cell++) {
        double source=cell?2046.5:1024.5;int x=(int)cell*32;
        glBegin(GL_QUADS);
        glTexCoord2d(source,0.5);glVertex2i(x,0);
        glTexCoord2d(source+2,0.5);glVertex2i(x+32,0);
        glTexCoord2d(source+2,16.5);glVertex2i(x+32,32);
        glTexCoord2d(source,16.5);glVertex2i(x,32);glEnd();
    }
    taxi_axis_shader_end();GLint active_after,binding_after;
    glGetIntegerv(0x84e0,&active_after);glGetIntegerv(GL_TEXTURE_BINDING_2D,&binding_after);
    if(active_before!=active_after||binding_before!=binding_after)fail("Axis shader changed source texture binding/unit.");
    glFinish();uint8_t pixels[64*32*4];glReadPixels(0,height-32,64,32,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<32;y++)for(unsigned x=0;x<64;x++) {
        unsigned texel_x=(unsigned)floor(3.25+0.25*(y+0.5));
        unsigned texel_y=x<32?(unsigned)floor(0.25+(x+0.5)/32.0):15;
        unsigned i=((31-y)*64+x)*4;
        if(pixels[i]!=texel_x*16||pixels[i+1]!=texel_y*16||pixels[i+2]!=77||pixels[i+3]!=255)
            fail("Continuous GPU subpixel/table endpoint mismatch.");
    }
    glDeleteTextures(1,&image);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts("PASS continuous GPU axis adaptation: 2048 subpixel reads, varying UV within source rows, span512/interpolation and final span1023; source texture unit/binding retained. Synthetic fixture, not original-game fidelity.");
}
static void check_axis_continuous_coverage(int width,int height){
    TaxiIndexedSpan spans[16];uint8_t texel[4]={200,100,50,255},pixels[32*32*4];
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLuint image;glGenTextures(1,&image);glBindTexture(GL_TEXTURE_2D,image);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,texel);
    unsigned written=0,retained=0;
    for(unsigned columns=0;columns<2;columns++){
        for(unsigned i=0;i<16;i++)spans[i]=(TaxiIndexedSpan){2+(int)(i%3),2+i%2,0,0,0,0,0,0};
        glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
        if(!taxi_axis_shader_begin(spans,16,(int)columns,0,1,1,0))fail("Continuous coverage shader setup failed.");
        glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(16,0);glVertex2i(32,0);
        glTexCoord2i(16,16);glVertex2i(32,32);glTexCoord2i(0,16);glVertex2i(0,32);glEnd();taxi_axis_shader_end();
        glFinish();glReadPixels(0,height-32,32,32,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        for(unsigned y=0;y<32;y++)for(unsigned x=0;x<32;x++){
            double px=(x+0.5)/2,py=(y+0.5)/2;
            double axis=columns?(px-0.5)/2:py-0.5,position=(columns?py:px)-0.5;
            int i=(int)floor(axis);if(i<0)i=0;if(i>15)i=15;
            double factor=columns?1:2,start=spans[i].x,end=start+spans[i].pairs*factor;
            if(i<15){double t=fmax(axis,0)-floor(fmax(axis,0));start+=(spans[i+1].x-start)*t;end+=(spans[i+1].x+spans[i+1].pairs*factor-end)*t;}
            int inside=position>=start-0.5&&position<end-0.5;unsigned at=((31-y)*32+x)*4;
            if(pixels[at]!=(inside?200:0)||pixels[at+1]!=(inside?100:0)||pixels[at+2]!=(inside?50:0)||pixels[at+3]!=255){fprintf(stderr,"Continuous coverage axis%u pixel%u,%u\n",columns,x,y);fail("Continuous interpolated raster boundary mismatch.");}
            if(inside)written++;else retained++;
        }
    }
    if(!written||!retained)fail("Continuous coverage fixture lacked both write and retain regions.");
    glDeleteTextures(1,&image);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS continuous axis coverage:2048 GPU pixels, %u writes/%u retained; varying row/column extents, controlled adaptation fixture not original 4K frame proof.\n",written,retained);
}
static void check_shade_lookup_boundaries(int width,int height,int flat) {
    TaxiIndexedSpan spans[12];uint8_t texels[4]={0,1,2,255},lookup[2560];uint16_t palette[256];
    for(unsigned i=0;i<12;i++)spans[i]=(TaxiIndexedSpan){0,4,0,0,(uint32_t)((int)i-1)*4096+2048,16384,0,0};
    for(unsigned i=0;i<2560;i++)lookup[i]=(uint8_t)(i%256+i/256*19);
    for(unsigned bank=0;bank<10;bank++)lookup[bank*256+1]=0;
    for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)((i*3%32)|((i*7%32)<<5)|((i*11%32)<<10));
    palette[0]=31|(31<<10);TaxiIndexedTexture texture={4,1,2,0,texels,4};
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glClearColor(7/255.0f,19/255.0f,31/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLint alignment_before;glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment_before);
    if(!taxi_axis_shader_begin(spans,12,0,0,4,1,1)||!taxi_axis_shader_lookup(&texture,lookup,palette,7))fail("Shade boundary fixture failed.");
    if(flat&&!taxi_axis_shader_flat(0,255))fail("Flat shade boundary setup failed.");
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(8,0);glVertex2i(8,0);
    glTexCoord2i(8,12);glVertex2i(8,12);glTexCoord2i(0,12);glVertex2i(0,12);glEnd();
    taxi_axis_shader_end();GLint alignment_after;glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment_after);
    if(alignment_before!=alignment_after)fail("Shade lookup changed unpack alignment.");
    glFinish();uint8_t pixels[8*12*4];glReadPixels(0,height-12,8,12,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<12;y++)for(unsigned x=0;x<8;x++) {
        int bank=(int)y-1;if(bank<0)bank=0;if(bank>9)bank=9;unsigned texel=flat?((x+y)&1?255:0):texels[x/2];
        uint16_t color=taxi_palette_fade_cap_rgb555(palette[lookup[bank*256+texel]],7);
        for(unsigned c=0;c<3;c++) {
            unsigned expected=(flat||texel)?(((color>>(c*5))&31)*255/31):(c==0?7:c==1?19:31);
            if(abs((int)pixels[((11-y)*8+x)*4+c]-(int)expected)>1)fail("Shade GPU bank/key/mapped-zero/fade mismatch.");
        }
        if(pixels[((11-y)*8+x)*4+3]!=255)fail("Shade mapped zero lost opaque alpha.");
    }
    if(!taxi_axis_shader_begin(spans,12,0,0,4,1,1)||
       !taxi_axis_shader_lookup(&texture,lookup,palette,7)||
       (flat&&!taxi_axis_shader_flat(0,255))||
       !taxi_axis_shader_bitmap_coverage(1))fail("BG coverage shader setup failed.");
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(8,0);glVertex2i(8,0);
    glTexCoord2i(8,12);glVertex2i(8,12);glTexCoord2i(0,12);glVertex2i(0,12);glEnd();
    taxi_axis_shader_end();glFinish();glReadPixels(0,height-12,8,12,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<12;y++)for(unsigned x=0;x<8;x++){
        int bank=(int)y-1;if(bank<0)bank=0;if(bank>9)bank=9;
        unsigned texel=flat?((x+y)&1?255:0):texels[x/2];
        unsigned mapped=lookup[bank*256+texel];
        unsigned alpha=(!flat&&!texel)?255:mapped?255:0;
        uint16_t color=taxi_palette_fade_cap_rgb555(palette[mapped],7);
        unsigned at=((11-y)*8+x)*4;
        if(pixels[at+3]!=alpha)fail("BG coverage lost original mapped index.");
        if(flat||texel)for(unsigned c=0;c<3;c++)
            if(abs((int)pixels[at+c]-(int)(((color>>(c*5))&31)*255/31))>1)
                fail("BG coverage changed raster color.");
    }
    puts("PASS shaded BG coverage:96 pixels, all10 lookup banks, keyed-zero discard versus written mapped-zero, cap-independent alpha.");
    if(flat) {
        uint8_t material[28]={0};material[15]=255;
        TaxiWorldAsset fixture_world={.materials=material,.nm=1};
        TaxiWorldFramePacket packet={.texture=255,.raster_flags=512|32,.raster_flags_valid=1};packet.polygon.count=4;
        const int32_t x[4]={0,8,8,0},y[4]={0,0,12,12};
        for(unsigned i=0;i<4;i++){packet.polygon.vertices[i].screen_x=x[i];packet.polygon.vertices[i].screen_y=y[i];packet.polygon.vertices[i].z=1024;}
        TaxiWorldFrame frame={.world=&fixture_world,.packets=&packet,.count=1};
        QueueDraw context={.frame=&frame,.scale=1,.depth_valid=1,.far_depth=2048,.shade_depth=1024};
        if(taxi_world_indexed_callback(&packet,&fixture_world)!=0x0300694c)fail("Flat shaded fixture callback mismatch.");
        native_axis_snapped_check=1;unsigned before=native_axis_draws;
        if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0)||native_axis_draws!=before+1)fail("Flat shaded production dispatch failed.");
        native_axis_snapped_check=0;glFinish();glReadPixels(0,height-12,8,12,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        uint8_t oracle[38400];memset(oracle,111,sizeof oracle);
        if(taxi_world_indexed_packet(oracle,&packet,&fixture_world,&native_indexed_scene,native_scene_lookup,2048,1024)!=1)fail("Flat shaded native oracle failed.");
        for(unsigned row=0;row<12;row++)for(unsigned col=0;col<8;col++) {
            uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[row*240+col]],(int16_t)city_fade_cap);
            for(unsigned c=0;c<3;c++)if(abs((int)pixels[((11-row)*8+col)*4+c]-(int)(((color>>(c*5))&31)*255/31))>1)
                fail("Flat shaded production GPU/native oracle mismatch.");
        }
        puts("PASS production694c flat-shaded dispatch:96 native indexed oracle pixels; synthetic packet/real registered palette, not live source selection.");
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts(flat?"PASS flat shaded GPU boundaries: 96 pixels, all10 banks/clamps, row parity, index0 opaque and cap7; diagnostic fixture.":"PASS shaded GPU boundaries: 96 pixels, all10 banks, negative/high clamps, texel0 key before lookup, mapped0 opaque, texel255 and cap7; diagnostic fixture.");
}
static void check_special_lookup_bytes(int width,int height) {
    uint8_t texels[256],lookup[2560];uint16_t palette[256];TaxiIndexedSpan spans[2];
    for(unsigned i=0;i<256;i++){texels[i]=(uint8_t)i;palette[i]=(uint16_t)((i&31)|(((i>>5)&7)<<5)|((i*11%32)<<10));}
    for(unsigned i=0;i<2560;i++)lookup[i]=(uint8_t)(i%256+i/256*19);
    for(unsigned y=0;y<2;y++)spans[y]=(TaxiIndexedSpan){0,128,0,y*16384,3*4096,16384,0,0};
    TaxiIndexedTexture texture={128,2,7,1,texels,256};
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glClearColor(7/255.0f,19/255.0f,31/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    if(!taxi_axis_shader_begin(spans,2,0,0,128,2,1)||!taxi_axis_shader_lookup(&texture,lookup,palette,31)||!taxi_axis_shader_special())fail("Special lookup fixture failed.");
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(256,0);glVertex2i(256,0);
    glTexCoord2i(256,2);glVertex2i(256,2);glTexCoord2i(0,2);glVertex2i(0,2);glEnd();taxi_axis_shader_end();glFinish();
    uint8_t pixels[256*2*4];glReadPixels(0,height-2,256,2,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned y=0;y<2;y++)for(unsigned x=0;x<256;x++) {
        unsigned texel=y*128+x/2,next=(texel&~3u)|((texel+1)&3u);
        unsigned mapped=(x&1)?(lookup[3*256+texel]|lookup[3*256+next]):lookup[3*256+texel];
        uint16_t color=palette[mapped];
        for(unsigned c=0;c<3;c++) {
            unsigned expected=texel?(((color>>(c*5))&31)*255/31):(c==0?7:c==1?19:31);
            if(abs((int)pixels[((1-y)*256+x)*4+c]-(int)expected)>1)fail("Special rotated lookup byte/OR/key mismatch.");
        }
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts("PASS special shaded GPU: all256 texels/512 pair pixels, four byte rotations, aligned-word next wrap, pre-palette OR and zero key; synthetic fixture.");
}
/* Original indexed oracle plus controlled embedded colors; active BG palette unproved. */
static void check_marker_source_pixels(int width,int height){
    TaxiPassengerRender saved=queued_passenger_markers[0];unsigned saved_count=queued_passenger_marker_count;
    int saved_cap=city_fade_cap,saved_snapped=native_axis_snapped_check;
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');
    if(!tail)fail("Marker GPU fixture path failed.");
    unsigned checked=0;
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    city_fade_cap=31;native_axis_snapped_check=1;queued_passenger_marker_count=1;
    for(unsigned k=0;k<10;k++){
        uint8_t oracle[38400],palette_bytes[512],pixels[38400*4];
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../reports/marker-original-frame-%u.bin",k);
        FILE *f=fopen(path,"rb");if(!f||fread(oracle,1,sizeof oracle,f)!=sizeof oracle||fgetc(f)!=EOF)fail("Original marker framebuffer fixture missing.");fclose(f);
        snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/passenger-markers/%s-%u.rgb555",k<5?"ring":"class",k%5);
        f=fopen(path,"rb");if(!f||fread(palette_bytes,1,512,f)!=512||fgetc(f)!=EOF)fail("Marker color fixture missing.");fclose(f);
        queued_passenger_markers[0]=(TaxiPassengerRender){0};
        queued_passenger_markers[0].texture=k;
        queued_passenger_markers[0].screen=(TaxiTrafficScreenBounds){30,20,94,84};
        glClearColor(7/255.0f,19/255.0f,31/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
        QueueDraw context={.scale=1};
        if(draw_queue_packet(&context,TAXI_RENDER_PASSENGER_MARKER,0))fail("Marker production GPU callback missed.");
        glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
            unsigned index=oracle[y*240+x];unsigned color=palette_bytes[index*2]|((unsigned)palette_bytes[index*2+1]<<8);
            for(unsigned c=0;c<3;c++){
                unsigned expect=index?(((color>>(c*5))&31)*255/31):(c==0?7:c==1?19:31);
                unsigned actual=pixels[((159-y)*240+x)*4+c];
                if(abs((int)actual-(int)expect)>1){fprintf(stderr,"Marker%u pixel%u,%u channel%u got%u expected%u\n",k,x,y,c,actual,expect);fail("Original marker GPU pixel comparison failed.");}
                checked++;
            }
        }
    }
    queued_passenger_markers[0]=saved;queued_passenger_marker_count=saved_count;city_fade_cap=saved_cap;native_axis_snapped_check=saved_snapped;
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS original marker GPU:10 source frames,%u RGB channels;240x160 snapped, controlled embedded palettes; original active palette/4K continuous pixels unverified.\n",checked);
}

static void check_axis_source_packet(int width,int height,int family) {
    const char *names[5]={"rows","columns","clipped","unclipped","shaded"};const char *name=names[family];
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');
    if(!tail)fail("Axis fixture path failed.");
    snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),family==4?"../reports/city%u-%s-stress-packet.bin":"../reports/city%u-perspective-%s-packet.bin",scene_index+1,name);
    FILE *file=fopen(path,"rb");uint32_t info[11]={0};uint8_t material[28];TaxiWorldFramePacket packet={0};
    if(!file||fread(info,4,family==4?11:9,file)!=(size_t)(family==4?11:9)||fread(material,28,1,file)!=1||
       info[0]!=(family==4?0x31504853u:0x31505655u)||info[1]!=(unsigned)scene_index+1||info[3]<3||info[3]>8||
       info[4]>=world.nm||info[5]>=native_indexed_scene.count||memcmp(material,world.materials+info[4]*28,28))fail("Axis fixture metadata mismatch.");
    packet.material=info[4];packet.texture=info[5];packet.source_index=info[7];
    packet.raster_flags=(uint16_t)info[6];packet.raster_flags_valid=1;packet.polygon.count=info[3];
    for(unsigned i=0;i<info[3];i++) {
        int32_t v[5];if(fread(v,sizeof v,1,file)!=1)fail("Axis fixture truncated.");
        TaxiWorldClipVertex *out=packet.polygon.vertices+i;
        out->screen_x=v[0];out->screen_y=v[1];out->u=v[2];out->v=v[3];out->z=v[4];
    }
    if(fgetc(file)!=EOF)fail("Axis fixture trailing data.");fclose(file);
    if(taxi_world_indexed_callback(&packet,&world)!=info[8])fail("Axis fixture callback mismatch.");
    uint8_t oracle[38400],lookup[2560];memset(oracle,111,sizeof oracle);
    for(unsigned i=0;i<2560;i++)lookup[i]=(uint8_t)(i%256+i/256*19);
    const uint8_t *active_lookup=family==4?lookup:native_scene_lookup;
    if(taxi_world_indexed_packet(oracle,&packet,&world,&native_indexed_scene,active_lookup,(int32_t)info[9],(int32_t)info[10])!=1)fail("Axis native oracle unsupported.");
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    uint16_t background=taxi_palette_fade_cap_rgb555(native_scene_palette[111],(int16_t)city_fade_cap);
    glClearColor((background&31)/31.0f,((background>>5)&31)/31.0f,((background>>10)&31)/31.0f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    TaxiWorldFrame frame={.world=&world,.packets=&packet,.count=1};QueueDraw context={.frame=&frame,.scale=1,.depth_valid=family==4,.far_depth=info[9],.shade_depth=info[10]};
    native_shade_lookup_check=family==4?lookup:NULL;
    native_axis_snapped_check=1;unsigned before=native_axis_draws;
    if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0)||native_axis_draws!=before+1)fail("Axis fixture missed production shader.");
    native_axis_snapped_check=0;native_shade_lookup_check=NULL;glFinish();uint8_t pixels[38400*4];glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    if(family>=0&&family<5){
        for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
            uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[y*240+x]],(int16_t)city_fade_cap);
            for(unsigned c=0;c<3;c++){
                unsigned expected=((color>>(5*c))&31)*255/31;
                unsigned actual=pixels[((159-y)*240+x)*4+c];
                if(abs((int)actual-(int)expected)>2){fprintf(stderr,"Captured %s pixel %u,%u channel%u got%u expected%u\n",name,x,y,c,actual,expected);fail("Captured axis complete GPU coverage mismatch.");}
            }
        }
        printf("PASS captured %s GPU complete coverage:38400 source-screen pixels against native indexed oracle; real captured packet, source edge producer/full frame unverified.\n",name);
    }

    unsigned checked=0;
    for(int y=1;y<159;y++)for(int x=1;x<239;x++) {
        int sign=0,inside=1;
        for(unsigned i=0;i<packet.polygon.count;i++) {
            const TaxiWorldClipVertex *a=packet.polygon.vertices+i,*b=packet.polygon.vertices+(i+1)%packet.polygon.count;
            double dx=(double)b->screen_x-a->screen_x,dy=(double)b->screen_y-a->screen_y;
            double cross=dx*(y+0.5-a->screen_y)-dy*(x+0.5-a->screen_x);
            if(cross*cross<4*(dx*dx+dy*dy)||(!dx&&!dy)){inside=0;break;}
            int edge=cross>0?1:-1;if(sign&&edge!=sign){inside=0;break;}sign=edge;
        }
        if(!inside)continue;
        uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[y*240+x]],(int16_t)city_fade_cap);
        for(unsigned c=0;c<3;c++)if(abs((int)pixels[((159-y)*240+x)*4+c]-(int)(((color>>(c*5))&31)*255/31))>2) {
            fprintf(stderr,"Axis %s mismatch (%d,%d) oracle index%u channel%u got%u expected%u\n",name,x,y,oracle[y*240+x],c,pixels[((159-y)*240+x)*4+c],((color>>(c*5))&31)*255/31);
            fail("Axis source sampling GPU pixel mismatch.");
        }
        checked++;
    }
    if(checked<8)fail("Axis fixture has too few interior pixels.");
    if(family==2){
        TaxiIndexedTexture source;if(taxi_indexed_asset_texture(&native_indexed_scene,packet.texture,&source))fail("Clipped boundary texture unavailable.");
        size_t texel=0;while(texel<source.size&&(!source.pixels[texel]||native_scene_palette[source.pixels[texel]]==native_scene_palette[111]))texel++;
        if(texel==source.size)fail("Clipped boundary texture has no distinguishable opaque texel.");
        const int edges[4][2]={{-8,0},{-1,1},{239,247},{240,248}};unsigned boundary_pixels=0;
        for(unsigned test=0;test<4;test++){
            packet.polygon.count=4;int xs[4]={edges[test][0],edges[test][1],edges[test][1],edges[test][0]},ys[4]={4,4,7,7};
            for(unsigned v=0;v<4;v++){TaxiWorldClipVertex *point=packet.polygon.vertices+v;point->screen_x=xs[v];point->screen_y=ys[v];point->u=(int32_t)((texel%source.width)<<14);point->v=(int32_t)((texel/source.width)<<14);point->z=4096;}
            memset(oracle,111,sizeof oracle);TaxiPairColumnWork work;
            if(taxi_world_indexed_packet_work(oracle,&packet,&world,&native_indexed_scene,native_scene_lookup,0,0,&work)!=1)fail("Clipped boundary oracle rejected fixture.");
            if((test==3&&work.texture_byte_reads)||(test!=3&&work.texture_byte_reads!=3))fail("Clipped boundary source coverage unexpected.");
            glClear(GL_COLOR_BUFFER_BIT);native_axis_snapped_check=1;
            if(draw_queue_packet(&context,TAXI_RENDER_WORLD,0))fail("Clipped boundary production draw failed.");
            native_axis_snapped_check=0;glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
            for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
                uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[y*240+x]],(int16_t)city_fade_cap);
                for(unsigned channel=0;channel<3;channel++)if(abs((int)pixels[((159-y)*240+x)*4+channel]-(int)(((color>>(channel*5))&31)*255/31))>2)fail("Clipped boundary GPU pixels differ from source-resolution oracle.");
                boundary_pixels++;
            }
        }
        if(glGetError()!=GL_NO_ERROR)fail("Clipped boundary GPU readback failed.");
        printf("PASS clipped GPU boundary coverage: %u complete source-screen pixels across touch-left, odd-left, right-clip and outside cases; actual production shader/quad, controlled native fixtures not original full-frame proof.\n",boundary_pixels);
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS axis %s GPU source sampling: texture%u, %u interior pixels; snapped diagnostic, continuous4K interpolation separate.\n",name,packet.texture,checked);
}
static void check_special_source_packet(int width,int height,unsigned family) {
    const char *names[3]={"clipped","unclipped","shaded"};char path[MAX_PATH];
    GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');if(!tail)fail("Special fixture path failed.");
    snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../reports/city%u-special-%s-packet.bin",scene_index+1,names[family]);
    FILE *file=fopen(path,"rb");uint32_t info[16];uint8_t material[28];
    if(!file||fread(info,sizeof info,1,file)!=1||fread(material,28,1,file)!=1||info[0]!=0x31505342||
       info[1]!=(unsigned)scene_index+1||info[3]>=world.nm||info[4]>=native_indexed_scene.count||info[15]!=TAXI_RENDER_SPECIAL||
       memcmp(material,world.materials+info[3]*28,28))fail("Special source fixture metadata mismatch.");
    if(fgetc(file)!=EOF)fail("Special source fixture trailing data.");fclose(file);
    TaxiWorldFramePacket packet={.material=info[3],.texture=info[4],.raster_flags=(uint16_t)info[5],
        .raster_flags_valid=1,.source_index=info[6],.kind=TAXI_RENDER_SPECIAL};
    memcpy(&packet.special.screen.left,info+10,4);memcpy(&packet.special.screen.top,info+11,4);
    memcpy(&packet.special.screen.right,info+12,4);memcpy(&packet.special.screen.bottom,info+13,4);
    memcpy(&packet.special.depth,info+14,4);
    if(taxi_world_indexed_callback(&packet,&world)!=info[7])fail("Special source callback mismatch.");
    uint8_t oracle[38400],lookup[2560];memset(oracle,111,sizeof oracle);
    for(unsigned i=0;i<2560;i++)lookup[i]=(uint8_t)(i%256+i/256*19);
    const uint8_t *active_lookup=family==2?lookup:native_scene_lookup;
    if(taxi_world_indexed_packet(oracle,&packet,&world,&native_indexed_scene,active_lookup,(int32_t)info[8],(int32_t)info[9])!=1)fail("Special source native oracle failed.");
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    uint16_t background=taxi_palette_fade_cap_rgb555(native_scene_palette[111],(int16_t)city_fade_cap);
    glClearColor((background&31)/31.0f,((background>>5)&31)/31.0f,((background>>10)&31)/31.0f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    TaxiWorldFrame frame={.world=&world,.packets=&packet,.count=1};
    QueueDraw context={.frame=&frame,.scale=1,.depth_valid=1,.far_depth=info[8],.shade_depth=info[9]};
    native_axis_snapped_check=1;native_shade_lookup_check=family==2?lookup:NULL;unsigned before=native_axis_draws;
    if(draw_queue_packet(&context,TAXI_RENDER_SPECIAL,0)||native_axis_draws!=before+1)fail("Special source production dispatch failed.");
    native_axis_snapped_check=0;native_shade_lookup_check=NULL;glFinish();uint8_t pixels[38400*4];glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    TaxiTrafficScreenBounds b=packet.special.screen;unsigned checked=0;
    for(int y=0;y<160;y++)for(int x=0;x<240;x++) {
        if(x<=b.left||x>=b.right-1||y<=b.top||y>=b.bottom-1)continue;
        uint16_t color=taxi_palette_fade_cap_rgb555(native_scene_palette[oracle[y*240+x]],(int16_t)city_fade_cap);
        for(unsigned c=0;c<3;c++)if(abs((int)pixels[((159-y)*240+x)*4+c]-(int)(((color>>(c*5))&31)*255/31))>2) {
            fprintf(stderr,"Special %s GPU mismatch x%d y%d index%u\n",names[family],x,y,oracle[y*240+x]);
            fail("Special GPU/native indexed source pixel mismatch.");
        }
        checked++;
    }
    if(checked<4)fail("Special source fixture lacks interior pixel coverage.");
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS captured special %s production GPU: city%u camera%u texture%u %u interior pixels; %s.\n",names[family],info[1],info[2],info[4],checked,family==2?"artificial depth/lookup stress state":"source producer-zero phase");
}
static void check_attribute_material_shader(int width,int height) {
    uint8_t material[28]={0};uint16_t palette[256]={0};
    for(unsigned i=0;i<16;i++) {
        material[i]=(uint8_t)i;
        palette[i]=(uint16_t)((i+1)|((i*7%32)<<5)|((i*13%32)<<10));
    }
    glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_ALPHA_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    if(!taxi_material_shader_begin(material,palette,31))fail("Attribute shader fixture compilation failed.");
    for(unsigned cell=0;cell<12;cell++) {
        double attribute=(int)cell-2+0.25;
        glBegin(GL_QUADS);
        glTexCoord3d(cell*8,0,attribute);glVertex2i(cell*8,0);
        glTexCoord3d(cell*8+8,0,attribute);glVertex2i(cell*8+8,0);
        glTexCoord3d(cell*8+8,8,attribute);glVertex2i(cell*8+8,8);
        glTexCoord3d(cell*8,8,attribute);glVertex2i(cell*8,8);
        glEnd();
    }
    taxi_material_shader_end();glFinish();
    uint8_t pixels[96*8*4];glReadPixels(0,height-8,96,8,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    for(unsigned row=0;row<8;row++)for(unsigned x=0;x<96;x++) {
        int bank=(int)(x/8)-2;if(bank<0)bank=0;if(bank>7)bank=7;
        uint16_t color=palette[bank*2+((x+7-row)&1)];
        for(unsigned channel=0;channel<3;channel++) {
            int expected=(((color>>(channel*5))&31)*255+15)/31;
            if(abs((int)pixels[(row*96+x)*4+channel]-expected)>1)
                fail("Attribute shader GPU pattern/row parity pixel mismatch.");
        }
        if(pixels[(row*96+x)*4+3]!=255)fail("Attribute flat shader is not opaque.");
    }
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    puts("PASS GPU attribute flat patterns: all8 palette pairs, negative/high clamps, row parity and opacity via768 pixel readbacks; diagnostic fixtures.");
}
static void check_live_smoke_composition(void){
    TaxiEffectSprite saved=native_effect_requests[0];int old_cap=city_fade_cap,old_attract=source_attract_ai;
    GLuint old_bg=native_effect_background;int old_w=native_effect_background_width,old_h=native_effect_background_height;
    int old_valid=native_effect_background_valid,old_coverage=native_effect_background_coverage_valid;
    unsigned old_drawn=native_effect_blended;
    uint8_t background[64*64*4],lower[64*64*4],actual[64*64*4];
    uint16_t bg555=(uint16_t)(3|(19<<5)|(27<<10));int16_t matrix[4]={256,0,0,256};unsigned checked=0,witnesses=0;
    glPushAttrib(GL_ALL_ATTRIB_BITS);glViewport(0,0,64,64);glDisable(GL_BLEND);glDisable(GL_ALPHA_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,64,64,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLuint bg;glGenTextures(1,&bg);native_effect_background=bg;native_effect_background_width=native_effect_background_height=64;
    native_effect_background_valid=native_effect_background_coverage_valid=1;city_fade_cap=31;source_attract_ai=0;
    static const int caps[3]={0,7,31};
    for(unsigned cap=0;cap<3;cap++)for(unsigned coverage=0;coverage<2;coverage++)for(unsigned f=0;f<4;f++)for(unsigned doubled=0;doubled<2;doubled++){
        city_fade_cap=caps[cap];uint16_t faded_bg=taxi_palette_fade_cap_rgb555(bg555,(int16_t)caps[cap]);
        for(unsigned i=0;i<4096;i++){
            for(unsigned k=0;k<3;k++){unsigned c=(faded_bg>>(5*k))&31;background[4*i+k]=(uint8_t)(c*255/31);}
            background[4*i+3]=coverage?255:0;
        }
        glBindTexture(GL_TEXTURE_2D,bg);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,background);
        glClearColor(17/255.0f,19/255.0f,23/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
        /* A real taxi half is the lower OBJ. Smoke must use immutable BG,
           while keyed/out-of-field pixels retain that lower OBJ. */
        glBindTexture(GL_TEXTURE_2D,taxi_textures[0]);
        if(!taxi_obj_shader_begin(matrix,0,0,1))fail("Smoke overlap taxi setup failed.");
        glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(64,0);glVertex2i(64,0);
        glTexCoord2i(64,64);glVertex2i(64,64);glTexCoord2i(0,64);glVertex2i(0,64);glEnd();taxi_obj_shader_end();
        glReadPixels(0,0,64,64,GL_RGBA,GL_UNSIGNED_BYTE,lower);
        native_effect_requests[0]=(TaxiEffectSprite){.position={32,32,600},.scale=(int16_t)(doubled?257:256),.frame=(uint8_t)f,.group=8,.bucket=2,.flags=4};
        TaxiEffectFrame frame;TaxiObjField field;int16_t mapping[4];
        if(!taxi_effect_asset_frame(&native_effect_asset,0,8,f,0,&frame)||taxi_obj_field(32,32,frame.width,frame.height,doubled?257:256,0,&field)||taxi_obj_matrix_values(0,256,doubled?257:256,mapping))fail("Smoke oracle setup failed.");
        uint8_t faded_source[8192];taxi_palette_fade_cap_rgba(frame.rgba,faded_source,(size_t)frame.width*frame.height,(int16_t)caps[cap]);
        draw_native_effect(0,0,0,1);glReadPixels(0,0,64,64,GL_RGBA,GL_UNSIGNED_BYTE,actual);
        for(unsigned y=0;y<64;y++)for(unsigned x=0;x<64;x++){
            unsigned index=(63-y)*64+x;uint8_t expected[4];memcpy(expected,lower+4*index,4);
            int sx,sy;int mapped=taxi_obj_affine_point(mapping,frame.width,frame.height,field.double_size,(int)x-field.left,(int)y-field.top,&sx,&sy);
            if(mapped==1){
                const uint8_t *pixel=faded_source+4*((unsigned)sy*frame.width+(unsigned)sx);
                if(pixel[3]){
                    uint16_t color=0;for(unsigned k=0;k<3;k++)color|=(uint16_t)(((pixel[k]*31+127)/255)<<(5*k));
                    uint16_t result=taxi_palette_obj_alpha_rgb555(color,faded_bg,coverage?2:5,0x1f4f,0x0808);
                    for(unsigned k=0;k<3;k++)expected[k]=coverage?(uint8_t)(((result>>(5*k))&31)*255/31):pixel[k];expected[3]=255;
                    if(memcmp(expected,lower+4*index,3))witnesses++;
                }
            }
            if(memcmp(expected,actual+4*index,4))fail("Actual smoke draw overlaps/composes incorrectly.");checked++;
        }
    }
    glDeleteTextures(1,&bg);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    native_effect_requests[0]=saved;city_fade_cap=old_cap;source_attract_ai=old_attract;native_effect_background=old_bg;
    native_effect_background_width=old_w;native_effect_background_height=old_h;native_effect_background_valid=old_valid;native_effect_background_coverage_valid=old_coverage;native_effect_blended=old_drawn;
    if(!witnesses)fail("Smoke overlap fixture lacks color witnesses.");
    printf("PASS actual smoke draw: %u pixels, %u distinct lower-OBJ witnesses, all4 original smoke frames, normal/double fields, BG2/backdrop, palette caps0/7/31 BEFORE blend; diagnostic poses.\n",checked,witnesses);
}
static void check_native_obj_blending(void){
    uint8_t source[256],background[256],actual[256];uint16_t src555[64],bg555[64];
    for(unsigned i=0;i<64;i++){
        src555[i]=(uint16_t)((i&31)|(((i*3)&31)<<5)|(((i*7)&31)<<10));
        bg555[i]=(uint16_t)(((31-i)&31)|(((i*5)&31)<<5)|(((i*11)&31)<<10));
        for(unsigned k=0;k<3;k++){
            unsigned a=(src555[i]>>(k*5))&31,b=(bg555[i]>>(k*5))&31;
            source[4*i+k]=(uint8_t)((a<<3)|(a>>2));background[4*i+k]=(uint8_t)((b<<3)|(b>>2));
        }
        source[4*i+3]=i%7?255:0;background[4*i+3]=i%2?255:0;
    }
    glPushAttrib(GL_ALL_ATTRIB_BITS);glViewport(0,0,8,8);
    glDisable(GL_BLEND);glDisable(GL_ALPHA_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,8,8,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    GLuint textures[2];glGenTextures(2,textures);
    for(unsigned t=0;t<2;t++){
        glBindTexture(GL_TEXTURE_2D,textures[t]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,8,8,0,GL_RGBA,GL_UNSIGNED_BYTE,t?background:source);
    }
    static const unsigned coefficients[4]={0,8,16,31};unsigned checked=0;
    int16_t matrix[4]={256,0,0,256};
    if(taxi_obj_shader_background(textures[1],8,8,0x1f4f,0x0808))fail("Inactive OBJ blending accepted.");
    for(unsigned a=0;a<4;a++)for(unsigned b=0;b<4;b++)for(unsigned targets=0;targets<4;targets++){
        uint16_t cnt=(uint16_t)(((targets&1)?0x400:0)|((targets&2)?0x2000:0));
        uint16_t alpha=(uint16_t)(coefficients[a]|coefficients[b]<<8);
        glClearColor(17/255.0f,19/255.0f,23/255.0f,0);glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D,textures[0]);
        if(!taxi_obj_shader_frame_begin(matrix,0,8,8,8,8,0,0,1)||
           !taxi_obj_shader_background(textures[1],8,8,cnt,alpha))fail("OBJ blending setup failed.");
        if(taxi_obj_shader_background(textures[1],8,8,cnt,alpha))fail("Duplicate OBJ background owner accepted.");
        glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(8,0);glVertex2i(8,0);
        glTexCoord2i(8,8);glVertex2i(8,8);glTexCoord2i(0,8);glVertex2i(0,8);glEnd();taxi_obj_shader_end();
        glReadPixels(0,0,8,8,GL_RGBA,GL_UNSIGNED_BYTE,actual);
        for(unsigned y=0;y<8;y++)for(unsigned x=0;x<8;x++){
            unsigned dst=y*8+x,src=(7-y)*8+x;uint8_t expected[4]={17,19,23,0};
            if(source[4*src+3]){
                unsigned layer=background[4*dst+3]?2:5;
                uint16_t color=taxi_palette_obj_alpha_rgb555(src555[src],bg555[dst],layer,cnt,alpha);
                int eligible=(cnt&(1u<<(8+layer)))!=0;
                for(unsigned k=0;k<3;k++)expected[k]=eligible?(uint8_t)(((color>>(5*k))&31)*255/31):source[4*src+k];
                expected[3]=255;
            }
            if(memcmp(expected,actual+4*dst,4))fail("Native OBJ forced alpha differs from RGB555 oracle.");checked++;
        }
    }
    glDeleteTextures(2,textures);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
    printf("PASS forced semi-OBJ GPU: %u pixels, BG2/backdrop targets, keyed source zero, coefficient0/8/16/31 clamp; native RGB555 oracle.\n",checked);
}
static void check_native_obj_shader(void) {
    const int16_t matrices[4][4]={{255,0,0,255},{0,255,-255,0},{128,0,0,128},{181,181,-181,181}};
    uint8_t art[128*64*4],readback[256*256*4];unsigned checked=0;
    GLint viewport[4],bound;GLfloat clear[4];
    glGetIntegerv(GL_VIEWPORT,viewport);glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
    glGetFloatv(GL_COLOR_CLEAR_VALUE,clear);
    
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,128,128,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glDisable(GL_DEPTH_TEST);
    for(unsigned view=0;view<64;view++){
    glBindTexture(GL_TEXTURE_2D,taxi_textures[view]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,art);
    for(unsigned mode=0;mode<2;mode++){
    int dimension=mode?256:128;glViewport(0,0,dimension,dimension);
    for(unsigned c=0;c<4;c++)for(unsigned half=0;half<2;half++){
        glClearColor(1,0,1,0);glClear(GL_COLOR_BUFFER_BIT);
        uint8_t state_material[28]={0};
        if(!taxi_material_shader_begin(state_material,native_scene_palette,31))fail("OBJ state fixture shader unavailable.");
        GLint prior_program,prior_active,prior_texture;
        glGetIntegerv(0x8b8d,&prior_program);glGetIntegerv(0x84e0,&prior_active);
        glGetIntegerv(GL_TEXTURE_BINDING_2D,&prior_texture);
        if(!prior_program)fail("OBJ program restoration fixture is not active.");
        if(!taxi_obj_shader_begin(matrices[c],1,half,!mode))fail("Native OBJ shader unavailable.");
        glBegin(GL_QUADS);
        glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i(128,0);glVertex2i(128,0);
        glTexCoord2i(128,128);glVertex2i(128,128);glTexCoord2i(0,128);glVertex2i(0,128);
        glEnd();taxi_obj_shader_end();
        GLint restored_program,restored_active,restored_texture;
        glGetIntegerv(0x8b8d,&restored_program);glGetIntegerv(0x84e0,&restored_active);
        glGetIntegerv(GL_TEXTURE_BINDING_2D,&restored_texture);
        if(restored_program!=prior_program||restored_active!=prior_active||restored_texture!=prior_texture)
            fail("OBJ shader changed caller program or texture state.");
        taxi_material_shader_end();glFinish();glReadPixels(0,0,dimension,dimension,GL_RGBA,GL_UNSIGNED_BYTE,readback);
        for(int y=0;y<dimension;y++)for(int x=0;x<dimension;x++){
            int u,v;const uint8_t background[3]={255,0,255};const uint8_t *expected=background;
            int valid;
            if(!mode)valid=taxi_obj_affine_point(matrices[c],64,64,1,x,y,&u,&v);
            else{
                /* Independent exact quarter-pixel arithmetic for 2x native coverage. */
                int dx=2*x-257,dy=2*y-257;
                int nu=matrices[c][0]*dx+matrices[c][1]*dy+32768;
                int nv=matrices[c][2]*dx+matrices[c][3]*dy+32768;
                u=nu>=0?nu/1024:-((-nu+1023)/1024);
                v=nv>=0?nv/1024:-((-nv+1023)/1024);
                valid=u>=0&&u<64&&v>=0&&v<64;
            }
            if(valid){
                const uint8_t *p=art+4*(v*128+u+64*(int)half);if(p[3])expected=p;
            }
            if(memcmp(readback+4*((dimension-1-y)*dimension+x),expected,3))fail("Native OBJ shader texel mismatch.");
            checked++;
        }
    }
    }}
    glEnable(GL_DEPTH_TEST);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);glBindTexture(GL_TEXTURE_2D,(GLuint)bound);
    glClearColor(clear[0],clear[1],clear[2],clear[3]);
    if(glGetError()!=GL_NO_ERROR)fail("OBJ shader GPU error.");
    printf("PASS native OBJ affine shader: %u field pixels, all64 actual taxi views/both halves, four matrices, snapped and continuous2x coordinates; diagnostic only.\n",checked);
}
#include "effect_gpu_check.inc"
static void check_native_taxi_half_overlap(int width,int height) {
    /* The overlap witness needs the source camera size; prototype pullback
       can separate opaque halves. Restore user framing after this fixture. */
    int saved_framing=prototype_camera_framing;prototype_camera_framing=0;
    int saved_effect_count=native_effect_request_count;native_effect_request_count=0; /* This fixture isolates only the two taxi halves. */
    size_t bytes=(size_t)width*height*4;
    uint8_t *images[3];for(unsigned i=0;i<3;i++){images[i]=malloc(bytes);if(!images[i])fail("Taxi overlap allocation failed.");}
    TaxiDriveWorld saved_car=car;int32_t saved_roll=previous_body_roll;
    TaxiSimulationClock saved_clock=drive_clock;unsigned overlap=0,distinct=0;
    GLfloat clear[4];glGetFloatv(GL_COLOR_CLEAR_VALUE,clear);
    for(unsigned pose=0;pose<2;pose++){
        car.world.angle_1a=previous_body_roll=pose?2048:0;drive_clock.phase=0;
        for(unsigned image=0;image<3;image++){
            taxi_diagnostic_half_mask=image==2?3:1u<<image;
            glClearColor(1,0,1,0);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            if(!draw_source_taxi(width,height))fail("Taxi overlap source path unavailable.");
            glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,images[image]);
        }
        /* Source two-entry tie: larger centerX has lower index.
           At0 right wins; at180 left wins. Compare isolated actual GPU halves. */
        unsigned winner=pose?0:1;
        for(size_t p=0;p<bytes;p+=4){
            int visible[2];for(unsigned h=0;h<2;h++)
                visible[h]=images[h][p]!=255||images[h][p+1]!=0||images[h][p+2]!=255;
            unsigned selected=visible[winner]?winner:1-winner;
            if(memcmp(images[2]+p,images[selected]+p,3))fail("Taxi GPU half overlap ordering mismatch.");
            if(visible[0]&&visible[1]){
                overlap++;if(memcmp(images[0]+p,images[1]+p,3))distinct++;
            }
        }
    }
    taxi_diagnostic_half_mask=3;car=saved_car;previous_body_roll=saved_roll;drive_clock=saved_clock;native_effect_request_count=saved_effect_count;prototype_camera_framing=saved_framing;
    glClearColor(clear[0],clear[1],clear[2],clear[3]);
    for(unsigned i=0;i<3;i++)free(images[i]);
    if(!overlap||!distinct)fail("Taxi GPU overlap fixture did not exercise distinct opaque half pixels.");
    if(glGetError()!=GL_NO_ERROR)fail("Taxi overlap GPU readback failed.");
    draw(width,height);
    printf("PASS production taxi GPU half overlap: 0/180-degree poses, %u opaque overlap pixels, %u distinct-color witnesses; continuous geometry, other OBJ priorities unverified.\n",overlap,distinct);
}
static void check_native_taxi_facing(int width,int height) {
    if(!drive_mode||!recovered_camera||capture_orbit_set)fail("Taxi facing check requires source camera driving without orbit.");
    TaxiDriveWorld saved_car=car;int32_t saved_heading=camera_heading,saved_previous_heading=previous_camera_heading;
    int32_t saved_vehicle=previous_vehicle_heading,saved_roll=previous_body_roll;
    int32_t saved_offset=camera_yaw_offset,saved_previous_offset=previous_camera_yaw_offset,saved_target=camera_yaw_target;
    TaxiSimulationClock saved_clock=drive_clock;unsigned checked=0;
    car.world.heading_1c=0;previous_vehicle_heading=0;car.world.angle_1a=0;previous_body_roll=0;drive_clock.phase=0;
    const int32_t headings_to_check[]={0,33*4096,-32*4096,-32*4096+1,4096*4095+2048};
    const int32_t offsets_to_check[]={-64,0,64};
    for(unsigned offset_case=0;offset_case<3;offset_case++){
      camera_yaw_offset=previous_camera_yaw_offset=offsets_to_check[offset_case];
    for(unsigned pose=0;pose<sizeof headings_to_check/sizeof headings_to_check[0];pose++) {
      car.world.heading_1c=previous_vehicle_heading=headings_to_check[pose];
      for(unsigned view=0;view<64;view++) {
        camera_heading=previous_camera_heading=(int32_t)(view*64*4096);
        if(!draw_source_taxi(width,height))fail("Source taxi drawing path was not selected.");
        GLint bound;glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
        /* Original15586: camera=view*64, heading0 -> wrapped difference
           -view*64, then (32-difference)&4095 ASR6 -> view. */
        /* Source15594..559C biases negative heading by4095 before ASR12:
           division truncates toward zero, including negative fractional headings. */
        int32_t heading_word=headings_to_check[pose];
        int32_t biased=heading_word<0?heading_word+4095:heading_word;
        uint32_t shifted=((uint32_t)biased>>12)|(biased<0?0xfff00000u:0);
        int32_t angle;memcpy(&angle,&shifted,4);
        unsigned expected=(32u-(uint32_t)(angle-((int32_t)view*64+offsets_to_check[offset_case])))&4095u;
        expected>>=6;
        if((GLuint)bound!=taxi_textures[expected])fail("Native taxi source view texture selection mismatch.");
        checked++;
      }
    }
    }
    camera_yaw_offset=saved_offset;previous_camera_yaw_offset=saved_previous_offset;camera_yaw_target=saved_target;
    car=saved_car;camera_heading=saved_heading;previous_camera_heading=saved_previous_heading;
    previous_vehicle_heading=saved_vehicle;previous_body_roll=saved_roll;drive_clock=saved_clock;
    if(glGetError()!=GL_NO_ERROR)fail("Taxi facing GPU diagnostic failed.");draw(width,height);
    printf("PASS integrated native taxi facing: %u source camera/heading views selected original GPU textures; diagnostic poses, OBJ affine/priority and physical steering unverified.\n",checked);
}
static void check_pause_scene_rgba(int width,int height){
    if(native_pause.menu.dialog||!native_pause.menu.frozen||!native_pause.alternate)fail("Scene palette restore check requires main pause fixture.");
    size_t compared=0;unsigned cash_checks=0;int saved_cap=city_fade_cap;
    const int caps[]={31,17,0};
    for(unsigned phase=0;phase<3;phase++){
        if(phase){
            if(taxi_pause_runtime_frame(&native_pause,0,0,0,0,240,&native_pause_frame)||taxi_pause_runtime_frame(&native_pause,8,0,0,0,240,&native_pause_frame))fail("Scene palette source transition failed.");
            if(native_pause.alternate!=(phase==2))fail("Scene palette transition state incorrect.");
            draw(width,height);glFinish();
        }
        for(unsigned cap_index=0;cap_index<3;cap_index++){
        city_fade_cap=caps[cap_index];draw(width,height);glFinish();
        for(unsigned i=0;i<texture_count+sprite_source_count;i++){
            Texture *t=i<texture_count?textures+i:sprite_sources+i-texture_count;
            if(i>=texture_count&&t->pause_bank<0)continue;
            int converted=i<texture_count||(t->pause_bank!=0&&t->pause_bank!=2&&t->pause_bank!=11);
            size_t pixels=(size_t)t->width*t->height;
            unsigned char *actual=malloc(pixels*4),*expected=malloc(pixels*4);
            if(!actual||!expected)fail("Scene palette readback allocation failed.");
            if(native_pause.alternate&&converted){
                taxi_palette_gameplay_alternate_rgba(t->pixels,expected,pixels);
                taxi_palette_fade_cap_rgba(expected,expected,pixels,(int16_t)city_fade_cap);
            }else taxi_palette_fade_cap_rgba(t->pixels,expected,pixels,(int16_t)city_fade_cap);
            glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,actual);
            if(memcmp(actual,expected,pixels*4)||glGetError()!=GL_NO_ERROR)fail("Scene palette GPU upload/restore differs from original decoded texture.");
            for(unsigned slot=0;slot<19;slot++)if(t->id==cash_owned_slot_textures[slot])cash_checks++;
            compared+=pixels*4;free(actual);free(expected);
        }
    }
        }
    city_fade_cap=saved_cap;draw(width,height);
    if(cash_checks!=171)fail("Cash palette check did not cover all 19 slots in all nine palette/cap cases.");
    printf("PASS dynamic cash palette: %u slot readbacks across alternate/normal/alternate and caps31/17/0; diagnostic palette settings, not original full-frame proof.\n",cash_checks);
    printf("PASS scene palette GPU restore: %u scene textures plus qualified sprites, %zu RGBA bytes across pause/resume/pause; qualified taxi/traffic/shadow/font/clock/cash/guidance/expiry banks included, immutable source pixels and alpha retained; other sprite banks/full original frame unverified.\n",texture_count,compared);
}

static void check_render_state(int width,int height){
    TaxiDriveWorld saved_car=car;TaxiPassengerScene saved_passengers=passenger_scene;
    TaxiMaterialBlink saved_materials=native_materials;
    uint8_t saved_panel[4096];memcpy(saved_panel,native_panel_pixels,sizeof saved_panel);
    unsigned saved_panel_generation=native_panel_generation;
    TaxiTrafficActor saved_actors[50];TaxiTrafficSubmission saved_submissions[50];
    memcpy(saved_actors,traffic_actors,sizeof saved_actors);memcpy(saved_submissions,traffic_submissions,sizeof saved_submissions);
    TaxiTrafficGlobals saved_globals=traffic_globals;
    uint32_t saved_counter=traffic_counter;uint8_t saved_timer=traffic_timer;
    TaxiSimulationClock saved_clock=drive_clock;
    int32_t saved_heading=camera_heading,saved_pitch=camera_pitch;
    const uint64_t phases[3]={0,UINT64_C(2194500000000),UINT64_C(4389000000000)};
    for(unsigned i=0;i<3;i++){
        /* alpha1 is diagnostic-only; never pass this boundary phase to advance. */
        unsigned axis_before=native_axis_draws;
        drive_clock.phase=phases[i];draw(width,height);glFinish();
        if(i==0)printf("LIVE native4K continuous axis sampling: %u actual world polygon draws; source spans with GPU interpolation adaptation.\n",native_axis_draws-axis_before);
        if(memcmp(&native_materials,&saved_materials,sizeof saved_materials)||
           memcmp(saved_panel,native_panel_pixels,sizeof saved_panel)||native_panel_generation!=saved_panel_generation||
           memcmp(&car,&saved_car,sizeof car)||memcmp(&passenger_scene,&saved_passengers,sizeof passenger_scene)||
           memcmp(traffic_actors,saved_actors,sizeof saved_actors)||memcmp(traffic_submissions,saved_submissions,sizeof saved_submissions)||
           memcmp(&traffic_globals,&saved_globals,sizeof saved_globals)||traffic_counter!=saved_counter||traffic_timer!=saved_timer||
           camera_heading!=saved_heading||camera_pitch!=saved_pitch)fail("Rendering changed authoritative gameplay state.");
        if(glGetError()!=GL_NO_ERROR)fail("Intermediate render failed.");
    }
    drive_clock=saved_clock;draw(width,height);
    puts("RENDER STATE: alpha0/0.5/1 gameplay, customers/RNG, traffic actors/submissions/globals and camera unchanged PASS; presentation diagnostics only.");
}
#include "challenge_flag_gpu_check.inc"
#include "challenge_ball_gpu_check.inc"
/* Same source course/camera with only the Flag queue entry toggled. */
static void capture_live_effect_source_inputs(int ticks){
    FILE *f=fopen(native_effect_boundary_capture_path?native_effect_boundary_capture_path:"outputs/crazy-taxi-native/reports/live-effect-source-inputs.json","wb");
    if(!f)fail("Effect source inputs capture unavailable.");
    TaxiCameraView c=native_effect_capture_camera;
    fprintf(f,"{\"ticks\":%d,\"camera\":[%d,%d,%d,%d,%d,%d],\"focal\":192,\"center\":[120,80],\"phase\":%d,\"disabled\":%d,\"random_state\":%u,\"post_random_state\":%u,\"records\":[",ticks,c.position.x,c.position.y,c.position.z,c.pitch,c.yaw,c.roll,native_effect_capture_phase,native_effect_capture_disabled,native_effect_capture_random_before,native_effect_capture_random_after);
    for(unsigned i=0;i<9;i++){
        fprintf(f,"%s[",i?",":"");for(unsigned j=0;j<72;j++)fprintf(f,"%s%u",j?",":"",native_effect_capture_records[i].bytes[j]);fputc(']',f);
    }
    fputs("],\"requests\":[",f);
    for(int i=0;i<native_effect_request_count;i++){
        TaxiEffectSprite *r=native_effect_requests+i;
        fprintf(f,"%s[%d,%d,%d,%d,%u,%u,%u,%u,%u,%u]",i?",":"",r->position.x,r->position.y,r->position.z,r->scale,r->group,r->frame,r->palette,r->bucket,r->priority,r->flags);
    }
    fputs("]}",f);if(fclose(f))fail("Effect source capture write failed.");
}
static void check_live_effect_pixels(int width,int height){
    size_t bytes=(size_t)width*height*4;
    uint8_t *with=malloc(bytes),*without=malloc(bytes),*repeat=malloc(bytes);
    if(!with||!without||!repeat)fail("Live effect pixel allocation failed.");
    TaxiDriveWorld saved_car=car;TaxiPassengerScene saved_passengers=passenger_scene;
    TaxiActorEffects saved_effects=native_actor_effects;
    int count=native_effect_request_count;
    for(int i=0;i<count;i++){
        TaxiEffectSprite *r=native_effect_requests+i;TaxiEffectFrame f;
        if(!taxi_effect_asset_frame(&native_effect_asset,source_attract_ai!=0,r->group,r->frame,r->palette,&f))fail("Live effect observation asset missing.");
        printf("LIVE EFFECT FIELD: request%d position%d,%d depth%d scale%d group%u frame%u texture%ux%u flags%u.\n",i,r->position.x,r->position.y,r->position.z,r->scale,r->group,r->frame,f.width,f.height,r->flags);
    }
    draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,with);
    diagnostic_hide_effect_pixels=1;
    draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,without);
    diagnostic_hide_effect_pixels=0;
    draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,repeat);
    if(memcmp(with,repeat,bytes))fail("Live effect frame changed across repeated render.");
    if(memcmp(&saved_car,&car,sizeof car)||memcmp(&saved_passengers,&passenger_scene,sizeof passenger_scene)||memcmp(&saved_effects,&native_actor_effects,sizeof native_actor_effects))
        fail("Live effect pixel comparison changed simulation state.");
    unsigned changed=0;int min_x=width,min_y=height,max_x=-1,max_y=-1;
    for(size_t i=0;i<bytes;i+=4)if(memcmp(with+i,without+i,3)){
        int x=(int)((i/4)%(unsigned)width),y=height-1-(int)((i/4)/(unsigned)width);
        int inside=0;double pixel=(double)height/160;
        for(int r=0;r<count&&!inside;r++){
            TaxiEffectSprite *request=native_effect_requests+r;TaxiEffectFrame frame;TaxiObjField field;
            if(!taxi_effect_asset_frame(&native_effect_asset,source_attract_ai!=0,request->group,request->frame,request->palette,&frame)||taxi_obj_field(request->position.x,request->position.y,frame.width,frame.height,request->scale,0,&field))fail("Effect contribution bounds unavailable.");
            double left=width*0.5+(field.left-120)*pixel,top=field.top*pixel;
            inside=x+0.5>=left-1&&x+0.5<=left+field.width*pixel+1&&y+0.5>=top-1&&y+0.5<=top+field.height*pixel+1;
        }
        if(!inside)fail("Live effect changed pixels outside its projected field union.");
        changed++;if(x<min_x)min_x=x;if(x>max_x)max_x=x;if(y<min_y)min_y=y;if(y>max_y)max_y=y;
    }
    free(with);free(without);free(repeat);
    if(!changed||glGetError()!=GL_NO_ERROR)fail("Live effects had no verified visible pixel contribution.");
    printf("PASS live effect pixel contribution: %u changed pixels, bounds %d,%d..%d,%d; effect draws toggled only with full OBJ sorting retained, repeated frame identical, gameplay/RNG/pool unchanged; not original-game pixel oracle.\n",changed,min_x,min_y,max_x,max_y);
}
static void check_flag_scene_pixels(int width,int height){
 size_t size=(size_t)width*height*4;
 uint8_t *without=malloc(size),*with=malloc(size);if(!without||!with)fail("Flag scene pixel allocation failed.");
 flag_scene_mask=1;draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,without);
 flag_scene_mask=0;draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,with);
 int32_t min_x=INT32_MAX,min_y=INT32_MAX,max_x=INT32_MIN,max_y=INT32_MIN;
 for(unsigned j=0;j<4;j++){TaxiPoint point=queued_flag.screen_corners[j];if(point.x<min_x)min_x=point.x;if(point.x>max_x)max_x=point.x;if(point.y<min_y)min_y=point.y;if(point.y>max_y)max_y=point.y;}
 unsigned changed=0;
 for(size_t i=0;i<size;i+=4){
  if(!memcmp(without+i,with+i,3))continue;
  double scale=(double)height/160,offset=(double)width/2-120*scale;
  double x=((double)((i/4)%(unsigned)width)-offset)/scale,y=(double)(height-1-(int)((i/4)/(unsigned)width))/scale;
  if(x<min_x-1||x>max_x+1||y<min_y-1||y>max_y+1)fail("Flag scene changed pixels outside projected marker bounds.");
  int matched=0;
  for(unsigned j=0;j<2048&&!matched;j++)if(live_flag_indices[j]){
   uint16_t color=native_scene_palette[live_flag_indices[j]];matched=1;
   for(unsigned c=0;c<3;c++)if(abs((int)with[i+c]-(int)(((color>>(5*c))&31)*255/31))>2)matched=0;
  }
  if(!matched)fail("Flag scene changed a pixel outside original bitmap palette.");
  changed++;
 }
 free(without);free(with);if(!changed)fail("Flag scene queue has no visible marker contribution.");
 printf("PASS Flag scene visible contribution: %u changed original-palette pixels in %dx%d continuous render, only queue marker toggled; controlled camera, not ROM oracle.\n",changed,width,height);
}

static void check_earned_credits_quit(int width,int height,const char *capture_path){
    if(startup_stage!=3||!passenger_scene.session.fare.qualified_1691||(audio_live&&!native_earned_audio_check)||
       passenger_scene.session.trip.active_1694)fail("Earned Quit check requires qualified completed native fares and muted diagnostic.");
    TaxiFareState earned=passenger_scene.session.fare;
    unsigned customers=passenger_scene.session.end.completed_1692;
    startup_quit_frozen_car=car;startup_quit_frozen_clock=passenger_scene.session.clock.remaining_16a8;
    pause_host_frames=1;startup_audio_cleanup_smoke=4;startup_quit_input_tick=1;
    static const uint16_t keys[]={8,0,128,0,128,0,1,0,128,0,1};
    for(unsigned i=0;i<sizeof keys/sizeof keys[0];i++)startup_tick(keys[i]);
    for(unsigned i=0;i<100&&!startup_quit_complete;i++)startup_tick(0);
    if(!startup_quit_complete||native_pause.menu.outer_state!=2||!native_pause.menu.exit_flag||
       !native_pause.menu.frozen||city_fade.active_177b||startup_quit_fade_ticks!=33)
        fail("Earned native Quit Yes/fade failed.");
    startup_quit_parent=(TaxiNormalExit){.outer=passenger_scene.session.outer_state_1ea8,
        .fade=city_fade.active_177b,.frozen=native_pause.menu.frozen,.ended=passenger_scene.session.ended_1697,
        .mode=(uint8_t)((native_settings[0]&128)!=0),.qualified=passenger_scene.session.fare.qualified_1691,
        .exit_flag=native_pause.menu.exit_flag};
    unsigned earned_cleanup_stops=0,earned_active_owned=0;
    for(unsigned i=0;i<10;i++)if(live_sound_state.handles[i]){
        if(live_sound_state.handles[i]>4)fail("Earned exit audio owner invalid.");
        if(audio_channels[live_sound_state.handles[i]-1].sound)earned_active_owned++;
    }
    TaxiNormalExitRequests requests;
    if(taxi_normal_exit_begin(&startup_quit_parent,&requests)!=1||requests.count!=4||
       requests.calls[0]!=0x08010aac||requests.calls[1]!=0x08027754||requests.calls[2]!=0x0802771c||requests.calls[3]!=0x08027730)
        fail("Earned Quit cleanup requests invalid.");
    for(unsigned i=0;i<requests.count;i++){
        switch(requests.calls[i]){
        case 0x08010aac:earned_cleanup_stops=taxi_game_audio_cleanup_dispatch(live_sound_state.handles,live_game_sound_dispatch,NULL);break;
        case 0x08027754:taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_RESUME);break;
        case 0x0802771c:taxi_audio_settings_restore_effects(&audio_settings_state);break;
        case 0x08027730:taxi_music_playback(&music_tracker.state.playing,TAXI_MUSIC_STOP);break;
        default:fail("Unknown earned Quit cleanup request.");
        }
    }
    if(native_earned_audio_check){
        int16_t effects[352]={0};
        for(unsigned i=0;i<4;i++)if(taxi_audio_channel_mix(audio_channels+i,effects,352,audio_bank.codebook,608,
            taxi_audio_effective_volume(audio_global_volume,audio_channels[i].volume))<0)fail("Earned exit effects mix failed.");
        for(unsigned i=0;i<352;i++)if(effects[i])fail("Earned exit effects continued after cleanup.");
    }
    if(taxi_normal_exit_prepare(&startup_quit_parent,0,&requests)!=2||requests.count!=1||requests.calls[0]!=0x080243a4)
        fail("Actual earned qualification did not request credits.");
    passenger_scene.session.outer_state_1ea8=startup_quit_parent.outer;
    passenger_scene.session.ended_1697=startup_quit_parent.ended;
    native_pause.menu.outer_state=startup_quit_parent.outer;music_context.main_mode=startup_quit_parent.main_mode;
    credits_live_begin(1);int returned=0;
    for(unsigned tick=0;tick<3000&&!returned;tick++){
        startup_tick(0);
        if(tick%16==0||credits_live_runtime.complete){draw(width,height);glFinish();}
        if(tick==320){
            char capture[MAX_PATH];if(snprintf(capture,sizeof capture,"%s.earned-credits.bmp",capture_path)>=(int)sizeof capture)
                fail("Earned credits capture path too long.");
            draw(width,height);glFinish();snapshot(capture,width,height);
        }
        returned=taxi_normal_exit_resume(&startup_quit_parent,credits_live_runtime.complete,&requests);
        if(returned<0||requests.count||(!credits_live_runtime.complete&&(!startup_quit_parent.frozen||startup_quit_parent.phase!=3)))
            fail("Credits parent resumed before actual return.");
    }
    if(returned!=1||!credits_live_runtime.complete||credits_live_runtime.flow.line!=126||
       startup_quit_parent.callback!=0x080174dd||startup_quit_parent.frozen||startup_quit_parent.results||
       memcmp(&earned,&passenger_scene.session.fare,sizeof earned)||customers!=passenger_scene.session.end.completed_1692)
        fail("Earned credits return lost parent or fare ownership.");
    if(native_earned_audio_check&&(!earned_cleanup_stops||audio_request_counts[29]!=1||
       music_frames<=credits_music_frames_before||music_nonzero_samples<=credits_music_nonzero_before||
       audio_rejected||audio_busy))fail("Earned credits original audio mix/cleanup failed.");
    if(native_earned_audio_check)printf("EARNED CREDITS AUDIO: cleanup%u stops/%u active owned, next352 effects zero, musicframes%u nonzero%u sound29%u capturedframes%u rejected%u unavailable%u; native PCM capture, device/listening unverified.\n",
        earned_cleanup_stops,earned_active_owned,music_frames-credits_music_frames_before,music_nonzero_samples-credits_music_nonzero_before,
        audio_request_counts[29],audio_frames,audio_rejected,audio_busy);
    credits_live_active=0;glDeleteTextures(1,&credits_live_texture);credits_live_texture=0;
    native_pause.menu.frozen=startup_quit_parent.frozen;music_context.main_mode=startup_quit_parent.main_mode;
    normal_live_input=native_pause.input;normal_live_flow.fade=city_fade;
    startup_parent_begin(1,startup_quit_parent.redraw);
    for(unsigned i=0;i<80;i++)startup_tick(0);
    if(startup_stage!=9||normal_live_flow.fade.active_177b||!music_context.main_mode||result_live_active||
       name_live_active||profile_live_active||game_over_active||glGetError()!=GL_NO_ERROR)
        fail("Earned credits native menu return failed.");
    printf("PASS EARNED QUIT CREDITS: cash%ld.%02ld customers%u qualified%u, actual Quit Yes/%u fade ticks, credits%u ticks/%u lines, callback174DD/menu9; offscreen diagnostic, device audio/scanout/full fidelity excluded.\n",
        (long)earned.total_whole_1698,(long)earned.total_fraction_169c,customers,earned.qualified_1691,
        startup_quit_fade_ticks,credits_live_ticks,credits_live_runtime.flow.line);
}
int main(int argc, char **argv) {
    if(argc>1&&!strcmp(argv[1],"--startup-preview"))return taxi_startup_screens_viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):1559);
    if(argc>1&&!strcmp(argv[1],"--intro-preview"))return taxi_startup_intro_viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):584);
    const char *image_path=NULL,*live_snapshot_path=NULL,*reference_render_pose_path=NULL;unsigned live_snapshot_update=350;int live_snapshot_done=0;
    const char *scene="city1";
    int detail=0,script_ticks=0,start_fullscreen=0,pacing_frames=0,pickup_preview=0,live_4k_render_check=0,async_4k_render_check=0,visible_4k_check=0,window_surface_smoke=0;unsigned live_4k_completed=0,live_4k_submitted=0;FILE *performance_trace=NULL;const char *performance_trace_path=NULL;LARGE_INTEGER performance_previous={0};unsigned performance_frames=0;
    int expiry_preview=-1,expiry_restore=0,expiry_results=-1,results_cache_check=0;
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i],"--snapshot") && i+1<argc) image_path=argv[++i];
        else if(!strcmp(argv[i],"--performance-trace")&&i+1<argc){if(performance_trace)fail("Duplicate performance trace.");performance_trace_path=argv[++i];performance_trace=fopen(performance_trace_path,"wb");if(!performance_trace)fail("Cannot open performance trace.");fprintf(performance_trace,"frame,update_ms,prepare_ms,submit_ms,gpu_finish_ms,checks_ms,swap_ms,interval_ms,updates,gpu_timestamp_ms,pre_due_ms,pump_ms,pacer_call_ms,deadline_late_ms,blit_cpu_ms,swap_only_ms\n");}
        else if(!strcmp(argv[i],"--native-4k-blit-check"))target_blit_check=1;
        else if(!strcmp(argv[i],"--native-4k-target"))native_4k_target=1;
        else if(!strcmp(argv[i],"--precise-presentation-wait"))precise_presentation_wait=1;
        else if(!strcmp(argv[i],"--native-4k-window-smoke")){window_surface_smoke=native_4k_target=native_elapsed_time=drive_mode=audio_preview=audio_output_disabled=1;audio_smoke_frames=360;}
        else if(!strcmp(argv[i],"--visible-4k-render-check"))visible_4k_check=1;
        else if(!strcmp(argv[i],"--async-4k-render-check"))async_4k_render_check=live_4k_render_check=1;
        else if(!strcmp(argv[i],"--live-4k-render-check"))live_4k_render_check=1;
        else if(!strcmp(argv[i],"--reference-render-pose")&&i+1<argc)reference_render_pose_path=argv[++i];
        else if(!strcmp(argv[i],"--live-snapshot")&&i+1<argc)live_snapshot_path=argv[++i];
        else if(!strcmp(argv[i],"--live-snapshot-update")&&i+1<argc){char *end;unsigned long n=strtoul(argv[++i],&end,10);if(*end||n<1||n>100000)fail("Live capture update must be1..100000.");live_snapshot_update=(unsigned)n;}
        else if(!strcmp(argv[i],"--source-column-work-report")&&i+1<argc){source_column_work_report_path=argv[++i];traffic_preview=1;}
        else if(!strcmp(argv[i],"--pause-scene-rgba-check")){pause_overlay_check=1;pause_scene_rgba_check=1;}
        else if(!strcmp(argv[i],"--material-phase-trace")&&i+1<argc){native_material_phase_trace=fopen(argv[++i],"wb");if(!native_material_phase_trace)fail("Material phase trace creation failed.");}
        else if(!strcmp(argv[i],"--pause-freeze-check"))pause_freeze_check=1;
        else if(!strcmp(argv[i],"--pause-host-frame-check"))pause_host_frame_check=1;
        else if(!strcmp(argv[i],"--pause-overlay-check"))pause_overlay_check=1;
        else if(!strcmp(argv[i],"--pause-dialog")&&i+1<argc){pause_overlay_dialog=(unsigned)atoi(argv[++i]);if(pause_overlay_dialog>2)fail("Pause dialog fixture must be0..2.");}
        else if(!strcmp(argv[i],"--traffic-preview"))traffic_preview=1;
        else if(!strcmp(argv[i],"--queue-occlusion-check"))queue_occlusion_check=1;
        else if(!strcmp(argv[i],"--render-state-check"))render_state_check=1;
        else if(!strcmp(argv[i],"--tip-bitmap-gpu-check"))native_tip_bitmap_gpu_check=1;
        else if(!strcmp(argv[i],"--challenge-flag-gpu-check"))flag_gpu_check=1;
        else if(!strcmp(argv[i],"--challenge-ball-gpu-check"))ball_gpu_check=1;
        else if(!strcmp(argv[i],"--axis-source-check"))native_axis_source_check=1;
        else if(!strcmp(argv[i],"--taxi-facing-check"))native_taxi_facing_check=1;
        else if(!strcmp(argv[i],"--challenge-briefing-preview")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>26)fail("Challenge briefing preview must be0..26.");
            challenge_briefing_preview=(int)value;normal_menu_preview=0;
        }
        else if(!strcmp(argv[i],"--challenge-grid-preview")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>10)fail("Challenge grid preview must be0..10.");
            challenge_grid_preview=(int)value;normal_menu_preview=0;
        }
        else if(!strcmp(argv[i],"--normal-menu-cache-check"))normal_menu_cache_check=1;
        else if(!strcmp(argv[i],"--normal-menu-flow-check"))normal_menu_flow_check=1;
        else if(!strcmp(argv[i],"--title-entry-check"))title_entry_check=1;
        else if(!strcmp(argv[i],"--startup-parent-check")){startup_mode=1;startup_check=1;startup_parent_check=1;}
        else if(!strcmp(argv[i],"--reward-drive-probe")){reward_drive_probe=1;startup_mode=1;startup_check=1;startup_gameplay_check=1;route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-earned-quit-check")||!strcmp(argv[i],"--startup-earned-audio-quit-check")){native_earned_audio_check=!strcmp(argv[i],"--startup-earned-audio-quit-check");startup_mode=startup_check=startup_gameplay_check=startup_parent_check=1;route_probe_active=native_multi_fare_probe=native_earned_quit_check=1;startup_timed_check=3;route_probe_speed_reference=60000;route_probe_speed_steering=1;}
        else if(!strcmp(argv[i],"--startup-multi-fare-probe")){startup_mode=1;startup_check=1;startup_gameplay_check=1;route_probe_active=native_multi_fare_probe=1;}
        else if(!strcmp(argv[i],"--startup-route-probe")){startup_mode=1;startup_check=1;startup_gameplay_check=1;route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-pickup-only-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;route_probe_active=1;native_pickup_only_check=1;}
        else if(!strcmp(argv[i],"--startup-material-reset-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;route_probe_active=1;native_pickup_only_check=1;native_material_reset_check=1;}
        else if(!strcmp(argv[i],"--held-gear-trace")&&i+1<argc){if(native_held_gear_trace)fail("Duplicate held gear trace.");native_held_gear_trace=fopen(argv[++i],"w");if(!native_held_gear_trace)fail("Cannot open held gear trace.");taxi_drive_world_gear_trace(native_held_gear_trace);}
        else if(!strcmp(argv[i],"--tip-collection-trace")&&i+1<argc){
            if(native_tip_collection_trace)fail("Duplicate collection trace.");native_tip_collection_trace=fopen(argv[++i],"w");if(!native_tip_collection_trace)fail("Cannot open collection trace.");taxi_tip_collection_trace(native_tip_collection_trace);
        }
        else if(!strcmp(argv[i],"--reward-update-trace")&&i+1<argc){
            if(native_reward_update_trace)fail("Duplicate reward update trace.");native_reward_update_trace=fopen(argv[++i],"w");if(!native_reward_update_trace)fail("Cannot open reward update trace.");taxi_reward_session_update_trace(native_reward_update_trace);
        }
        else if(!strcmp(argv[i],"--reward-trace")&&i+1<argc){
            if(native_reward_trace)fail("Duplicate reward trace.");
            native_reward_trace=fopen(argv[++i],"w");if(!native_reward_trace)fail("Cannot open reward trace.");taxi_reward_session_trace(native_reward_trace);
        }
        else if(!strcmp(argv[i],"--traffic-impact-all-trace")&&i+1<argc){
            if(native_traffic_impact_trace)fail("Duplicate traffic impact trace.");
            native_traffic_impact_trace=fopen(argv[++i],"w");if(!native_traffic_impact_trace)fail("Cannot open full traffic impact trace.");
            taxi_traffic_impact_trace_all(native_traffic_impact_trace);
        }
        else if(!strcmp(argv[i],"--traffic-impact-trace")&&i+1<argc){
            if(native_traffic_impact_trace)fail("Duplicate traffic impact trace.");
            native_traffic_impact_trace=fopen(argv[++i],"w");if(!native_traffic_impact_trace)fail("Cannot open traffic impact trace.");
            taxi_traffic_impact_trace(native_traffic_impact_trace);
        }
        else if(!strcmp(argv[i],"--fare-settlement-trace")&&i+1<argc){
            if(native_fare_settlement_trace)fail("Duplicate fare settlement trace.");
            native_fare_settlement_trace=fopen(argv[++i],"w");if(!native_fare_settlement_trace)fail("Cannot open fare settlement trace.");
            taxi_fare_settlement_trace(native_fare_settlement_trace);
        }
        else if(!strcmp(argv[i],"--destination-selection-trace")&&i+1<argc){
            if(native_destination_selection_trace)fail("Duplicate destination selection trace.");
            native_destination_selection_trace=fopen(argv[++i],"w");if(!native_destination_selection_trace)fail("Cannot open destination selection trace.");
            taxi_material_selection_trace(native_destination_selection_trace);
        }
        else if(!strcmp(argv[i],"--cash-glyph-trace")&&i+1<argc){
            if(native_cash_glyph_trace)fail("Duplicate cash glyph trace.");
            native_cash_glyph_trace=fopen(argv[++i],"w");if(!native_cash_glyph_trace)fail("Cannot open cash glyph trace.");
            taxi_cash_glyphs_trace(native_cash_glyph_trace);
        }
        else if(!strcmp(argv[i],"--passenger-money-trace")&&i+1<argc){
            if(native_passenger_money_trace)fail("Duplicate passenger money trace.");
            native_passenger_money_trace=fopen(argv[++i],"w");if(!native_passenger_money_trace)fail("Cannot open passenger money trace.");
            taxi_passenger_money_trace(native_passenger_money_trace);
        }
        else if(!strcmp(argv[i],"--passenger-fare-trace")&&i+1<argc){
            if(native_passenger_fare_trace)fail("Duplicate passenger fare trace.");
            native_passenger_fare_trace=fopen(argv[++i],"w");if(!native_passenger_fare_trace)fail("Cannot open passenger fare trace.");
            taxi_passenger_fare_trace(native_passenger_fare_trace);
        }
        else if(!strcmp(argv[i],"--impact-voice-trace")&&i+1<argc){
            if(native_impact_voice_trace)fail("Duplicate impact voice trace.");
            native_impact_voice_trace=fopen(argv[++i],"w");if(!native_impact_voice_trace)fail("Cannot open impact voice trace.");
        }
        else if(!strcmp(argv[i],"--passenger-pickup-trace")&&i+1<argc){
            if(native_passenger_pickup_trace)fail("Duplicate passenger pickup trace.");
            native_passenger_pickup_trace=fopen(argv[++i],"w");if(!native_passenger_pickup_trace)fail("Cannot open passenger pickup trace.");
            taxi_passenger_pickup_trace(native_passenger_pickup_trace);
        }
        else if(!strcmp(argv[i],"--passenger-idle-trace")&&i+1<argc){
            if(native_passenger_idle_trace)fail("Duplicate passenger idle trace.");
            native_passenger_idle_trace=fopen(argv[++i],"w");if(!native_passenger_idle_trace)fail("Cannot open passenger idle trace.");
            taxi_passenger_idle_trace(native_passenger_idle_trace);
        }
        else if(!strcmp(argv[i],"--random-trace")&&i+1<argc){
            if(native_random_trace)fail("Duplicate random trace.");
            native_random_trace=fopen(argv[++i],"w");if(!native_random_trace)fail("Cannot open random trace.");
            taxi_random_trace(native_random_trace);
        }
        else if(!strcmp(argv[i],"--traffic-frame-trace")&&i+1<argc){
            if(native_traffic_frame_trace)fail("Duplicate traffic frame trace.");
            native_traffic_frame_trace=fopen(argv[++i],"w");if(!native_traffic_frame_trace)fail("Cannot open traffic frame trace.");
            taxi_traffic_frame_trace(native_traffic_frame_trace);
        }
        else if(!strcmp(argv[i],"--traffic-heading-trace")&&i+1<argc){
            if(native_traffic_heading_trace)fail("Duplicate traffic heading trace.");
            native_traffic_heading_trace=fopen(argv[++i],"w");if(!native_traffic_heading_trace)fail("Cannot open traffic heading trace.");
            taxi_traffic_heading_trace(native_traffic_heading_trace);
        }
        else if(!strcmp(argv[i],"--traffic-follow-sampled-trace")&&i+1<argc){
            if(native_traffic_actor_trace)fail("Duplicate traffic actor trace.");
            native_traffic_actor_trace=fopen(argv[++i],"w");if(!native_traffic_actor_trace)fail("Cannot open sampled traffic trace.");
            taxi_traffic_actor_trace_sampled(native_traffic_actor_trace);
        }
        else if(!strcmp(argv[i],"--traffic-spawn-trace")&&i+1<argc){
            if(native_traffic_spawn_trace)fail("Duplicate traffic spawn trace.");
            native_traffic_spawn_trace=fopen(argv[++i],"w");if(!native_traffic_spawn_trace)fail("Cannot open traffic spawn trace.");
            taxi_traffic_spawn_trace(native_traffic_spawn_trace);
        }
        else if(!strcmp(argv[i],"--traffic-actor-trace")&&i+1<argc){
            if(native_traffic_actor_trace)fail("Duplicate traffic actor trace.");
            native_traffic_actor_trace=fopen(argv[++i],"w");if(!native_traffic_actor_trace)fail("Cannot open traffic actor trace.");
            taxi_traffic_actor_trace(native_traffic_actor_trace);
        }
        else if(!strcmp(argv[i],"--traffic-response-trace")&&i+1<argc){
            if(native_traffic_response_trace)fail("Duplicate traffic response trace.");
            native_traffic_response_trace=fopen(argv[++i],"w");if(!native_traffic_response_trace)fail("Cannot open traffic response trace.");
            taxi_traffic_player_response_trace(native_traffic_response_trace);
        }
        else if(!strcmp(argv[i],"--traffic-effect-trace")&&i+1<argc){
            if(native_traffic_effect_trace)fail("Duplicate traffic effect trace.");
            native_traffic_effect_trace=fopen(argv[++i],"w");if(!native_traffic_effect_trace)fail("Cannot open traffic effect trace.");
            taxi_traffic_effects_trace(native_traffic_effect_trace);
        }
        else if(!strcmp(argv[i],"--traffic-contact-trace")&&i+1<argc){
            if(native_traffic_contact_trace)fail("Duplicate traffic contact trace.");
            native_traffic_contact_trace=fopen(argv[++i],"w");if(!native_traffic_contact_trace)fail("Cannot open traffic contact trace.");
            taxi_traffic_collision_trace(native_traffic_contact_trace);
        }
        else if(!strcmp(argv[i],"--response-state-trace")&&i+1<argc){if(native_response_state_trace)fail("Duplicate response trace.");native_response_state_trace=fopen(argv[++i],"w");if(!native_response_state_trace)fail("Response trace unavailable.");taxi_vehicle_response_observer(observe_response);}
        else if(!strcmp(argv[i],"--traction-state-trace")&&i+1<argc){
            if(native_traction_state_trace)fail("Duplicate traction trace.");native_traction_state_trace=fopen(argv[++i],"w");if(!native_traction_state_trace)fail("Traction trace unavailable.");taxi_traction_observer(observe_traction);
        }
        else if(!strcmp(argv[i],"--motion-state-trace")&&i+1<argc){
            if(native_motion_state_trace)fail("Duplicate motion trace.");native_motion_state_trace=fopen(argv[++i],"w");if(!native_motion_state_trace)fail("Motion trace unavailable.");taxi_motion_observer(observe_motion);
        }
        else if(!strcmp(argv[i],"--orientation-state-trace")&&i+1<argc){
            if(native_orientation_state_trace)fail("Duplicate orientation trace.");
            native_orientation_state_trace=fopen(argv[++i],"w");if(!native_orientation_state_trace)fail("Orientation trace unavailable.");taxi_orientation_observer(observe_orientation);
        }
        else if(!strcmp(argv[i],"--camera-time-owner-trace"))camera_time_owner_trace=1;
        else if(!strcmp(argv[i],"--camera-state-trace")&&i+1<argc){
            if(native_camera_state_trace)fail("Duplicate camera state trace.");
            native_camera_state_trace=fopen(argv[++i],"w");if(!native_camera_state_trace)fail("Cannot open camera state trace.");
        }
        else if(!strcmp(argv[i],"--effect-state-trace")&&i+1<argc){
            if(native_effect_state_trace)fail("Duplicate effect state trace.");
            native_effect_state_trace=fopen(argv[++i],"w");if(!native_effect_state_trace)fail("Cannot open effect state trace.");
        }
        else if(!strcmp(argv[i],"--route-turn-brake"))route_probe_turn_brake=1;
        else if(!strcmp(argv[i],"--route-segment-distance")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<256||value>8192)fail("Route segment distance must be256..8192.");route_probe_segment_distance=(int)value;}
        else if(!strcmp(argv[i],"--route-avoid-distance")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<256||value>2048)fail("Route avoidance distance must be256..2048.");route_probe_avoid_distance=(int)value;}
        else if(!strcmp(argv[i],"--route-avoid-waypoint")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1||value>127)fail("Route avoidance waypoint must be1..127.");route_probe_avoid_waypoint=(int)value;}
        else if(!strcmp(argv[i],"--route-avoid-end-margin")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1024||value>32768)fail("Route avoidance end margin must be1024..32768.");route_probe_avoid_end_margin=(int)value;}
        else if(!strcmp(argv[i],"--route-bend-speed")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<40000||value>180000)fail("Route bend speed must be40000..180000.");route_probe_bend_speed=(int)value;}
        else if(!strcmp(argv[i],"--route-bend-distance")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1024||value>32768)fail("Route bend distance must be1024..32768.");route_probe_bend_distance=(int)value;}
        else if(!strcmp(argv[i],"--route-final-bend-speed")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<40000||value>180000)fail("Final bend speed must be40000..180000.");route_probe_final_bend_speed=(int)value;}
        else if(!strcmp(argv[i],"--route-avoid-late-straights"))route_probe_avoid_late_straights=1;
        else if(!strcmp(argv[i],"--route-avoid-extra-waypoint")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1||value>127)fail("Extra avoidance waypoint must be1..127.");route_probe_avoid_extra_waypoint=(int)value;}
        else if(!strcmp(argv[i],"--route-speed-steering"))route_probe_speed_steering=1;
        else if(!strcmp(argv[i],"--route-steering-waypoint")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1||value>127)fail("Route steering waypoint must be1..127.");route_probe_steering_waypoint=(int)value;}
        else if(!strcmp(argv[i],"--route-speed-reference")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<60000||value>240000)fail("Route input speed reference must be60000..240000.");route_probe_speed_reference=(int)value;route_probe_speed_steering=1;}
        else if(!strcmp(argv[i],"--startup-timed-check")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<1||value>3)fail("Timed fixture must be1..3.");
            startup_timed_check=(unsigned)value;startup_mode=1;startup_check=1;startup_parent_check=1;startup_gameplay_check=1;
        }
        else if(!strcmp(argv[i],"--normal-menu-slide-q12")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value < -240*4096||value > 240*4096)fail("Normal menu slide exceeds original viewport.");normal_menu_slide_q12=(int32_t)value;
        }
        else if(!strcmp(argv[i],"--normal-menu-preview")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<0||value>8)fail("Normal menu preview must be0..8.");normal_menu_preview=(int)value;
        }
        else if(!strcmp(argv[i],"--normal-menu-cap")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value<0||value>31)fail("Normal menu cap must be0..31.");normal_menu_cap=(int)value;
        }
        else if(!strcmp(argv[i],"--startup-loading-pacing-check"))startup_loading_pacing_check=1;
        else if(!strcmp(argv[i],"--startup-loading-close-check"))startup_loading_close_check=1;
        else if(!strcmp(argv[i],"--startup-reload-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_reload_check=1;taxi_cache_check=1;}
        else if (!strcmp(argv[i],"--scene") && i+1<argc) scene=argv[++i];
        else if (!strcmp(argv[i],"--detail")) detail=1;
        else if(!strcmp(argv[i],"--challenge-bound-menu-entry-check")){challenge_bound_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--challenge-turn-menu-entry-check")){challenge_turn_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--challenge-drift-menu-entry-check")){challenge_drift_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--challenge-bound-failure-check")){bound_failure_check=1;challenge_live_submode=5;challenge_bound_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;challenge_jump_retry_check=2;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-bound-retry-check")){bound_retry_check=1;challenge_live_submode=5;challenge_bound_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;challenge_jump_retry_check=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-turn-failure-check")){turn_failure_check=1;challenge_live_submode=4;challenge_turn_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;challenge_jump_retry_check=2;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-turn-retry-check")){turn_retry_check=1;challenge_live_submode=4;challenge_turn_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;challenge_jump_retry_check=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-drift-failure-check")){drift_failure_check=1;challenge_drift_drive_preview=challenge_jump_drive_preview=1;challenge_jump_retry_check=2;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-drift-retry-check")){drift_retry_check=1;challenge_drift_drive_preview=challenge_jump_drive_preview=1;challenge_jump_retry_check=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-ball-menu-entry-check")){challenge_ball_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--challenge-ball-retry-check")){ball_retry_check=1;challenge_ball_drive_preview=challenge_jump_drive_preview=1;challenge_jump_retry_check=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-ball-failure-check")){ball_failure_check=1;challenge_ball_drive_preview=challenge_jump_drive_preview=1;challenge_jump_retry_check=2;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--turn-earned-capture-check")){turn_earned_capture_check=1;challenge_jump_retry_check=3;}
        else if(!strcmp(argv[i],"--drift-earned-capture-check")){drift_earned_capture_check=1;challenge_jump_retry_check=3;}
        else if(!strcmp(argv[i],"--ball-earned-capture-check")){ball_earned_capture_check=1;challenge_jump_retry_check=3;}
        else if(!strcmp(argv[i],"--challenge-ball-contact-flow-check")){ball_contact_flow_check=1;challenge_ball_drive_preview=challenge_jump_drive_preview=1;challenge_jump_retry_check=3;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-flag-menu-entry-check")){challenge_flag_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--challenge-jump-menu-entry-check")){challenge_jump_menu_entry_check=1;startup_mode=startup_check=startup_title_check=1;}
        else if(!strcmp(argv[i],"--camera-distinct-headings"))camera_distinct_headings=1;
        else if(!strcmp(argv[i],"--taxi-owned-graphics"))taxi_owned_graphics=1;
        else if(!strcmp(argv[i],"--taxi-live-pixels-check")){taxi_live_pixels_check=1;taxi_owned_graphics=1;}
        else if(!strcmp(argv[i],"--source-frame-cpu"))source_frame_cpu_requested=1;
        else if(!strcmp(argv[i],"--source-scene-gpu-check")){source_scene_gpu_requested=1;source_frame_cpu_requested=1;}
        else if(!strcmp(argv[i],"--owned-sprite-lifecycle-check"))taxi_owned_lifecycle_check=1;
        else if(!strcmp(argv[i],"--camera-owner-trace"))camera_owner_trace=1;
        else if(!strcmp(argv[i],"--camera-xbox-lookback-check"))camera_xbox_lookback_check=1;
        else if(!strcmp(argv[i],"--camera-lookback-check"))camera_lookback_check=1;
        else if(!strcmp(argv[i],"--taxi-source-capture")&&i+1<argc)taxi_source_capture_path=argv[++i];
        else if(!strcmp(argv[i],"--prototype-camera-framing"))prototype_camera_framing=1;
        else if(!strcmp(argv[i],"--challenge-analog-trace")&&i+1<argc){char *end;long value=strtol(argv[++i],&end,10);if(end==argv[i]||*end||value < -32768||value>32767)fail("Stick sample must be -32768..32767.");challenge_trace_stick=(int16_t)value;challenge_analog_trace=challenge_camera_trace=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-camera-trace")){challenge_camera_trace=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-entry-fade-check")){challenge_entry_fade_check=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-retry-music-check")){challenge_result_audio_check=8;challenge_jump_retry_check=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-failure-music-check")){challenge_result_audio_check=7;challenge_jump_retry_check=2;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-music-audio-check")){challenge_result_audio_check=6;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-result-audio-check")){challenge_result_audio_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-retry-audio-check")){challenge_result_audio_check=3;challenge_jump_retry_check=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-failure-audio-disabled-check")){challenge_result_audio_check=4;challenge_jump_retry_check=2;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-success-audio-disabled-check")){challenge_result_audio_check=5;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-failure-audio-check")){challenge_result_audio_check=2;challenge_jump_retry_check=2;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-cross-profile-check")){challenge_jump_cross_profile_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-unranked-check")){challenge_jump_unranked_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-new-profile-check")){challenge_jump_new_profile_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-discard-check")){challenge_jump_discard_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--effect-boundary-capture")&&i+1<argc)native_effect_boundary_capture_path=argv[++i];
        else if(!strcmp(argv[i],"--jump-effect-render-check")){jump_effect_render_check=1;challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-success-check")){challenge_jump_retry_check=3;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-return-check")){challenge_jump_retry_check=2;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-jump-retry-check")){challenge_jump_retry_check=1;challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-flag-scene-render-check")){flag_scene_render_check=1;challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--flag-earned-capture-check")){flag_earned_capture_check=1;challenge_jump_retry_check=3;}
        else if(!strcmp(argv[i],"--challenge-flag-contact-flow-check")){flag_contact_flow_check=1;challenge_jump_retry_check=3;challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--bound-input-replay")&&i+1<argc){turn_input_replay_path=argv[++i];challenge_live_submode=5;challenge_bound_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--turn-input-replay")&&i+1<argc){turn_input_replay_path=argv[++i];challenge_live_submode=4;challenge_turn_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--flag-input-replay")&&i+1<argc){flag_input_replay_path=argv[++i];challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-flag-retry-check")){flag_retry_check=1;challenge_jump_retry_check=1;challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-flag-failure-check")){flag_failure_check=1;challenge_jump_retry_check=2;challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--drift-xbox-input-replay")&&i+1<argc){drift_xbox_replay=1;drift_raw_replay_path=argv[++i];challenge_drift_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--jump-raw-input-replay")&&i+1<argc){jump_raw_input_replay=1;drift_raw_replay_path=argv[++i];challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--drift-raw-input-replay")&&i+1<argc){drift_raw_replay_path=argv[++i];challenge_drift_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--ball-input-replay")&&i+1<argc){ball_input_replay_path=argv[++i];challenge_ball_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-bound-drive-preview")){challenge_live_submode=5;challenge_bound_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-turn-drive-preview")){challenge_live_submode=4;challenge_turn_drive_preview=challenge_jump_drive_preview=passenger_diagnostic=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-drift-drive-preview")){challenge_drift_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-ball-drive-preview")){challenge_ball_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if(!strcmp(argv[i],"--challenge-flag-drive-preview")){challenge_flag_drive_preview=challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if (!strcmp(argv[i],"--challenge-jump-drive-preview")){challenge_jump_drive_preview=1;drive_mode=1;scene="startup";recovered_camera=1;}
        else if (!strcmp(argv[i],"--drive")) drive_mode=1;
        else if (!strcmp(argv[i],"--attract-ai"))source_attract_ai=1;
        else if (!strcmp(argv[i],"--attract-session-check")){attract_session_check=source_attract_ai=drive_mode=1;}
        else if (!strcmp(argv[i],"--endgame-diagnostic"))endgame_diagnostic=1;
        else if (!strcmp(argv[i],"--results-cache-check"))results_cache_check=1;
        else if (!strcmp(argv[i],"--recovered-camera")) recovered_camera=1;
        else if (!strcmp(argv[i],"--passenger-diagnostic")) passenger_diagnostic=1;
        else if (!strcmp(argv[i],"--pickup-preview")) pickup_preview=1;
        else if (!strcmp(argv[i],"--passenger-marker-render-check")) native_marker_fixture=1;
        else if (!strcmp(argv[i],"--profile-input-check"))profile_input_check=1;
        else if (!strcmp(argv[i],"--profile-interactive"))profile_interactive=1;
        else if (!strcmp(argv[i],"--profile-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>3)fail("Profile existence flags must be0..3.");profile_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--profile-edit-column")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>2)fail("Profile edit column must be0..2.");profile_edit_column=(int)value;
        }
        else if (!strcmp(argv[i],"--profile-option")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>2)fail("Profile option must be0..2.");profile_option=(int)value;
        }
        else if (!strcmp(argv[i],"--post-register-check")){post_results_check=1;post_register_check=1;}
        else if (!strcmp(argv[i],"--startup-attract-check")){startup_mode=startup_check=startup_attract_check=startup_title_interactive=1;}
        else if (!strcmp(argv[i],"--startup-title-check")){startup_mode=1;startup_check=1;startup_title_check=1;}
        else if (!strcmp(argv[i],"--startup-title-interactive")){startup_mode=1;startup_title_interactive=1;}
        else if (!strcmp(argv[i],"--startup-interactive"))startup_mode=1;
        else if (!strcmp(argv[i],"--startup-second-city-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_second_city=1;}
        else if (!strcmp(argv[i],"--startup-profile-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_expiry_check=1;startup_profile_check=1;}
        else if (!strcmp(argv[i],"--startup-expiry-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_expiry_check=1;}
        else if (!strcmp(argv[i],"--startup-warm-reload-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_warm_reload_check=1;}
        else if(!strcmp(argv[i],"--startup-retry-input-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;startup_retry_input_check=1;}
        else if (!strcmp(argv[i],"--startup-gameplay-check")){startup_mode=1;startup_check=1;startup_gameplay_check=1;}
        else if (!strcmp(argv[i],"--startup-input-check")){startup_mode=1;startup_check=1;}
        else if (!strcmp(argv[i],"--loading-cache-check"))loading_cache_check=1;
        else if ((!strcmp(argv[i],"--loading-preview")||!strcmp(argv[i],"--loading-cap")||!strcmp(argv[i],"--loading-backdrop"))&&i+1<argc) {
            int kind=!strcmp(argv[i],"--loading-preview")?0:!strcmp(argv[i],"--loading-cap")?1:2;
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>(kind==0?4:kind==1?31:255))fail("Invalid loading preview parameter.");
            if(!kind)loading_preview=(int)value;else if(kind==1)loading_cap=(int)value;else {loading_clear_index=(int)value;loading_backdrop_explicit=1;}
        }
        else if (!strcmp(argv[i],"--city-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1)fail("City preview must be0..1.");city_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--city-locked"))city_locked=1;
        else if (!strcmp(argv[i],"--city-cache-check"))city_cache_check=1;
        else if (!strcmp(argv[i],"--city-interactive")){city_interactive=1;city_preview=0;}
        else if (!strcmp(argv[i],"--city-input-check")){city_input_check=1;city_preview=0;}
        else if (!strcmp(argv[i],"--city-cap")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>31)fail("City cap must be0..31.");city_preview_cap=(int)value;
        }
        else if (!strcmp(argv[i],"--driver-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>3)fail("Driver preview must be0..3.");driver_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--driver-cap")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>31)fail("Driver cap must be0..31.");driver_preview_cap=(int)value;
        }
        else if (!strcmp(argv[i],"--driver-cache-check"))driver_cache_check=1;
        else if (!strcmp(argv[i],"--driver-input-check")){driver_input_check=1;driver_preview=0;}
        else if (!strcmp(argv[i],"--driver-interactive")){driver_interactive=1;driver_preview=0;}
        else if (!strcmp(argv[i],"--gameplay-xbox-replay")&&i+1<argc) {
            gameplay_xbox_replay=fopen(argv[++i],"r");if(!gameplay_xbox_replay)fail("Native Xbox replay file unavailable.");
        }
        else if (!strcmp(argv[i],"--gameplay-key-replay")&&i+1<argc) {
            gameplay_key_replay=fopen(argv[++i],"r");if(!gameplay_key_replay)fail("Original key replay file unavailable.");
        }
        else if (!strcmp(argv[i],"--gameplay-step-replay")&&i+1<argc) {
            gameplay_step_replay=fopen(argv[++i],"r");if(!gameplay_step_replay)fail("Original timing replay file unavailable.");
            unsigned initial_delta;
            if(fscanf(gameplay_step_replay,"initial_delta %u",&initial_delta)!=1||initial_delta>60)fail("Original timing replay initial delta invalid.");
            gameplay_global_delta=(int32_t)initial_delta;
        }
        else if (!strcmp(argv[i],"--startup-idle-check")) {startup_idle_check=1;startup_mode=startup_check=startup_gameplay_check=1;}
        else if (!strcmp(argv[i],"--startup-driver-check")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>3)fail("Startup driver must be0..3.");
            startup_driver_selection=(int)value;startup_mode=startup_check=startup_gameplay_check=1;
        }
        else if (!strcmp(argv[i],"--driver")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>3)fail("Driver must be0..3.");native_game_driver=(unsigned)value;
        }
        else if (!strcmp(argv[i],"--taxi-cache-check"))taxi_cache_check=1;
        else if (!strcmp(argv[i],"--save-file")&&i+1<argc)native_save_path=argv[++i];
        else if (!strcmp(argv[i],"--post-results-check"))post_results_check=1;
        else if (!strcmp(argv[i],"--name-input-check"))name_input_check=1;
        else if (!strcmp(argv[i],"--name-entry-interactive"))name_interactive=1;
        else if (!strcmp(argv[i],"--name-ring-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>42)fail("Name ring selection must be 0..42.");name_ring_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--name-flow-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1000)fail("Name flow preview must be 0..1000 calls.");name_flow_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--name-flight-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>51)fail("Name flight preview must be 0..51 calls.");name_flight_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--game-over-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1000)fail("Game Over preview must be 0..1000 frames.");game_over_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--results-preview"))results_preview=1;
        else if (!strcmp(argv[i],"--results-flow-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1000)fail("Results flow preview must be 0..1000 frames.");
            results_flow_ticks=(int)value;results_preview=1;
        }
        else if (!strcmp(argv[i],"--results-ticks")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1000)fail("Results ticks must be 0..1000.");
            results_ticks=(int)value;
        }
        else if (!strcmp(argv[i],"--expiry-results")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>1000)fail("Expiry results ticks must be 0..1000.");
            expiry_results=(int)value;
        }
        else if (!strcmp(argv[i],"--expiry-restore"))expiry_restore=1;
        else if (!strcmp(argv[i],"--expiry-preview")&&i+1<argc) {
            char *end;long value=strtol(argv[++i],&end,10);
            if(end==argv[i]||*end||value<0||value>201)fail("Expiry preview must be 0..201 ticks.");
            expiry_preview=(int)value;
        }
        else if (!strcmp(argv[i],"--fullscreen")) start_fullscreen=1;
        else if (!strcmp(argv[i],"--audio-preview"))audio_preview=1;
        else if(!strcmp(argv[i],"--audio-output-disabled"))audio_output_disabled=1;
        else if(!strcmp(argv[i],"--native-elapsed-time"))native_elapsed_time=1;
        else if(!strcmp(argv[i],"--respawn-clock-smoke")){respawn_clock_smoke=native_elapsed_time=drive_mode=audio_preview=audio_output_disabled=1;audio_smoke_frames=360;}
        else if(!strcmp(argv[i],"--focus-suspend-smoke")){focus_suspend_smoke=native_elapsed_time=drive_mode=audio_preview=audio_output_disabled=1;audio_smoke_frames=360;}
        else if(!strcmp(argv[i],"--passenger-presentation-trace")&&i+1<argc){passenger_presentation_trace=fopen(argv[++i],"w");if(!passenger_presentation_trace)fail("Passenger presentation trace unavailable.");}
        else if(!strcmp(argv[i],"--presentation-time-trace")&&i+1<argc){presentation_time_trace=fopen(argv[++i],"w");if(!presentation_time_trace)fail("Presentation timing trace open failed.");}
        else if(!strcmp(argv[i],"--timestamped-presentation"))timestamped_presentation=1;
        else if(!strcmp(argv[i],"--experimental-update-ticks")&&i+1<argc){
            const char *value=argv[++i];if(experimental_update_set||value[0]<'1'||value[0]>'8'||value[1])fail("Experimental update ticks require a single value1..8.");
            experimental_update_ticks=(unsigned)(value[0]-'0');experimental_update_set=1;
        }
        else if(!strcmp(argv[i],"--reference-driving-schedule")&&i+1<argc){if(reference_driving_path)fail("Duplicate reference schedule.");reference_driving_path=argv[++i];}
        else if(!strcmp(argv[i],"--presentation-delay-ms")&&i+1<argc){char *end=NULL;double ms=strtod(argv[++i],&end);if(end==argv[i]||*end||!isfinite(ms)||ms<0||ms>250)fail("Presentation delay must be a finite number from0 to250 milliseconds.");if(presentation_delay_configured)fail("Duplicate presentation delay.");presentation_delay_ns=(uint64_t)llround(ms*1000000.0);presentation_delay_configured=1;}
        else if(!strcmp(argv[i],"--input-time-trace")&&i+1<argc){input_time_trace=fopen(argv[++i],"w");if(!input_time_trace)fail("Input timing trace open failed.");}
        else if(!strcmp(argv[i],"--host-time-trace")&&i+1<argc){host_time_trace=fopen(argv[++i],"w");if(!host_time_trace)fail("Host timing trace open failed.");}
        else if(!strcmp(argv[i],"--recovered-engine-audio")){recovered_engine_audio=1;audio_preview=1;}
        else if(!strcmp(argv[i],"--recovered-game-audio")){recovered_game_audio=recovered_engine_audio=1;audio_preview=1;}
        else if(!strcmp(argv[i],"--startup-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=1;}
        else if(!strcmp(argv[i],"--startup-attract-audio-smoke")){startup_mode=audio_preview=startup_audio_smoke=startup_attract_audio_smoke=startup_title_interactive=1;}
        else if(!strcmp(argv[i],"--startup-tip-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=startup_tip_audio_smoke=1;taxi_traffic_near_miss_observation_begin();recovered_game_audio=recovered_engine_audio=route_probe_active=traffic_preview=1;}
        else if(!strcmp(argv[i],"--startup-audio-cleanup-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=startup_audio_cleanup_smoke=1;recovered_game_audio=recovered_engine_audio=route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-frozen-audio-cleanup-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=1;startup_audio_cleanup_smoke=2;recovered_game_audio=recovered_engine_audio=route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-quit-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=1;startup_audio_cleanup_smoke=3;recovered_game_audio=recovered_engine_audio=route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-quit-menu-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=1;startup_audio_cleanup_smoke=4;recovered_game_audio=recovered_engine_audio=route_probe_active=1;}
        else if(!strcmp(argv[i],"--live-delivery-movement-trace")&&i+1<argc){if(live_delivery_movement_trace)fail("Duplicate delivery movement trace.");live_delivery_movement_trace=fopen(argv[++i],"wb");if(!live_delivery_movement_trace)fail("Cannot open movement trace.");fprintf(live_delivery_movement_trace,"source_counter,waypoint,start_x,start_y,start_z,dx,dy,dz,preserve_y,result_x,result_y,result_z,normal_x,normal_y,normal_z,queries,collided\n");taxi_movement_observer(observe_delivery_movement);}
        else if(!strcmp(argv[i],"--live-delivery-query-trace")&&i+1<argc){if(live_delivery_query_trace)fail("Duplicate delivery query trace.");live_delivery_query_trace=fopen(argv[++i],"wb");if(!live_delivery_query_trace)fail("Cannot open live delivery query trace.");fprintf(live_delivery_query_trace,"source_counter,waypoint,query_x,query_y,query_z,kind,height,polygon\n");}
        else if(!strcmp(argv[i],"--startup-delivery-audio-smoke")){startup_mode=audio_preview=startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=live_delivery_smoke=native_elapsed_time=recovered_game_audio=recovered_engine_audio=route_probe_active=route_probe_speed_steering=1;}
        else if(!strcmp(argv[i],"--startup-pickup-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_pickup_audio_smoke=1;recovered_game_audio=recovered_engine_audio=route_probe_active=1;}
        else if(!strcmp(argv[i],"--startup-game-logic-smoke")){startup_mode=startup_audio_smoke=startup_game_audio_smoke=audio_output_disabled=1;}
        else if(!strcmp(argv[i],"--startup-game-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=1;startup_game_audio_smoke=1;}
        else if(!strcmp(argv[i],"--startup-credits-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=startup_game_audio_smoke=startup_credits_audio_smoke=1;recovered_game_audio=recovered_engine_audio=1;}
        else if(!strcmp(argv[i],"--startup-natural-expiry-audio-smoke")){startup_mode=audio_preview=startup_audio_smoke=startup_game_audio_smoke=startup_expiry_audio_smoke=natural_expiry_smoke=native_elapsed_time=1;}
        else if(!strcmp(argv[i],"--startup-expiry-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=1;startup_game_audio_smoke=1;startup_expiry_audio_smoke=1;}
        else if(!strcmp(argv[i],"--startup-return-audio-smoke")){startup_mode=1;audio_preview=1;startup_audio_smoke=1;startup_game_audio_smoke=1;startup_expiry_audio_smoke=1;startup_return_audio_smoke=1;}
        else if(!strcmp(argv[i],"--audio-smoke-frames")&&i+1<argc){
            char *end;long value=strtol(argv[++i],&end,10);
            if(*end||value<1||value>4096)fail("Audio smoke frames must be1..4096.");
            audio_smoke_frames=(unsigned)value;
        }
        else if (!strcmp(argv[i],"--capture-orbit") && i+1<argc) {
            char *end=NULL;double value=strtod(argv[++i],&end);
            if(!end||end==argv[i]||*end||!isfinite(value)||value < -360||value > 360) fail("Capture orbit must be -360..360 degrees.");
            capture_orbit=(float)value;capture_orbit_set=1;
        }
        else if (!strcmp(argv[i],"--steer") && i+1<argc) {
            const char *direction=argv[++i];
            if(!strcmp(direction,"left")) script_steer=-1;
            else if(!strcmp(direction,"right")) script_steer=1;
            else fail("Scripted steering must be left or right.");
        }
        else if (!strcmp(argv[i],"--pacing-frames") && i+1<argc) {pacing_frames=atoi(argv[++i]);if(pacing_frames<60||pacing_frames>3600) fail("Pacing frame count must be 60..3600.");}
        else if(!strcmp(argv[i],"--gear-input-sequence"))gear_input_sequence=1;
        else if(!strcmp(argv[i],"--effect-request-trace")&&i+1<argc){
            if(native_effect_trace)fail("Duplicate effect trace output.");
            native_effect_trace=fopen(argv[++i],"w");if(!native_effect_trace)fail("Cannot open effect trace.");
            fputs("tick,phase,voice,state,speed,scale,smoke,requests,emissions\n",native_effect_trace);
        }
        else if (!strcmp(argv[i],"--ticks") && i+1<argc) {script_ticks=atoi(argv[++i]);if(script_ticks<0||script_ticks>36000) fail("Invalid tick count.");}
        else fail("Usage: scene-viewer.exe [--scene city1|city2|startup] [--detail] [--drive] [--fullscreen] [--ticks N] [--steer left|right] [--snapshot output.bmp] [--capture-orbit degrees] [--pacing-frames 60..3600]");
    }
    if (strcmp(scene,"city1") && strcmp(scene,"city2") && strcmp(scene,"startup")) fail("Unknown scene.");
    if(source_frame_cpu_requested&&(!image_path||!source_column_work_report_path||!taxi_source_capture_path||!taxi_owned_graphics))fail("CPU source frame requires snapshot, source workload and owned taxi captures.");
    if(taxi_source_capture_path&&(!recovered_camera||capture_orbit_set))fail("Taxi source capture requires driving with recovered camera and no orbit override.");
    if(loading_cache_check&&loading_preview<0)fail("Loading cache check requires loading preview.");
    if(normal_menu_preview>=0&&(!image_path||drive_mode||startup_mode||city_preview>=0||driver_preview>=0||profile_preview>=0||results_preview||name_interactive||name_ring_preview>=0||game_over_preview>=0||pacing_frames||loading_preview>=0))fail("Normal menu preview requires standalone snapshot.");
    if(normal_menu_cache_check&&normal_menu_preview<0)fail("Normal menu cache check requires menu preview.");
    if(normal_menu_flow_check&&normal_menu_preview<0)fail("Normal menu flow check requires menu preview.");
    if(normal_menu_slide_q12&&(normal_menu_preview<0||normal_menu_cap!=31))fail("Normal menu slide fixture requires preview and original full palette.");
    if(startup_loading_pacing_check&&(!startup_gameplay_check||!image_path))fail("Loading pacing check requires startup gameplay check and snapshot.");
    if(startup_loading_close_check&&(!startup_gameplay_check||!image_path))fail("Loading close check requires startup gameplay check and snapshot.");
    if(loading_preview>=0&&(!image_path||drive_mode||startup_mode||city_preview>=0||driver_preview>=0||results_preview||profile_preview>=0||name_interactive||name_ring_preview>=0||game_over_preview>=0||pacing_frames))fail("Loading preview requires a standalone snapshot.");
    if(pacing_frames && (!image_path || !drive_mode)) fail("Pacing probe requires --drive and --snapshot.");
    if(script_steer && ((!drive_mode && !startup_gameplay_check) || (!image_path && !audio_smoke_frames))) fail("Scripted steering requires --drive or --startup-gameplay-check and either --snapshot or bounded --audio-smoke-frames.");
    if(audio_preview&&((!drive_mode&&!startup_mode)||image_path))fail("Audio preview requires an interactive native driving or startup window.");
    if(experimental_update_set){
        if(!native_elapsed_time||(!audio_smoke_frames&&!live_delivery_smoke)||reference_driving_path||!audio_output_disabled)fail("Experimental update ticks require bounded native elapsed gameplay with disabled audio output, without a reference schedule.");
        printf("EXPERIMENTAL UPDATE GROUP: minimum%u source ticks; diagnostic policy, original workload cadence not reproduced.\n",experimental_update_ticks);
    }
    if(audio_smoke_frames&&!audio_preview)fail("Audio smoke frames require native audio preview.");
    if(capture_orbit_set&&(!drive_mode||!image_path)) fail("Capture orbit requires --drive and --snapshot.");
    if((queue_occlusion_check||render_state_check)&&(!image_path||(!drive_mode&&!startup_mode)||!traffic_preview||!recovered_camera||capture_orbit_set))
        fail("Queue occlusion check requires recovered-camera traffic driving snapshot without orbit.");
    if(challenge_jump_drive_preview&&(startup_mode||source_attract_ai||(passenger_diagnostic&&!challenge_turn_drive_preview&&!challenge_bound_drive_preview)||strcmp(scene,"startup")))fail("Jump driving preview requires its original course without another session owner.");
    if(passenger_diagnostic&&!drive_mode) fail("Customer diagnostic requires --drive.");
    if(pickup_preview&&(!image_path||!drive_mode||!passenger_diagnostic)) fail("Pickup preview requires driving customer diagnostic and snapshot.");
    if(expiry_preview>=0&&(!image_path||!drive_mode||!passenger_diagnostic||pickup_preview||pacing_frames))fail("Expiry fixture requires driving customer diagnostic snapshot only.");
    if(endgame_diagnostic&&(!drive_mode||!passenger_diagnostic||results_preview||expiry_restore))fail("Endgame diagnostic requires driving/customer mode without restoration or standalone results preview.");
    if(expiry_results>=0&&(expiry_preview<201||expiry_restore||results_preview))fail("Expiry results require completed timeout/fade fixture without restoration or standalone results preview.");
    if(expiry_restore&&expiry_preview<168)fail("Expiry restoration requires a completed expiry fixture.");
    if(drift_raw_replay_path){
        if(!jump_raw_input_replay&&(!image_path||script_ticks<2003||script_ticks>(drift_earned_capture_check?3600:2200)||ball_input_replay_path||flag_input_replay_path||drift_retry_check||drift_failure_check||(challenge_jump_retry_check&&!drift_earned_capture_check)))fail("Drift raw replay requires2003..2200 tick snapshot without fixtures.");
        if(jump_raw_input_replay&&(!image_path||script_ticks<1||script_ticks>1800||drift_xbox_replay||challenge_drift_drive_preview||challenge_jump_retry_check))fail("Jump raw replay requires bounded snapshot without scripted outcome fixtures.");
        FILE *f=fopen(drift_raw_replay_path,"rb");if(!f)fail("Drift raw replay missing.");unsigned raw;
        if(drift_xbox_replay){unsigned buttons,rt,lt;int lx,ly;
            while(fscanf(f,"%u %d %d %u %u",&buttons,&lx,&ly,&rt,&lt)==5){if(buttons>65535||lx<-32768||lx>32767||ly<-32768||ly>32767||rt>255||lt>255||drift_raw_count>=1800)fail("Invalid bounded synthetic XInput sample.");drift_xbox_samples[drift_raw_count++]=taxi_xbox_decode((uint16_t)buttons,(int16_t)lx,(int16_t)ly,(uint8_t)rt,(uint8_t)lt);}
        }else while(fscanf(f,"%u",&raw)==1){if(raw>1023||drift_raw_count>=1800)fail("Drift raw replay accepts1800 original10-bit button states.");drift_raw_replay[drift_raw_count++]=(uint16_t)raw;}
        if((jump_raw_input_replay?!drift_raw_count:drift_raw_count!=1800)||fgetc(f)!=EOF||ferror(f))fail("Drift raw replay invalid.");fclose(f);
    }
    if(turn_input_replay_path){
        if(!image_path||pickup_preview||script_ticks<204||script_ticks>(turn_earned_capture_check?3600:challenge_bound_drive_preview?2700:2400)||flag_input_replay_path||ball_input_replay_path||drift_raw_replay_path||turn_retry_check||turn_failure_check||bound_retry_check||bound_failure_check||(challenge_bound_drive_preview&&turn_earned_capture_check))fail("Course input replay requires bounded snapshot without placement/outcome fixtures.");
        FILE *f=fopen(turn_input_replay_path,"rb");if(!f)fail("Turn input replay missing.");unsigned mask,held;int analog;
        while(fscanf(f,"%u %d %u",&mask,&analog,&held)==3){
            if(mask>63||analog < -192||analog>192||held>1023||turn_input_count>=(challenge_bound_drive_preview?2400u:2100u))fail("Course replay exceeds original timer capacity or invalid mask/stick/held input.");
            turn_input_held[turn_input_count]=(uint16_t)held;turn_input_masks[turn_input_count]=(uint8_t)mask;turn_input_analog[turn_input_count++]=(int16_t)analog;
        }
        if(!turn_input_count||fgetc(f)!=EOF||ferror(f))fail("Invalid Turn input replay.");fclose(f);
    }
    if(ball_input_replay_path){
        if(!image_path||ball_contact_flow_check||flag_input_replay_path)fail("Ball input replay requires bounded snapshot without contact fixtures.");
        FILE *f=fopen(ball_input_replay_path,"rb");if(!f)fail("Ball input replay missing.");unsigned mask;int analog;
        while(fscanf(f,"%u %d",&mask,&analog)==2){
            if(mask>63||analog < -192||analog>192||ball_input_count>=2700)fail("Ball replay accepts1..2700 action masks0..63 and original stick commands-192..192.");
            ball_input_masks[ball_input_count]=(uint8_t)mask;ball_input_analog[ball_input_count++]=(int16_t)analog;
        }
        if(!ball_input_count||fgetc(f)!=EOF||ferror(f))fail("Ball input replay invalid.");fclose(f);
    }
    if(turn_earned_capture_check&&(!turn_input_replay_path||script_ticks<3000))fail("Earned Turn registration check requires input replay and3000+ticks.");
    if(drift_earned_capture_check&&(!drift_raw_replay_path||script_ticks<3000))fail("Earned Drift registration check requires raw replay and3000+ticks.");
    if(ball_earned_capture_check&&!ball_input_replay_path)fail("Earned Ball check requires input replay.");
    if(flag_earned_capture_check&&!flag_input_replay_path)fail("Earned Flag check requires input replay.");
    if(flag_input_replay_path){
        if(!image_path||flag_contact_flow_check||flag_retry_check||flag_failure_check)fail("Flag replay requires bounded snapshot without outcome fixtures.");
        FILE *f=fopen(flag_input_replay_path,"rb");if(!f)fail("Flag replay input missing.");unsigned mask;
        while(fscanf(f,"%u",&mask)==1){if(mask>63||flag_input_count>=1200)fail("Flag replay accepts1..1200 six-action masks0..63.");flag_input_replay[flag_input_count++]=(uint8_t)mask;}
        if(!flag_input_count||fgetc(f)!=EOF||ferror(f))fail("Flag replay input invalid.");fclose(f);
    }
    char asset_path[MAX_PATH];
    DWORD n=GetModuleFileNameA(NULL,asset_path,MAX_PATH);
    if (!n || n>=MAX_PATH) fail("Executable path is too long.");
    char *slash=strrchr(asset_path,'\\');
    if (!slash || (size_t)(slash-asset_path)+32>=MAX_PATH) fail("Cannot locate assets.");
    snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/%s.ctm",scene);
    if(challenge_bound_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge5.ctm");
    else if(challenge_turn_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge4.ctm");
    else if(challenge_drift_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge3.ctm");
    if(challenge_ball_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge2.ctm");
    if(challenge_flag_drive_preview)snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/challenge-worlds/challenge1.ctm");
    if(async_4k_render_check&&(!performance_trace||audio_smoke_frames<1||audio_smoke_frames>4096||startup_mode||live_snapshot_path))fail("Async4K check requires1..4096 bounded direct gameplay frames and performance trace, without screenshot capture.");
    if(live_delivery_movement_trace&&!live_delivery_smoke)fail("Movement trace requires live delivery smoke.");
    if(live_delivery_query_trace&&!live_delivery_smoke)fail("Delivery query trace requires live input-only delivery smoke.");
    if(window_surface_smoke&&(startup_mode||start_fullscreen||live_4k_render_check||audio_smoke_frames!=360))fail("Window smoke requires standalone360-frame drive without initial fullscreen or4Kquery checks.");
    if(visible_4k_check&&(!live_4k_render_check||!native_4k_target))fail("Visible4K control requires bounded4Kcheck and independent target.");
    if(live_4k_render_check&&(image_path||start_fullscreen||!native_elapsed_time||(!audio_smoke_frames&&!startup_game_audio_smoke)))fail("Live4K rendering check requires bounded elapsed gameplay without fullscreen or snapshot mode.");
    if(target_blit_check&&!native_4k_target)fail("Blit check requires native4K target.");
    if(native_4k_target&&(image_path||!native_elapsed_time))fail("Native4K target requires live elapsed presentation, not snapshot-only mode.");
    if(reference_render_pose_path&&(!image_path||!drive_mode||!recovered_camera||script_ticks||timestamped_presentation||prototype_camera_framing))fail("Reference pose is a zero-tick recovered-camera snapshot fixture only.");
    if(live_snapshot_path&&(!timestamped_presentation||image_path||(!audio_smoke_frames&&!startup_game_audio_smoke)))fail("Live capture requires bounded timestamped gameplay.");
    if(reference_driving_path&&(!drive_mode||startup_mode||!native_elapsed_time||!timestamped_presentation||!recovered_camera||!native_4k_target||!async_4k_render_check||!audio_smoke_frames||!audio_output_disabled||traffic_preview||passenger_diagnostic||scene_index||image_path||route_probe_active||prototype_camera_framing))fail("Reference driving requires bounded standalone native4K elapsed/history/recovered-camera check with device audio disabled and city1, without gameplay actors or snapshots.");
    if(presentation_delay_configured&&!timestamped_presentation)fail("Presentation delay requires timestamped presentation.");
    if(timestamped_presentation&&(!native_elapsed_time||!recovered_camera||image_path||(!drive_mode&&!startup_mode)))fail("Timestamped presentation requires elapsed driving or startup and recovered camera.");
    load_scene(asset_path);
    if (detail) detail_view();
    if(city_interactive||city_input_check||startup_mode) {
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/results/default-scores.cts");load_native_settings(asset_path);
    }
    if(drive_mode)load_drive_state(scene,NULL,NULL);
    if(source_attract_ai){
        if(!drive_mode||(strcmp(scene,"city1")&&strcmp(scene,"city2")))fail("Attract AI requires driving in city1 or city2.");
        snprintf(slash+1,MAX_PATH-(size_t)(slash+1-asset_path),"../assets/ai/%s-path.cta",scene);
        if(taxi_ai_path_load(&attract_ai_path,asset_path))fail("Original attract path load failed.");
        if(attract_session_check){
            if(passenger_diagnostic||traffic_preview)fail("Attract session check requires a standalone AI diagnostic.");
            attract_probe_settings[4]=1-(uint8_t)scene_index;
            taxi_attract_session_begin(&attract_session,attract_probe_settings,(uint8_t)native_game_driver,(uint16_t)scene_index);
            attract_exit_fade.interval_1771=attract_exit_fade.reload_1772=1;
        }
        puts("Original attract AI controls actor0; diagnostic driving only, title parent/camera/audio attract ownership pending.");
    }
    if(profile_input_check&&(profile_preview!=0||!image_path||profile_interactive||drive_mode||results_preview||name_interactive||name_ring_preview>=0||game_over_preview>=0||pacing_frames))fail("Profile input check requires standalone profile0 snapshot.");
    if(profile_interactive) {
        if(image_path||drive_mode||results_preview||name_interactive||name_ring_preview>=0||game_over_preview>=0||pacing_frames)fail("Interactive profile requires standalone window.");
        if(profile_preview<0)profile_preview=0;
    }
    if(profile_edit_column>=0&&profile_preview<0)fail("Profile edit fixture requires profile preview.");
    if(profile_preview>=0&&!profile_interactive&&(!image_path||drive_mode||results_preview||name_interactive||name_ring_preview>=0||game_over_preview>=0||pacing_frames||profile_option>=(profile_preview?3:2)))fail("Profile preview requires standalone snapshot and valid option.");
    if(post_results_check&&(!image_path||!drive_mode||!passenger_diagnostic||!endgame_diagnostic||expiry_preview>=0||results_preview||name_ring_preview>=0||name_interactive||game_over_preview>=0||pacing_frames||pickup_preview))fail("Post-results check requires standalone driving/endgame/customer snapshot.");
    if(name_input_check&&(name_ring_preview!=0||!image_path||name_interactive||name_flow_preview>=0||name_flight_preview>=0))fail("Name input check requires standalone ring0 snapshot.");
    if(name_interactive&&(image_path||drive_mode||results_preview||name_ring_preview>=0||name_flow_preview>=0||name_flight_preview>=0||game_over_preview>=0||pacing_frames))fail("Interactive name entry must run standalone without snapshot fixtures.");
    if(name_ring_preview>=0&&(!image_path||drive_mode||results_preview||game_over_preview>=0||pacing_frames))fail("Name ring preview requires standalone snapshot.");
    if(game_over_preview>=0&&(!image_path||drive_mode||results_preview||pacing_frames))fail("Game Over preview requires standalone snapshot.");
    if(results_flow_ticks>=0&&results_ticks)fail("Results flow and ungated animation fixture cannot be combined.");
    if(results_ticks&&!results_preview)fail("Results ticks require results preview.");
    if(results_preview&&(!image_path||drive_mode||pacing_frames))fail("Results art preview requires snapshot only.");
    SetProcessDPIAware();
    WNDCLASSW wc={0}; wc.style=CS_OWNDC; wc.lpfnWndProc=window_proc;
    wc.hInstance=GetModuleHandleW(NULL); wc.lpszClassName=L"TaxiNativeScene"; wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    if (!RegisterClassW(&wc)) fail("Window class creation failed.");
    int width=(image_path||(live_4k_render_check&&!native_4k_target))?3840:1280, height=(image_path||(live_4k_render_check&&!native_4k_target))?2160:720;
    if(native_earned_audio_check){
        if(!image_path)fail("Earned audio capture requires an offscreen snapshot diagnostic.");
        if(taxi_audio_asset_load(&audio_bank,"outputs/crazy-taxi-native/assets/audio"))fail("Earned audio bank unavailable.");
        FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");music_bytes=malloc(181241);
        if(!f||!music_bytes||fread(music_bytes,1,181241,f)!=181241||fgetc(f)!=EOF)fail("Earned music module invalid.");
        fclose(f);if(taxi_music_asset_index(&music_asset,music_bytes,181241))fail("Earned music index invalid.");
        f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");
        if(!f||fread(music_pitch,1,1536,f)!=1536||fgetc(f)!=EOF)fail("Earned music pitch table invalid.");fclose(f);
        native_audio_capture=fopen("outputs/crazy-taxi-native/reports/earned-quit-native-pcm.s8","wb");
        if(!native_audio_capture)fail("Earned native PCM capture unavailable.");
        audio_live=1;recovered_game_audio=recovered_engine_audio=1;audio_global_volume=audio_settings_state.effects_volume;
    }
    DWORD style=(image_path||live_4k_render_check)?WS_POPUP:WS_OVERLAPPEDWINDOW;
    RECT size={0,0,width,height}; AdjustWindowRect(&size,style,FALSE);
    HWND window=CreateWindowW(wc.lpszClassName,L"Crazy Taxi asset viewer - NOT PLAYABLE | Drag: orbit | Right-drag: pan | F: detail | R: reset | T: textures | F11: fullscreen",style,
        CW_USEDEFAULT,CW_USEDEFAULT,size.right-size.left,size.bottom-size.top,NULL,NULL,wc.hInstance,NULL);
    if (!window) fail("Window creation failed.");
    if(drive_mode) SetWindowTextW(window,L"Crazy Taxi native driving prototype | Arrows / Xbox stick + RT/LT | R: respawn | Drag: camera | F11: fullscreen");
    if(passenger_diagnostic) SetWindowTextW(window,L"Customer/fare diagnostic | R: reset | V: camera | F11: fullscreen");
    HDC dc=GetDC(window);
    PIXELFORMATDESCRIPTOR format={0}; format.nSize=sizeof(format); format.nVersion=1;
    format.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    format.iPixelType=PFD_TYPE_RGBA; format.cColorBits=32; format.cDepthBits=24;
    int index=ChoosePixelFormat(dc,&format);
    if (!index || !SetPixelFormat(dc,index,&format)) fail("OpenGL pixel format failed.");
    HGLRC context=wglCreateContext(dc);
    if (!context || !wglMakeCurrent(dc,context)) fail("OpenGL context creation failed.");
    if(native_4k_target)target_create();
    if(target_blit_check)target_verify_blit(window);
    typedef void (APIENTRY *TaxiGenQueriesFn)(GLsizei,GLuint*);
    typedef void (APIENTRY *TaxiQueryCounterFn)(GLuint,GLenum);
    typedef void (APIENTRY *TaxiQueryResultFn)(GLuint,GLenum,uint64_t*);
    typedef void (APIENTRY *TaxiQueryAvailableFn)(GLuint,GLenum,GLint*);
    TaxiGenQueriesFn profile_gen_queries=NULL;TaxiQueryCounterFn profile_query_counter=NULL;TaxiQueryResultFn profile_query_result=NULL;GLuint *profile_queries=NULL,*profile_blit_queries=NULL;unsigned profile_query_capacity=async_4k_render_check?(unsigned)audio_smoke_frames:1;TaxiQueryAvailableFn profile_query_available=NULL;
    if(performance_trace){
        if(!live_4k_render_check)fail("GPU performance trace requires live4K rendering check.");
        FARPROC available=wglGetProcAddress("glGetQueryObjectiv");if((uintptr_t)available<=3||(uintptr_t)available==UINTPTR_MAX)fail("GPU query availability unavailable.");memcpy(&profile_query_available,&available,sizeof available);
        FARPROC gen=wglGetProcAddress("glGenQueries"),counter=wglGetProcAddress("glQueryCounter"),result=wglGetProcAddress("glGetQueryObjectui64v");
        if((uintptr_t)gen<=3||(uintptr_t)counter<=3||(uintptr_t)result<=3||(uintptr_t)gen==UINTPTR_MAX||(uintptr_t)counter==UINTPTR_MAX||(uintptr_t)result==UINTPTR_MAX)fail("GPU timer queries unavailable.");
        memcpy(&profile_gen_queries,&gen,sizeof gen);memcpy(&profile_query_counter,&counter,sizeof counter);memcpy(&profile_query_result,&result,sizeof result);profile_queries=calloc(profile_query_capacity*2,sizeof *profile_queries);if(!profile_queries)fail("GPU query allocation failed.");profile_gen_queries((GLsizei)(profile_query_capacity*2),profile_queries);
        if(native_4k_target&&async_4k_render_check){profile_blit_queries=calloc(profile_query_capacity*2,sizeof *profile_blit_queries);if(!profile_blit_queries)fail("Blit GPU query allocation failed.");profile_gen_queries((GLsizei)(profile_query_capacity*2),profile_blit_queries);}
    }

    typedef BOOL (WINAPI *TaxiSwapIntervalFn)(int);
    FARPROC swap_symbol=wglGetProcAddress("wglSwapIntervalEXT");
    int swap_requested=(image_path||native_elapsed_time)?0:1,swap_applied=0;
    if(swap_symbol&&(uintptr_t)swap_symbol>3&&(uintptr_t)swap_symbol!=UINTPTR_MAX) {
        TaxiSwapIntervalFn swap_interval;memcpy(&swap_interval,&swap_symbol,sizeof swap_interval);
        swap_applied=swap_interval(swap_requested)!=0;
    }
    printf("PRESENTATION: swap interval%d request %s; driver/display override and scanout remain unverified.\n",swap_requested,swap_applied?"accepted":"unavailable");
    loading_window=window;loading_dc=dc;startup_loading_snapshot=image_path!=NULL;
    if(name_flow_preview>=0&&(name_ring_preview<0||name_flight_preview>=0))fail("Name flow preview requires standalone ring selection without flight fixture.");
    if(name_flight_preview>=0&&name_ring_preview<0)fail("Name flight preview requires name ring selection.");
    if(name_ring_preview>=0) {
        load_name_ring_preview();taxi_name_entry_begin(&name_preview_state,&name_ring_layout,160);name_preview_state.selection=(uint8_t)name_ring_preview;
        if(name_flow_preview>=0) {
            TaxiNameFlow flow;if(taxi_name_flow_begin(&flow,&name_ring_layout,160,0,0))fail("Name flow begin failed.");flow.entry.selection=(uint8_t)name_ring_preview;
            for(int i=0;i<name_flow_preview;i++)taxi_name_flow_step(&flow,name_ring_characters,1,0,0,flow.entry.selection==42,0);
            name_preview_state=flow.entry;name_upload_cap(flow.palette_cap);
            printf("NAME FLOW FIXTURE: calls%d cap%d fade%u complete%u; END confirm fixture, retained interval0, device input pending.\n",name_flow_preview,flow.palette_cap,flow.fade.active_177b,flow.complete);
        }
        if(name_flight_preview>=0) {
            for(int i=0;i<name_flight_preview;i++)taxi_name_entry_step(&name_preview_state,name_ring_characters,0,0,!name_preview_state.flying,0,0);
            printf("NAME FLIGHT FIXTURE: calls%d length%u flying%u xyQ8(%ld,%ld). Explicit repeated confirm requests, not device input.\n",name_flight_preview,name_preview_state.length,name_preview_state.flying,(long)name_preview_state.flight_x,(long)name_preview_state.flight_y);
        }
    }
    if(name_interactive) {
        load_name_ring_preview();
        if(taxi_name_flow_begin(&name_live_flow,&name_ring_layout,160,0,0))fail("Interactive name entry initialization failed.");
        name_preview_state=name_live_flow.entry;name_upload_cap(0);
        SetWindowTextW(window,L"Native name-entry diagnostic | Xbox: D-pad/stick, A confirm, B delete, Start confirm | Keyboard: arrows, Z/Enter confirm, X delete | F11 fullscreen");
        puts("NAME INTERACTIVE DIAGNOSTIC: Xbox/keyboard adapter, original native menu timing; explicit Axel/rank1/retainedinterval0, profile/save pending.");
    }
    if(name_input_check) {
        if(taxi_name_flow_begin(&name_live_flow,&name_ring_layout,160,0,0))fail("Name input check initialization failed.");
        for(unsigned i=0;i<33;i++)name_interactive_tick(0);
        for(unsigned i=0;i<150;i++)name_interactive_tick(1);
        if(name_live_flow.entry.length!=1||name_live_flow.entry.flying)fail("Held confirm repeated initial.");
        name_interactive_tick(0);name_interactive_tick(2);name_interactive_tick(0);
        if(name_live_flow.entry.length)fail("Native back input failed.");
        for(unsigned letter=0;letter<3;letter++) {
            name_interactive_tick(1);for(unsigned i=0;i<18;i++)name_interactive_tick(0);
        }
        for(unsigned i=0;i<10;i++)name_interactive_tick(0);
        if(name_live_flow.entry.selection!=42||name_live_flow.entry.length!=3||name_live_flow.entry.exiting)fail("Native automatic END failed.");
        name_interactive_tick(1);for(unsigned i=0;i<34;i++)name_interactive_tick(0);
        if(!game_over_active||!name_live_flow.complete)fail("Native name-to-Game-Over transition failed.");
        /* Re-entry restores immutable source pixels in the same GL process. */
        game_over_active=0;memset(&name_live_input,0,sizeof name_live_input);
        if(taxi_name_flow_begin(&name_live_flow,&name_ring_layout,160,0,0))fail("Name re-entry failed.");
        for(unsigned i=0;i<34;i++)name_interactive_tick(0);
        unsigned char *pixels=malloc(240*160*4);if(!pixels)fail("Name texture check allocation failed.");
        for(unsigned i=0;i<1455;i++) {
            Texture *t=name_sources+i;glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
            if(memcmp(pixels,t->pixels,t->width*t->height*4))fail("Name re-entry texture restoration failed.");
        }
        free(pixels);puts("PASS native name input check: held A, fresh B delete, three initials, automatic END, fresh confirm, Game Over, re-entry and1455 byte-exact restored textures. Synthetic logical keys, not physical Xbox verification.");
    }
    if(startup_mode) {
        if((startup_check?!image_path:image_path!=NULL)||drive_mode||city_preview>=0||driver_preview>=0||profile_preview>=0||name_interactive||profile_interactive||results_preview||name_ring_preview>=0||game_over_preview>=0||pacing_frames)fail("Startup requires standalone window or input-check snapshot.");
        city_preview=0;load_city_preview();driver_preview=0;load_driver_preview();city_begin();startup_stage=1;
        if(startup_title_check||startup_title_interactive)title_live_begin();
        if(startup_title_check){
            uint32_t random=passenger_scene.random_state;
            for(unsigned frame=0;frame<50;frame++)startup_tick(0);
            if(startup_stage!=TAXI_STARTUP_TITLE_STAGE||live_title.fade.active_177b||live_title.bldy)fail("Live title entry failed.");
            startup_tick(8);
            for(unsigned frame=0;startup_stage==TAXI_STARTUP_TITLE_STAGE&&frame<40;frame++)startup_tick(0);
            if(startup_stage!=9||!normal_live_flow.main||normal_live_flow.mask!=1||passenger_scene.random_state!=random)fail("Live title main initialization failed.");
            for(unsigned frame=0;frame<50;frame++)startup_tick(0);
            if(startup_stage!=9||normal_live_flow.fade.active_177b||normal_live_flow.choice.selection||taxi_normal_menu_flow_cap(&normal_live_flow)!=31)fail("Title main settled state failed.");
            startup_tick(128);startup_tick(0);startup_tick(0);
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=15||challenge_live_flow.fade.active_177b)fail("Main Crazy Box live entry failed.");
            uint8_t challenge_fixture_saved[512];memcpy(challenge_fixture_saved,native_settings,512);
            uint8_t *fixture_group=native_settings+208*native_current_profile;
            fixture_group[8]='B';fixture_group[9]='A';fixture_group[10]='A';fixture_group[11]=1;
            for(unsigned n=0;n<2;n++){startup_tick(128);startup_tick(0);startup_tick(0);}
            if(challenge_live_selection!=2)fail("Live Crazy Box profile-to-grid navigation failed.");
            startup_tick(4);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=16||challenge_live_flow.fade.active_177b||challenge_live_submode||challenge_live_flow.briefing.page3875)fail("Live Crazy Box rules entry failed.");
            startup_tick(1);startup_tick(0);startup_tick(0);
            if(challenge_live_flow.briefing.page3875!=1)fail("Live Crazy Box advice toggle failed.");
            for(unsigned n=0;n<360;n++)startup_tick(0);
            if(!challenge_live_flow.briefing.blink)fail("Live Crazy Box advice timer failed.");
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=15||challenge_live_flow.fade.active_177b||challenge_live_selection!=2)fail("Live Crazy Box briefing return failed.");
            if(challenge_bound_menu_entry_check||challenge_turn_menu_entry_check||challenge_jump_menu_entry_check||challenge_flag_menu_entry_check||challenge_ball_menu_entry_check||challenge_drift_menu_entry_check){
                if(challenge_bound_menu_entry_check){startup_tick(16);startup_tick(0);startup_tick(0);for(unsigned n=0;n<2;n++){startup_tick(128);startup_tick(0);startup_tick(0);}if(challenge_live_selection!=7)fail("Crazy Box Bound navigation failed.");}
                if(challenge_turn_menu_entry_check){startup_tick(16);startup_tick(0);startup_tick(0);startup_tick(128);startup_tick(0);startup_tick(0);if(challenge_live_selection!=6)fail("Crazy Box Turn navigation failed.");}
                if(challenge_drift_menu_entry_check){startup_tick(16);startup_tick(0);startup_tick(0);if(challenge_live_selection!=5)fail("Crazy Box Drift navigation failed.");}
                if(challenge_ball_menu_entry_check){for(unsigned n=0;n<2;n++){startup_tick(128);startup_tick(0);startup_tick(0);}if(challenge_live_selection!=4)fail("Crazy Box Ball navigation failed.");}
                if(challenge_flag_menu_entry_check){startup_tick(128);startup_tick(0);startup_tick(0);if(challenge_live_selection!=3)fail("Crazy Box Flag navigation failed.");}
                native_settings[0]=(uint8_t)((native_settings[0]&224)|27);
                startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
                if(startup_stage!=3||!drive_mode||!challenge_jump_drive_preview||scene_index!=2||(passenger_diagnostic&&!challenge_turn_menu_entry_check&&!challenge_bound_menu_entry_check))fail("Crazy Box native Jump entry failed.");
                if(native_settings[0]!=155||drive_context.driver.motion.orientation.settings!=155)fail("Challenge entry lost original option bits.");
                if(challenge_flag_menu_entry_check&&(world.nv!=403||world.np!=225||drive_context.driver.motion.orientation.submode!=1||!challenge_flag_drive_preview||live_flag_actor.bytes[0]!=1||live_flag_timer.remaining_1552!=1200))fail("Flag menu course/controller/actor ownership failed.");
                if(challenge_ball_menu_entry_check&&(world.nv!=510||world.np!=217||drive_context.driver.motion.orientation.submode!=2||!challenge_ball_drive_preview||live_ball_state.remaining_1558!=2700))fail("Ball menu course/controller ownership failed.");
                if(challenge_drift_menu_entry_check&&(world.nv!=336||world.np!=258||drive_context.driver.motion.orientation.submode!=3||!challenge_drift_drive_preview||live_drift_state.remaining_164e!=1800||live_drift_option3!=4))fail("Drift menu course/controller ownership failed.");
                if(challenge_bound_menu_entry_check&&(world.nv!=807||world.np!=737||drive_context.driver.motion.orientation.submode!=5||!challenge_bound_drive_preview||live_bound_state.remaining_1660!=2400||live_bound_option3!=4||passenger_scene.count!=1||!passenger_diagnostic))fail("Bound menu course/controller/actor ownership failed.");
                if(challenge_turn_menu_entry_check&&(world.nv!=646||world.np!=497||drive_context.driver.motion.orientation.submode!=4||!challenge_turn_drive_preview||live_turn_state.remaining_165c!=2100||live_turn_option3!=4||passenger_scene.count!=1||!passenger_diagnostic))fail("Turn menu course/controller/actor ownership failed.");
                for(unsigned n=0;n<200;n++)startup_tick(0);
                if(live_game_start.active||simulation_error)fail("Crazy Box native Jump start failed.");
                challenge_jump_return_grid();for(unsigned n=0;n<40;n++)startup_tick(0);
                if(startup_stage!=15||drive_mode||challenge_live_flow.fade.active_177b)fail("Jump entry diagnostic grid restoration failed.");
                printf("PASS main/CrazyBox/%s live resource handoff and original start sequence; explicit diagnostic grid restoration.\n",challenge_bound_menu_entry_check?"Bound":challenge_turn_menu_entry_check?"Turn":challenge_drift_menu_entry_check?"Drift":challenge_ball_menu_entry_check?"Ball":challenge_flag_menu_entry_check?"Flag":"Jump");
            }
            puts("PASS live Crazy Box profile-valid fixture: source bank7 profile-to-grid navigation, Select rules, A advice, original360-tick advice change, Back retains grid selection. Fixture is not an earned profile.");
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=9||normal_live_flow.fade.active_177b||normal_live_main_selection!=1)fail("Crazy Box Back to main failed.");
            memcpy(native_settings,challenge_fixture_saved,512);
            startup_tick(64);startup_tick(0);startup_tick(0);
            if(normal_live_main_selection)fail("Crazy Box return selection reset failed.");
            puts("PASS live Crazy Box main entry and original Back callback return; challenge sessions and animated layer motion pending.");
            puts("PASS live native title-to-main stage: Start input, natural fade exit, actual parent handoff and settled menu; original attract still pending.");
        }
        if(startup_parent_check){
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=8||normal_live_flow.fade.active_177b||normal_live_flow.mask!=1)fail("City cancel to mode failed.");
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=9||normal_live_flow.fade.active_177b||normal_live_flow.mask!=0x1000)fail("Mode Back to main failed.");
            for(unsigned n=0;n<2;n++){startup_tick(128);startup_tick(0);startup_tick(0);}
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=11||options_live_flow.fade.active_177b||options_live_selection)fail("Main Options entry failed.");
            uint8_t old_time=native_settings[5];
            startup_tick(old_time==70?32:16);startup_tick(0);startup_tick(0);
            if(native_settings[5]!=(uint8_t)(old_time+(old_time==70?-5:5)))fail("Live Options Time adjustment failed.");
            char submenu_capture[MAX_PATH];
            if(snprintf(submenu_capture,sizeof submenu_capture,"%s.options.bmp",image_path)>=(int)sizeof submenu_capture)fail("Options capture path too long.");
            draw(width,height);glFinish();snapshot(submenu_capture,width,height);
            for(unsigned n=0;n<3;n++){startup_tick(128);startup_tick(0);startup_tick(0);}
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=12||audio_live_flow.fade.active_177b||audio_live_selection)fail("Options Sound entry failed.");
            startup_tick(128);startup_tick(0);startup_tick(0);
            uint8_t old_volume=native_settings[2];
            startup_tick(old_volume?32:16);startup_tick(0);startup_tick(0);
            if(native_settings[2]!=(uint8_t)(old_volume+(old_volume?-8:8)))fail("Live Sound volume adjustment failed.");
            if(snprintf(submenu_capture,sizeof submenu_capture,"%s.sound.bmp",image_path)>=(int)sizeof submenu_capture)fail("Sound capture path too long.");
            draw(width,height);glFinish();snapshot(submenu_capture,width,height);
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=11||options_live_flow.fade.active_177b||options_live_selection!=3)fail("Sound cancellation to Options failed.");
            startup_tick(128);startup_tick(0);startup_tick(0);
            uint8_t old_control=native_settings[0];
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=10||control_fade.active_177b)fail("Options Control entry failed.");
            for(unsigned n=0;n<3;n++){
                uint8_t before=native_settings[0];startup_tick(1);startup_tick(0);startup_tick(0);
                if(native_settings[0]!=taxi_control_layout_next(before))fail("Control layout confirm failed.");
            }
            if(native_settings[0]!=old_control)fail("Control three-layout cycle lost settings.");
            if(snprintf(submenu_capture,sizeof submenu_capture,"%s.control.bmp",image_path)>=(int)sizeof submenu_capture)fail("Control capture path too long.");
            draw(width,height);glFinish();snapshot(submenu_capture,width,height);
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=11||options_live_selection!=4||options_live_flow.fade.active_177b)fail("Control cancellation to Options failed.");
            puts("PASS native Control: original three-layout cycle, preserved settings, input-only Options return and GPU bytes; footer/background priority incomplete.");
            startup_tick(2);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=9||normal_live_flow.fade.active_177b||normal_live_main_selection!=2)fail("Options cancellation to main failed.");
            /* Restore source settings so this diagnostic retains the existing
               subsequent gameplay fixture's original configuration. */
            native_settings[5]=old_time;native_settings[2]=old_volume;
            audio_settings_state.effects_volume=audio_global_volume=old_volume;
            for(unsigned n=0;n<2;n++){startup_tick(64);startup_tick(0);startup_tick(0);}
            puts("PASS live native Options/Sound: input-only settings changes, retained parent/main selection, completed fades and byte-exact GPU frames. Playback pending.");
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=8||normal_live_flow.fade.active_177b)fail("Main confirm to mode failed.");
            for(unsigned selection=0;selection<startup_timed_check;selection++){startup_tick(128);startup_tick(0);startup_tick(0);}
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=1||(native_settings[0]&0x60)!=(startup_timed_check<<5))fail("Selected rules to city failed.");
            puts("STARTUP PARENT: real city cancel/mode Back/main confirm/Normal Rules city reentry and GPU frames passed.");
        }
        if(startup_attract_check) {
            uint8_t saved_city=native_settings[4];unsigned retained_driver=native_game_driver;
            unsigned n=0;while(startup_stage!=14&&n++<1500)startup_tick(0);
            if(startup_stage!=14||!live_attract||native_game_driver!=retained_driver||simulation_error)fail("Title timeout to native attract failed.");
            for(unsigned i=0;i<180;i++)startup_tick(0);
            if(simulation_error||!attract_session.active||city_fade.active_177b)fail("Live attract driving/fade failed.");
            while((native_presentation_counter&127)>111)startup_tick(0);
            char demo_capture[MAX_PATH];
            if(snprintf(demo_capture,sizeof demo_capture,"%s.demo.bmp",image_path)>=(int)sizeof demo_capture)fail("Demo capture path too long.");
            draw(width,height);glFinish();snapshot(demo_capture,width,height);
            startup_tick(8);n=0;while(startup_stage==14&&n++<90)startup_tick(0);
            if(startup_stage!=TAXI_STARTUP_TITLE_STAGE||live_attract||source_attract_ai||native_settings[4]!=saved_city||native_game_driver!=retained_driver)fail("Live attract cancellation/title restore failed.");
            for(unsigned i=0;i<50;i++)startup_tick(0);
            if(live_title.fade.active_177b||live_title.bldy)fail("Returned title did not settle.");
            n=0;while(startup_stage!=14&&n++<1500)startup_tick(0);
            if(startup_stage!=14)fail("Repeated title timeout did not enter demo.");
            n=0;unsigned score_captured=0;
            while(startup_stage==14&&n++<10000) {
                startup_tick(0);
                if(attract_session.flow.blink_enabled&&attract_session.flow.phase<2&&!(score_captured&(1u<<attract_session.flow.phase))&&attract_session.flow.blink_frame>=20) {
                    char score_capture[MAX_PATH];
                    if(snprintf(score_capture,sizeof score_capture,"%s.scores%u.bmp",image_path,attract_session.flow.phase)>=(int)sizeof score_capture)fail("Score capture path too long.");
                    draw(width,height);glFinish();snapshot(score_capture,width,height);score_captured|=1u<<attract_session.flow.phase;
                }
            }
            if(score_captured!=3)fail("Natural demo did not reveal both score pages.");
            puts("PASS natural demo revealed and rendered both original saved-score pages.");
            if(startup_stage!=TAXI_STARTUP_TITLE_STAGE||attract_session.active||live_attract||simulation_error||native_settings[4]!=saved_city||native_game_driver!=retained_driver)fail("Natural attract timeout/title restore failed.");
            for(unsigned i=0;i<50;i++)startup_tick(0);
            startup_tick(8);for(unsigned i=0;i<90;i++)startup_tick(0);
            if(startup_stage!=9||normal_live_flow.fade.active_177b)fail("Returned demo title could not enter main menu.");
            startup_tick(1);for(unsigned i=0;i<90;i++)startup_tick(0);
            if(startup_stage!=8)fail("Post-demo main to mode failed.");
            startup_tick(1);for(unsigned i=0;i<90;i++)startup_tick(0);
            if(startup_stage!=1)fail("Post-demo mode to city failed.");
            startup_tick(1);for(unsigned i=0;i<90;i++)startup_tick(0);
            if(startup_stage!=2)fail("Post-demo city to driver failed.");
            startup_tick(1);for(unsigned i=0;startup_stage!=3&&i<120;i++)startup_tick(0);
            unsigned post_demo_start_ticks=0;
            while(live_game_start.active&&post_demo_start_ticks<500){startup_tick(0);post_demo_start_ticks++;}
            if(live_game_start.active||post_demo_start_ticks<315)fail("Post-demo original startup handoff failed.");
            for(unsigned i=0;i<60;i++)startup_tick(0);
            if(startup_stage!=3||live_attract||source_attract_ai||!endgame_diagnostic||simulation_error)fail("Post-demo normal gameplay ownership failed.");
            puts("PASS same-process demo/title/main/mode/city/driver/loading/normal driving handoff: AI override cleared, normal endgame enabled,60 native updates.");
            printf("PASS live title timeout -> original demo loading/AI -> Start fade -> title, repeated demo natural timeout%u calls -> title -> main: retained driver, saved city, original template/path; synthetic input, full trajectory/boot RNG/audio oracle pending.\n",n);
        }
        if(startup_check&&!startup_title_check&&!startup_attract_check) {
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(1);for(unsigned n=0;n<40;n++)startup_tick(0);
            if(startup_stage!=2||native_settings[4]!=0)fail("Startup city-confirm transition failed.");
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(16);startup_tick(0);startup_tick(0);
            if(driver_preview!=1)fail("Startup driver navigation failed.");
            startup_tick(2);for(unsigned n=0;n<40;n++)startup_tick(0);
            if(startup_stage!=1||native_settings[4])fail("Startup driver-back transition failed.");
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=2||driver_preview)fail("Startup driver-reset transition failed.");
            puts("STARTUP INPUT: city-confirm, driver navigation/back, city reentry and reset driver0 passed; gameplay and parent-back remain pending.");
        }
    }
    if(city_preview>=0&&!startup_mode) {
        if((city_interactive?image_path!=NULL:!image_path)||drive_mode||driver_preview>=0||profile_preview>=0||profile_interactive||name_interactive||results_preview||name_ring_preview>=0||game_over_preview>=0||pacing_frames)fail("City screen requires standalone window or snapshot.");
        load_city_preview();
        if(city_interactive)city_begin();
        if(city_input_check) {
            uint8_t before[8];memcpy(before,native_settings,8);
            for(unsigned unlocked=0;unlocked<2;unlocked++) {
                native_settings[7]=(uint8_t)unlocked;city_begin();
                for(unsigned n=0;n<50;n++)city_tick(0);
                city_tick(16);for(unsigned n=0;n<100;n++)city_tick(16);
                if(city_preview!=1||city_scroll_x!=(unlocked?256u:0u)||city_scroll_y!=(unlocked?0u:256u))fail("City held navigation/scroll fixture failed.");
                city_tick(0);city_tick(0);city_tick(1);city_tick(0);
                if(unlocked) {
                    for(unsigned n=0;n<40;n++)city_tick(0);
                    if(!city_flow.complete||city_flow.choice.exiting!=1||city_preview_cap)fail("City confirm fade fixture failed.");
                } else {
                    if(city_flow.choice.exiting||city_flow.fade.active_177b)fail("Locked city confirm fixture failed.");
                    city_tick(2);for(unsigned n=0;n<40;n++)city_tick(0);
                    if(!city_flow.complete||city_flow.choice.exiting!=2||city_preview_cap)fail("City back fade fixture failed.");
                }
                for(unsigned i=0;i<8;i++)if(i!=4&&i!=7&&native_settings[i]!=before[i])fail("City settings ownership failed.");
            }
            memcpy(native_settings,before,8);city_begin();for(unsigned n=0;n<50;n++)city_tick(0);
            if(city_preview||city_preview_cap!=31||city_scroll_x||city_scroll_y)fail("City reentry fixture failed.");
            puts("CITY INPUT: production tick, held fresh edge, locked/unlocked scroll, confirm/back fade, settings ownership and reentry passed; physical Xbox not verified.");
        }
    }
    if(profile_preview>=0)load_profile_preview();
    if(driver_preview>=0&&!startup_mode) {
        if((driver_interactive?image_path!=NULL:!image_path)||drive_mode||profile_preview>=0||profile_interactive||name_interactive||results_preview||name_ring_preview>=0||game_over_preview>=0||pacing_frames)fail("Driver screen requires standalone window or snapshot.");
        load_driver_preview();
        if(driver_interactive||driver_input_check) {
            taxi_driver_flow_begin(&driver_flow);memset(&driver_input,0,sizeof driver_input);
            driver_scroll_x=16;driver_scroll_y=0;driver_cap_upload(0);
        }
        if(driver_input_check) {
            for(unsigned n=0;n<50;n++)driver_tick(0);
            if(driver_flow.fade.active_177b||driver_scroll_x)fail("Driver entry/scroll fixture failed.");
            for(unsigned d=1;d<4;d++) {
                driver_tick(16);for(unsigned n=0;n<100;n++)driver_tick(16);
                if(driver_preview!=(int)d)fail("Driver held-right fresh-bank fixture failed.");
                driver_tick(0);driver_tick(0);
            }
            driver_tick(16);driver_tick(0);driver_tick(0);
            if(driver_preview!=3)fail("Driver upper-bound fixture failed.");
            driver_tick(48);driver_tick(0);driver_tick(0);
            if(driver_preview!=2)fail("Driver simultaneous-left priority fixture failed.");
            driver_tick(1);for(unsigned n=0;n<40;n++)driver_tick(0);
            if(!driver_flow.complete||driver_flow.choice.exiting!=1||driver_preview_cap)fail("Driver confirm fade fixture failed.");
            taxi_driver_flow_begin(&driver_flow);memset(&driver_input,0,sizeof driver_input);
            for(unsigned n=0;n<50;n++)driver_tick(0);
            driver_tick(2);for(unsigned n=0;n<40;n++)driver_tick(0);
            if(!driver_flow.complete||driver_flow.choice.exiting!=2)fail("Driver back fade fixture failed.");
            taxi_driver_flow_begin(&driver_flow);memset(&driver_input,0,sizeof driver_input);
            for(unsigned n=0;n<50;n++)driver_tick(0);
            if(driver_preview||driver_preview_cap!=31||driver_scroll_x)fail("Driver reentry fixture failed.");
            printf("DRIVER INPUT: production tick, held edge, bounds, priority, confirm/back fade and reentry passed.\n");
        }
    }
    if(loading_preview>=0)load_loading_preview();
    if(normal_menu_preview>=0)load_normal_menu_preview();
    if(challenge_grid_preview>=0){
        for(unsigned selection=0;selection<11;selection++)challenge_grid_update(selection);
        challenge_grid_update((unsigned)challenge_grid_preview);
        puts("CHALLENGE GRID GPU:11 selection states byte-exact upload; static grid only, detail/footer/live flow pending.");
    }
    if(challenge_briefing_preview>=0){
        for(unsigned page=0;page<27;page++)challenge_briefing_update(page);
        challenge_briefing_update((unsigned)challenge_briefing_preview);
        puts("CHALLENGE BRIEFING GPU:27 original-asset composed pages byte-exact upload; snapshot only, live challenge flow pending.");
    }
    if(profile_input_check) {
        taxi_profile_flow_begin(&profile_flow,0,0,0,0,0);profile_cap(0);
        for(unsigned i=0;i<34;i++)profile_tick(0);
        if(profile_flow.fade.active_177b||profile_uploaded_cap!=31||profile_source_count!=1571)fail("Profile fade-in check failed.");
        profile_tick(1);profile_tick(0);
        if(!profile_flow.choice.editing)fail("Profile confirm input check failed.");
        profile_tick(1);profile_tick(0);
        if(profile_flow.choice.exiting||!profile_flow.choice.editing)fail("Profile duplicate guard failed.");
        profile_tick(64);profile_tick(0);
        if(profile_names[0][0]!='B')fail("Profile character repeat check failed.");
        profile_tick(16);profile_tick(0);
        if(profile_flow.choice.edit.column!=1)fail("Profile column input check failed.");
        profile_tick(2);profile_tick(0);
        if(profile_flow.choice.editing||memcmp(profile_names[0],"AAA",3)||profile_flow.choice.choice!=1)fail("Profile cancel input check failed.");
        profile_tick(1);profile_tick(0);profile_tick(64);profile_tick(0);
        profile_tick(1);profile_tick(0);
        if(profile_flow.choice.exiting!=1||!profile_flow.choice.new_profile)fail("Profile register input check failed.");
        for(unsigned i=0;i<34;i++)profile_tick(0);
        if(!profile_flow.complete||profile_uploaded_cap!=0)fail("Profile fade-out completion check failed.");
        taxi_profile_flow_begin(&profile_flow,0,0,0,0,0);memset(&profile_input,0,sizeof profile_input);
        memset(profile_names,'A',sizeof profile_names);
        for(unsigned i=0;i<34;i++)profile_tick(0);
        if(profile_uploaded_cap!=31)fail("Profile restored cap check failed.");
        for(unsigned i=0;i<profile_source_count;i++) {
            Texture *t=profile_sources+i;size_t n=(size_t)t->width*t->height*4;
            uint8_t *pixels=malloc(n);if(!pixels)fail("Profile GPU check allocation failed.");
            glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
            if(memcmp(pixels,t->pixels,n))fail("Profile GPU restoration failed.");free(pixels);
        }
        profile_tick(128);profile_tick(0);profile_tick(1);profile_tick(0);
        if(profile_flow.choice.exiting!=2)fail("Profile skip input check failed.");
        for(unsigned i=0;i<34;i++)profile_tick(0);
        if(!profile_flow.complete||profile_uploaded_cap!=0)fail("Profile skip fade failed.");
        taxi_profile_flow_begin(&profile_flow,0,0,0,0,0);memset(&profile_input,0,sizeof profile_input);
        for(unsigned i=0;i<34;i++)profile_tick(0);
        profile_tick(1);profile_tick(0);profile_tick(64);profile_tick(0);
        puts("PROFILE INPUT CHECK: production tick confirm/duplicate/edit/column/cancel/register/skip/fade/reset passed;1571 GPU textures restored byte-exact. Physical controller/settings commit pending.");
    }
    if(profile_interactive) {
        taxi_profile_flow_begin(&profile_flow,profile_preview&1,profile_preview&2,0,0,0);profile_cap(0);
        SetWindowTextW(window,L"Native profile diagnostic | Arrows edit/select, Z/Enter confirm, X back | Xbox D-pad, A/B | Settings commit pending");
    }
    if(results_preview)load_results_preview();
    if(game_over_preview>=0) {
        game_over_begin_flow(0,0);for(int i=0;i<game_over_preview;i++)game_over_tick_flow();
        printf("GAME OVER FIXTURE: frame%d glyph%u scale%d cap%d complete%u. Source text/assets/timing; GL affine approximation.\n",game_over_preview,game_over_state.glyph,game_over_state.scale,game_over_cap,game_over_complete);
    }
    load_scene_graphics();
    if(drive_mode) {
        load_drive_graphics();
        if(bound_failure_check||turn_failure_check||flag_failure_check||ball_failure_check||drift_failure_check){memcpy(native_settings+208*native_current_profile+8,"AAA",3);native_settings[208*native_current_profile+11]=1;}
        if(challenge_jump_retry_check==3&&!challenge_jump_new_profile_check){memcpy(native_settings+208*native_current_profile+8,"AAA",3);native_settings[208*native_current_profile+11]=1;}
        if(challenge_jump_cross_profile_check){memcpy(native_settings+216,"BBB",3);native_settings[219]=1;}
        if(challenge_jump_unranked_check){
            for(unsigned i=0;i<3;i++){
                uint32_t packed=((300u-50u*i)<<15)|1u;
                for(unsigned j=0;j<4;j++)native_settings[108+208u*native_current_profile+4*i+j]=(uint8_t)(packed>>(8*j));
            }
            memcpy(session_settings_snapshot+108+208u*native_current_profile,native_settings+108+208u*native_current_profile,12);
        }
        if(challenge_result_audio_check){
            char directory[MAX_PATH];GetModuleFileNameA(NULL,directory,MAX_PATH);char *tail=strrchr(directory,92);if(!tail)fail("Challenge audio path failed.");
            snprintf(tail+1,MAX_PATH-(size_t)(tail+1-directory),"../assets/audio");
            if(taxi_audio_asset_load(&audio_bank,directory))fail("Challenge audio assets failed.");
            audio_live=1;audio_global_volume=64;audio_settings_state.effects_enabled=(uint8_t)(challenge_result_audio_check<4||challenge_result_audio_check==8);
            if(challenge_result_audio_check>=6){
                snprintf(tail+1,MAX_PATH-(size_t)(tail+1-directory),"../assets/music/original-module.bin");
                FILE *f=fopen(directory,"rb");music_bytes=malloc(181241);
                if(!f||!music_bytes||fread(music_bytes,1,181241,f)!=181241||fgetc(f)!=EOF)fail("Challenge music module failed.");fclose(f);
                if(taxi_music_asset_index(&music_asset,music_bytes,181241))fail("Challenge music index failed.");
                snprintf(tail+1,MAX_PATH-(size_t)(tail+1-directory),"../assets/music/pitch-table.bin");f=fopen(directory,"rb");
                if(!f||fread(music_pitch,1,1536,f)!=1536||fgetc(f)!=EOF)fail("Challenge pitch table failed.");fclose(f);
                audio_settings_state.music_enabled=1;passenger_scene.random_state=0; /* explicit fresh diagnostic stream */
                reset_drive();
            }
        }
        FILE *challenge_camera_report=NULL;
        if(challenge_camera_trace){challenge_camera_report=fopen("outputs/crazy-taxi-native/reports/challenge-camera-motion.csv","wb");if(!challenge_camera_report)fail("Challenge camera trace failed.");fprintf(challenge_camera_report,"tick,x,y,z,dx,dz,heading,camera_yaw,frame,eye_x,eye_y,eye_z,away_dot,view_dx,view_dz,depth,entry_speed,speed,collided,queries,normal_x,normal_y,normal_z,corrected_x,corrected_z,stick,analog_command,camera_height,camera_distance,camera_zoom,camera_base,body_x,body_y,body_depth,body_scale,presentation_x,presentation_y,presentation_depth,presentation_scale\n");}
        FILE *flag_route_report=NULL;
        if(flag_input_replay_path){char name[MAX_PATH];int n=snprintf(name,sizeof name,"%s.trace.csv",flag_input_replay_path);if(n<0||(size_t)n>=sizeof name)fail("Flag replay trace path too long.");flag_route_report=fopen(name,"wb");if(!flag_route_report)fail("Flag replay trace failed.");fprintf(flag_route_report,"tick,step,input,x,y,z,heading,speed,gear,phase,remaining,caught,ended,queries,collided,motion_heading,ground\n");}
        int flag_contact_fixture_placed=0,flag_contact_result_captured=0;unsigned ball_contact_placements=0;
        for(int i=0;i<script_ticks&&drive_mode;i++){
            if(ball_contact_flow_check&&!live_game_start.active&&!drive_context.height.flag_153c){
                for(unsigned a=0;a<20;a++)if((live_balls[a].bytes[0]&1)&&!live_balls[a].bytes[10]){
                    /* Explicit original20actor contact/result plumbing fixture:
                       relocate taxi to current source actor, NOT earned driving. */
                    memcpy(&car.world.height.position,live_balls[a].bytes+16,12);car.world.height.position.y+=64;previous_position=car.world.height.position;
                    ball_contact_placements++;break;
                }
            }
            if(flag_contact_flow_check&&!flag_contact_fixture_placed&&!live_game_start.active){
                /* Explicit contact plumbing fixture, not a driven capture. */
                memcpy(&car.world.height.position,live_flag_actor.bytes+16,12);
                car.world.height.position.y+=64;previous_position=car.world.height.position;
                flag_contact_fixture_placed=1;
                puts("FLAG CONTACT FIXTURE: taxi placed at source actor after intro; not earned gameplay.");
            }
            TaxiPoint camera_trace_before=car.world.height.position;
            int32_t camera_trace_entry_speed=car.world.speed_544;
            TaxiXboxSample trace_pad=challenge_analog_trace?taxi_xbox_decode(0,challenge_trace_stick,0,255,0):(TaxiXboxSample){0};
            drive_tick(trace_pad,1);
            if(challenge_ball_drive_preview&&i%60==59){
                TaxiPassengerState retained[20];memcpy(retained,live_balls,sizeof retained);
                TaxiChallengeBall state_before=live_ball_state;draw(width,height);
                if(memcmp(retained,live_balls,sizeof retained)||memcmp(&state_before,&live_ball_state,sizeof state_before))fail("Ball presentation mutated simulation/animation.");
            }
            if(flag_route_report)fprintf(flag_route_report,"%d,%u,%u,%d,%d,%d,%d,%d,%d,%u,%d,%d,%u,%u,%d,%d,%d\n",i,flag_input_index,flag_input_mask,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.heading_1c,car.world.speed_544,car.driver.power.gears.gear,car.driver.motion.motion.orientation.phase_e6a,live_flag_timer.remaining_1552,live_flag_timer.caught_1550,drive_context.height.flag_1697,route_probe_movement.queries,route_probe_movement.collided,car.world.heading_20,car.world.ground_36c);
            if((flag_contact_flow_check||flag_earned_capture_check)&&!flag_contact_result_captured&&challenge_jump_success.phase16bc&&challenge_jump_success.hold16be>200&&city_fade_cap==31){
                char capture[MAX_PATH];int n=snprintf(capture,sizeof capture,flag_earned_capture_check?"%s.earned-result.bmp":"%s.contact-result.bmp",image_path);
                if(n<0||(size_t)n>=sizeof capture)fail("Flag contact result capture path too long.");
                draw(3840,2160);glFinish();snapshot(capture,3840,2160);flag_contact_result_captured=1;
            }
            if(challenge_camera_report){
                TaxiPoint now=car.world.height.position;TaxiCameraView view=taxi_camera_normal_view(now,camera_pitch,camera_heading,camera_yaw_offset,&camera_offsets,camera_base,&runtime.trig);
                int32_t dx=now.x-camera_trace_before.x,dz=now.z-camera_trace_before.z;TaxiPoint move=taxi_rotate_camera((TaxiPoint){dx,0,dz},-view.pitch,-view.yaw,0,&runtime.trig);TaxiPoint projected;
                TaxiPoint body=now;body.y-=192;
                if(!taxi_camera_project(&projected,body,view.position,view.pitch,view.yaw,view.roll,192,&runtime.trig))fail("Challenge camera taxi projection failed.");
                TaxiCameraOffsets presented_offsets=presentation_offsets(camera_offsets);
                TaxiCameraView shown=taxi_camera_normal_view(now,camera_pitch,camera_heading,camera_yaw_offset,&presented_offsets,camera_base,&runtime.trig);
                TaxiPoint shown_body;
                if(!taxi_camera_project(&shown_body,body,shown.position,shown.pitch,shown.yaw,shown.roll,192,&runtime.trig))fail("Challenge presentation projection failed.");
                int32_t raw_denominator=camera_offsets.distance_770/4096-camera_base;
                int32_t shown_denominator=presented_offsets.distance_770/4096-camera_base;
                int32_t raw_scale_value=raw_denominator?192*2048/raw_denominator:0;
                int32_t shown_scale_value=shown_denominator?192*2048/shown_denominator:0;
                if(raw_scale_value==256)raw_scale_value=257;
                if(shown_scale_value==256)shown_scale_value=257;
                int64_t dot=(int64_t)dx*(now.x-view.position.x)+(int64_t)dz*(now.z-view.position.z);
                fprintf(challenge_camera_report,"%d,%d,%d,%d,%d,%d,%d,%d,%u,%d,%d,%d,%lld,%d,%d,%d,%d,%d,%d,%u,%d,%d,%d,%d,%d,%d,%d",i,now.x,now.y,now.z,dx,dz,car.world.heading_1c,view.yaw,taxi_sprite_frame(view.yaw,car.world.heading_1c),view.position.x,view.position.y,view.position.z,(long long)dot,move.x,move.z,projected.z,camera_trace_entry_speed,car.world.speed_544,route_probe_movement.collided,route_probe_movement.queries,route_probe_movement.normal.x,route_probe_movement.normal.y,route_probe_movement.normal.z,route_probe_movement.position.x,route_probe_movement.position.z,challenge_analog_trace?challenge_trace_stick:0,challenge_analog_trace?taxi_xbox_steering_command(challenge_trace_stick):0);
                fprintf(challenge_camera_report,",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",camera_offsets.height_76c,camera_offsets.distance_770,camera_offsets.zoom_2628,camera_base,projected.x,projected.y,projected.z,raw_scale_value,shown_body.x,shown_body.y,shown_body.z,shown_scale_value);
            }
            if(challenge_entry_fade_check){
                if(i<33&&(live_game_start.phase!=-1||live_game_start.axes[0]!=1024||live_game_start.axes[2]!=1024))fail("Challenge intro advanced during entry fade.");
                if(i==32&&(city_fade.active_177b||city_fade.progress_178a!=31||city_fade.endpoint_count_1774!=2))fail("Challenge33tick fade boundary failed.");
                if(i==201&&!live_game_start.active)fail("Challenge start ended before203ticks.");
                if(i==202&&live_game_start.active)fail("Challenge start did not complete at203ticks.");
            }
            if(challenge_result_audio_check){
                int16_t mixed[352]={0};
                if(challenge_result_audio_check>=6){
                    if(taxi_music_clock_gate(&music_divider,1,(uint8_t)music_loaded,music_tracker.state.playing)){if(!taxi_music_tracker_tick(&music_tracker,&music_asset,music_pitch,21024,&music_context))fail("Challenge music dispatch failed.");music_dispatches++;}
                    for(unsigned voice=0;voice<music_tracker.state.state[2];voice++)if(taxi_music_voice_mix(music_tracker.voice+voice,&music_asset,mixed,352,32,0,256))fail("Challenge music mix failed.");
                    for(unsigned sample=0;sample<352;sample++)if(mixed[sample])music_nonzero_samples++;
                }
                for(unsigned c=0;c<4;c++)if(taxi_audio_channel_mix(audio_channels+c,mixed,352,audio_bank.codebook,608,taxi_audio_effective_volume(audio_global_volume,audio_channels[c].volume))<0)fail("Challenge native voice mix failed.");
                for(unsigned sample=0;sample<352;sample++)if(mixed[sample])audio_nonzero_samples++;
            }
        }
        if(flag_route_report){int bad=ferror(flag_route_report);bad|=fclose(flag_route_report);if(bad)fail("Flag input replay trace write failed.");puts("Native Flag input-only movement trace captured; no original ROM trajectory equivalence claim.");}
        if(challenge_camera_report){if(ferror(challenge_camera_report)||fclose(challenge_camera_report))fail("Challenge camera trace write failed.");puts("Native actual Jump camera motion trace captured; source camera/sprite selection diagnostic, no visual fidelity claim.");}
        if(challenge_entry_fade_check){if(script_ticks!=203||simulation_error||car.world.height.position.x!=-22528||car.world.height.position.z!=0||car.world.height.position.y!=38075)fail("Challenge passive entry placement failed.");puts("PASS native Jump entry:33 fade updates gate intro,170 source start updates,203 total, passive ground settles, X/Z unchanged.");}
        if(challenge_result_audio_check){
            if(challenge_result_audio_check==6||challenge_result_audio_check==7){if(!music_dispatches||!music_nonzero_samples||!music_context.main_mode||music_starts[4]!=1||music_context.context!=&passenger_scene.random_state)fail("Challenge music owner/mix failed.");printf("PASS native challenge music: dispatches%u nonzero samples%u, original selected order, one registration/menu order5 handoff, shared diagnostic stream; no full ROM RNG/audio-host claim.\n",music_dispatches,music_nonzero_samples);}
            if(challenge_result_audio_check==8){
                unsigned selected=music_starts[0]+music_starts[1]+music_starts[2]+music_starts[3];
                if(selected!=2||music_starts[4]||music_context.main_mode||!music_tracker.state.playing||!music_dispatches||!music_nonzero_samples||music_context.context!=&passenger_scene.random_state)fail("Challenge Retry music restart failed.");
                printf("PASS native Retry music: two challenge starts, no menu starts, tracker playing, dispatches%u nonzero%u; no physical controller or audible host claim.\n",music_dispatches,music_nonzero_samples);
            }
            if(audio_rejected||(challenge_result_audio_check<5&&!audio_nonzero_samples))fail("Challenge audio requests/mix rejected.");
            if((challenge_result_audio_check==3||challenge_result_audio_check==8)&&(audio_request_counts[77]!=1||audio_request_counts[70]!=1||audio_request_counts[71]!=1||audio_request_counts[72]||challenge_jump_retries!=1))fail("Challenge retry voice gate failed.");
            if(challenge_result_audio_check==4&&(audio_request_counts[77]||audio_request_counts[70]!=1||audio_request_counts[72]||challenge_failed_voice_busy()))fail("Original failure ungated70/disabled choices failed.");
            if(challenge_result_audio_check==5&&(audio_request_counts[76]||audio_request_counts[68]||audio_request_counts[69]))fail("Original disabled success voice gate failed.");
            if(challenge_result_audio_check==1&&(audio_request_counts[76]!=1||audio_request_counts[68]!=1||audio_request_counts[69]))fail("Challenge success voice requests failed.");
            if(challenge_result_audio_check==2&&(audio_request_counts[77]!=1||audio_request_counts[70]!=1||audio_request_counts[72]!=1||challenge_failed_voice_busy()))fail("Challenge failure voice requests/gate failed.");
            printf("PASS native result voice mixer: case%d nonzero%u failed%u return%u success%u best%u; no audible host playback claim.\n",challenge_result_audio_check,audio_nonzero_samples,audio_request_counts[70],audio_request_counts[72],audio_request_counts[76],audio_request_counts[68]);
        }
        if(challenge_jump_retry_check==3&&!jump_effect_render_check&&!flag_contact_flow_check&&!flag_earned_capture_check&&!ball_contact_flow_check&&!ball_earned_capture_check&&!drift_earned_capture_check&&!turn_earned_capture_check){
            if(!drive_context.height.flag_153c||drive_context.height.distance_1540!=44497||drive_context.height.flag_153d!=(challenge_jump_unranked_check?0:1)||simulation_error){
                fprintf(stderr,"JUMP CHECK DETAILS: success=%u distance=%d expected_distance=44497 best=%u expected_best=%u simulation_error=%d stage=%d driving=%d; effects emissions=%u max_active=%u projected_frames=%u requests=%d.\n",
                    drive_context.height.flag_153c,drive_context.height.distance_1540,drive_context.height.flag_153d,challenge_jump_unranked_check?0u:1u,simulation_error,startup_stage,drive_mode,
                    native_effect_emissions,native_effect_max_active,native_effect_visible_frames,native_effect_request_count);
                if(native_effect_trace)fflush(native_effect_trace);
                fail("Native earned Jump success check failed.");
            }
            if(script_ticks>=1400){if(drive_mode||startup_stage!=15)fail("Jump success registration/grid return failed.");for(unsigned tick=0;tick<33;tick++)startup_tick(0);}
            if(script_ticks>=1400&&challenge_jump_cross_profile_check){
                if(native_current_profile!=1||!profile_commit_done||!profile_persist_requested||memcmp(native_settings+108,session_settings_snapshot+108,12)||memcmp(native_settings+8,"AAA",3)||memcmp(native_settings+216,"BBB",3)||!(native_settings[316]&128)||!(native_settings[316]&1))fail("Earned Jump cross-profile registration failed.");
                puts("PASS actual earned Jump profile0 landing -> select profile1 -> snapshot/recalculate ownership profile1, provisional profile0 record removed, both names retained.");
            }
            if(script_ticks>=1400&&challenge_jump_unranked_check){
                if(!profile_commit_done||!profile_persist_requested||memcmp(native_settings+108+208u*native_current_profile,session_settings_snapshot+108+208u*native_current_profile,12))fail("Successful unranked Jump registration failed.");
                puts("PASS actual successful Jump with rank0 still enters profile registration; prior top3 records unchanged.");
            }
            if(script_ticks>=1400&&challenge_jump_new_profile_check){
                if(session_settings_snapshot[11]||session_settings_snapshot[219]||!profile_commit_done||!profile_persist_requested||!native_settings[11]||memcmp(native_settings+8,"BAA",3))fail("Earned Jump empty-profile registration failed.");
                puts("PASS earned Jump empty-profile registration: original name editor BAA, valid profile created after acceptance, record committed.");
            }
            if(script_ticks>=1400&&challenge_jump_discard_check){
                if(!profile_commit_done||profile_persist_requested||memcmp(native_settings,session_settings_snapshot,512))fail("Earned Jump discard snapshot restoration failed.");
                puts("PASS earned Jump DontRegister: original512-byte snapshot restored, no persistence request, CrazyBox returned.");
            }
            if(script_ticks>=1400&&native_save_path&&!challenge_jump_discard_check&&!challenge_jump_unranked_check){
                uint8_t restored[512],rows[800]={0};
                if(!profile_commit_done||!profile_persist_requested||native_save_write_status||
                   taxi_save_file_load(native_save_path,restored,rows)||memcmp(restored,native_settings,512))
                    fail("Earned Jump registration disk roundtrip failed.");
                const uint8_t *record=restored+108+208u*native_current_profile;
                uint32_t packed=(uint32_t)record[0]|(uint32_t)record[1]<<8|(uint32_t)record[2]<<16|(uint32_t)record[3]<<24;
                if(!(packed&1)||(packed>>15)!=173||((packed>>8)&127)!=81||
                   !restored[11+208u*native_current_profile]||memcmp(restored+8+208u*native_current_profile,challenge_jump_cross_profile_check?"BBB":challenge_jump_new_profile_check?"BAA":"AAA",3))
                    fail("Earned Jump saved record/name ownership failed.");
                puts("PASS earned Jump registration disk reload: exact512 bytes, actual173.81m record, selected diagnostic name/valid profile. No physical Xbox claim.");
            }
            printf("PASS earned native Jump success distance%d rank%u hold%d outer%u; original success display/timed return, profile name fixture AAA.\n",drive_context.height.distance_1540,drive_context.height.flag_153d,challenge_jump_success.hold16be,challenge_jump_success.outer1ea8);
        }
        if(challenge_jump_retry_check==2){
            if(drive_mode||startup_stage!=15||simulation_error)fail("Native Jump return handoff failed.");
            for(unsigned tick=0;tick<33;tick++)startup_tick(0);
            if(challenge_live_flow.fade.active_177b||challenge_live_flow.finished)fail("Native challenge returned grid did not settle.");
            if(bound_failure_check&&(challenge_live_selection!=7||challenge_bound_drive_preview||passenger_diagnostic))fail("Bound retained grid selection/owner release failed.");
            if(turn_failure_check&&(challenge_live_selection!=6||challenge_turn_drive_preview||passenger_diagnostic))fail("Turn retained grid selection/owner release failed.");
            if(drift_failure_check&&(challenge_live_selection!=5||challenge_drift_drive_preview))fail("Drift retained grid selection/owner release failed.");
            if(ball_failure_check&&(challenge_live_selection!=4||challenge_ball_drive_preview))fail("Ball retained grid selection/owner release failed.");
            if(flag_failure_check&&(challenge_live_selection!=3||challenge_flag_drive_preview))fail("Flag retained grid selection/owner release failed.");
            printf("PASS native %s failure/No confirmation/exit fade/Crazy Box grid settled; selection%u profile%u.\n",bound_failure_check?"Bound source height":turn_failure_check?"Turn timeout":drift_failure_check?"Drift timeout":flag_failure_check?"Flag timeout":"Jump landing",challenge_live_selection,native_current_profile);
        }
        if(challenge_jump_retry_check==1){
            if(bound_retry_check)printf("BOUND RETRY TRACE: retries%u ended%u intro%u remaining%d position%d,%d,%d failurephase%d hold%d outer%u fade%u simulation%d\n",challenge_jump_retries,drive_context.height.flag_1697,live_game_start.active,live_bound_state.remaining_1660,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,challenge_jump_failure.phase16bc,challenge_jump_failure.hold16be,challenge_jump_failure.outer1ea8,city_fade.active_177b,simulation_error);
            if(challenge_jump_retries!=1||drive_context.height.flag_1697||simulation_error)fail("Native Jump failure/retry check failed.");
            printf("PASS Jump native failure choice/33tick exit fade/retry vehicle reset: retries%u ended%u startactive%u. Silent diagnostic, physical controller not verified.\n",challenge_jump_retries,drive_context.height.flag_1697,live_game_start.active);
        }
        if(attract_session_check&&(attract_session.active||simulation_error||attract_probe_settings[4]!=attract_session.saved_city||attract_exit_fade.active_177b))
            fail("Attract session diagnostic did not finish through native driving and fade.");
        if(pickup_preview) {
            unsigned index=passenger_scene.count;uint64_t nearest=UINT64_MAX;
            for(unsigned i=0;i<passenger_scene.count;i++) if(challenge_turn_drive_preview?((passenger_scene.customers[i].bytes[0]&3)==1&&!(passenger_scene.customers[i].bytes[0]&64)):passenger_scene.customers[i].bytes[0]==1) {
                int64_t dx=(int64_t)passenger_scene.spawns[i].x-car.world.height.position.x;
                int64_t dz=(int64_t)passenger_scene.spawns[i].z-car.world.height.position.z;
                uint64_t d=(uint64_t)(dx*dx+dz*dz);if(d<nearest){nearest=d;index=i;}
            }
            if(index==passenger_scene.count) fail("No eligible preview customer.");
            memcpy(&car.world.height.position,passenger_scene.customers[index].bytes+16,12);
            TaxiPassengerFrameInput input={.taxi=car.world.height.position,.delta=1,.speed=0,
                .camera_word_10=taxi_camera_transform_yaw(camera_heading,camera_yaw_offset),.delivery_gate=1};
            TaxiPassengerFrameEffects effect;
            if(challenge_turn_drive_preview){
                /* Explicit parked actor fixture: tests source boarding and
                   live destination rendering; never an earned drive claim. */
                uint8_t rank=0;unsigned ticks=0;
                while(!passenger_scene.session.trip.active_1694&&ticks++<100){
                    if(taxi_passenger_scene_turn_frame_observed(&passenger_scene,&passenger_asset,&world,&runtime.trig,&input,&effect,&rank,native_destination_selected,NULL))fail("Turn parked boarding fixture failed.");
                }
                if(!passenger_scene.session.trip.active_1694||passenger_scene.pickup.destination_index!=0||rank||!native_panel_generation||guidance_route.count!=38)fail("Turn parked destination ownership failed.");
                previous_position=car.world.height.position;
                for(unsigned i=0;i<20;i++)drive_tick((TaxiXboxSample){0},1);
                if(simulation_error||!passenger_scene.session.trip.active_1694||guidance_angle<0)fail("Turn live active guidance fixture failed.");
                printf("TURN PARKED ACTIVE FIXTURE: source boarding%u, destination0/panel generation%u/38 route points/current frame%u/target frame%u; actual20 native driving callbacks; taxi relocated, NOT earned pickup.\n",ticks,native_panel_generation,turn_guidance_frame,((unsigned)guidance_angle>>7)&31);
            }else for(unsigned i=0;i<4;i++) {
                if(taxi_passenger_scene_frame(&passenger_scene,&passenger_asset,&world,&runtime.trig,&input,&effect)) fail("Pickup preview failed.");
                taxi_cash_glyphs_targets(&cash_glyphs,&passenger_scene.session.fare);
                taxi_cash_glyphs_step(&cash_glyphs);
            }
            int32_t selected=passenger_scene.pickup.destination_index;
            TaxiPoint camera=taxi_camera_position(input.taxi,camera_pitch,camera_heading,468*4096,3072*4096,0,&runtime.trig);
            if(selected<0||taxi_guidance_route_angle(guidance_route.segments,guidance_route.count,selected,
                passenger_asset.destinations[selected],input.taxi,camera,input.camera_word_10,&runtime.trig,&runtime.traction,&guidance_angle)) fail("Guidance preview failed.");
            printf("PARKED PICKUP FIXTURE: native boarding completed; teleported taxi to original customer; not a driven fare.\n");
        }
        previous_position=car.world.height.position;
        if(expiry_preview>=0) {
            passenger_scene.session.clock.remaining_16a8=1;
            passenger_scene.session.gate_1700=0;
            TaxiPassengerFrameInput input={.taxi=car.world.height.position,.delta=1,.speed=1,.delivery_gate=1};
            TaxiPassengerFrameEffects effect;unsigned fades=0;
            for(int i=0;i<=expiry_preview;i++) {
                if(taxi_passenger_scene_frame(&passenger_scene,&passenger_asset,&world,&runtime.trig,&input,&effect))fail("Expiry fixture failed.");
                fades+=effect.expiry_fade;
                city_fade_tick(effect.expiry_fade,1);
            }
            printf("EXPIRY FIXTURE: forced clock1; %d animation ticks, scale%d, hold%d, outerstate%u, fade requests%u. Not a naturally timed run; palette fade/results pending.\n",
                expiry_preview,passenger_scene.session.expiry.scale_16c0,passenger_scene.session.expiry.hold_16be,passenger_scene.session.outer_state_1ea8,fades);
            if(expiry_results>=0&&endgame_diagnostic) {
                for(int i=0;i<=expiry_results;i++)result_live_tick((TaxiXboxSample){0},1);
                if(!result_live_active)fail("Live timeout transition did not initialize results.");
                printf("LIVE RESULTS FIXTURE: frame%d phase%u cap%d complete%u; forced timeout and ordinary live controller.\n",expiry_results,result_state.phase,results_palette_cap,result_live_flow.complete);
            } else if(expiry_results>=0) {
                TaxiFareSession *session=&passenger_scene.session;
                if(session->outer_state_1ea8!=2||city_fade.active_177b)fail("Expiry results attempted before original timeout fade completion.");
                load_results_preview();
                result_customers=session->end.completed_1692;result_rank=session->rank_16d4;result_class=session->fare.rating_1693;
                if(result_class>8||result_rank>99)fail("Results source class/rank outside artwork bounds.");
                if(taxi_results_state_begin_money(&result_state,session->fare.total_whole_1698,session->fare.total_fraction_169c))fail("Session cash cannot initialize original results animation.");
                for(int i=0;i<expiry_results;i++)taxi_results_state_step(&result_state,1,(int8_t)result_customers,(int8_t)result_rank,0);
                results_preview=1;
                printf("CONNECTED EXPIRY RESULTS DIAGNOSTIC: original timeout fade completed; actual session cash%d.%02d customers%u class%u rank%u, tick%d phase%u. Axel explicit; results fade-in and subsequent menus pending.\n",session->fare.total_whole_1698,session->fare.total_fraction_169c,result_customers,result_class,result_rank,expiry_results,result_state.phase);
            }
            if(expiry_restore) {
                draw(3840,2160);glFinish(); /* Upload faded copies first. */
                reset_drive();draw(3840,2160);glFinish();
                for(unsigned i=0;i<texture_count+sprite_source_count;i++) {
                    Texture *t=i<texture_count?textures+i:sprite_sources+i-texture_count;
                    size_t bytes=(size_t)t->width*t->height*4;
                    unsigned char *pixels=malloc(bytes);if(!pixels)fail("Restore verification allocation failed.");
                    glBindTexture(GL_TEXTURE_2D,t->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
                    if(memcmp(pixels,t->pixels,bytes))fail("Reset failed to restore original texture pixels.");
                    free(pixels);
                }
                if(glGetError()!=GL_NO_ERROR)fail("Reset texture verification GL error.");
                printf("RESTORE FIXTURE PASS: %u city and %u sprite GPU textures equal retained original pixels after same-process reset.\n",texture_count,sprite_source_count);
            }
        }
        if(capture_orbit_set) camera_orbit=capture_orbit;
        if(challenge_drift_drive_preview){
            if(world.nv!=336||world.np!=258||drive_context.driver.motion.orientation.submode!=3||live_drift_option3!=4||simulation_error)fail("Drift livecourse owner failed.");
            if(script_ticks>=2100&&!drift_retry_check&&!drift_raw_replay_path&&(live_drift_state.remaining_164e||!drive_context.height.flag_1697||drive_context.height.flag_153c||live_drift_warnings!=1))fail("Drift forward timeout failed.");
            if(drift_raw_replay_path){
                if(live_drift_state.remaining_164e||!drive_context.height.flag_1697||!drive_context.height.flag_153c||drive_context.driver.motion.orientation.counter_164c!=83||drive_context.height.flag_153d!=1||car.world.height.position.x!=(drift_xbox_replay?15671:15690)||car.world.height.position.y!=-239||car.world.height.position.z!=(drift_xbox_replay?-18116:-17909)||drift_raw_index!=1799)fail("Drift earned raw route differs from original native headless result.");
                if(drift_xbox_replay)puts("PASS earned native Drift synthetic Xbox replay:83 combos/original expiry/rank1/position15671,-239,-18116; decoder+human steering adapter/trigger throttle/original gear actions; no state injection or physical controller claim.");
                else puts("PASS earned native Drift raw-button replay:83 combos, original1800tick expiry/rank1/position15690,-239,-17909;1799 inputs consumed, no state injection. Registration/save/Xbox/GBA fidelity pending.");
            }
            printf("PASS Drift live native course/controller: %d ticks remaining%d count%u ended%u success%u warnings%u; no relocation; Original HUD/rank/results/menu wired; earned success verification pending.\n",script_ticks,live_drift_state.remaining_164e,drive_context.driver.motion.orientation.counter_164c,drive_context.height.flag_1697,drive_context.height.flag_153c,live_drift_warnings);
        }
        if(turn_earned_capture_check){
            if(drive_mode||startup_stage!=15||challenge_live_selection!=6||challenge_turn_drive_preview||profile_challenge_owner||simulation_error||!profile_commit_done||!profile_persist_requested||turn_input_index!=2092||passenger_scene.session.end.completed_1692!=1)fail("Earned Turn profile/grid ownership failed.");
            for(unsigned tick=0;tick<33;tick++)startup_tick(0);
            uint32_t packed=0;memcpy(&packed,native_settings+156+208u*native_current_profile,4);
            if(!(packed&1)||(packed>>15)!=2092||memcmp(native_settings+108+208u*native_current_profile,session_settings_snapshot+108+208u*native_current_profile,48)||memcmp(native_settings+8+208u*native_current_profile,"AAA",3)||!native_settings[11+208u*native_current_profile])fail("Earned Turn own record/name/other courses retention failed.");
            if(native_save_path){uint8_t restored[512],rows[800];if(native_save_write_status||taxi_save_file_load(native_save_path,restored,rows)||memcmp(restored,native_settings,512))fail("Earned Turn save readback failed.");}
            puts("PASS earned native Turn input-only delivery -> original timed success/profile/grid selection6/group156/elapsed2092/save readback. AAA registration inputs separate fixture; physical Xbox/GBA fidelity pending.");
        }
        if(drift_earned_capture_check){
            if(drive_mode||startup_stage!=15||challenge_live_selection!=5||challenge_drift_drive_preview||profile_challenge_owner||simulation_error||!profile_commit_done||!profile_persist_requested||drive_context.driver.motion.orientation.counter_164c!=83||drift_raw_index!=1799)fail("Earned Drift profile/grid ownership failed.");
            for(unsigned tick=0;tick<33;tick++)startup_tick(0);
            uint32_t packed=0;memcpy(&packed,native_settings+144+208u*native_current_profile,4);
            if(!(packed&1)||(packed>>15)!=83||memcmp(native_settings+108+208u*native_current_profile,session_settings_snapshot+108+208u*native_current_profile,36)||memcmp(native_settings+8+208u*native_current_profile,"AAA",3)||!native_settings[11+208u*native_current_profile])fail("Earned Drift own record/name/other courses retention failed.");
            if(native_save_path){uint8_t restored[512],rows[800];if(native_save_write_status||taxi_save_file_load(native_save_path,restored,rows)||memcmp(restored,native_settings,512))fail("Earned Drift save readback failed.");}
            puts("PASS earned native Drift raw-button83 -> original timed success/profile/grid selection5/group144/save readback. AAA registration inputs are a separate fixture; physical Xbox/GBA fidelity pending.");
        }
        if(ball_earned_capture_check){
            if(ball_contact_placements||drive_mode||startup_stage!=15||challenge_live_selection!=4||live_ball_state.popped_1556!=20||!profile_commit_done||!profile_persist_requested||challenge_ball_drive_preview||profile_challenge_owner||simulation_error)fail("Earned Ball result/profile/grid flow failed.");
            for(unsigned n=0;n<33;n++)startup_tick(0);
            uint32_t packed=0;memcpy(&packed,native_settings+132+208u*native_current_profile,4);
            unsigned elapsed=(unsigned)(2700-live_ball_state.remaining_1558);
            if(!(packed&1)||(packed>>15)!=elapsed||elapsed>=2700||ball_input_index+1!=elapsed||memcmp(native_settings+108,session_settings_snapshot+108,24))fail("Earned Ball elapsed/record/other courses retention failed.");
            if(native_save_path){uint8_t readback[512],runtime_rows[800];if(native_save_write_status||taxi_save_file_load(native_save_path,readback,runtime_rows)||memcmp(readback,native_settings,512))fail("Earned Ball save/readback failed.");}
            printf("PASS earned native Ball20 driving: consumed%u inputs, elapsed%u/rank%u, original success/profile/grid1-3/group132/save readback; no taxi placement. Profile AAA input fixture is separate from earned driving; independent GBA fidelity remains unverified.\n",ball_input_index,elapsed,drive_context.height.flag_153d);
        }
        if(ball_contact_flow_check){
            if(drive_mode||startup_stage!=15||challenge_live_selection!=4||!ball_contact_placements||live_ball_state.popped_1556!=20||!profile_commit_done||!profile_persist_requested||challenge_ball_drive_preview||profile_challenge_owner)fail("Ball controlledcontact/result/profile/grid flow failed.");
            for(unsigned n=0;n<33;n++)startup_tick(0);
            uint32_t packed=0;memcpy(&packed,native_settings+132+208u*native_current_profile,4);
            if(!(packed&1)||(packed>>15)!=(uint32_t)(2700-live_ball_state.remaining_1558)||memcmp(native_settings+108,session_settings_snapshot+108,24))fail("Ball own time record/other courses retention failed.");
            if(native_save_path){uint8_t readback[512],runtime_rows[800];if(native_save_write_status||taxi_save_file_load(native_save_path,readback,runtime_rows)||memcmp(readback,native_settings,512))fail("Ball fixture save/readback failed.");}
            printf("PASS Ball controlled20actor contact fixture: %u taxi placements, elapsed%d/rank%u, originalsuccess hold/profile/grid1-3/group132 retention/save readback; NOT earned-all20 driving.\n",ball_contact_placements,2700-live_ball_state.remaining_1558,drive_context.height.flag_153d);
        }
        if(flag_contact_flow_check){
            const uint8_t *record=native_settings+120+208u*native_current_profile;
            uint32_t packed=(uint32_t)record[0]|(uint32_t)record[1]<<8|(uint32_t)record[2]<<16|(uint32_t)record[3]<<24;
            if(!flag_contact_fixture_placed||!flag_contact_result_captured||!live_flag_timer.caught_1550||live_flag_timer.remaining_1552!=1199||drive_mode||startup_stage!=15||challenge_live_selection!=3||!profile_commit_done||!profile_persist_requested||!(packed&1)||(packed>>15)!=1||memcmp(native_settings+108,session_settings_snapshot+108,12))fail("Flag contact/profile flow fixture failed.");
            for(unsigned tick=0;tick<33;tick++)startup_tick(0);
            if(challenge_live_flow.fade.active_177b||challenge_live_selection!=3||profile_challenge_owner||challenge_flag_drive_preview)fail("Flag contact grid did not settle/release owners.");
            if(native_save_path){
                uint8_t restored[512],rows[800];
                if(native_save_write_status||taxi_save_file_load(native_save_path,restored,rows)||memcmp(restored,native_settings,512))fail("Flag contact profile disk reload failed.");
                puts("PASS controlled Flag registration disk reload: exact512 bytes/checksum, elapsed1 record; fixture is not earned capture.");
            }
            printf("PASS Flag controlled contact -> elapsed1/rank%u -> timed success/registration commit/grid selection3; Jump record retained; fixture is not earned capture.\n",drive_context.height.flag_153d);
        }
        if(flag_earned_capture_check){
            const uint8_t *record=native_settings+120+208u*native_current_profile;
            uint32_t packed=(uint32_t)record[0]|(uint32_t)record[1]<<8|(uint32_t)record[2]<<16|(uint32_t)record[3]<<24;
            if(flag_contact_fixture_placed||!flag_contact_result_captured||!live_flag_timer.caught_1550||live_flag_timer.remaining_1552!=256||drive_mode||startup_stage!=15||challenge_live_selection!=3||!profile_commit_done||!profile_persist_requested||!(packed&1)||(packed>>15)!=944||memcmp(native_settings+108,session_settings_snapshot+108,12))fail("Earned Flag capture/profile sequence failed.");
            for(unsigned tick=0;tick<33;tick++)startup_tick(0);
            if(challenge_live_flow.fade.active_177b||challenge_live_selection!=3||profile_challenge_owner||challenge_flag_drive_preview)fail("Earned Flag grid did not settle/release owners.");
            if(native_save_path){uint8_t restored[512],rows[800];if(native_save_write_status||taxi_save_file_load(native_save_path,restored,rows)||memcmp(restored,native_settings,512))fail("Earned Flag disk reload failed.");puts("PASS earned Flag registration disk reload: exact512 bytes/checksum, elapsed944 record; input-only driving.");}
            printf("PASS earned Flag input-only capture at elapsed944/rank%u -> timed success/registration commit/grid selection3; Jump record retained. No original ROM trajectory equivalence claim.\n",drive_context.height.flag_153d);
        }
        if(bound_retry_check){
            if(live_bound_state.remaining_1660!=2400||live_bound_state.failure_latch165e||turn_guidance_frame||live_bound_warnings||live_flag_warning_palette||!live_game_start.active||live_bound_option3!=4||passenger_scene.count!=1||passenger_scene.customers[0].bytes[0]!=1||passenger_scene.pickup.destination_index!=-1||passenger_scene.session.trip.active_1694||passenger_scene.session.end.completed_1692)fail("Bound Retry timer/actor/trip reset failed.");
            puts("PASS native Bound source height failure/Yes/fade/Retry:2400timer, originalactor, latch/trip/destination/warning reset, intro active; scripted confirmation, not earned delivery.");
        }
        if(turn_retry_check){
            if(live_turn_state.remaining_165c!=2100||turn_guidance_frame||live_turn_warnings||live_flag_warning_palette||!live_game_start.active||live_turn_option3!=4||passenger_scene.count!=1||passenger_scene.customers[0].bytes[0]!=1||passenger_scene.pickup.destination_index!=-1||passenger_scene.session.trip.active_1694||passenger_scene.session.end.completed_1692)fail("Turn Retry timer/actor/trip reset failed.");
            puts("PASS native Turn timeout/Yes/fade/Retry:2100 timer, one original actor, cleared trip/destination/warning, intro active; scripted confirmation, not earned delivery.");
        }
        if(drift_retry_check){if(live_drift_state.remaining_164e!=1800||live_flag_warning_palette||!live_game_start.active||drive_context.driver.motion.orientation.counter_164c||live_drift_option3!=4)fail("Drift Retry initializer reset failed.");puts("PASS native Drift timeout/Yes/fade/Retry: original1800 countdown, counter0, option4, no warning, intro active. Scripted input, not physical Xbox.");}
        if(ball_retry_check){
            if(live_ball_state.remaining_1558!=2700||live_ball_state.popped_1556||live_flag_warning_palette||!live_game_start.active||car.world.height.position.x!=live_challenge_spawn.query.x||car.world.height.position.z!=live_challenge_spawn.query.z)fail("Ball Retry timer/GRID/intro reset failed.");
            for(unsigned i=0;i<20;i++)if(live_balls[i].bytes[0]!=1||live_balls[i].bytes[10]||live_ball_state.velocity_155c[i][0]||live_ball_state.velocity_155c[i][2])fail("Ball Retry actors/impulses not reset.");
            puts("PASS Ball original timeout/Yes/fade/Retry:20 active actors/no pop/zero impulses,2700timer/no warning/GRID vehicle and intro reset. Scripted input, not physical Xbox.");
        }
        if(flag_retry_check&&(live_flag_timer.remaining_1552!=1200||live_flag_timer.caught_1550||live_flag_timer.scale_1554!=256||live_flag_actor.bytes[0]!=1||live_flag_warning_palette||!live_game_start.active||car.world.height.position.x!=live_challenge_spawn.query.x||car.world.height.position.z!=live_challenge_spawn.query.z))fail("Flag Retry timer/actor/vehicle/intro reset failed.");
        if(flag_retry_check)puts("PASS native Flag timeout/Yes/33tick exit fade/Retry resets original actor, timer1200, scale256, warning palette, GRID vehicle and intro; scripted input, not physical Xbox.");
        if((challenge_flag_drive_preview||flag_failure_check)&&!flag_contact_flow_check){
            if(!flag_input_replay_path&&!flag_retry_check&&script_ticks>=1500&&(!drive_context.height.flag_1697||live_flag_timer.remaining_1552||drive_context.height.flag_153c||live_flag_timer.caught_1550))fail("Flag forward timeout integration failed.");
            if(!flag_retry_check&&car.world.height.position.x==live_challenge_spawn.query.x&&car.world.height.position.z==live_challenge_spawn.query.z)fail("Flag diagnostic did not drive.");
            printf("FLAG VIEWER DRIVE: submode%u remaining%d ended%u success%u scale%d actorflags%u; source course/start/controller/native motion; full GBA fidelity unverified.\n",drive_context.driver.motion.orientation.submode,live_flag_timer.remaining_1552,drive_context.height.flag_1697,drive_context.height.flag_153c,live_flag_timer.scale_1554,live_flag_actor.bytes[0]);
        }
        if(simulation_error) fail("Driving simulation failed.");
        printf("Native driving prototype: %d scripted ticks; position %d,%d,%d; speed %d. Camera/size provisional.\n",
            script_ticks,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544);
        if(traffic_preview)printf("TRAFFIC PREVIEW: %u successful spawns, %u ticks with projected submissions; counter%u; source RNG/audio and world occlusion incomplete.\n",
            traffic_spawned,traffic_visible_frames,traffic_counter);
    }
    if(results_cache_check) {
        if(!image_path||!results_loaded)fail("Results cache check requires a loaded results snapshot.");
        int previous_cap=results_palette_cap;
        unsigned char *pixels=malloc(240*160*4);if(!pixels)fail("Results cache verification allocation failed.");
        const int caps[]={0,7,16,31};size_t checked=0;
        for(unsigned cap_index=0;cap_index<4;cap_index++) {
            int cap=caps[cap_index];result_snapshot_cap(cap);
            for(unsigned i=0;i<1529;i++) {
                Texture *t=result_sources+i;glBindTexture(GL_TEXTURE_2D,t->id);
                glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
                for(size_t n=0;n<(size_t)t->width*t->height;n++) {
                    for(unsigned channel=0;channel<4;channel++) {
                        unsigned original=t->pixels[n*4+channel],expected=original;
                        if(channel<3&&((original*31+127)/255)>(unsigned)cap)
                            expected=(unsigned)cap*255/31;
                        if(pixels[n*4+channel]!=expected)fail("Results GPU palette cap/alpha mismatch.");
                    }
                    checked++;
                }
            }
        }
        free(pixels);result_snapshot_cap(previous_cap);
        printf("RESULTS CACHE CHECK PASS: all1529 GPU textures, caps0/7/16/31, %zu pixels; alpha retained and full-cap source restored.\n",checked);
    }
    if (glGetError()!=GL_NO_ERROR) fail("Texture upload failed.");
    printf("Renderer: %s\n",glGetString(GL_RENDERER));
    LARGE_INTEGER frequency,start,end; QueryPerformanceFrequency(&frequency);
    if(pacing_frames) {
        double *intervals=calloc((size_t)pacing_frames,sizeof(*intervals));
        if(!intervals) fail("Pacing allocation failed.");
        QueryPerformanceCounter(&start);LARGE_INTEGER previous=start;
        FramePacer pacer={start.QuadPart,frequency.QuadPart,1,0};
        uint64_t total_ticks=0; double maximum_work_ms=0;
        for(int frame=0;frame<pacing_frames;) {
            QueryPerformanceCounter(&end);if(!frame_due(&pacer,end)) continue;
            double dt=(double)(end.QuadPart-previous.QuadPart)/frequency.QuadPart;
            intervals[frame++]=dt*1000;previous=end;
            uint32_t ticks=0;
            if(taxi_clock_advance(&drive_clock,(uint64_t)(fmin(dt,0.1)*1e9),&ticks)) fail("Pacing clock failed.");
            total_ticks+=ticks;
            for(uint32_t i=0;i<ticks;i++) drive_tick((TaxiXboxSample){0},1);
            if(simulation_error) fail("Paced simulation failed.");
            LARGE_INTEGER simulated,submitted,completed;QueryPerformanceCounter(&simulated);
            draw(width,height);QueryPerformanceCounter(&submitted);
            glFinish();QueryPerformanceCounter(&completed);
            double work_ms=(double)(completed.QuadPart-end.QuadPart)*1000/frequency.QuadPart;
            if(work_ms>maximum_work_ms)maximum_work_ms=work_ms;
            if(work_ms>1000.0/60.0)printf("PACED OVERRUN: frame%d ticks%u simulation%.3f submit%.3f GPUwait%.3f ms.\n",frame,ticks,
                (double)(simulated.QuadPart-end.QuadPart)*1000/frequency.QuadPart,
                (double)(submitted.QuadPart-simulated.QuadPart)*1000/frequency.QuadPart,
                (double)(completed.QuadPart-submitted.QuadPart)*1000/frequency.QuadPart);
        }
        printf("PACED WORK: maximum simulation/render/GPU-completion %.3f ms; fine timer request %s.\n",maximum_work_ms,presentation_timer_active?"active":"unavailable");
        double elapsed=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart;
        qsort(intervals,(size_t)pacing_frames,sizeof(*intervals),compare_intervals);
        printf("PACED PROBE: %d frames, %.3f seconds, %.3f fps; interval median %.3f ms, p95 %.3f ms, max %.3f ms; %llu skipped deadlines; %llu native ticks. Hidden GPU-complete rendering, not monitor scanout or physical input validation.\n",
            pacing_frames,elapsed,pacing_frames/elapsed,intervals[pacing_frames/2],intervals[(pacing_frames*95)/100],intervals[pacing_frames-1],
            (unsigned long long)pacer.skipped,(unsigned long long)total_ticks);
        free(intervals);
    }
    if(post_results_check) {
        const uint8_t ranks[5]={1,10,11,99,1},modes[5]={0,0,0,0,32};
        for(unsigned test=0;test<5;test++) {
            reset_drive();TaxiFareSession *session=&passenger_scene.session;
            /* Explicit post-timeout source-state fixture, not earned fares. */
            session->outer_state_1ea8=2;session->settings.bytes[0]=modes[test];
            session->fare.total_whole_1698=12345;session->fare.total_fraction_169c=67;
            session->end.completed_1692=3;session->fare.rating_1693=1;session->rank_16d4=ranks[test];
            city_fade.active_177b=0;city_fade.progress_178a=31;city_fade.interval_1771=1;city_fade.reload_1772=2;
            TaxiPoint fixed=car.world.height.position;unsigned ticks=0;
            while(!name_live_active&&!game_over_active&&ticks<2500) {drive_tick((TaxiXboxSample){0},0);ticks++;}
            int eligible=taxi_name_entry_eligible(modes[test],ranks[test]);
            if(!result_live_active||!result_live_flow.complete||name_live_active!=eligible||game_over_active==eligible)fail("Post-results eligibility transition failed.");
            if(memcmp(&fixed,&car.world.height.position,sizeof fixed))fail("Post-results transition moved the taxi.");
            if(result_customers!=3||result_class!=1||result_rank!=ranks[test]||!strstr(result_state.cash,"12,345.67"))fail("Results did not retain source fixture totals.");
            if(eligible) {
                if(name_display_rank!=ranks[test]||name_live_flow.fade.reload_1772!=2)fail("Name entry rank/fade inheritance failed.");
                for(unsigned n=0;n<100&&name_live_flow.fade.active_177b;n++)drive_tick((TaxiXboxSample){0},0);
                drive_tick((TaxiXboxSample){0},0);
                if(name_live_flow.fade.active_177b||name_live_flow.entry.length||name_cap!=31)fail("Name entry fade/input reset failed.");
            }
            if(!eligible) {
                uint8_t before[512];memcpy(before,native_settings,512);memcpy(before,session->settings.bytes,8);
                for(unsigned n=0;n<1500&&!profile_live_active;n++)drive_tick((TaxiXboxSample){0},0);
                if(!profile_live_active||profile_preview!=0||profile_flow.fade.reload_1772!=2||memcmp(before,native_settings,512))fail("Game Over profile entry ownership failed.");
                for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
                profile_tick(0);
                if(profile_flow.fade.active_177b||profile_uploaded_cap!=31||profile_edit_column!=-1||memcmp(profile_names[0],native_settings+8,3))fail("Live profile fade/input initialization failed.");
                if(memcmp(&fixed,&car.world.height.position,sizeof fixed))fail("Live profile moved taxi.");
                if(!modes[test]) {
                    profile_tick(128);profile_tick(0);profile_tick(1);profile_tick(0);
                    for(unsigned n=0;n<100&&!profile_flow.complete;n++)profile_tick(0);
                    if(!profile_commit_done||profile_persist_requested||memcmp(native_settings,session_settings_snapshot,512))fail("Live skip commit snapshot failed.");
                    uint8_t committed[512];memcpy(committed,native_settings,512);
                    for(unsigned n=0;n<20;n++)profile_tick(1);
                    if(memcmp(committed,native_settings,512))fail("Live commit repeated or overwritten.");
                }

            }
            printf("PASS post-results native fixture: rank%u mode%u customers3 cash12345.67 -> %s after%u calls; world frozen. Explicit seeded fields, not a driven fare.\n",ranks[test],modes[test],eligible?"name entry":"Game Over",ticks);
            reset_drive();if(name_live_active||game_over_active||result_live_active||results_preview||profile_live_active||profile_preview>=0)fail("Post-results reset failed.");
        }
        /* Final capture exercises cached source asset re-entry at rank10. */
        TaxiFareSession *session=&passenger_scene.session;session->outer_state_1ea8=2;
        session->settings.bytes[0]=0; /* explicit normal-mode fixture after mode32 case */
        session->fare.total_whole_1698=12345;session->fare.total_fraction_169c=67;
        session->end.completed_1692=3;session->fare.rating_1693=1;session->rank_16d4=10;
        for(unsigned n=0;n<2500&&!name_live_active;n++)drive_tick((TaxiXboxSample){0},0);
        if(!name_live_active)fail("Post-results cached re-entry failed.");
        for(unsigned n=0;n<34;n++)drive_tick((TaxiXboxSample){0},0);
        if(name_display_rank!=10||name_cap!=31)fail("Final name entry re-entry failed.");
        if(post_register_check) {
            uint8_t tail[720];memcpy(tail,native_score_rows+80,720);
            for(unsigned letter=0;letter<3;letter++) {
                name_interactive_tick(1);name_interactive_tick(0);
                for(unsigned n=0;n<18;n++)name_interactive_tick(0);
            }
            for(unsigned n=0;n<500&&(name_live_flow.entry.automatic||name_live_flow.entry.move);n++)name_interactive_tick(0);
            if(name_live_flow.entry.length!=3||name_live_flow.entry.selection!=42||memcmp(name_live_flow.entry.name,"AAA",3))fail("Live registration initials fixture failed.");
            name_interactive_tick(1);name_interactive_tick(0);
            for(unsigned n=0;n<1800&&!profile_live_active;n++)drive_tick((TaxiXboxSample){0},0);
            if(!profile_live_active)fail("Name/GameOver/profile registration transition failed.");
            for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
            profile_tick(0);profile_tick(1);profile_tick(0);
            if(!profile_flow.choice.editing)fail("Live new profile edit entry failed.");
            profile_tick(64);profile_tick(0);
            if(memcmp(profile_names[0],"BAA",3))fail("Live profile letter change failed.");
            profile_tick(1);profile_tick(0);
            for(unsigned n=0;n<100&&!profile_flow.complete;n++)profile_tick(0);
            if(!profile_commit_done||!profile_persist_requested||native_settings[11]!=1||memcmp(native_settings+8,"BAA",3))fail("Live profile registration commit failed.");
            if(memcmp(native_settings+424+9*8,"AAA",3)||memcmp(native_score_rows,native_settings+424,80)||memcmp(native_score_rows+80,tail,720))fail("Live registered leaderboard ownership failed.");
            const uint8_t *row=native_settings+424+9*8+4;
            uint32_t packed=(uint32_t)row[0]|(uint32_t)row[1]<<8|(uint32_t)row[2]<<16|(uint32_t)row[3]<<24;
            if(packed!=((12345u<<15)|(67u<<8)|(native_game_driver<<5)|(1u<<1)|1))fail("Live registered score fields failed.");
            const uint8_t *history=native_settings+12;
            uint32_t record=(uint32_t)history[0]|(uint32_t)history[1]<<8|(uint32_t)history[2]<<16|(uint32_t)history[3]<<24;
            if(record!=packed)fail("Live profile history commit failed.");
            uint8_t committed[512];memcpy(committed,native_settings,512);
            for(unsigned n=0;n<20;n++)profile_tick(1);
            if(memcmp(committed,native_settings,512))fail("Live registration repeated commit.");
            if(native_save_path) {
                uint8_t reloaded[512],rows[800];memset(reloaded,173,512);memcpy(rows,native_score_rows,800);
                if(native_save_write_status||taxi_save_file_load(native_save_path,reloaded,rows)||memcmp(committed,reloaded,512)||memcmp(rows,native_score_rows,800))fail("Production registration disk reload failed.");
                puts("PASS native production registration: saved512 bytes reload exactly, raw100 rows retained; separate-process session startup still pending.");
            }
            reset_drive();
            if(memcmp(passenger_scene.session.settings.bytes,committed,8)||memcmp(session_settings_snapshot,committed,512)||
                passenger_scene.session.clock.remaining_16a8!=taxi_game_initial_clock(&passenger_scene.session.settings,0))fail("Committed settings lost on session restart.");
            puts("PASS native session restart: first8 settings, full512 snapshot and source initial clock retained.");
            puts("PASS native full registration fixture: Results->AAA initials->GameOver->BAA profile->rank10 raw leaderboard/history; once-only commit,90 rows retained.");
            /* Explicit second existing profile fixture, not an earned record. */
            memcpy(native_settings+216,"CCA",3);native_settings[219]=1;
            reset_drive();result_rank=99;result_class=1;
            passenger_scene.session.fare.total_whole_1698=1;
            passenger_scene.session.fare.total_fraction_169c=0;
            uint8_t first_profile[208];memcpy(first_profile,native_settings+8,208);
            profile_live_begin(1,1);
            for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
            profile_tick(128);profile_tick(0); /* fresh Down selects profile index1 */
            if(profile_flow.choice.option!=1)fail("Second existing profile navigation failed.");
            profile_tick(1);profile_tick(0);
            for(unsigned n=0;n<100&&!profile_flow.complete;n++)profile_tick(0);
            if(!profile_commit_done||native_current_profile!=1||profile_flow.choice.choice!=2||
                memcmp(first_profile,native_settings+8,208))fail("Second profile commit/current ownership failed.");
            if(native_save_path) {
                uint8_t saved[512],rows[800];memcpy(rows,native_score_rows,800);
                if(native_save_write_status||taxi_save_file_load(native_save_path,saved,rows)||memcmp(saved,native_settings,512))fail("Second profile disk reload failed.");
            }
            reset_drive();profile_live_begin(1,1);
            for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
            if(native_current_profile!=1||profile_flow.choice.current!=1||profile_flow.choice.option!=1||
                memcmp(profile_names[0],"BAA",3)||memcmp(profile_names[1],"CCA",3))fail("Second profile retained re-entry failed.");
            puts("PASS native second-profile fixture: selected index1, first profile preserved, disk reload checked when configured, next live screen retains index1.");
        }

    }
    if(gameplay_key_replay&&gameplay_xbox_replay)fail("Input replay sources must be exclusive.");
    if(native_elapsed_time&&((!drive_mode&&!startup_mode)||image_path||gameplay_step_replay))fail("Live elapsed timing requires driving/startup without snapshot/replay.");
    if((gameplay_key_replay||gameplay_xbox_replay)&&!gameplay_step_replay)fail("Original key replay requires diagnostic timing replay.");
    if(gameplay_step_replay&&(!startup_gameplay_check||!startup_idle_check||route_probe_active))fail("Timing replay requires ordinary startup idle diagnostic.");
    if(startup_gameplay_check) {
        if(startup_second_city) {
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(2);for(unsigned n=0;n<40;n++)startup_tick(0);
            if(startup_stage!=1)fail("Second city fixture did not return to city menu.");
            native_settings[7]=1; /* Explicit unlock fixture, not earned unlock. */
            for(unsigned n=0;n<50;n++)startup_tick(0);
            startup_tick(16);startup_tick(0);startup_tick(0);
            if(native_settings[4]!=1)fail("Second city menu selection failed.");
            startup_tick(1);for(unsigned n=0;n<40;n++)startup_tick(0);
            if(startup_stage!=2)fail("Second city did not open driver menu.");
        }
        for(unsigned n=0;n<50;n++)startup_tick(0);
        unsigned desired_driver=startup_driver_selection>=0?(unsigned)startup_driver_selection:1;
        while((unsigned)driver_preview!=desired_driver){
            startup_tick((unsigned)driver_preview<desired_driver?16:32);
            startup_tick(0);startup_tick(0);
        }
        unsigned chosen=driver_preview;
        startup_tick(1);for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
        if(startup_loading_rendered!=(startup_loading_close_check?0u:52u)||startup_loading_flow.animation_bank!=2||startup_stage!=3||native_game_driver!=chosen||!passenger_scene_initialized||scene_index!=native_settings[4])fail("Startup gameplay handoff failed.");
        if(startup_warm_reload_check){
            TaxiPassengerScene expected=passenger_scene;
            if(taxi_passenger_scene_reinit(&expected,&passenger_asset,&world,0))fail("Warm reload reference initialization failed.");
            uint32_t retained_rng=passenger_scene.random_state;
            (void)music_shared_random(&retained_rng);
            /* Warm loading now runs the qualified14A58 vehicle owner after
               menu music selection. Predict its shared RNG on isolated owners;
               this is an integration regression, not a whole-ROM oracle. */
            TaxiDriveWorld expected_car=car;
            TaxiDriveWorldContext expected_context=drive_context;
            TaxiVehicleStartPacked expected_vehicle_owner=native_vehicle_start_owner;
            TaxiVehicleStartPoint expected_start;
            unsigned expected_template=taxi_runtime_template_bank((uint8_t)source_attract_ai);
            if(taxi_vehicle_start_gameplay(&expected_car,&expected_context,&expected_vehicle_owner,&retained_rng,
                &runtime.starts[scene_index],runtime.templates[expected_template],584,
                native_settings_loaded?native_settings[0]:0,0,taxi_world_asset_ground,&world,&expected_start)!=1)
                fail("Warm reload vehicle reference initialization failed.");
            unsigned retained_driver=native_game_driver,retained_city=native_settings[4];
            uint8_t settings_before[512];memcpy(settings_before,native_settings,512);
            startup_loading_begin();
            for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
            if(startup_stage!=3||startup_loading_rendered!=52||native_game_driver!=retained_driver||
               native_settings[4]!=retained_city||memcmp(settings_before,native_settings,512)||
               passenger_scene.random_state!=retained_rng||
               memcmp(expected.customers,passenger_scene.customers,sizeof expected.customers))
            {fprintf(stderr,"WARM DETAIL: stage%u renders%u driver%u/%u city%u/%u settings%d rng%u/%u customers%d\n",startup_stage,startup_loading_rendered,native_game_driver,retained_driver,native_settings[4],retained_city,memcmp(settings_before,native_settings,512),passenger_scene.random_state,retained_rng,memcmp(expected.customers,passenger_scene.customers,sizeof expected.customers));
                fail("Warm same-city reload lost passenger heights, settings or live RNG.");}
            puts("PASS native warm same-city reload: retained customer height inputs/settings/driver,52 actual loading renders, music selection and recovered vehicle startup RNG retained; diagnostic reload, not playable Retry.");
        }
        if(startup_timed_check){
            const int32_t clocks[]={0,10800,18000,36000};
            if(passenger_scene.session.clock.remaining_16a8!=clocks[startup_timed_check]||(passenger_scene.session.settings.bytes[0]&0x60)!=(startup_timed_check<<5))fail("Timed mode source clock handoff failed.");
            printf("TIMED RULES: menu mode%u -> original initial clock%ld verified before first driving tick.\n",startup_timed_check,(long)clocks[startup_timed_check]);
        }
        if(startup_loading_close_check){
            if(!window_quit_requested||loading_work_active)fail("Loading close request was lost.");
            puts("STARTUP LOADING CLOSE: real queued reset guarded, close latched, no subsequent work presentations; initialization completed safely for cleanup.");
        }
        unsigned start_ticks=0;
        unsigned start_capture_mask=0;
        while(live_game_start.active&&start_ticks<500){
            drive_tick((TaxiXboxSample){0},!startup_idle_check);draw_game_start_art(width,height);start_ticks++;
            int phase=live_game_start.phase;
            if(phase>=0&&phase<=2&&!(start_capture_mask&(1u<<phase))&&
                (phase<2?live_game_start.hold==45:live_game_start.scale>=280)){
                draw(width,height);glFinish();char capture[MAX_PATH];
                snprintf(capture,sizeof capture,"outputs/crazy-taxi-native/reports/start-art-phase%d.bmp",phase);
                snapshot(capture,width,height);start_capture_mask|=1u<<phase;
            }
        }
        if(!gameplay_step_replay&&start_capture_mask!=7)fail("Startup stage captures incomplete.");
        if(audio_settings_state.effects_enabled&&(live_game_start_sound_counts[0]!=1||live_game_start_sound_counts[1]!=1||live_game_start_sound_counts[2]!=1))fail("Original startup sound requests mismatch.");
        if(live_game_start.active||(!gameplay_step_replay&&start_ticks<315)||simulation_error)fail("Original startup sequence did not finish naturally.");
        printf("STARTUP SEQUENCE: %u native ticks including fade; original315-step ordinary sequence completed before driver controls; recovered art submitted and three sound requests verified. Full original pixel/audio fidelity remains unverified.\n",start_ticks);
        FILE *orientation_trace=fopen("outputs/crazy-taxi-native/reports/start-camera-motion.csv","wb");
        if(!orientation_trace)fail("Camera motion trace could not open.");
        fprintf(orientation_trace,"tick,x,z,dx,dz,heading1c,heading20,camera_heading,yaw_offset,frame,eye_x,eye_z,away_dot,view_dx,view_dz\n");
        for(unsigned n=0;n<60;n++){
            TaxiPoint before=car.world.height.position;drive_tick((TaxiXboxSample){0},!startup_idle_check);
            TaxiPoint now=car.world.height.position;
            TaxiPoint eye=taxi_camera_position(now,camera_pitch,camera_heading,
                camera_offsets.height_76c,camera_offsets.distance_770,camera_base,&runtime.trig);
            int32_t dx=now.x-before.x,dz=now.z-before.z;
            int32_t angle=taxi_camera_transform_yaw(camera_heading,camera_yaw_offset);
            TaxiPoint view_move=taxi_rotate_camera((TaxiPoint){dx,0,dz},-(camera_pitch/4096),-angle,0,&runtime.trig);
            int64_t dot=(int64_t)dx*(now.x-eye.x)+(int64_t)dz*(now.z-eye.z);
            fprintf(orientation_trace,"%u,%d,%d,%d,%d,%d,%d,%d,%d,%u,%d,%d,%lld,%d,%d\n",
                n,now.x,now.z,dx,dz,car.world.heading_1c,car.world.heading_20,camera_heading,camera_yaw_offset,
                taxi_sprite_frame((int16_t)angle,car.world.heading_1c),eye.x,eye.z,(long long)dot,view_move.x,view_move.z);
        }
        fclose(orientation_trace);
        if(simulation_error)fail("Startup gameplay fixture simulation failed.");
        puts(gameplay_key_replay||gameplay_xbox_replay?"STARTUP INPUT REPLAY: selected city/driver->60 controlled native input ticks passed; full fidelity incomplete.":startup_idle_check?"STARTUP IDLE: selected city/driver->60 native no-input ticks passed; original comparison required.":"STARTUP GAMEPLAY: menus->selected city/driver->60 native scripted driving/passenger ticks passed; fidelity still incomplete.");
        if(startup_retry_input_check)check_native_retry_input();
        if(route_probe_active){
            for(unsigned fare_attempt=0;fare_attempt<(native_multi_fare_probe?8u:1u);fare_attempt++){
            unsigned completed_before=passenger_scene.session.end.completed_1692;
            if(native_multi_fare_probe){
                route_probe_brake=1;script_steer=0;
                while(passenger_scene.session.gate_1700&&!passenger_scene.session.ended_1697)
                    {drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                printf("MULTI FARE BEGIN: attempt%u completed%u cash%ld remaining%ld qualified%u position(%ld,%ld).\n",
                    fare_attempt+1,completed_before,(long)passenger_scene.session.fare.total_whole_1698,
                    (long)passenger_scene.session.clock.remaining_16a8,passenger_scene.session.fare.qualified_1691,
                    (long)car.world.height.position.x,(long)car.world.height.position.z);
                if(passenger_scene.session.ended_1697)fail("Multi-fare session expired before next pickup.");
            }
            FILE *probe=fopen("outputs/crazy-taxi-native/reports/input-route-probe.csv","wb");if(!probe)fail("Route probe report failed.");
            fprintf(probe,"tick,x,y,z,speed,heading,steer,brake,nearest,dist_squared,pickup_pending,trip_active\n");
            for(unsigned tick=0;tick<900&&!passenger_scene.session.ended_1697;tick++){
                int nearest=-1;int64_t best=INT64_MAX;
                for(unsigned i=0;i<passenger_scene.count;i++)if((passenger_scene.customers[i].bytes[0]&1)&&!(passenger_scene.customers[i].bytes[0]&64)){
                    int64_t dx=(int64_t)car.world.height.position.x-passenger_scene.spawns[i].x;
                    int64_t dz=(int64_t)car.world.height.position.z-passenger_scene.spawns[i].z;
                    int64_t distance=dx*dx+dz*dz;if(distance<best){best=distance;nearest=(int)i;}
                }
                /* Explicit test-driver commands; no native physics/state edits.
                   Brake when source search marks a customer within its radius. */
                route_probe_brake=nearest>=0&&(passenger_scene.customers[nearest].bytes[0]&4);
                script_steer=0;
                if(nearest>=0&&!route_probe_brake){
                    int64_t dx=(int64_t)passenger_scene.spawns[nearest].x-car.world.height.position.x;
                    int64_t dz=(int64_t)passenger_scene.spawns[nearest].z-car.world.height.position.z;
                    int desired=0;int64_t alignment=INT64_MIN;
                    /* Test-driver navigation only: actual source trig and
                       positive localZ movement from motion.c, no new physics. */
                    for(int angle=0;angle<4096;angle++){
                        int64_t dot=-(int64_t)runtime.trig.sine[angle]*dx+(int64_t)runtime.trig.cosine[angle]*dz;
                        if(dot>alignment){alignment=dot;desired=angle;}
                    }
                    int current=(int)((uint32_t)car.world.heading_20>>12)&4095;
                    int difference=((desired-current+2048)&4095)-2048;
                    script_steer=difference>32?-1:difference < -32?1:0;
                }
                {drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                fprintf(probe,"%u,%ld,%ld,%ld,%ld,%ld,%d,%d,%d,%lld,%ld,%u\n",tick,
                    (long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z,
                    (long)car.world.speed_544,(long)car.world.heading_20,script_steer,route_probe_brake,nearest,
                    (long long)best,(long)passenger_scene.pickup.pending_1704,passenger_scene.session.trip.active_1694);
                if(simulation_error)fail("Input-only route probe invalid state.");
                if(passenger_scene.session.trip.active_1694)break;
            }
            if(ferror(probe)||fclose(probe))fail("Route probe report write failed.");
            if(!passenger_scene.session.trip.active_1694||passenger_scene.pickup.pending_1704||passenger_scene.pickup.destination_index<0||car.world.speed_544||passenger_scene.session.fare.pending_whole_16a0<=0)fail("Input-only pickup probe failed.");
            if(native_pickup_only_check) {
                if(native_panel_generation!=1 || native_materials.destination!=passenger_scene.pickup.destination_index)
                    fail("Native selection preview pickup generation failed.");
                for(unsigned edge=0;edge<64;edge++)if(native_panel_pixels[edge]!=native_scene_border ||
                    native_panel_pixels[4032+edge]!=native_scene_border || native_panel_pixels[edge*64]!=native_scene_border ||
                    native_panel_pixels[edge*64+63]!=native_scene_border)fail("Native pickup preview border failed.");
                FILE *capture=fopen("outputs/crazy-taxi-native/reports/live-pickup-panel.raw","wb");
                if(!capture || fwrite(native_panel_pixels,1,4096,capture)!=4096)fail("Native pickup preview capture failed.");
                fclose(capture);
                puts("PASS native input-only pickup generated original indexed destination panel synchronously before fare; exact source camera/cache consumer, host projection explicit.");
                route_probe_brake=1;script_steer=0;
                for(unsigned tick=0;tick<20;++tick){drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                if(simulation_error || passenger_scene.session.distance_display.offset_16c4!=4)
                    fail("Native destination panel slide failed.");
                for(unsigned tick=0;tick<340;++tick){drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                if(passenger_scene.session.trip.active_1695!=2 ||
                   passenger_scene.session.distance_display.offset_16c4!=-68)
                    fail("Native destination panel hold/retract failed.");
                TaxiXboxSample select={.connected=1,.active_low_keys=0x3fb};
                drive_tick(select,1);
                if(passenger_scene.session.trip.active_1695!=0)fail("Native Select hide edge failed.");
                drive_tick(select,1);
                if(passenger_scene.session.trip.active_1695!=0)fail("Native Select held repeated edge.");
                {drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}drive_tick(select,1);
                if(passenger_scene.session.trip.active_1695!=1)fail("Native Select reopen edge failed.");
                for(unsigned tick=0;tick<20;++tick){drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                if(simulation_error || passenger_scene.session.distance_display.offset_16c4!=4 || native_panel_generation!=1)
                    fail("Native destination panel reopened slide failed.");
                for(unsigned material=0;material<512;++material) {
                    int highlighted=0;
                    for(unsigned slot=0;slot<4;++slot)if(native_materials.indices[slot]==material)highlighted=1;
                    uint8_t expected=highlighted?((native_materials.clock&16)?0:1):0;
                    if(native_materials.cache[material]!=expected)fail("Live original material blink cache mismatch.");
                }
                printf("PASS live original material cache: clock%u, selected destination%ld, all512 bytes verified.\n",
                    native_materials.clock,(long)native_materials.destination);
                puts("PASS live destination preview hold/retract and Select edge hide/reopen; held Select does not repeat, panel generated once.");
                if(native_material_reset_check) {
                    uint32_t retained_clock=native_materials.clock;
                    reset_drive();
                    if(native_materials.clock!=retained_clock || native_materials.destination!=-1 || native_materials.has_record)
                        fail("Native material reset clock/selection ownership failed.");
                    for(unsigned i=0;i<512;++i)if(native_materials.cache[i])fail("Native reset retained highlight.");
                    for(unsigned tick=0;tick<20;++tick){drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                    if(simulation_error || native_materials.clock!=retained_clock+20)
                        fail("Native retained material clock advance failed.");
                    puts("PASS live session reset clears selection/cache while retaining material blink clock; next20 ticks advance retained clock.");
                }
                goto native_snapshot_ready;
            }
            int32_t destination=passenger_scene.pickup.destination_index;
            const uint8_t *record=passenger_asset.destinations[destination];
            int32_t target_x,target_z;memcpy(&target_x,record+24,4);memcpy(&target_z,record+32,4);
            const TaxiPassengerRouteAsset *original_route=NULL;
            TaxiRoutePoint origin=passenger_scene.spawns[passenger_scene.pickup.selected_index];
            for(unsigned i=0;i<passenger_asset.route_count;i++){
                const TaxiPassengerRouteAsset *candidate=passenger_asset.routes+i;
                if(candidate->points[0].x!=origin.x||candidate->points[0].z!=origin.z)continue;
                TaxiRoutePoint end=candidate->points[candidate->count-1];TaxiGroundHit hit={0};
                int found=taxi_world_asset_ground(&world,(TaxiPoint){end.x,0,end.z},&hit);
                if(found<0)fail("Original route endpoint query failed.");
                if(found&&taxi_destination_accepts(destination,record,hit.material)){original_route=candidate;break;}
            }
            if(!original_route)fail("Earned customer original route missing.");
            FILE *source_route=fopen("outputs/crazy-taxi-native/reports/delivery-source-route.csv","wb");if(!source_route)fail("Source route report failed.");
            fprintf(source_route,"point,x,z\n");
            for(unsigned i=0;i<original_route->count;i++)fprintf(source_route,"%u,%ld,%ld\n",i,(long)original_route->points[i].x,(long)original_route->points[i].z);
            if(ferror(source_route)||fclose(source_route))fail("Source route write failed.");
            if(reward_drive_probe){
                reward_drive_running=1;reward_drive_tick=0;route_probe_brake=0;
                unsigned phase2=0;
                for(unsigned t=0;t<540&&passenger_scene.session.trip.active_1694&&!passenger_scene.session.ended_1697;t++){
                    script_steer=t%180<40?0:1;
                    {drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                    if(car.driver.power.gears.phase_e6a==2)phase2++;
                    if(simulation_error)fail("Reward input-only drive failed.");
                }
                reward_drive_running=0;script_steer=0;
                TaxiRewardStats counts=taxi_reward_session_stats();
                printf("REWARD INPUT PROBE: %u controller ticks, %u drift-phase ticks, %u requests/%u tips/%u expirations; source passenger pickup retained, no pose/score injection.\n",reward_drive_tick,phase2,counts.requests,counts.created,counts.expired);
            }
            unsigned waypoint=original_route->count>1?1:0;
            printf("INPUT DELIVERY ROUTE: original source%u, %u points.\n",original_route->source_index,original_route->count);
            FILE *delivery=fopen("outputs/crazy-taxi-native/reports/input-delivery-probe.csv","wb");if(!delivery)fail("Delivery probe report failed.");
            fprintf(delivery,"tick,x,y,z,speed,heading,steer,brake,material,accepted_material,trip_remaining,main_remaining,trip_active,completed,cash_whole,waypoint,traffic_collisions,desired_heading,heading_difference,obstacle_collision,normal_x,normal_z,body_heading,drift,analog_command\n");
            FILE *contacts=fopen("outputs/crazy-taxi-native/reports/input-delivery-contacts.csv","wb");if(!contacts)fail("Delivery contact report failed.");
            fprintf(contacts,"tick,waypoint,actor,route,flags_before,flags_after,timer_before,timer_after,actor_x_before,actor_z_before,actor_x_after,actor_z_after,path_x_after,path_z_after,path_segment_after,player_x,player_z,tick_entry_speed,tick_exit_speed\n");
            route_probe_obstacles=fopen("outputs/crazy-taxi-native/reports/input-delivery-obstacles.csv","wb");if(!route_probe_obstacles)fail("Obstacle report failed.");
            fprintf(route_probe_obstacles,"tick,waypoint,query_x,query_y,query_z,kind,polygon,edge_count,v0x,v0y,v0z,v1x,v1y,v1z,v2x,v2y,v2z,v3x,v3y,v3z\n");
            for(unsigned tick=0;tick<15000&&passenger_scene.session.trip.active_1694&&!passenger_scene.session.ended_1697;tick++){
                uint32_t material=(uint32_t)(int32_t)(int16_t)car.world.material_e60;
                route_probe_brake=taxi_destination_accepts(destination,record,material);
                TaxiRoutePoint point=original_route->points[waypoint];
                int64_t dx=(int64_t)point.x-car.world.height.position.x,dz=(int64_t)point.z-car.world.height.position.z;
                while(waypoint+1<original_route->count){
                    int reached=dx*dx+dz*dz<1024*1024;
                    if(!reached&&native_multi_fare_probe&&waypoint){
                        /* Test-driver target progression only: a crossed endpoint
                           plane within4096 source units avoids circling missed
                           waypoints. Original route/arrival/physics unchanged. */
                        TaxiRoutePoint previous=original_route->points[waypoint-1];
                        double sx=(double)point.x-previous.x,sz=(double)point.z-previous.z;
                        double px=(double)car.world.height.position.x-previous.x,pz=(double)car.world.height.position.z-previous.z;
                        double length=sx*sx+sz*sz,cross=px*sz-pz*sx;
                        reached=length>0&&px*sx+pz*sz>=length&&cross*cross<=4096.0*4096*length;
                    }
                    if(!reached)break;
                    point=original_route->points[++waypoint];
                    dx=(int64_t)point.x-car.world.height.position.x;dz=(int64_t)point.z-car.world.height.position.z;
                }
                /* Opt-in diagnostic pursuit of the current original segment.
                   Arrival and source route points remain unchanged. This supplies
                   controller inputs only; it is not recovered game AI. */
                if(route_probe_segment_distance&&waypoint&&(!route_probe_steering_waypoint||waypoint==(unsigned)route_probe_steering_waypoint)) {
                    TaxiRoutePoint start=original_route->points[waypoint-1];
                    double sx=(double)point.x-start.x,sz=(double)point.z-start.z;
                    double length_squared=sx*sx+sz*sz;
                    if(length_squared>0) {
                        double along=(((double)car.world.height.position.x-start.x)*sx+((double)car.world.height.position.z-start.z)*sz)/length_squared;
                        along+=route_probe_segment_distance/sqrt(length_squared);
                        if(along<0)along=0;if(along>1)along=1;
                        double target_x=start.x+along*sx,target_z=start.z+along*sz;
                        if(route_probe_avoid_distance&&(!route_probe_avoid_waypoint||waypoint==(unsigned)route_probe_avoid_waypoint||(route_probe_avoid_extra_waypoint&&waypoint==(unsigned)route_probe_avoid_extra_waypoint)||(route_probe_avoid_late_straights&&(waypoint==14||waypoint==21||waypoint==22)))&&traffic_preview&&!route_probe_brake) {
                            /* Diagnostic driver only: aim beside the closest active
                               traffic actor ahead on this segment. No actor is moved. */
                            double length=sqrt(length_squared),ux=sx/length,uz=sz/length;
                            double nearest=4096,lateral=0;int found=0;
                            for(unsigned i=0;i<traffic_submission_count;i++) {
                                const TaxiTrafficActor *actor=traffic_actors+i;if(actor->countdown>0)continue;
                                double ax=(double)actor->x-car.world.height.position.x,az=(double)actor->z-car.world.height.position.z;
                                double ahead=ax*ux+az*uz;
                                double side=((double)actor->x-start.x)*(-uz)+((double)actor->z-start.z)*ux;
                                if(ahead>=-512&&ahead<nearest&&fabs(side)<1536){nearest=ahead;lateral=side;found=1;}
                            }
                            if(found&&along<0.98&&(!route_probe_avoid_end_margin||(1-along)*length>route_probe_avoid_end_margin)) {
                                double offset=lateral>=0?-route_probe_avoid_distance:route_probe_avoid_distance;
                                target_x+=(-uz)*offset;target_z+=ux*offset;
                            }
                        }
                        dx=(int64_t)target_x-car.world.height.position.x;
                        dz=(int64_t)target_z-car.world.height.position.z;
                    }
                }
                int desired=0;int64_t alignment=INT64_MIN;
                for(int angle=0;angle<4096;angle++){
                    int64_t dot=-(int64_t)runtime.trig.sine[angle]*dx+(int64_t)runtime.trig.cosine[angle]*dz;
                    if(dot>alignment){alignment=dot;desired=angle;}
                }
                int current=(int)((uint32_t)car.world.heading_20>>12)&4095;
                int difference=((desired-current+2048)&4095)-2048;
                /* Diagnostic driver only: use the ordinary brake while turning
                   sharply at speed. Source vehicle/traffic/timers stay untouched. */
                int destination_brake=route_probe_brake;
                if(route_probe_bend_speed&&!destination_brake&&car.world.speed_544>route_probe_bend_speed) {
                    /* Opt-in diagnostic controller braking for audited bend10->11.
                       No source speed clamp, route edits or simulation shortcuts. */
                    int64_t end_x=(int64_t)point.x-car.world.height.position.x;
                    int64_t end_z=(int64_t)point.z-car.world.height.position.z;
                    if((waypoint==10&&end_x*end_x+end_z*end_z<(int64_t)route_probe_bend_distance*route_probe_bend_distance)||
                       (waypoint==11&&abs(difference)>64))route_probe_brake=1;
                }
                if(route_probe_final_bend_speed&&!destination_brake&&car.world.speed_544>route_probe_final_bend_speed) {
                    /* Diagnostic controller only, for source-audited final wall. */
                    int64_t end_x=(int64_t)point.x-car.world.height.position.x;
                    int64_t end_z=(int64_t)point.z-car.world.height.position.z;
                    if((waypoint==24&&end_x*end_x+end_z*end_z<(int64_t)route_probe_bend_distance*route_probe_bend_distance)||
                       (waypoint==25&&abs(difference)>64))route_probe_brake=1;
                }
                if(route_probe_turn_brake&&!destination_brake&&abs(difference)>256&&car.world.speed_544>40000)
                    route_probe_brake=1;
                script_steer=destination_brake?0:difference>32?-1:difference < -32?1:0;
                route_probe_analog_command=0;
                if(route_probe_speed_steering&&(!route_probe_steering_waypoint||waypoint==(unsigned)route_probe_steering_waypoint)&&!destination_brake&&abs(difference)>32) {
                    /* Diagnostic stick input only, matching original signed analog command range.
                       Reduce turn demand after traffic slows the car; source physics is untouched. */
                    int magnitude=abs(difference)*192/512;if(magnitude>192)magnitude=192;
                    int speed=car.world.speed_544;if(speed<0)speed=0;if(speed>route_probe_speed_reference)speed=route_probe_speed_reference;
                    magnitude=(int)((int64_t)magnitude*speed/route_probe_speed_reference);if(magnitude<8)magnitude=8;
                    route_probe_analog_command=difference>0?magnitude:-magnitude;
                }
                TaxiTrafficActor contact_before[50];memcpy(contact_before,traffic_actors,sizeof contact_before);
                int32_t contact_entry_speed=car.world.speed_544;
                route_probe_obstacle_tick=tick;route_probe_obstacle_waypoint=waypoint;
                {drive_tick((TaxiXboxSample){0},1);if(native_audio_capture)audio_frame();}
                if(reward_drive_probe&&!reward_image_captured&&taxi_reward_session_stats().created){
                    int active=0;for(unsigned j=0;j<10;j++)if(passenger_scene.session.tips[j][2])active=1;
                    if(active){draw(width,height);glFinish();snapshot("outputs/crazy-taxi-native/reports/active-earned-tip.bmp",width,height);reward_image_captured=1;
                        printf("ACTIVE TIP SNAPSHOT: input-earned active records, frame%u, %dx%d native render; original dollar text submitted; pixel oracle pending.\n",native_tip_frame,width,height);}
                }

                for(unsigned i=0;i<traffic_submission_count;i++)if(traffic_submissions[i].effects.collision) {
                    const TaxiTrafficActor *before=contact_before+i,*after=traffic_actors+i;
                    fprintf(contacts,"%u,%u,%u,%u,%u,%u,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%u,%ld,%ld,%ld,%ld\n",
                        tick,waypoint,i,after->route,before->flags,after->flags,
                        (long)before->collision_timer,(long)after->collision_timer,
                        (long)before->x,(long)before->z,(long)after->x,(long)after->z,
                        (long)after->path.x,(long)after->path.z,(unsigned)after->path.segment,
                        (long)car.world.height.position.x,(long)car.world.height.position.z,
                        (long)contact_entry_speed,(long)car.world.speed_544);
                }
                unsigned collisions=0;
                for(unsigned i=0;i<traffic_submission_count;i++)
                    if(traffic_submissions[i].effects.collision)++collisions;
                fprintf(delivery,"%u,%ld,%ld,%ld,%ld,%ld,%d,%d,%ld,%d,%ld,%ld,%u,%u,%ld,%u,%u,%d,%d,%d,%ld,%ld,%ld,%ld,%d\n",tick,
                    (long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z,
                    (long)car.world.speed_544,(long)car.world.heading_20,script_steer,route_probe_brake,
                    (long)car.world.material_e60,taxi_destination_accepts(destination,record,(uint32_t)(int32_t)(int16_t)car.world.material_e60),
                    (long)passenger_scene.session.trip.time_16ac,(long)passenger_scene.session.clock.remaining_16a8,
                    passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698,waypoint,collisions,
                    desired,difference,route_probe_movement.collided,(long)route_probe_movement.normal.x,(long)route_probe_movement.normal.z,(long)car.world.heading_1c,(long)car.world.drift_69c,route_probe_analog_command);
                if(simulation_error)fail("Input-only delivery invalid state.");
            }
            int obstacle_write_error=ferror(route_probe_obstacles);int obstacle_close_error=fclose(route_probe_obstacles);route_probe_obstacles=NULL;
            if(obstacle_write_error||obstacle_close_error)fail("Obstacle report write failed.");
            if(ferror(contacts)||fclose(contacts))fail("Delivery contact report write failed.");
            if(ferror(delivery)||fclose(delivery))fail("Delivery report write failed.");
            if(passenger_scene.session.end.completed_1692!=completed_before+1||passenger_scene.session.trip.active_1694||passenger_scene.pickup.destination_index!=-1||car.world.speed_544||passenger_scene.session.fare.total_whole_1698<=0||passenger_scene.session.fare.pending_whole_16a0)fail("Input-only original-route delivery failed.");
            printf("INPUT DELIVERY PROBE: original destination%ld target(%ld,%ld), completed%u cash%ld; no forced settlement.\n",(long)destination,(long)target_x,(long)target_z,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698);
            if(native_multi_fare_probe){
                printf("MULTI FARE SETTLED: attempt%u completed%u cash%ld.%02ld remaining%ld qualified%u; actual source settlement.\n",
                    fare_attempt+1,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698,
                    (long)passenger_scene.session.fare.total_fraction_169c,(long)passenger_scene.session.clock.remaining_16a8,
                    passenger_scene.session.fare.qualified_1691);
                if(passenger_scene.session.fare.qualified_1691)break;
            }
            }
            if(native_multi_fare_probe){
                if(!passenger_scene.session.fare.qualified_1691)fail("Input-only successive fares did not reach earned credits qualification.");
                puts("PASS input-only successive original pickups/deliveries earned credits qualification; actual qualified Quit/menu return still unverified.");
                route_probe_active=route_probe_brake=script_steer=0;
                if(native_earned_quit_check)check_earned_credits_quit(width,height,image_path);
                goto native_snapshot_ready;
            }
            int32_t earned_whole=passenger_scene.session.fare.total_whole_1698;
            int32_t earned_fraction=passenger_scene.session.fare.total_fraction_169c;
            TaxiPoint earned_position=car.world.height.position;int freeze_captured=0;
            route_probe_brake=1;script_steer=0;
            unsigned natural_ticks=0;int earned_result_captured=0;
            while(natural_ticks<8000&&!name_live_active&&!profile_live_active&&!game_over_complete){
                drive_tick((TaxiXboxSample){0},1);natural_ticks++;
                if(simulation_error)fail("Earned session countdown invalid state.");
                if(result_live_active){
                    if(!freeze_captured){earned_position=car.world.height.position;freeze_captured=1;}
                    else if(memcmp(&earned_position,&car.world.height.position,sizeof earned_position))fail("Earned results moved the world.");
                    if(!earned_result_captured&&result_state.phase>=6&&results_palette_cap==31&&!result_live_flow.animation.exiting) {
                        char earned_capture[MAX_PATH];
                        int length=snprintf(earned_capture,sizeof earned_capture,"%s.earned-results.bmp",image_path);
                        if(length<0||(size_t)length>=sizeof earned_capture)fail("Earned results capture path too long.");
                        draw(3840,2160);glFinish();snapshot(earned_capture,3840,2160);
                        if(glGetError()!=GL_NO_ERROR)fail("Earned results render failed.");
                        earned_result_captured=1;
                        printf("EARNED RESULTS CAPTURE: phase%u cap%d cash%ld.%02ld customers%u; native input-earned fare, original countdown unchanged.\n",result_state.phase,results_palette_cap,(long)earned_whole,(long)earned_fraction,result_customers);
                    }
                }
            }
            printf("EARNED COUNTDOWN STATE: ticks%u ended%u remaining%ld results%d complete%u profile%d name%d position(%ld,%ld,%ld)->(%ld,%ld,%ld).\n",natural_ticks,passenger_scene.session.ended_1697,(long)passenger_scene.session.clock.remaining_16a8,result_live_active,result_live_flow.complete,profile_live_active,name_live_active,(long)earned_position.x,(long)earned_position.y,(long)earned_position.z,(long)car.world.height.position.x,(long)car.world.height.position.y,(long)car.world.height.position.z);
            printf("EARNED GAME OVER STATE: active%d complete%d glyph%u hold%d fade%u progress%d interval%u reload%u outer%u cash%ld.%02ld.\n",game_over_active,game_over_complete,game_over_state.glyph,game_over_state.hold,game_over_fade.active_177b,game_over_fade.progress_178a,game_over_fade.interval_1771,game_over_fade.reload_1772,passenger_scene.session.outer_state_1ea8,(long)passenger_scene.session.fare.total_whole_1698,(long)passenger_scene.session.fare.total_fraction_169c);
            if(passenger_scene.session.outer_state_1ea8||passenger_scene.session.ended_1697||!music_context.main_mode||passenger_scene.session.clock.remaining_16a8||
                !result_live_active||!result_live_flow.complete||result_customers!=1||
                passenger_scene.session.fare.total_whole_1698!=earned_whole||
                passenger_scene.session.fare.total_fraction_169c!=earned_fraction||
                memcmp(&earned_position,&car.world.height.position,sizeof earned_position)||
                !freeze_captured||!earned_result_captured||(!name_live_active&&!profile_live_active&&!game_over_complete))fail("Earned fare natural results transition failed.");
            printf("EARNED SESSION: %u unforced countdown/endgame updates, cash%ld.%02ld customers%u retained; %s reached.\n",natural_ticks,(long)earned_whole,(long)earned_fraction,result_customers,name_live_active?"name entry":profile_live_active?"profile":"completed Game Over (settings not loaded)");
            if(!profile_live_active)fail("Earned session profile dependency unavailable.");
            for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
            profile_tick(0);profile_tick(1);profile_tick(0);
            if(profile_flow.choice.editing){profile_tick(1);profile_tick(0);}
            for(unsigned n=0;n<100&&!profile_flow.complete;n++)profile_tick(0);
            if(!profile_commit_done||!profile_persist_requested||!profile_flow.choice.choice)fail("Earned profile registration failed.");
            unsigned earned_profile=profile_flow.choice.choice-1;
            const uint8_t *earned_history=native_settings+12+12*native_settings[4]+208*earned_profile;
            uint32_t earned_record=(uint32_t)earned_history[0]|(uint32_t)earned_history[1]<<8|(uint32_t)earned_history[2]<<16|(uint32_t)earned_history[3]<<24;
            uint32_t expected_record=((uint32_t)earned_whole<<15)|((uint32_t)earned_fraction<<8)|((uint32_t)native_game_driver<<5)|((uint32_t)result_class<<1)|1;
            if(earned_record!=expected_record)fail("Earned profile history lost original fare/driver/class.");
            uint8_t earned_settings[512];memcpy(earned_settings,native_settings,512);
            if(native_save_path){
                uint8_t disk_settings[512],disk_rows[800];memcpy(disk_rows,native_score_rows,800);
                if(native_save_write_status||taxi_save_file_load(native_save_path,disk_settings,disk_rows)||memcmp(disk_settings,earned_settings,512)||memcmp(disk_rows,native_score_rows,800))fail("Earned profile disk roundtrip failed.");
                puts("EARNED DISK SAVE: exact512 profile/settings bytes and100 runtime ranking rows retained after reload.");
            }
            for(unsigned n=0;n<20;n++)profile_tick(1);
            if(memcmp(earned_settings,native_settings,512))fail("Earned profile commit repeated.");
            startup_tick(0);
            if(startup_stage!=9||profile_live_active||result_live_active||normal_live_flow.mask!=1)fail("Earned profile main return failed.");
            startup_tick(0); /* Release retained Confirm bank before a fresh press. */
            for(unsigned n=0;n<100&&normal_live_flow.fade.active_177b;n++)startup_tick(0);
            startup_tick(0);
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=8)fail("Earned main to mode failed.");
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=1)fail("Earned mode to city failed.");
            startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
            if(startup_stage!=2)fail("Earned city to driver failed.");
            startup_tick(1);for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
            /* Reentered city menu begins at city0; confirming it legitimately
               changes source settings byte4. Retained profile/history and all
               other bytes must still match the earned session exactly. */
            earned_settings[4]=0;
            if(scene_index!=0||native_settings[4]!=0)fail("Earned reentry city selection failed.");
            if(startup_stage!=3||profile_live_active||result_live_active||sprite_source_count!=(scene_index<2?1618u:1506u)||startup_loading_rendered!=52||memcmp(native_settings,earned_settings,512)||memcmp(session_settings_snapshot,earned_settings,512))fail("Earned profile new session lost settings/resources.");
            route_probe_active=0;
            for(unsigned n=0;n<60;n++)drive_tick((TaxiXboxSample){0},1);
            if(simulation_error||passenger_scene.session.fare.total_whole_1698||passenger_scene.session.end.completed_1692)fail("Earned profile second session reset failed.");
            puts("EARNED PROFILE: original earned cash/driver/class stored once in profile history; main/mode/city/driver/loading reentry and60 new native driving updates passed. Disk persistence only when explicitly configured.");
            route_probe_active=route_probe_brake=script_steer=0;
            puts("INPUT ROUTE PROBE: native pickup and original-route delivery/fare verified using only acceleration/steering/brake; no teleport/forced flags or settlement.");
        }
        if(startup_timed_check){
            int32_t initial_remaining=passenger_scene.session.clock.remaining_16a8;
            unsigned elapsed=0,gated=0;
            while(!passenger_scene.session.ended_1697&&elapsed<40000){
                TaxiPassengerFrameInput frame={.taxi=car.world.height.position,.speed=0,.delta=1,
                    .material=car.world.material_e60,.camera_word_10=taxi_camera_transform_yaw(camera_heading,camera_yaw_offset)};
                TaxiPassengerFrameEffects effects;
                int32_t previous=passenger_scene.session.clock.remaining_16a8;
                int gate=passenger_scene.session.gate_1700!=0;
                if(taxi_passenger_scene_frame(&passenger_scene,&passenger_asset,&world,&runtime.trig,&frame,&effects))fail("Timed full countdown frame failed.");
                elapsed++;gated+=(unsigned)gate;
                if(passenger_scene.session.clock.remaining_16a8!=previous-(gate?0:1))fail("Timed countdown cadence changed.");
                if(effects.main_expired!=(passenger_scene.session.clock.remaining_16a8==0))fail("Timed expiry boundary mismatch.");
            }
            if(!passenger_scene.session.ended_1697||passenger_scene.session.clock.remaining_16a8||elapsed-gated!=(unsigned)initial_remaining)fail("Timed full countdown did not expire exactly.");
            printf("TIMED COUNTDOWN: %u stationary native passenger frames (%u gated), exact clock decrement and unforced zero expiry; not a full driving/fare or wall-time run.\n",elapsed,gated);
        }
        if(startup_reload_check){
            unsigned expected_sources=sprite_source_count;
            for(unsigned session=0;session<4;session++){
                native_settings[4]=(uint8_t)(session%2);driver_flow.choice.driver=(uint8_t)session;
                startup_loading_begin();
                for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
                if(startup_stage!=3||scene_index!=session%2||native_game_driver!=session||sprite_source_count!=expected_sources||startup_loading_rendered!=52)fail("Repeated startup resource ownership failed.");
                for(unsigned i=0;i<sprite_source_count;i++){
                    Texture *source=sprite_sources+i;size_t bytes=(size_t)source->width*source->height*4;
                    unsigned char *pixels=malloc(bytes);if(!pixels)fail("Reload readback allocation failed.");
                    glBindTexture(GL_TEXTURE_2D,source->id);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
                    if(memcmp(pixels,source->pixels,bytes))fail("Reload sprite source mismatch.");free(pixels);
                }
                for(unsigned n=0;n<60;n++)drive_tick((TaxiXboxSample){0},1);
                if(simulation_error)fail("Reloaded native driving failed.");
            }
            printf("STARTUP RELOAD: four explicit scene/driver handoffs, %u retained sprites each, all GPU bytes and60 native updates per load passed; original parent reentry remains pending.\n",expected_sources);
        }
        if(startup_expiry_check) {
            passenger_scene.session.clock.remaining_16a8=1;passenger_scene.session.gate_1700=0;
            for(unsigned n=0;n<1000&&!result_live_active;n++)drive_tick((TaxiXboxSample){0},1);
            if(!result_live_active||!results_preview||native_game_driver!=chosen)fail("Startup timeout-to-results failed.");
            for(unsigned n=0;n<34;n++)drive_tick((TaxiXboxSample){0},1);
            printf("STARTUP EXPIRY: forced clock1 through native timeout/fade/results, driver%u retained; not a naturally earned run.\n",native_game_driver);
            if(startup_profile_check) {
                TaxiPoint fixed=car.world.height.position;
                for(unsigned n=0;n<4000&&!profile_live_active;n++)drive_tick((TaxiXboxSample){0},1);
                if(!profile_live_active||name_live_active||native_game_driver!=chosen)fail("Startup results/GameOver/profile transition failed.");
                if(memcmp(&fixed,&car.world.height.position,sizeof fixed))fail("Startup endgame moved the world.");
                for(unsigned n=0;n<100&&profile_flow.fade.active_177b;n++)profile_tick(0);
                profile_tick(0);profile_tick(128);profile_tick(0);profile_tick(1);profile_tick(0);
                for(unsigned n=0;n<100&&!profile_flow.complete;n++)profile_tick(0);
                if(!profile_commit_done||profile_persist_requested||memcmp(native_settings,session_settings_snapshot,512))fail("Startup skip profile snapshot restoration failed.");
                startup_tick(0);
                if(startup_stage!=9||profile_live_active||result_live_active||game_over_active||name_live_active||results_preview||normal_live_flow.mask!=1||taxi_normal_menu_flow_cap(&normal_live_flow)!=0)fail("Completed profile main return ownership failed.");
                uint8_t retained_reload=normal_live_flow.fade.reload_1772;
                if(retained_reload!=profile_flow.fade.reload_1772)fail("Profile menu fade interval lost.");
                for(unsigned n=0;n<100&&normal_live_flow.fade.active_177b;n++)startup_tick(0);
                if(normal_live_flow.fade.active_177b||startup_stage!=9)fail("Completed profile main entry failed.");
                startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
                if(startup_stage!=8)fail("Post-profile mode reentry failed.");
                startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
                if(startup_stage!=1)fail("Post-profile city reentry failed.");
                startup_tick(1);for(unsigned n=0;n<90;n++)startup_tick(0);
                if(startup_stage!=2||driver_preview)fail("Post-profile driver reset failed.");
                startup_tick(1);for(unsigned n=0;n<120&&startup_stage!=3;n++)startup_tick(0);
                if(startup_stage!=3||native_game_driver||sprite_source_count!=(scene_index<2?1618u:1506u)||startup_loading_rendered!=52||profile_live_active||result_live_active)fail("Post-profile new session ownership failed.");
                for(unsigned n=0;n<60;n++)drive_tick((TaxiXboxSample){0},1);
                if(simulation_error)fail("Post-profile new session driving failed.");
                puts("STARTUP PROFILE: forced timeout/results/GameOver/profile Skip/main return, world freeze and512-byte restoration passed; not a natural fare.");
            }
        }
    }
native_snapshot_ready:
    if(native_marker_fixture){
        if(!image_path||!drive_mode||!passenger_diagnostic)fail("Marker fixture requires native gameplay snapshot.");
        unsigned selected=passenger_scene.count;
        for(unsigned i=0;i<passenger_scene.count;i++)if((passenger_scene.customers[i].bytes[0]&3)==1&&passenger_scene.customers[i].bytes[1]<8){selected=i;break;}
        if(selected==passenger_scene.count)fail("Marker fixture lacks source customer.");
        TaxiPoint actor;memcpy(&actor,passenger_scene.customers[selected].bytes+16,12);
        car.world.height.position=actor;car.world.height.position.z-=4096;
        previous_position=car.world.height.position;
        camera_heading=previous_camera_heading=0;camera_pitch=previous_camera_pitch=0;
        camera_yaw_offset=previous_camera_yaw_offset=0;
        passenger_scene.pickup.pending_1704=0;passenger_scene.session.trip.active_1694=0;
        passenger_scene.marker_phase.frame_5f29=3;
        city_fade_cap=31;
        printf("MARKER CONTROLLED FIXTURE: taxi relocated near source customer%u; no earned pickup or ROM pixel claim.\n",selected);
    }
    if(challenge_bound_drive_preview){
        if(turn_input_replay_path)printf("BOUND INPUT REPLAY: consumed%u/%u, position%d,%d,%d speed%d destination%d active%u completed%u; actual native commands without placement/outcome injection.\n",turn_input_index,turn_input_count,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544,passenger_scene.pickup.destination_index,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692);
        if(simulation_error||world.nv!=807||world.np!=737||drive_context.driver.motion.orientation.submode!=5||passenger_scene.count!=1)fail("Bound live ownership/physics failed.");
        printf("BOUND LIVE: remaining%d ended%u success%u warnings%u actorflags%u destination%d active%u completed%u; native preview, earned completion and independent GBA fidelity unverified.\n",live_bound_state.remaining_1660,drive_context.height.flag_1697,drive_context.height.flag_153c,live_bound_warnings,passenger_scene.customers[0].bytes[0],passenger_scene.pickup.destination_index,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692);
    }
    if(challenge_turn_drive_preview){
        if(simulation_error||world.nv!=646||world.np!=497||drive_context.driver.motion.orientation.submode!=4||passenger_scene.count!=1||(passenger_scene.session.settings.bytes[0]&224)!=128)fail("Turn livepreview ownership/physics failed.");
        if(turn_input_replay_path)printf("TURN INPUT REPLAY: consumed%u/%u, destination%d active%u completed%u; raw native inputs, no placement/outcome injection.\n",turn_input_index,turn_input_count,passenger_scene.pickup.destination_index,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692);
        printf("TURN LIVE: remaining%d ended%u success%u warnings%u actorflags%u destination%d active%u completed%u; native preview, earned delivery and independent GBA fidelity unverified.\n",live_turn_state.remaining_165c,drive_context.height.flag_1697,drive_context.height.flag_153c,live_turn_warnings,passenger_scene.customers[0].bytes[0],passenger_scene.pickup.destination_index,passenger_scene.session.trip.active_1694,passenger_scene.session.end.completed_1692);
        if(script_ticks>=2400&&!turn_retry_check){
            if(turn_input_replay_path&&drive_context.height.flag_153c){
                if(!drive_context.height.flag_1697||live_turn_state.remaining_165c<=0||passenger_scene.session.trip.active_1694||passenger_scene.session.end.completed_1692!=1||passenger_scene.pickup.destination_index!=-1)fail("Turn input-only delivery ownership failed.");
                printf("PASS Turn input-only native delivery: remaining%d rank%u completed1; profile/save and independent GBA fidelity remain unverified.\n",live_turn_state.remaining_165c,drive_context.height.flag_153d);
            }else{
                if(live_turn_state.remaining_165c||!drive_context.height.flag_1697||drive_context.height.flag_153c||live_turn_warnings!=1)fail("Turn native timeout/countdown warning failed.");
                puts("PASS Turn native driven timeout and original single warning; not an earned delivery.");
            }
        }
    }
    if(challenge_ball_drive_preview){
        if(simulation_error||world.nv!=510||world.np!=217||drive_context.driver.motion.orientation.submode!=2||!live_ball_ticks||!ball_textures[0])fail("Ball livepreview ownership/physics failed.");
        draw(width,height);
        if(!live_ball_submissions)fail("Ball livepreview produced no visible actor packets.");
        printf("PASS Ball live native course/vehicle/controller/render/HUD: %u ticks remaining%d popped%d ended%u, %u balloon submissions. Native preview; independent GBA fidelity still unverified.\n",live_ball_ticks,live_ball_state.remaining_1558,live_ball_state.popped_1556,drive_context.height.flag_1697,live_ball_submissions);
        if(ball_input_replay_path)printf("BALL INPUT REPLAY: consumed%u/%u originalaction/stick inputs, popped%d remaining%d position%d,%d,%d speed%d; no contactfixture/teleport.\n",ball_input_index,ball_input_count,live_ball_state.popped_1556,live_ball_state.remaining_1558,car.world.height.position.x,car.world.height.position.y,car.world.height.position.z,car.world.speed_544);
        if(script_ticks>=2904&&!ball_retry_check){
            if(live_ball_state.remaining_1558||!drive_context.height.flag_1697||drive_context.height.flag_153c||!live_flag_warning_palette)fail("Ball live timeout/countdown warning failed.");
            puts("PASS Ball native driven timeout and original warning state; all20 earned success/profile pending.");
        }
    }
    if(flag_scene_render_check){if(!image_path)fail("Flag scene fixture requires snapshot.");check_flag_scene_pixels(width,height);}
    if(ball_gpu_check){if(!image_path)fail("Ball GPU check requires snapshot.");check_ball_gpu(width,height);draw(width,height);}
    if(native_tip_bitmap_gpu_check){if(!image_path||!native_tip_texture)fail("Tip GPU check requires gameplay snapshot assets.");check_tip_bitmap_gpu();draw(width,height);}
    if(flag_gpu_check){if(!image_path)fail("Flag GPU check requires snapshot.");check_flag_gpu(width,height);draw(width,height);}
    if(native_taxi_facing_check){check_native_taxi_facing(width,height);check_native_taxi_half_overlap(width,height);check_native_obj_shader();check_native_effect_shader();check_native_obj_blending();check_live_smoke_composition();draw(width,height);}
    if(title_entry_check){if(!image_path||startup_mode||drive_mode)fail("Title entry check requires standalone snapshot.");load_native_title_check();}
    if (image_path) {
        RECT client; GetClientRect(window,&client);
        if (client.right!=width || client.bottom!=height) fail("4K client buffer size mismatch.");
        if(native_pickup_only_check){check_attribute_material_shader(width,height);check_cached_texture_flat_packet(width,height);}
        if(native_pickup_only_check||native_axis_source_check){
            if(!drive_mode||scene_index>1)fail("Axis source check requires a loaded city driving scene.");
            draw(width,height);check_flat_scene_packet(width,height);check_marker_source_pixels(width,height);check_axis_source_packet(width,height,0);check_axis_source_packet(width,height,1);check_axis_source_packet(width,height,2);check_axis_source_packet(width,height,3);check_axis_source_packet(width,height,4);check_unshaded_bitmap_coverage();check_flat_bitmap_coverage();check_material_bitmap_coverage(width,height);check_axis_continuous_table(width,height);check_axis_continuous_coverage(width,height);check_shade_lookup_boundaries(width,height,0);check_shade_lookup_boundaries(width,height,1);check_special_lookup_bytes(width,height);for(unsigned family=0;family<3;family++)check_special_source_packet(width,height,family);
        }
        if(native_marker_fixture){
            size_t length=(size_t)width*height*4;
            unsigned char *without=malloc(length),*with=malloc(length);if(!without||!with)fail("Marker pixel capture allocation failed.");
            passenger_marker_mask=1;draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,without);
            passenger_marker_mask=0;draw(width,height);glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,with);
            unsigned changed=0;for(size_t i=0;i<length;i+=4)if(memcmp(without+i,with+i,3))changed++;
            free(without);free(with);
            if(!queued_passenger_marker_count||!changed||glGetError()!=GL_NO_ERROR)fail("Marker fixture has no visible native marker pixels.");
            printf("PASS MARKER PIXELS: %u queued, %u changed pixels at%dx%d; only marker drawing toggled, controlled fixture, not original ROM framebuffer comparison.\n",queued_passenger_marker_count,changed,width,height);
        }
        if(pause_freeze_check)check_pause_gameplay_freeze();
        if(pause_host_frame_check)check_pause_host_frames();
        if(pause_overlay_check)pause_overlay_fixture();
        if(reference_render_pose_path){
            FILE *f=fopen(reference_render_pose_path,"rb");int v[16];if(!f)fail("Reference pose fixture missing.");for(unsigned n=0;n<16;n++)if(fscanf(f,"%d",v+n)!=1)fail("Reference pose fixture truncated.");char extra;if(fscanf(f," %c",&extra)==1||ferror(f)||fclose(f))fail("Reference pose fixture malformed.");
            presented_pose=(TaxiRenderPose){{v[0],v[1],v[2]},{v[3],v[4],v[5]},{v[6],v[7],v[8]},{v[9],v[10],v[11]}};presented_pose_valid=1;presented_bracket=(TaxiRenderBracket){0};presentation_offset_history[0]=(TaxiCameraOffsets){v[12],v[13],v[14]};camera_base=v[15];
            printf("REFERENCE RENDER POSE: imported presentation values only; zero gameplay updates, not a playable original-state import.\n");
        }
        draw(width,height);
        if(taxi_source_capture_path&&!taxi_source_capture_written)fail("Snapshot did not draw a source taxi; capture was not produced.");
        if(source_scene_gpu_requested)check_combined_source_gpu();
        if(taxi_live_pixels_check){check_live_taxi_oam_pixels();draw(width,height);}
        if(taxi_owned_lifecycle_check){check_owned_sprite_lifecycle();draw(width,height);}
        if(pause_overlay_check){if(!pause_overlay_draws||glGetError()!=GL_NO_ERROR)fail("Pause overlay failed.");printf("PASS diagnostic original pause overlay: dialog%u, %u requests drawn at%dx%d; full freeze/palette/audio host lifecycle not yet connected.\n",pause_overlay_dialog,native_pause_frame.draw_count,width,height);}
        if(pause_scene_rgba_check)check_pause_scene_rgba(width,height);
        if(render_state_check)check_render_state(width,height);
        if(native_effect_boundary_capture_path&&!jump_effect_render_check)capture_live_effect_source_inputs(script_ticks);
        if(jump_effect_render_check){
            capture_live_effect_source_inputs(script_ticks);
            check_live_effect_pixels(width,height);
            if(!image_path||!drive_mode||simulation_error||!native_effect_request_count||!native_effect_emissions||!(native_effect_drawn+native_effect_blended)||native_effect_blend_pending||glGetError()!=GL_NO_ERROR)
                fail("Live Jump effect render observation lacked active drawn effects.");
            printf("PASS live Jump effect render observation: %d ticks, %u emissions, %d current requests, %u opaque draws, %u smoke draws; original scripted dash inputs, no injected effect state; not ROM pixel/trajectory oracle.\n",script_ticks,native_effect_emissions,native_effect_request_count,native_effect_drawn,native_effect_blended);
        }
        if(queue_occlusion_check){check_queue_occlusion(width,height);draw(width,height);}
        if(native_pickup_only_check && traffic_preview && recovered_camera) {

            printf("PASS native4K original eight-pattern attribute shader: %u polygon draws.\n",native_attribute_pattern_draws);
            if(!native_flat_pattern_draws)fail("No native original flat-pattern material drawn.");
            printf("PASS native4K flat-pattern callback integration: %u polygon draws; original palette pair/row parity, high-resolution geometry.\n",native_flat_pattern_draws);
        }
        glFinish(); snapshot(image_path,width,height);
        if(drive_mode)printf("EFFECT REQUESTS: %d cached original sprite requests; native opaque/qualified smoke submission enabled.\n",native_effect_request_count);
        if(drive_mode)printf("EFFECT DRAW: %u opaque, %u original8/8 smoke submissions, %u requests awaiting qualified background.\n",native_effect_drawn,native_effect_blended,native_effect_blend_pending);
        if(drive_mode)printf("EFFECT BACKGROUND: %u pre-OBJ captures, %u byte-exact GPU checks, %u qualified bitmap-coverage checks; fallback coverage unverified.\n",native_effect_background_captures,native_effect_background_checks,native_effect_background_coverage_checks);
        if(drive_mode)printf("EFFECT TRACE: %u actor-tail emissions, max %u active records, %u frames with projected requests; collision/expiry spawns excluded from emission count.\n",native_effect_emissions,native_effect_max_active,native_effect_visible_frames);
        if(traffic_preview&&recovered_camera&&passenger_diagnostic){
            printf("PASSENGER MARKERS: %u visible queued requests; source assets, native projection; full ROM pixels/palette unverified.\n",queued_passenger_marker_count);
            printf("PASSENGER QUEUE: %u original customer requests in captured frame; occluded requests may not leave visible pixels.\n",queued_passenger_count);
        }
        if(flag_scene_render_check){if(!flag_scene_submissions)fail("Flag source course fixture had no marker submission.");printf("PASS Flag source course marker queued: %u submissions; explicit controlled camera, original bitmap and course palette, continuous shader.\n",flag_scene_submissions);}
        QueryPerformanceCounter(&start);
        for (int frame=0; frame<120; ++frame) { yaw+=0.05f; draw(width,height); glFinish(); }
        QueryPerformanceCounter(&end);
        if (glGetError()!=GL_NO_ERROR) fail("OpenGL render test failed.");
        TaxiRewardStats reward_stats=taxi_reward_session_stats();
        printf("NORMAL MOVEMENT GATES: %u held updates, %u boarding/%u delivery-delay; taxi position unchanged each held gear-only update, existing effect pool continues; full original frame equivalence unverified.\n",native_gate_holds,native_gate_boarding_holds,native_gate_delivery_holds);
        printf("REWARD COVERAGE: %u updates, %u requests, %u created tips, %u expirations, %u active updates; native observations, original full-frame equivalence unverified.\n",reward_stats.updates,reward_stats.requests,reward_stats.created,reward_stats.expired,reward_stats.active_updates);
        printf("PASS: %u vertices, %u polygons; %dx%d native render; 120-frame offscreen throughput %.1f fps (not gameplay or display pacing).\n",
            vertex_count,face_count,width,height,120.0*frequency.QuadPart/(end.QuadPart-start.QuadPart));
    } else {
        ShowWindow(window,live_4k_render_check?(visible_4k_check?SW_SHOWNOACTIVATE:SW_HIDE):SW_SHOW);
        if(live_4k_render_check){RECT observed;GetClientRect(window,&observed);printf("4K WINDOW CONTROL: requested%s actual_visible%d client%ldx%ld; no monitor scanout claim.\n",visible_4k_check?"visible":"hidden",IsWindowVisible(window)!=0,(long)observed.right,(long)observed.bottom);if((IsWindowVisible(window)!=0)!=visible_4k_check)fail("4K window visibility control failed.");} if(start_fullscreen) fullscreen_toggle(window); QueryPerformanceCounter(&start);
        FramePacer pacer={start.QuadPart,frequency.QuadPart,1,0};
        int running=1;
        double visible_intervals[4096];unsigned visible_count=0;
        LARGE_INTEGER visible_previous={0};double maximum_swap_ms=0;
        double visible_update_ms=0,visible_draw_ms=0,visible_swap_ms=0;
        if(audio_smoke_frames){
            MONITORINFOEXA monitor={0};monitor.cbSize=sizeof monitor;
            DEVMODEA display={0};display.dmSize=sizeof display;
            if(GetMonitorInfoA(MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST),(MONITORINFO *)&monitor)&&
               EnumDisplaySettingsA(monitor.szDevice,ENUM_CURRENT_SETTINGS,&display))
                printf("DISPLAY DIAGNOSTIC: monitor%s desktop%lux%lu refresh%luHz; reported mode, not measured scanout.\n",
                    monitor.szDevice,(unsigned long)display.dmPelsWidth,(unsigned long)display.dmPelsHeight,(unsigned long)display.dmDisplayFrequency);
        }
        if(drive_mode||startup_mode){
            /* Logical source audio is independent of the optional Windows device. */
            TaxiGameSettings audio_settings;taxi_game_settings_defaults(&audio_settings);
            /* Source2668a..26694 installs saved settings byte2 at020075f5. */
            if(!native_settings_loaded){
                /* Direct driving skips normal boot05418..5434. Apply its
                   recovered default volumes and zero1ea5 domain gate here. */
                audio_settings_state.music_volume=audio_settings.bytes[1];
                audio_settings_state.effects_volume=audio_settings.bytes[2];
                if(taxi_audio_settings_start_gate(&audio_settings_state,0))fail("Direct driving audio gate failed.");
            }
            audio_global_volume=native_settings_loaded?native_settings[2]:audio_settings.bytes[2];
            char audio_path[MAX_PATH];DWORD length=GetModuleFileNameA(NULL,audio_path,MAX_PATH);
            if(!length||length>=MAX_PATH)fail("Audio asset path failed.");
            char *separator=strrchr(audio_path,'\\');if(!separator)fail("Audio asset directory failed.");
            snprintf(separator+1,MAX_PATH-(size_t)(separator+1-audio_path),"../assets/audio");
            if(taxi_audio_asset_load(&audio_bank,audio_path))fail("Original audio asset initialization failed.");
            if(audio_preview&&!audio_output_disabled&&taxi_audio_host_open(&audio_host,21024))fail("Native audio output initialization failed.");
            snprintf(separator+1,MAX_PATH-(size_t)(separator+1-audio_path),"../assets/music/original-module.bin");
            FILE *music_file=fopen(audio_path,"rb");music_bytes=malloc(181241);
            if(!music_file||!music_bytes||fread(music_bytes,1,181241,music_file)!=181241||fgetc(music_file)!=EOF)fail("Original music asset load failed.");
            fclose(music_file);
            if(taxi_music_asset_index(&music_asset,music_bytes,181241))fail("Original music asset indexing failed.");
            snprintf(separator+1,MAX_PATH-(size_t)(separator+1-audio_path),"../assets/music/pitch-table.bin");
            music_file=fopen(audio_path,"rb");
            if(!music_file||fread(music_pitch,1,1536,music_file)!=1536||fgetc(music_file)!=EOF)fail("Original music pitch load failed.");
            fclose(music_file);
            audio_live=1;
            /* The direct-driving shortcut has no startup controller handoff.
               Audio preview uses the recovered engine owner; the complete
               passenger/voice controller still requires ordinary startup. */
            if(drive_mode&&!startup_mode&&audio_preview)recovered_engine_audio=1;
        }
        if(reference_driving_path){if(taxi_reference_driving_load(&reference_driving,reference_driving_path))fail("Reference schedule/baseline unavailable or invalid.");install_reference_driving();taxi_render_pose_clear(&presentation_history);SetWindowTextW(window,L"Native source-reference driving test - not a playable port");printf("REFERENCE DRIVE FIXTURE: %u native physics updates, recorded variable schedule; offline baseline import, expected outputs validate only.\n",reference_driving.count);}
        /* Asset initialization is not elapsed gameplay. Start the native clock
           and presentation deadlines together after potentially blocking I/O. */
        if(native_elapsed_time){
            /* Prime lazy GPU uploads before starting elapsed gameplay time. */
            RECT warm_client;GetClientRect(window,&warm_client);draw(warm_client.right,warm_client.bottom);
            if(!SwapBuffers(dc))fail("Live timing initialization presentation failed.");
        }
        QueryPerformanceCounter(&start);pacer=(FramePacer){start.QuadPart,frequency.QuadPart,1,0};
        /* Cold ordinary boot leaves global physics delta zero. Intro BA44
           installs frame time only after the fade and phase-1 gates allow it.
           Reentries retain their existing owner; do not reseed per frame. */
        if(native_elapsed_time&&startup_mode)gameplay_global_delta=0;
        LARGE_INTEGER host_previous=start;live_host_time=(TaxiHostTime){.frame={.delta_3b00=1}};live_input_delta=1;
        int host_focus=(GetForegroundWindow()==window||audio_smoke_frames||startup_audio_smoke);
        unsigned host_reset_seen=host_reset_revision,host_rebases=0;int respawn_probe_done=0;
        unsigned focus_probe_frame=0,focus_held_frames=0,focus_saved_audio=0,focus_saved_updates=0;
        TaxiDriveWorld focus_saved_car={0};TaxiHostTime focus_saved_clock={0};TaxiAudioChannel focus_saved_channels[4]={0};
        unsigned window_smoke_stage=0;RECT window_smoke_original;GetWindowRect(window,&window_smoke_original);
        LONGLONG trace_pump_ticks=0,trace_pacer_ticks=0;double trace_deadline_late_ms=0;
        if(reference_driving_path)prepare_timestamped_pose(host_focus);
        while (running) {
            LARGE_INTEGER trace_loop_start,trace_pump_end,trace_pacer_end;if(performance_trace)QueryPerformanceCounter(&trace_loop_start);
            if(audio_host&&taxi_audio_host_pump(audio_host)<0)fail("Native audio output failed.");
            pump_window_messages();if(performance_trace){QueryPerformanceCounter(&trace_pump_end);trace_pump_ticks+=trace_pump_end.QuadPart-trace_loop_start.QuadPart;}if(window_quit_requested)running=0;
            if (!running) break;
            if(respawn_clock_smoke&&!respawn_probe_done&&audio_presented>=120){
                live_host_time.frame.delta_3b00=11; /* stale-step stress fixture */
                reset_drive();Sleep(120);respawn_probe_done=1;
            }
            QueryPerformanceCounter(&end);
            if(native_elapsed_time&&host_reset_seen!=host_reset_revision){
                host_previous=end;start=end;host_reset_seen=host_reset_revision;
                live_host_time.frame.delta_3b00=1;live_input_delta=1;live_host_ready=1;++host_rebases;
                if(respawn_clock_smoke&&respawn_probe_done&&live_host_time.frame.delta_3b00!=1)
                    fail("Respawn retained stale elapsed step.");
            }
            LONGLONG trace_deadline=pacer.epoch+(LONGLONG)(pacer.slot/60)*pacer.frequency+(LONGLONG)(pacer.slot%60)*pacer.frequency/60;
            int trace_due=frame_due(&pacer,end);if(performance_trace){QueryPerformanceCounter(&trace_pacer_end);trace_pacer_ticks+=trace_pacer_end.QuadPart-end.QuadPart;}if(!trace_due)continue;trace_deadline_late_ms=(end.QuadPart-trace_deadline)*1000.0/frequency.QuadPart;
            if(window_surface_smoke&&window_smoke_stage<4&&audio_presented>=60*(window_smoke_stage+1)){
                TaxiDriveWorld before_window=car;TaxiHostTime before_time=live_host_time;int old_fullscreen=fullscreen;
                if(window_smoke_stage==0){RECT desired={0,0,320,400};AdjustWindowRect(&desired,(DWORD)GetWindowLongPtrW(window,GWL_STYLE),FALSE);if(!SetWindowPos(window,NULL,0,0,desired.right-desired.left,desired.bottom-desired.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE))fail("Live resize smoke failed.");}
                else if(window_smoke_stage==1||window_smoke_stage==2)fullscreen_toggle(window);
                else if(!SetWindowPos(window,NULL,window_smoke_original.left,window_smoke_original.top,window_smoke_original.right-window_smoke_original.left,window_smoke_original.bottom-window_smoke_original.top,SWP_NOZORDER|SWP_NOACTIVATE))fail("Live window restore failed.");
                if(memcmp(&before_window,&car,sizeof car)||memcmp(&before_time,&live_host_time,sizeof live_host_time))fail("Window transition mutated source driving/time state.");
                if((window_smoke_stage==1||window_smoke_stage==2)&&fullscreen==old_fullscreen)fail("Fullscreen transition did not toggle.");RECT observed;GetClientRect(window,&observed);if(observed.right<=0||observed.bottom<=0)fail("Live transition client invalid.");if(window_smoke_stage==0&&(observed.right!=320||observed.bottom!=400))fail("Live resize client differs.");
                printf("NATIVE4K LIVE WINDOW: stage%u presentation%u client%ldx%ld fullscreen%d; source state unchanged.\n",window_smoke_stage,audio_presented,(long)observed.right,(long)observed.bottom,fullscreen);window_smoke_stage++;
            }
            double dt=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart;
            start=end; if (dt>0.1) dt=0.1;
            int active_host=(GetForegroundWindow()==window||audio_smoke_frames||startup_audio_smoke);
            if(focus_suspend_smoke){
                ++focus_probe_frame;
                active_host=focus_probe_frame<=120||focus_probe_frame>180;
                if(focus_probe_frame==121){
                    focus_saved_car=car;focus_saved_clock=live_host_time;
                    memcpy(focus_saved_channels,audio_channels,sizeof audio_channels);
                    focus_saved_audio=audio_frames;focus_saved_updates=live_host_updates;
                }
                if(focus_probe_frame>=121&&focus_probe_frame<=181){
                    if(memcmp(&car,&focus_saved_car,sizeof car)||memcmp(&live_host_time,&focus_saved_clock,sizeof live_host_time)||
                       memcmp(audio_channels,focus_saved_channels,sizeof audio_channels)||audio_frames!=focus_saved_audio||live_host_updates!=focus_saved_updates)
                        fail("Focus suspension advanced source state.");
                    if(focus_probe_frame<=180)focus_held_frames++;
                }
            }
            if(native_elapsed_time&&active_host!=host_focus){
                /* PC focus suspension advances no source game/mixer state.
                   Rebase wall time before the first resumed gameplay call. */
                host_previous=end;
                if(active_host){live_host_time.frame.delta_3b00=1;live_input_delta=1;live_host_ready=1;}
                if(audio_host&&(active_host?taxi_audio_host_resume(audio_host):taxi_audio_host_stop(audio_host)))
                    fail("Native focus audio transport failed.");
                host_focus=active_host;
            }
            TaxiXboxSample pad=taxi_xbox_poll_any();
            if(startup_mode) {
                if((native_elapsed_time?host_focus:(GetForegroundWindow()==window||startup_audio_smoke))) {
                    uint32_t ticks=0;
                    if(native_elapsed_time&&startup_stage==3)ticks=live_host_ready?1:0;
                    else if(taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks))fail("Startup native clock failed.");
                    uint16_t menu_mask=startup_audio_smoke?0:name_device_mask(pad);
                    if(!startup_audio_smoke){
                        if(GetAsyncKeyState(VK_UP)&0x8000)menu_mask|=64;
                        if(GetAsyncKeyState(VK_DOWN)&0x8000)menu_mask|=128;
                    }
                    for(uint32_t i=0;i<ticks&&!window_quit_requested&&(!native_elapsed_time||startup_stage!=3||live_host_ready);i++){
                        if(startup_audio_smoke){
                            menu_mask=0;
                            if(startup_attract_audio_smoke){
                                if(startup_stage==14){attract_audio_seen=1;if(++attract_audio_drive_ticks==180)menu_mask=8;}
                                if(attract_audio_seen&&startup_stage==TAXI_STARTUP_TITLE_STAGE)attract_audio_return_ticks++;
                                startup_audio_tick++;
                            }else if(startup_game_audio_smoke){
                                if(startup_audio_tick==60||startup_audio_tick==300)menu_mask=1;
                                if(startup_return_audio_smoke){
                                    return_profile_mask=0;
                                    if(profile_live_active&&!profile_flow.fade.active_177b&&return_profile_input<5){
                                        if(return_profile_input==1)return_profile_mask=128;
                                        if(return_profile_input==3)return_profile_mask=1;
                                        return_profile_input++;
                                    }
                                    if(startup_stage==9&&music_tracker.state.playing)return_music_frames++;
                                }
                                startup_audio_tick++;
                            }else switch(startup_audio_tick++){
                            case 60:case 300:case 1200:case 1450:menu_mask=2;break;
                            case 550:case 554:case 800:case 804:case 808:case 1060:case 1080:case 1120:menu_mask=128;break;
                            case 560:case 820:case 1100:case 1140:menu_mask=1;break;
                            case 1070:menu_mask=32;break;
                            case 1090:menu_mask=16;break;
                            default:break;
                            }
                        }
                        int elapsed_gameplay=native_elapsed_time&&startup_stage==3;
                        startup_tick(menu_mask);
                        if(elapsed_gameplay){live_host_ready=0;live_host_updates++;}
                        else audio_frame();
                        if(startup_game_audio_smoke&&startup_audio_tick==900)
                            printf("AUDIO CHECKPOINT: tick900 frames%u PCMhash%016llx RNG%u mixed%u dispatches%u preFX%u.\n",audio_frames,(unsigned long long)audio_pcm_hash,passenger_scene.random_state,music_frames,music_dispatches,music_nonzero_samples);
                    }
                    if(window_quit_requested)break;
                    if(startup_clock_reset){QueryPerformanceCounter(&start);startup_clock_reset=0;}
                    if(startup_stage==TAXI_STARTUP_TITLE_STAGE)SetWindowTextW(window,L"Crazy Taxi - native title prototype");
                    else if(startup_stage==14)SetWindowTextW(window,L"Crazy Taxi - native demo prototype | A / Start: return to title");
                    else if(startup_stage>=8)SetWindowTextW(window,startup_stage==10?L"Crazy Taxi - Control":startup_stage==11?L"Crazy Taxi - Options":startup_stage==12?L"Crazy Taxi - Sound":startup_stage==9?L"Crazy Taxi - Mode Selection":L"Crazy Taxi - Rules");
                    else if(startup_stage>=5)SetWindowTextW(window,L"Loading selected city - native prototype");
                    else if(startup_stage>=3)SetWindowTextW(window,startup_stage==3?L"Native driving prototype - full original gameplay pending":L"City cancelled - parent menu transition pending");
                } else {memset(&city_input,0,sizeof city_input);memset(&driver_input,0,sizeof driver_input);memset(&normal_live_input,0,sizeof normal_live_input);}
            } else if(city_interactive) {
                if(GetForegroundWindow()==window) {
                    uint32_t ticks=0;if(taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks))fail("City native clock failed.");
                    uint16_t mask=name_device_mask(pad);
                    for(uint32_t i=0;i<ticks;i++)city_tick(mask);
                    if(city_flow.complete)SetWindowTextW(window,city_flow.choice.exiting==1?L"City confirmed - driver transition pending":L"City cancelled - parent menu transition pending");
                } else memset(&city_input,0,sizeof city_input);
            } else if(driver_interactive) {
                if(GetForegroundWindow()==window) {
                    uint32_t ticks=0;if(taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks))fail("Driver native clock failed.");
                    uint16_t mask=name_device_mask(pad);
                    for(uint32_t i=0;i<ticks;i++)driver_tick(mask);
                    if(driver_flow.complete)SetWindowTextW(window,driver_flow.choice.exiting==1?L"Driver confirmed - gameplay integration pending":L"Driver selection cancelled - parent menu integration pending");
                } else memset(&driver_input,0,sizeof driver_input);
            } else if(profile_interactive) {
                if(GetForegroundWindow()==window) {
                    uint32_t ticks=0;if(taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks))fail("Profile native clock failed.");
                    uint16_t mask=name_device_mask(pad);
                    if(GetAsyncKeyState(VK_UP)&0x8000)mask|=64;
                    if(GetAsyncKeyState(VK_DOWN)&0x8000)mask|=128;
                    if(pad.connected&&!(pad.active_low_keys&64))mask|=64;
                    if(pad.connected&&!(pad.active_low_keys&128))mask|=128;
                    for(uint32_t i=0;i<ticks;i++)profile_tick(mask);
                } else memset(&profile_input,0,sizeof profile_input);
            } else if(name_interactive) {
                if(GetForegroundWindow()==window) {
                    uint32_t ticks=0;if(taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks))fail("Name native clock failed.");
                    uint16_t mask=name_device_mask(pad);
                    for(uint32_t i=0;i<ticks;i++)name_interactive_tick(mask);
                } else memset(&name_live_input,0,sizeof name_live_input);
            } else if(drive_mode) {
                if(reference_driving_path)commit_reference_driving(host_focus);
                else if((native_elapsed_time?host_focus:(GetForegroundWindow()==window||audio_smoke_frames)) && !simulation_error) {
                    uint32_t ticks=0;
                    if(!native_elapsed_time&&taxi_clock_advance(&drive_clock,(uint64_t)(dt*1e9),&ticks)) fail("Native clock failed.");
                    /* Bounded background smoke uses ordinary scripted throttle,
                       without reading keyboard input from another application. */
                    if(native_elapsed_time){
                        if(live_host_ready){drive_tick(audio_smoke_frames?(TaxiXboxSample){0}:pad,audio_smoke_frames!=0);live_host_ready=0;live_host_updates++;}
                    }else for(uint32_t i=0;i<ticks;i++){drive_tick(audio_smoke_frames?(TaxiXboxSample){0}:pad,audio_smoke_frames!=0);audio_frame();}
                    if(simulation_error) SetWindowTextW(window,L"Simulation stopped on invalid state - R to respawn");
                    else if(passenger_diagnostic) {
                        const TaxiFareSession *s=&passenger_scene.session;
                        int32_t selected=passenger_scene.pickup.destination_index;
                        const uint8_t *record=selected>=0?passenger_asset.destinations[selected]:NULL;
                        int32_t remaining=taxi_destination_distance(selected,record,
                            car.world.height.position.x,car.world.height.position.z);
                        wchar_t status[256];
                        swprintf(status,256,L"Fare diagnostic | Time %d | Trip %d | Paid %d.%02d | Pending %d.%02d | Destination %d / distance %d / arrow %d | %ls",
                            s->clock.remaining_16a8/60,s->trip.time_16ac/60,
                            s->fare.total_whole_1698,s->fare.total_fraction_169c,
                            s->fare.pending_whole_16a0,s->fare.pending_fraction_16a4,
                            selected,remaining,guidance_angle<0?-1:guidance_angle/128,s->ended_1697?L"Time expired - R to reset":L"R: reset");
                        SetWindowTextW(window,status);
                    }
                }
            } else if (pad.connected) {
                if (abs(pad.steering)>7849) yaw+=(float)(pad.steering/32768.0*70*dt);
                distance+=(float)((pad.brake-pad.accelerator)/255.0*dt*2);
            }
            RECT client; GetClientRect(window,&client);int target_window_width=client.right,target_window_height=client.bottom;
            if(native_4k_target){target_begin();client.right=3840;client.bottom=2160;}
            LARGE_INTEGER draw_start;QueryPerformanceCounter(&draw_start);
            if(!startup_mode||startup_stage==3)prepare_timestamped_pose(host_focus);else{presented_pose_valid=0;taxi_render_pose_clear(&presentation_history);}
            TaxiDriveWorld render_before=car;TaxiDriveWorldContext render_context_before=drive_context;TaxiTrafficActor render_traffic_before[50];if(timestamped_presentation)memcpy(render_traffic_before,traffic_actors,sizeof traffic_actors);TaxiPassengerScene render_passengers_before;TaxiActorEffects render_effects_before;TaxiEffectSprite render_requests_before[9];int render_count_before=native_effect_request_count;TaxiCashGlyphs render_cash_before=cash_glyphs;uint16_t render_tip_frame_before=native_tip_frame;uint8_t render_panel_before[4096];unsigned render_panel_generation_before=native_panel_generation;if(timestamped_presentation)memcpy(render_panel_before,native_panel_pixels,sizeof native_panel_pixels);int32_t render_guidance_before=guidance_angle;unsigned render_turn_frame_before=turn_guidance_frame;if(timestamped_presentation){render_passengers_before=passenger_scene;render_effects_before=native_actor_effects;memcpy(render_requests_before,native_effect_requests,sizeof native_effect_requests);}
            if(live_4k_render_check&&(client.right!=3840||client.bottom!=2160))fail("Live4K framebuffer dimensions changed.");
            LARGE_INTEGER submit_start,submit_end,finish_end;double gpu_timestamp_ms=async_4k_render_check?-1:0;unsigned query_slot=async_4k_render_check?performance_frames:0;if(performance_trace&&query_slot>=profile_query_capacity)fail("GPU query frame capacity exceeded.");QueryPerformanceCounter(&submit_start);if(performance_trace)profile_query_counter(profile_queries[query_slot*2],0x8e28);
            draw(client.right,client.bottom);if(performance_trace)profile_query_counter(profile_queries[query_slot*2+1],0x8e28);QueryPerformanceCounter(&submit_end);
            if(live_4k_render_check){if(!async_4k_render_check)glFinish();if(glGetError()!=GL_NO_ERROR)fail("Live4K GPU rendering failed.");live_4k_submitted++;if(!async_4k_render_check)live_4k_completed++;}QueryPerformanceCounter(&finish_end);
            if(performance_trace&&!async_4k_render_check){uint64_t first,last;profile_query_result(profile_queries[0],0x8866,&first);profile_query_result(profile_queries[1],0x8866,&last);if(last<first||glGetError()!=GL_NO_ERROR)fail("GPU timestamp query failed.");gpu_timestamp_ms=(last-first)/1000000.0;}
            if(timestamped_presentation&&(memcmp(&render_before,&car,sizeof car)||memcmp(&render_context_before,&drive_context,sizeof drive_context)||memcmp(render_traffic_before,traffic_actors,sizeof traffic_actors)||memcmp(&render_passengers_before,&passenger_scene,sizeof passenger_scene)||memcmp(&render_effects_before,&native_actor_effects,sizeof native_actor_effects)||memcmp(render_requests_before,native_effect_requests,sizeof native_effect_requests)||render_count_before!=native_effect_request_count||render_guidance_before!=guidance_angle||render_turn_frame_before!=turn_guidance_frame||memcmp(render_panel_before,native_panel_pixels,sizeof native_panel_pixels)||render_panel_generation_before!=native_panel_generation||memcmp(&render_cash_before,&cash_glyphs,sizeof cash_glyphs)||render_tip_frame_before!=native_tip_frame))fail("Timestamped rendering changed driving state.");
            if(live_snapshot_path&&!live_snapshot_done&&presented_pose_valid&&live_host_updates>=live_snapshot_update){
                glFinish();snapshot(live_snapshot_path,client.right,client.bottom);live_snapshot_done=1;
                char metadata[MAX_PATH];if(snprintf(metadata,sizeof metadata,"%s.json",live_snapshot_path)>=(int)sizeof metadata)fail("Live capture metadata path too long.");
                FILE *capture=fopen(metadata,"wb");if(!capture)fail("Live capture metadata write failed.");
                fprintf(capture,"{\"width\":%ld,\"height\":%ld,\"gameplay_updates\":%u,\"source_counter\":%u,\"taxi\":[%d,%d,%d],\"camera\":[%d,%d,%d],\"vehicle_angles\":[%d,%d,%d],\"camera_angles\":[%d,%d,%d],\"scope\":\"Actual live native framebuffer at sampled historical pose; diagnostic capture, not original pixel or performance proof\"}\n",(long)client.right,(long)client.bottom,live_host_updates,live_host_time.counter,presented_pose.taxi.x,presented_pose.taxi.y,presented_pose.taxi.z,presented_pose.camera.x,presented_pose.camera.y,presented_pose.camera.z,presented_pose.vehicle_angles.x,presented_pose.vehicle_angles.y,presented_pose.vehicle_angles.z,presented_pose.camera_angles.x,presented_pose.camera_angles.y,presented_pose.camera_angles.z);
                if(ferror(capture)||fclose(capture))fail("Live capture metadata close failed.");printf("LIVE SNAPSHOT: update%u %ldx%ld %s.\n",live_host_updates,(long)client.right,(long)client.bottom,live_snapshot_path);
            }
            LARGE_INTEGER swap_start,swap_end;QueryPerformanceCounter(&swap_start);
            if(audio_smoke_frames){
                visible_update_ms+=(double)(draw_start.QuadPart-end.QuadPart)*1000/frequency.QuadPart;
                visible_draw_ms+=(double)(swap_start.QuadPart-draw_start.QuadPart)*1000/frequency.QuadPart;
            }
            LARGE_INTEGER trace_blit_end;
            if(native_4k_target){if(profile_blit_queries)profile_query_counter(profile_blit_queries[query_slot*2],0x8e28);target_present(target_window_width,target_window_height);if(profile_blit_queries)profile_query_counter(profile_blit_queries[query_slot*2+1],0x8e28);}
            if(performance_trace)QueryPerformanceCounter(&trace_blit_end);
            if(!SwapBuffers(dc))fail("Native window presentation failed.");
            QueryPerformanceCounter(&swap_end);
            if(native_elapsed_time&&host_reset_seen!=host_reset_revision){
                /* Loading/reset inside startup_tick is not driving time. */
                host_previous=swap_end;host_reset_seen=host_reset_revision;
                live_host_time.frame.delta_3b00=1;live_input_delta=1;live_host_ready=1;++host_rebases;
            }
            if(native_elapsed_time&&host_focus&&(!startup_mode||startup_stage==3)){
                uint32_t ticks;int32_t prior_delta=live_host_time.frame.delta_3b00;
                uint64_t ns=(uint64_t)((double)(swap_end.QuadPart-host_previous.QuadPart)*1e9/frequency.QuadPart);host_previous=swap_end;
                if(taxi_host_time_advance(&live_host_time,ns,&ticks))fail("Live elapsed clock advance failed.");
                /* Audio consumes every elapsed tick, even when no gameplay
                   commit is needed. Never use the clamped render dt. */
                for(uint32_t i=0;i<ticks;i++){audio_frame();live_host_audio_ticks++;}
                int committed=taxi_host_time_present_after_ticks(&live_host_time,(uint8_t)native_pause.menu.frozen,experimental_update_ticks);
                if(committed<0)fail("Live elapsed presentation clock failed.");
                if(committed){live_input_delta=prior_delta;live_host_ready=1;}
                if(host_time_trace){fprintf(host_time_trace,"%llu %u %d %u %d %u %d %u %llu %u\n",(unsigned long long)ns,ticks,prior_delta,live_host_time.counter,live_host_time.frame.delta_3b00,(unsigned)native_pause.menu.frozen,committed,live_host_updates,(unsigned long long)live_host_audio_ticks,audio_frames);fflush(host_time_trace);}
            }
            if(native_elapsed_time&&!host_focus)host_previous=swap_end;
            if(native_elapsed_time&&startup_mode&&startup_stage!=3){
                /* Menu/loading presentations are not elapsed driving time.
                   Original loading audio remains serviced by its own owner. */
                host_previous=swap_end;
                live_host_time=(TaxiHostTime){.frame={.delta_3b00=1}};live_input_delta=1;
                live_host_ready=1;
            }
            if(performance_trace){double unit=1000.0/frequency.QuadPart;fprintf(performance_trace,"%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",performance_frames++,(draw_start.QuadPart-end.QuadPart)*unit,(submit_start.QuadPart-draw_start.QuadPart)*unit,(submit_end.QuadPart-submit_start.QuadPart)*unit,(finish_end.QuadPart-submit_end.QuadPart)*unit,(swap_start.QuadPart-finish_end.QuadPart)*unit,(swap_end.QuadPart-swap_start.QuadPart)*unit,performance_previous.QuadPart?(swap_end.QuadPart-performance_previous.QuadPart)*unit:0.0,live_host_updates,gpu_timestamp_ms,performance_previous.QuadPart?(end.QuadPart-performance_previous.QuadPart)*unit:0.0,trace_pump_ticks*unit,trace_pacer_ticks*unit,trace_deadline_late_ms,(trace_blit_end.QuadPart-swap_start.QuadPart)*unit,(swap_end.QuadPart-trace_blit_end.QuadPart)*unit);performance_previous=swap_end;trace_pump_ticks=trace_pacer_ticks=0;}
            double swap_ms=(double)(swap_end.QuadPart-swap_start.QuadPart)*1000/frequency.QuadPart;
            if(swap_ms>maximum_swap_ms)maximum_swap_ms=swap_ms;
            if(audio_smoke_frames)visible_swap_ms+=swap_ms;
            if(audio_smoke_frames) {
                if(visible_previous.QuadPart&&visible_count<4096)
                    visible_intervals[visible_count++]=(double)(swap_end.QuadPart-visible_previous.QuadPart)*1000/frequency.QuadPart;
                visible_previous=swap_end;
            }
            if(window_surface_smoke&&audio_presented+1==audio_smoke_frames){if(window_smoke_stage!=4||fullscreen||target_draws!=audio_smoke_frames||target_presents!=audio_smoke_frames)fail("Native4K live window smoke incomplete.");puts("PASS NATIVE4K LIVE WINDOW: resize/fullscreen/restore during360 gameplay renders, four transitions preserved source driving/time state.");}
            if(audio_smoke_frames&&(++audio_presented>=audio_smoke_frames||
               (startup_credits_audio_smoke&&credits_live_runtime.complete))) {
                double sum=0;for(unsigned i=0;i<visible_count;i++)sum+=visible_intervals[i];
                FILE *visible_trace=fopen(startup_credits_audio_smoke?"outputs/crazy-taxi-native/reports/credits-presentation-intervals.csv":"outputs/crazy-taxi-native/reports/visible-presentation-intervals.csv","wb");
                if(!visible_trace)fail("Visible presentation trace open failed.");
                fprintf(visible_trace,"interval,swap_return_ms\n");
                for(unsigned i=0;i<visible_count;i++)fprintf(visible_trace,"%u,%.6f\n",i,visible_intervals[i]);
                if(ferror(visible_trace)||fclose(visible_trace))fail("Visible presentation trace write failed.");
                qsort(visible_intervals,visible_count,sizeof(double),compare_intervals);
                if(visible_count)printf("VISIBLE PRESENTATION: %u completed swaps, client%ldx%ld, mean%.3fms p95%.3fms max%.3fms maxSwapWait%.3fms; CPU swap-return timing, not scanout proof.\n",audio_presented,
                    (long)client.right,(long)client.bottom,sum/visible_count,visible_intervals[visible_count*95/100],visible_intervals[visible_count-1],maximum_swap_ms);
                printf("VISIBLE WORK: mean update%.3fms draw-submit%.3fms swap-wait%.3fms; %llu skipped60Hz deadlines. GPU work may be charged to swap, CPU timing only.\n",
                    visible_update_ms/audio_presented,visible_draw_ms/audio_presented,visible_swap_ms/audio_presented,(unsigned long long)pacer.skipped);
                if(native_elapsed_time&&simulation_error)fail("Live elapsed gameplay stopped on invalid state.");
                if(respawn_clock_smoke){if(!respawn_probe_done||host_rebases!=1)fail("Respawn clock coverage missing.");printf("PASS RESPAWN CLOCK: stale delta11 reset to1, injected120ms reset work excluded, %u host rebase; source clock phase retained.\n",host_rebases);}
                if(focus_suspend_smoke){if(focus_held_frames!=60||live_host_updates<=focus_saved_updates)fail("Focus suspension/resume coverage missing.");printf("PASS FOCUS SUSPENSION: %u render frames retained taxi, clock, channels and mixer/update counts; resumed native gameplay. Controlled host events, not OS focus test.\n",focus_held_frames);}
                if(native_elapsed_time)printf("LIVE HOST: %u gameplay updates, %llu elapsed audio ticks; presentation commits only after a new elapsed tick.\n",live_host_updates,(unsigned long long)live_host_audio_ticks);
                running=0;
            }
            if(startup_credits_audio_smoke&&(credits_live_runtime.complete||startup_audio_tick>=4000)){
                printf("LIVE CREDITS AUDIO: ticks%u draws%u lines%u sound29%u menu-order-starts%u musicframes%u nonzero%u completed%u stopped%u rejected%u unavailable%u.\n",
                    credits_live_ticks,credits_live_draws,credits_live_runtime.flow.line,audio_request_counts[29],music_starts[4],
                    music_frames-credits_music_frames_before,music_nonzero_samples-credits_music_nonzero_before,
                    credits_live_runtime.complete,!music_tracker.state.playing,audio_rejected,audio_busy);
                if(!credits_live_runtime.complete||!credits_live_draws||credits_live_runtime.flow.line!=126||
                    audio_request_counts[29]!=1||music_starts[4]!=1||music_tracker.state.playing||
                    music_frames<=credits_music_frames_before||music_nonzero_samples<=credits_music_nonzero_before||audio_rejected||audio_busy||simulation_error)
                    fail("Live native credits render/audio/completion failed.");
                puts("PASS paced native credits assets/scroll/brightness/music/sound29/actual completion; diagnostic screen entry, earned qualified Quit/fullscreen fidelity unverified.");running=0;
            }else if(startup_attract_audio_smoke&&(attract_audio_return_ticks>=50||startup_audio_tick>=2000)) {
                unsigned starts=music_starts[0]+music_starts[1]+music_starts[2]+music_starts[3];
                printf("ATTRACT AUDIO: tick%u driven%u returned%u selectedstarts%u musicframes%u dispatches%u nonzero%u loadingframes%u stage%d stopped%d.\n",startup_audio_tick,attract_audio_drive_ticks,attract_audio_return_ticks,starts,music_frames,music_dispatches,music_nonzero_samples,loading_audio_frames,startup_stage,!music_tracker.state.playing);
                if(startup_stage!=TAXI_STARTUP_TITLE_STAGE||live_attract||source_attract_ai||attract_audio_return_ticks<50||starts!=1||!music_frames||!music_dispatches||!music_nonzero_samples||loading_audio_frames!=52||music_tracker.state.playing||audio_rejected||audio_busy||simulation_error)fail("Live title/demo audio return failed.");
                running=0;
            }else if(startup_return_audio_smoke&&(return_music_frames>=60||startup_audio_tick>=6000)){
                printf("RETURN AUDIO DIAGNOSTIC: tick%u stage%d profileinputs%u resumedframes%u menu-starts%u mainmode%u fadegate%d factor%d.\n",startup_audio_tick,startup_stage,return_profile_input,return_music_frames,music_starts[4],music_context.main_mode,music_session_fade_gate(),music_fade_factor);
                if(startup_stage!=9||return_music_frames<60||!music_starts[4]||!music_tracker.state.playing||!music_context.main_mode||music_session_fade_gate()||audio_rejected||audio_busy||simulation_error)fail("Native postgame menu music return failed.");
                running=0;
            }else if(startup_audio_cleanup_smoke==4&&startup_quit_complete){
                if(!startup_quit_menu_started){
                    TaxiNormalExitRequests requests={0};int finished=0;
                    if(startup_quit_credits_waiting){
                        finished=taxi_normal_exit_resume(&startup_quit_parent,credits_live_runtime.complete,&requests);
                        if(finished<0)fail("Quit credits return owner invalid.");
                        if(finished){
                            credits_live_active=0;glDeleteTextures(1,&credits_live_texture);credits_live_texture=0;
                            startup_quit_credits_waiting=0;
                        }
                    }else{
                        live_normal_exit_audio_check();
                        /* Source1691 comes from actual earned fare state. */
                        finished=taxi_normal_exit_prepare(&startup_quit_parent,0,&requests);
                        if(finished<0)fail("Quit parent preparation invalid.");
                        if(finished==2){
                            if(requests.count!=1||requests.calls[0]!=0x080243a4)fail("Quit credits request invalid.");
                            passenger_scene.session.outer_state_1ea8=startup_quit_parent.outer;
                            passenger_scene.session.ended_1697=startup_quit_parent.ended;
                            native_pause.menu.outer_state=startup_quit_parent.outer;
                            music_context.main_mode=startup_quit_parent.main_mode;
                            startup_quit_credits_waiting=1;startup_quit_credits_entered=1;
                            credits_live_begin(1);finished=0;
                        }
                    }
                    if(finished){
                        if(finished!=1||requests.count||
                            startup_quit_parent.outer||startup_quit_parent.ended||startup_quit_parent.frozen||
                            !startup_quit_parent.main_mode||!startup_quit_parent.redraw||startup_quit_parent.results||
                            startup_quit_parent.callback!=0x080174dd)fail("Quit parent finish requests/state invalid.");
                        passenger_scene.session.outer_state_1ea8=startup_quit_parent.outer;
                        passenger_scene.session.ended_1697=startup_quit_parent.ended;
                        native_pause.menu.outer_state=startup_quit_parent.outer;
                        native_pause.menu.frozen=startup_quit_parent.frozen;
                        music_context.main_mode=startup_quit_parent.main_mode;
                        if(result_live_active||name_live_active||profile_live_active||game_over_active)
                            fail("Quit incorrectly entered result/name/profile flow.");
                        normal_live_input=native_pause.input;normal_live_flow.fade=city_fade;
                        startup_parent_begin(1,startup_quit_parent.redraw);
                        startup_quit_menu_started=1;
                    }
                }else if(startup_stage==9&&!normal_live_flow.fade.active_177b)startup_quit_menu_frames++;
                if(startup_quit_menu_frames>=60){
                    printf("LIVE QUIT MENU: %u settled frames, menu starts%u mainmode%u results%u fadegate%d rejected%u unavailable%u.\n",
                        startup_quit_menu_frames,music_starts[4],music_context.main_mode,startup_quit_parent.results,
                        music_session_fade_gate(),audio_rejected,audio_busy);
                    if(startup_stage!=9||music_starts[4]!=(unsigned)(1+startup_quit_credits_entered)||!music_tracker.state.playing||!music_context.main_mode||
                        music_session_fade_gate()||audio_rejected||audio_busy||simulation_error)
                        fail("Quit native menu/music return failed.");
                    puts("PASS native Quit Yes/fade/cleanup/parent finish skips results and returns to original main menu/music; diagnostic inputs, earned qualified credits/retry/full reentry unverified.");
                    running=0;
                }
            }else if(live_delivery_smoke&&(passenger_scene.session.end.completed_1692||result_live_active||startup_audio_tick>=12000)){
                printf("LIVE DELIVERY TERMINAL: presentations%u sourcecounter%u updates%u completed%u cash%ld.%02ld active%u pending%ld waypoint%u clock%ld rejected%u unavailable%u.\n",startup_audio_tick,live_host_time.counter,live_host_updates,passenger_scene.session.end.completed_1692,(long)passenger_scene.session.fare.total_whole_1698,(long)passenger_scene.session.fare.total_fraction_169c,passenger_scene.session.trip.active_1694,(long)passenger_scene.pickup.pending_1704,live_delivery_waypoint,(long)passenger_scene.session.clock.remaining_16a8,audio_rejected,audio_busy);
                if(!live_delivery_route||!passenger_scene.session.end.completed_1692||passenger_scene.session.fare.total_whole_1698<=0||passenger_scene.session.trip.active_1694||passenger_scene.pickup.pending_1704||!audio_nonzero_samples||audio_rejected||audio_busy||simulation_error)fail("Live input-only delivery/fare/audio did not complete.");puts("PASS LIVE DELIVERY: original-route input-only boarding/delivery and positive native fare settlement with actual elapsed source clock; no pose/clock/cash/completion edits.");running=0;
            }else if(!live_delivery_smoke&&startup_pickup_audio_smoke&&((startup_audio_cleanup_smoke>=3?startup_quit_complete:startup_pickup_audio_hold>=(startup_tip_audio_smoke?960u:180u))||startup_audio_tick>=(startup_tip_audio_smoke?2800u:2000u))){
                unsigned context_starts=0,driver_starts=0;
                for(unsigned index=83;index<131;index++)context_starts+=audio_request_counts[index];
                for(unsigned index=131;index<203;index++)driver_starts+=audio_request_counts[index];
                if(audio_host){
                TaxiAudioHostStats host_stats;
                if(taxi_audio_host_stats(audio_host,&host_stats))fail("Audio queue measurement failed.");
                printf("AUDIO HOST QUEUE: writes%llu submitted%llu played%llu positiontype%u elapsed%.3f peakFIFO%u full%u waits%u pending%d.\n",(unsigned long long)host_stats.writes,(unsigned long long)host_stats.submitted_samples,(unsigned long long)host_stats.played_samples,host_stats.position_type,host_stats.elapsed_seconds,host_stats.peak_fifo,host_stats.full,host_stats.waits,taxi_audio_host_pending(audio_host));
                }
                printf("LIVE PICKUP AUDIO: tick%u driving%u hold%u active%u pending%ld contextstarts%u driverstarts%u voiceRNG%u speed%ld rejected%u unavailable%u.\n",startup_audio_tick,startup_pickup_audio_driving,startup_pickup_audio_hold,passenger_scene.session.trip.active_1694,(long)passenger_scene.pickup.pending_1704,context_starts,driver_starts,recovered_game_random,(long)car.world.speed_544,audio_rejected,audio_busy);
                if(startup_pickup_audio_hold<180||!passenger_scene.session.trip.active_1694||passenger_scene.pickup.pending_1704||car.world.speed_544||!context_starts||!audio_nonzero_samples||audio_rejected||audio_busy||simulation_error)fail("Paced input-only pickup audio failed.");
                if(startup_tip_audio_smoke){
                    TaxiTrafficNearMissObservation coverage=taxi_traffic_near_miss_observation();
                    printf("NEAR-MISS PREDICATE: calls%u maxspeed%ld speed%u cooldown%u bounds%u behind%u hits%u collisions%u nearestbox%llu.\n",coverage.calls,(long)coverage.max_speed,coverage.speed_ok,coverage.cooldown_ok,coverage.bounds_ok,coverage.behind,coverage.hits,coverage.collisions,(unsigned long long)coverage.nearest_box);
                    printf("LIVE TIP COVERAGE: flags%u rewards%u gated%u sound44%u sound48%u sound49%u.\n",recovered_game_near_miss,recovered_game_rewards,recovered_game_reward_gated,audio_request_counts[44],audio_request_counts[48],audio_request_counts[49]);
                    if(!recovered_game_near_miss||!recovered_game_rewards||!audio_request_counts[44])fail("Input-only traffic route did not verify a real near-miss tip sound.");
                }
                if(startup_audio_cleanup_smoke)live_normal_exit_audio_check();
                if(timestamped_presentation&&passenger_diagnostic){if(!timestamped_timer_draws)fail("Timestamped trip timer branch not exercised.");printf("TIMESTAMPED TIMER ANCHORS: %u.\n",timestamped_timer_draws);}
                puts("PASS paced native input-only boarding and original passenger voice playback; no taxi/customer position or fare edits.");running=0;
            }else if(!startup_credits_audio_smoke&&!startup_pickup_audio_smoke&&startup_game_audio_smoke&&!startup_return_audio_smoke&&(startup_audio_tick>=(natural_expiry_smoke?6000:900)||(natural_expiry_smoke&&result_live_active&&music_fade_factor==64&&!music_tracker.state.playing&&!music_session_fade_gate()))){
                if(!startup_expiry_audio_smoke){
                    if(live_game_start.active)fail("Live audio startup sequence did not complete.");
                    for(unsigned i=0;i<3;i++)if(live_game_start_sound_counts[i]!=1||audio_request_counts[51+i]!=1)
                        fail("Live startup countdown sound was missing or repeated.");
                    puts("STARTUP LIVE AUDIO: recovered countdown completed; original indices51/52/53 each requested exactly once through native audio output.");
                }
                if(startup_expiry_audio_smoke){
                    printf("EXPIRY AUDIO DIAGNOSTIC: %s initial clock%ld, factor%d fadedframes%u ended%u outer%u remaining%ld gate%d playing%u main%u result%u sourcecounter%u updates%u.\n",natural_expiry_smoke?"natural":"forced",(long)(natural_expiry_smoke?natural_expiry_initial_clock:1),music_fade_factor,music_faded_frames,passenger_scene.session.ended_1697,passenger_scene.session.outer_state_1ea8,(long)passenger_scene.session.clock.remaining_16a8,music_session_fade_gate(),(unsigned)music_tracker.state.playing,(unsigned)music_context.main_mode,(unsigned)result_live_active,live_host_time.counter,live_host_updates);
                    if(!expiry_audio_armed||music_fade_factor!=64||!music_faded_frames||passenger_scene.session.clock.remaining_16a8||music_session_fade_gate()||music_tracker.state.playing||!music_context.main_mode||!result_live_active)fail("Original gameplay expiry audio fade failed.");
                    if(natural_expiry_smoke){if(live_host_time.counter-natural_expiry_initial_counter<(uint32_t)natural_expiry_initial_clock)fail("Natural expiry elapsed fewer source ticks than initial clock.");printf("PASS NATURAL EXPIRY: original initialclock%ld elapsedsource%u presentations%u; no clock shortening, expiry/results and native audio owners.\n",(long)natural_expiry_initial_clock,live_host_time.counter-natural_expiry_initial_counter,startup_audio_tick);}
                }
                if(startup_stage!=3||!music_frames||!music_nonzero_samples||!music_dispatches||loading_audio_frames!=52||audio_rejected||audio_busy||simulation_error||(!startup_expiry_audio_smoke&&music_context.main_mode)||music_context.context!=&passenger_scene.random_state)fail("Normal startup gameplay audio smoke failed.");
                printf("PASS normal startup gameplay music: city/driver confirms, loading, stage3, shared RNG%u; source initial seed and loading audio scheduling pending.\n",passenger_scene.random_state);
                running=0;
            }else if(startup_audio_smoke&&!startup_game_audio_smoke&&startup_audio_tick>=1650){
                printf("STARTUP AUDIO STATE: stage%d main%u options%u audio%u settings%u global%u stops%u effect1%u nonzero%u rejected%u full%u ticks%u.\n",startup_stage,normal_live_main_selection,options_live_selection,audio_live_selection,native_settings[2],audio_global_volume,audio_test_stops,audio_request_counts[1],audio_nonzero_samples,audio_rejected,audio_busy,startup_audio_tick);
                if(startup_stage!=9||normal_live_main_selection!=2||native_settings[2]!=56||audio_global_volume!=56||!audio_test_stops||!audio_request_counts[1]||!audio_nonzero_samples||!music_frames||!music_dispatches||!music_nonzero_samples||!music_starts[0]||!music_starts[4]||music_context.sound_active||audio_rejected||audio_busy)fail("Startup audio integration smoke failed.");
                puts("PASS startup audio smoke: input-only Options/Sound path, selected effect stop/start, retained volume56, nonzero accepted native samples and no rejected/full frames. Listening not verified.");
                running=0;
            }
        }
    }
    if(audio_live){
        printf("LOGICAL AUDIO: frames%u PCMhash%016llx RNG%u output%s.\n",audio_frames,(unsigned long long)audio_pcm_hash,passenger_scene.random_state,native_audio_capture?"capture":audio_host?"device":"disabled");
        if(recovered_game_audio)printf("GAME SOUND TRAFFIC COVERAGE: near-miss flags%u, reward gated%u, traffic spawned%u, visible frames%u.\n",recovered_game_near_miss,recovered_game_reward_gated,traffic_spawned,traffic_visible_frames);
        if(recovered_game_audio)printf("RECOVERED GAME SOUND DIAGNOSTIC: %u frames, %u ordered requests, %u rewards, %u voice RNG requests; active owners%u/%u/%u/%u/%u/%u/%u/%u/%u/%u. Cleanup/reentry pending.\n",recovered_game_frames,recovered_game_requests,recovered_game_rewards,recovered_game_random,live_sound_state.handles[0],live_sound_state.handles[1],live_sound_state.handles[2],live_sound_state.handles[3],live_sound_state.handles[4],live_sound_state.handles[5],live_sound_state.handles[6],live_sound_state.handles[7],live_sound_state.handles[8],live_sound_state.handles[9]);
        if(recovered_engine_audio&&!recovered_game_audio)printf("RECOVERED ENGINE DIAGNOSTIC: %u controller frames, handles%u/%u, source divisor%ld, rpm%d; full sound controller pending.\n",recovered_engine_frames,live_engine_audio.handle[0],live_engine_audio.handle[1],(long)car.speed_divisor_548,car.driver.power.gears.rpm_538);
        audio_live=0;
        if(taxi_audio_host_close(&audio_host))fail("Native audio shutdown failed.");
        taxi_audio_asset_free(&audio_bank);
        free(music_bytes);music_bytes=NULL;
        printf("MUSIC: %u source frames mixed, %u gated tracker dispatches, %u nonzero pre-FX samples; Sound music test integration, full startup music ownership pending.\n",music_frames,music_dispatches,music_nonzero_samples);
        printf("MUSIC STARTS: selected0 %u, restored-menu4 %u.\n",music_starts[0],music_starts[4]);
        printf("LOADING AUDIO: %u source presentation frames serviced.\n",loading_audio_frames);
        printf("AUDIO PREVIEW: %u original requests, %u rejected, %u unavailable output buffers, %u source audio frames (%s); recovered settings volume%u, music/voices and complete sound ownership pending.\n",audio_requests,audio_rejected,audio_busy,audio_frames,native_earned_audio_check?"PCM capture":audio_preview&&!audio_output_disabled?"device queued":"logical only",(unsigned)audio_global_volume);
        printf("AUDIO SIGNAL: %u accepted non-silent frames, %u nonzero signed8 samples.\n",audio_nonzero_frames,audio_nonzero_samples);
        for(unsigned i=0;i<203;i++)if(audio_request_counts[i])
            printf("AUDIO REQUEST: original index%u count%u.\n",i,audio_request_counts[i]);
    }
    for (uint32_t i=0;i<texture_count;++i) {glDeleteTextures(1,&textures[i].id);free(textures[i].pixels);}
    if(challenge_jump_end_texture)glDeleteTextures(1,&challenge_jump_end_texture);
    taxi_font_asset_free(&challenge_jump_end_font);
    if(normal_menu_loaded){glDeleteTextures(1,&normal_menu_texture);for(unsigned i=0;i<2;i++)taxi_font_asset_free(normal_menu_fonts+i);}
    if(loading_loaded) {glDeleteTextures(901,loading_ids);taxi_font_asset_free(&loading_font);}
    if(native_material_phase_trace){fclose(native_material_phase_trace);native_material_phase_trace=NULL;}
    free(profile_background_phases);
    if(city_preview>=0)glDeleteTextures(8,city_preview_ids);
    if(driver_preview>=0)glDeleteTextures(11,driver_preview_ids);
    for(unsigned i=0;i<profile_source_count;i++)free(profile_sources[i].pixels);
    if(profile_loaded) {glDeleteTextures(1,&profile_background);glDeleteTextures(2,profile_prompt_icons);for(unsigned i=0;i<2;i++){glDeleteTextures(profile_fonts[i].count*profile_fonts[i].palettes,profile_font_ids[i]);taxi_font_asset_free(profile_fonts+i);}}
    if(name_ring_loaded) {for(unsigned i=0;i<1455;i++)free(name_sources[i].pixels);glDeleteTextures(96,name_ring_ids);glDeleteTextures(1,&name_ring_bg);taxi_font_asset_free(&name_ring_font);glDeleteTextures(14,name_panel_ids);for(unsigned i=0;i<2;i++){glDeleteTextures(672,name_label_ids[i]);taxi_font_asset_free(name_label_fonts+i);}}
    if(game_over_loaded) {glDeleteTextures(96,game_over_ids);glDeleteTextures(1,&game_over_background);taxi_font_asset_free(&game_over_font);free(game_over_bg);}
    if(results_loaded) {for(unsigned i=0;i<1529;i++)free(result_sources[i].pixels);for(unsigned s=0;s<3;s++) {glDeleteTextures(result_fonts[s].count*result_fonts[s].palettes,result_font_ids[s]);taxi_font_asset_free(result_fonts+s);}glDeleteTextures(1,&result_background);glDeleteTextures(14,result_sprite_ids);}
    if(precise_presentation_wait)printf("PRECISE PRESENTATION WAIT: high-resolution timer waits%u final-spin passes%u; rational60Hz deadline and elapsed simulation owners unchanged.\n",precise_timer_waits,precise_spin_passes);
    if(async_4k_render_check){
        if(performance_frames!=live_4k_submitted||performance_frames!=(unsigned)audio_smoke_frames)fail("Async4K submitted frame count mismatch.");
        LARGE_INTEGER drain_start,drain_end;QueryPerformanceCounter(&drain_start);glFinish();QueryPerformanceCounter(&drain_end);
        char gpu_path[MAX_PATH];if(snprintf(gpu_path,sizeof gpu_path,"%s.gpu.csv",performance_trace_path)>=(int)sizeof gpu_path)fail("GPU trace path too long.");FILE *gpu_file=fopen(gpu_path,"wb");if(!gpu_file)fail("GPU completion trace unavailable.");fprintf(gpu_file,profile_blit_queries?"frame,start_ns,end_ns,gpu_ms,blit_start_ns,blit_end_ns,blit_gpu_ms\n":"frame,start_ns,end_ns,gpu_ms\n");
        for(unsigned n=0;n<performance_frames;n++){
            GLint begin_ready=0,end_ready=0;profile_query_available(profile_queries[n*2],0x8867,&begin_ready);profile_query_available(profile_queries[n*2+1],0x8867,&end_ready);if(!begin_ready||!end_ready)fail("GPU query incomplete after drain.");
            uint64_t first,last;profile_query_result(profile_queries[n*2],0x8866,&first);profile_query_result(profile_queries[n*2+1],0x8866,&last);if(last<first)fail("GPU timestamp order invalid.");fprintf(gpu_file,"%u,%llu,%llu,%.6f",n,(unsigned long long)first,(unsigned long long)last,(last-first)/1000000.0);
            if(profile_blit_queries){GLint a_ready=0,b_ready=0;uint64_t a,b;profile_query_available(profile_blit_queries[n*2],0x8867,&a_ready);profile_query_available(profile_blit_queries[n*2+1],0x8867,&b_ready);if(!a_ready||!b_ready)fail("Blit GPU queries incomplete after drain.");profile_query_result(profile_blit_queries[n*2],0x8866,&a);profile_query_result(profile_blit_queries[n*2+1],0x8866,&b);if(a<last||b<a)fail("Blit GPU timestamp order invalid.");fprintf(gpu_file,",%llu,%llu,%.6f",(unsigned long long)a,(unsigned long long)b,(b-a)/1000000.0);}fputc('\n',gpu_file);live_4k_completed++;
        }
        if(ferror(gpu_file)||fclose(gpu_file)||glGetError()!=GL_NO_ERROR)fail("GPU completion trace failed.");printf("ASYNC4K GPU: submitted%u completed%u final_drain_ms%.6f; no per-frame finish/result waits.\n",live_4k_submitted,live_4k_completed,(drain_end.QuadPart-drain_start.QuadPart)*1000.0/frequency.QuadPart);
    }
    if(performance_trace){if(ferror(performance_trace)||fclose(performance_trace))fail("Performance trace close failed.");performance_trace=NULL;}
    if(reference_driving_path){if(reference_driving.next!=reference_driving.count)fail("Reference schedule did not finish within presentation budget.");printf("PASS REFERENCE DRIVE INTEGRATION: %u exact native driving updates with recorded variable physics and internal4K history presentation; not general playable timing or full gameplay proof.\n",reference_driving.next);taxi_reference_driving_free(&reference_driving);}
    if(live_4k_render_check)printf("LIVE4K GPU COMPLETED: %u frames at3840x2160; native framebuffer, %s; no monitor scanout claim.\n",live_4k_completed,async_4k_render_check?"all distinct frame queries completed after final drain":"GPU completion checked each frame");
    if(live_snapshot_path&&!live_snapshot_done)fail("Requested live gameplay capture was not reached.");
    if(timestamped_presentation)printf("TIMESTAMPED HUD: clock%u cash%u tip_requests%u expiry%u.\n",timestamped_clock_draws,timestamped_cash_draws,timestamped_tip_requests,timestamped_expiry_draws);
    if(timestamped_presentation)printf("TIMESTAMPED DESTINATION PANEL DRAWS: %u.\n",timestamped_panel_draws);
    if(timestamped_presentation)printf("TIMESTAMPED GUIDANCE/SHADOW: guidance%u shadow%u.\n",timestamped_guidance_draws,timestamped_shadow_views);
    if(timestamped_presentation)printf("TIMESTAMPED EFFECT MOTION: blends%u snaps%u.\n",timestamped_effect_blends,timestamped_effect_snaps);
    if(timestamped_presentation)printf("TIMESTAMPED CACHED EFFECT REQUESTS: %u.\n",timestamped_effect_requests);
    if(timestamped_presentation)printf("TIMESTAMPED TRAFFIC VIEWS: %u.\n",timestamped_traffic_views);
    if(timestamped_presentation)printf("TIMESTAMPED FALLBACK PASSENGER DRAWS: %u.\n",timestamped_fallback_passenger_draws);
    if(timestamped_presentation)printf("TIMESTAMPED PASSENGER VIEWS: %u.\n",timestamped_passenger_views);
    if(timestamped_presentation)printf("TIMESTAMPED PASSENGER CULL: records%u eligibility_skips%u attempts%u culled%u visible%u.\n",timestamped_passenger_records,timestamped_passenger_skips,timestamped_passenger_attempts,timestamped_passenger_culled,timestamped_passenger_views);
    if(passenger_diagnostic) {glDeleteTextures(144,passenger_textures);glDeleteTextures(10,passenger_marker_textures);glDeleteTextures(160,guidance_textures);glDeleteTextures(52,clock_textures);glDeleteTextures(15,expiry_textures);glDeleteTextures(19,cash_owned_slot_textures);glDeleteTextures(4,cash_label_textures);glDeleteTextures(896,font_textures);taxi_passenger_asset_free(&passenger_asset);}
    if(attract_prompt_font.count){glDeleteTextures(224,attract_prompt_ids);taxi_font_asset_free(&attract_prompt_font);}
    if(attract_score_font.count){glDeleteTextures(155,attract_score_ids);glDeleteTextures(4,attract_driver_ids);taxi_font_asset_free(&attract_score_font);}
    if(title_texture)glDeleteTextures(1,&title_texture);taxi_obj_shader_free();taxi_material_shader_free();taxi_axis_shader_free();
    if(native_flat_pattern_texture)glDeleteTextures(1,&native_flat_pattern_texture);
    if(native_panel_texture)glDeleteTextures(1,&native_panel_texture);
    taxi_ai_path_free(&attract_ai_path);
    taxi_indexed_asset_free(&native_indexed_scene);
    if(source_column_work_report_path&&!source_column_work_report_done)fail("Requested source column workload report was not generated.");
    free(queued_world);free(queued_packets);free(queued_normal_seen);free(queued_special_seen);
    if(drive_mode) {glDeleteTextures(64,taxi_textures);taxi_world_asset_free(&world);taxi_traffic_assets_free(&traffic_assets);}
    if(live_delivery_movement_trace){taxi_movement_observer(NULL);int trace_error=ferror(live_delivery_movement_trace);if(fclose(live_delivery_movement_trace)||trace_error)fail("Movement trace close failed.");live_delivery_movement_trace=NULL;}
    if(live_delivery_query_trace){int trace_error=ferror(live_delivery_query_trace);if(fclose(live_delivery_query_trace)||trace_error)fail("Delivery query trace close failed.");live_delivery_query_trace=NULL;printf("LIVE DELIVERY QUERY TRACE: %u original movement-query calls recorded.\n",live_delivery_query_records);}
    if(native_4k_target)target_destroy();
    free(profile_blit_queries);free(profile_queries);wglMakeCurrent(NULL,NULL); wglDeleteContext(context); ReleaseDC(window,dc); DestroyWindow(window);
    for(unsigned i=0;i<sprite_source_count;i++)free(sprite_sources[i].pixels);
    if(native_traffic_impact_trace){taxi_traffic_impact_trace(NULL);if(ferror(native_traffic_impact_trace)||fclose(native_traffic_impact_trace))fail("Traffic impact trace close failed.");native_traffic_impact_trace=NULL;}
    if(native_fare_settlement_trace){taxi_fare_settlement_trace(NULL);if(ferror(native_fare_settlement_trace)||fclose(native_fare_settlement_trace))fail("Fare settlement trace close failed.");native_fare_settlement_trace=NULL;}
    if(native_destination_selection_trace){taxi_material_selection_trace(NULL);if(ferror(native_destination_selection_trace)||fclose(native_destination_selection_trace))fail("Destination selection trace close failed.");native_destination_selection_trace=NULL;}
    if(native_audio_capture){if(ferror(native_audio_capture)||fclose(native_audio_capture))fail("Native PCM capture close failed.");native_audio_capture=NULL;}
    if(native_held_gear_trace){taxi_drive_world_gear_trace(NULL);if(ferror(native_held_gear_trace)||fclose(native_held_gear_trace))fail("Held gear trace close failed.");native_held_gear_trace=NULL;}
    if(native_tip_collection_trace){taxi_tip_collection_trace(NULL);if(ferror(native_tip_collection_trace)||fclose(native_tip_collection_trace))fail("Collection trace close failed.");native_tip_collection_trace=NULL;}
    if(native_reward_update_trace){taxi_reward_session_update_trace(NULL);if(ferror(native_reward_update_trace)||fclose(native_reward_update_trace))fail("Reward update trace close failed.");native_reward_update_trace=NULL;}
    if(native_reward_trace){taxi_reward_session_trace(NULL);if(ferror(native_reward_trace)||fclose(native_reward_trace))fail("Reward trace close failed.");native_reward_trace=NULL;}
    if(native_cash_glyph_trace){taxi_cash_glyphs_trace(NULL);if(ferror(native_cash_glyph_trace)||fclose(native_cash_glyph_trace))fail("Cash glyph trace close failed.");native_cash_glyph_trace=NULL;}
    if(native_passenger_money_trace){taxi_passenger_money_trace(NULL);if(ferror(native_passenger_money_trace)||fclose(native_passenger_money_trace))fail("Passenger money trace close failed.");native_passenger_money_trace=NULL;}
    if(native_passenger_fare_trace){taxi_passenger_fare_trace(NULL);if(ferror(native_passenger_fare_trace)||fclose(native_passenger_fare_trace))fail("Passenger fare trace close failed.");native_passenger_fare_trace=NULL;}
    if(native_impact_voice_trace){if(ferror(native_impact_voice_trace)||fclose(native_impact_voice_trace))fail("Impact voice trace close failed.");native_impact_voice_trace=NULL;}
    if(native_passenger_pickup_trace){taxi_passenger_pickup_trace(NULL);if(ferror(native_passenger_pickup_trace)||fclose(native_passenger_pickup_trace))fail("Passenger pickup trace close failed.");native_passenger_pickup_trace=NULL;}
    if(native_passenger_idle_trace){taxi_passenger_idle_trace(NULL);if(ferror(native_passenger_idle_trace)||fclose(native_passenger_idle_trace))fail("Passenger idle trace close failed.");native_passenger_idle_trace=NULL;}
    if(native_random_trace){taxi_random_trace(NULL);if(ferror(native_random_trace)||fclose(native_random_trace))fail("Random trace close failed.");native_random_trace=NULL;}
    if(native_traffic_frame_trace){taxi_traffic_frame_trace(NULL);if(ferror(native_traffic_frame_trace)||fclose(native_traffic_frame_trace))fail("Traffic frame trace close failed.");native_traffic_frame_trace=NULL;}
    if(native_traffic_heading_trace){taxi_traffic_heading_trace(NULL);if(ferror(native_traffic_heading_trace)||fclose(native_traffic_heading_trace))fail("Traffic heading trace close failed.");native_traffic_heading_trace=NULL;}
    if(native_traffic_spawn_trace){taxi_traffic_spawn_trace(NULL);if(ferror(native_traffic_spawn_trace)||fclose(native_traffic_spawn_trace))fail("Traffic spawn trace close failed.");native_traffic_spawn_trace=NULL;}
    if(native_traffic_actor_trace){taxi_traffic_actor_trace(NULL);if(ferror(native_traffic_actor_trace)||fclose(native_traffic_actor_trace))fail("Traffic actor trace close failed.");native_traffic_actor_trace=NULL;}
    if(native_traffic_response_trace){taxi_traffic_player_response_trace(NULL);if(ferror(native_traffic_response_trace)||fclose(native_traffic_response_trace))fail("Traffic response trace close failed.");native_traffic_response_trace=NULL;}
    if(native_traffic_effect_trace){taxi_traffic_effects_trace(NULL);if(ferror(native_traffic_effect_trace)||fclose(native_traffic_effect_trace))fail("Traffic effect trace close failed.");native_traffic_effect_trace=NULL;}
    if(native_traffic_contact_trace){taxi_traffic_collision_trace(NULL);if(ferror(native_traffic_contact_trace)||fclose(native_traffic_contact_trace))fail("Traffic contact trace close failed.");native_traffic_contact_trace=NULL;}
    if(native_response_state_trace){taxi_vehicle_response_observer(NULL);if(fclose(native_response_state_trace))fail("Response close failed.");native_response_state_trace=NULL;}
    if(native_traction_state_trace){taxi_traction_observer(NULL);if(fclose(native_traction_state_trace))fail("Traction trace close failed.");native_traction_state_trace=NULL;}
    if(native_motion_state_trace){taxi_motion_observer(NULL);if(fclose(native_motion_state_trace))fail("Motion trace close failed.");native_motion_state_trace=NULL;}
    if(native_orientation_state_trace){taxi_orientation_observer(NULL);if(fclose(native_orientation_state_trace))fail("Orientation trace close failed.");native_orientation_state_trace=NULL;}
    taxi_graphics_package_release(&taxi_owned_package);
    if(native_camera_state_trace){if(fclose(native_camera_state_trace))fail("Camera state trace close failed.");native_camera_state_trace=NULL;}
    if(native_effect_state_trace){if(fclose(native_effect_state_trace))fail("Effect state trace close failed.");native_effect_state_trace=NULL;}
    if(native_effect_trace){if(fclose(native_effect_trace))fail("Effect trace write failed.");native_effect_trace=NULL;}
    free(vertices); free(faces);free(materials);free(textures);return 0;
}





















































