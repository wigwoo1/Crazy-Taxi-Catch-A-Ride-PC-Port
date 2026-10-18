#include "restart_setup_native.h"
#include "reward_session.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t signed_word(const uint8_t *p){uint32_t u=word(p);int32_t v;memcpy(&v,&u,4);return v;}
static int valid(TaxiRestartSetupNative *o){
 if(!o||!o->car||!o->drive||!o->vehicle||!o->runtime||!o->marker||!o->session||!o->scene||!o->passengers||!o->traffic||!o->traffic_parameters||!o->traffic_sprites||!o->traffic_globals||!o->traffic_timer||!o->counter_67d0||!o->audio||!o->engine_audio||!o->audio_auxiliary||!o->effects||!o->effect_ring||!o->pause||!o->intro_1696||!o->flag_262c||!o->yaw_2618||!o->yaw_target_261c)return -1;
 TaxiPassengerSetupOwned *p=o->passengers;TaxiPassengerInitWorld *w=p->world;
 if(!p->passengers||!w||!w->asset||!w->noncustomers||!p->loading||!p->loading->service||!p->loading->state||p->passengers->states!=o->scene->customers||p->passengers->settings_1eb0!=o->session->settings||p->passengers->animation_1684!=o->session->session_165e+38||w->noncustomers->count_430c!=&o->session->reset_430c||w->asset->count>128)return -1;
 if((o->session->settings[0]&224)==128||!p->passengers->override_1708||*p->passengers->override_1708||o->traffic_capacity<taxi_traffic_density(o->session->settings[3]))return -1;
 return 0;
}
static int loading_inputs(TaxiRestartSetupNative *o){
 TaxiOrdinaryLoadingPending *p=o->passengers->loading;const TaxiLoadingPaletteRetained *r=p->retained_palette;
 if(!p->presentation||!p->presentation->palette||!p->presentation->palette->registry||!p->presentation->palette->fade||o->passengers->phase!=TAXI_PASSENGER_SETUP_IDLE)return -1;
 const TaxiPaletteFadeStart *f=p->presentation->palette->fade;
 if(p->phase!=TAXI_LOADING_IDLE||!p->published||!p->published->valid||!r||!r->generation||r->generation!=p->published->generation||r->kind!=1||r->scratch.written!=3||!r->scratch.next_record||r->scratch.next_record>128||r->scratch.next_record!=p->presentation->palette->registry->count)return -1;
 if(r->before.masks.mode!=1||r->before.masks.background_mask!=1||r->before.masks.object_mask!=1||r->before.masks.cap!=32||!r->before.fade.active_177b||r->before.scale_x_177c!=r->before.scale_y_177e||f->fade.active_177b||f->masks.mode!=1||f->masks.background_mask!=1||f->masks.object_mask!=1)return -1;
 if(r->scratch.color_cursor_identity!=0x030050d0u&&r->scratch.color_cursor_identity!=0x030052b0u)return -1;
 o->retained_endpoint_y=(int32_t)r->scratch.next_record;o->passengers->world->contact.material=r->scratch.color_cursor_identity;return 0;
}
static int invoke(TaxiRestartSetupNative *o,uint32_t target){
 TaxiPassengerSetupOwned *p=o->passengers;TaxiPassengerScene *s=o->scene;
 if(target==0x08014a58){
  int rc=taxi_vehicle_start_gameplay(o->car,o->drive,o->vehicle,&s->random_state,o->marker,o->runtime->templates[0],584,o->session->settings[0],0,taxi_world_asset_ground,p->world->world,&o->resolved);return rc<0?-1:0;
 }
 if(target==0x08006580)return taxi_traffic_scene_reinitialize(o->traffic,o->traffic_capacity,taxi_traffic_density(o->session->settings[3]),o->traffic_parameters,o->traffic_sprites);
 if(target==0x0800b6ec){
  if(taxi_session_init_fare(o->session,&s->session,p->loading->state->gate.width_7542)||taxi_effect_pool_session_slots_reset(o->effects))return -1;
  s->pickup.pending_1704=signed_word(o->session->session_165e+166);s->pickup.selected_index=s->pickup.dialogue_index=-1;
  s->pickup.approach_x_7534=signed_word(o->session->scroll_7530+4);s->pickup.approach_z_7538=signed_word(o->session->scroll_7530+8);
  s->marker_phase=(TaxiPassengerMarkerPhase){o->session->effect_5f20[8],o->session->effect_5f20[9],o->session->effect_5f20[10]};
  o->drive->height.flag_1697=o->drive->driver.motion.orientation.flag_1697=s->session.ended_1697;
  o->drive->driver.motion.orientation.flag_1ea8=s->session.outer_state_1ea8;o->drive->driver.motion.orientation.flag_1694=s->session.trip.active_1694;
  o->drive->driver.freeze_2620=o->session->sound_2618[8];*o->counter_67d0=o->session->counter_67d0;
  o->pause->frozen=o->session->effect_5f20[0];o->pause->outer_state=s->session.outer_state_1ea8;p->loading->service->frozen_5f20=o->pause->frozen;
  o->pause->palette_request=o->session->effect_5f20[4];p->loading->service->pending_5f24=o->pause->palette_request;
  *o->intro_1696=o->session->session_165e[56];*o->flag_262c=o->session->sound_2618[20];*o->yaw_2618=signed_word(o->session->sound_2618);*o->yaw_target_261c=signed_word(o->session->sound_2618+4);return 0;
 }
 if(target==0x0800df8c){
  if(taxi_material_blink_select(p->world->selection,-1,NULL,0,o->session->settings[0],NULL,NULL))return -1;
  s->pickup.destination_index=-1;return 0;
 }
 if(target==0x080227ac){
  int rc;
  if(p->phase==TAXI_PASSENGER_SETUP_IDLE){
   s->count=p->world->asset->count;s->render_entries_valid=0;s->use_native_random=1;
   for(unsigned i=0;i<s->count;i++){const uint8_t *raw=p->world->asset->source[i];s->spawns[i]=(TaxiRoutePoint){signed_word(raw+4),signed_word(raw+12)};s->destinations[i]=raw[1];}
   rc=taxi_passenger_setup_owned_begin(p,o->retained_endpoint_y);
  }else rc=taxi_passenger_setup_owned_step(p);
  if(rc<=0)return rc<0?-1:1;
  s->eligible=0;for(unsigned i=0;i<s->count;i++)if(s->customers[i].bytes[1]<=9&&!(s->customers[i].bytes[0]&2))s->eligible++;
  return 0;
 }
 if(target==0x08011490){
  TaxiGameAudioControllerInput input={0};
  if(taxi_game_audio_frame_import(o->audio,&input,o->car,o->drive,s,o->traffic_globals,*o->traffic_timer))return -1;
  taxi_game_audio_controller_reset(o->audio,o->audio_auxiliary);o->engine_audio->handle[0]=o->audio->handles[0];o->engine_audio->handle[1]=o->audio->handles[1];
  taxi_game_audio_frame_commit(o->audio,o->drive,s,o->traffic_globals,o->traffic_timer);return 0;
 }
 if(target==0x0802954c){
  if(taxi_reward_session_reset(&s->session))return -1;
  o->traffic_globals->combo_flag_9aa6=s->session.end.flag_9aa6;o->traffic_globals->combo_9aa8=s->session.combo_whole_9aa8;o->traffic_globals->combo_9aac=s->session.combo_fraction_9aac;return 0;
 }
 return taxi_effect_pool_reset(o->effects,o->effect_ring);
}
int taxi_restart_setup_native(void *context,TaxiNormalRestartFlow *flow,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiRestartSetupNative *o=context;static const uint32_t targets[8]={0x08014a58,0x08006580,0x0800b6ec,0x0800df8c,0x080227ac,0x08011490,0x0802954c,0x08021c10};
 if(!o||(o->phase!=0&&o->phase!=1)||(!o->phase&&o->index)||!flow||!args||!value)return -1;
 if(valid(o)||flow->phase!=TAXI_RESTART_SETUP||flow->setup.phase!=2||o->index>=8||flow->setup.index!=o->index||target!=targets[o->index]||(o->phase&&o->flow!=flow))return -1;
 for(unsigned i=0;i<7;i++)if(args[i])return -1;
 if(!o->phase&&o->use_loading_scratch&&loading_inputs(o))return -1;
 if(!o->phase)o->flow=flow;o->phase=2;*value=0;int rc=invoke(o,target);
 if(rc<0){o->phase=255;return -1;}if(rc==1){o->phase=1;return 1;}
 o->index++;o->phase=o->index==8?3:1;return 0;
}
void taxi_restart_setup_native_cancel(TaxiRestartSetupNative *o){
 if(!o||o->phase!=1)return;
 if(o->index==4)taxi_passenger_setup_owned_cancel(o->passengers);o->phase=255;
}
