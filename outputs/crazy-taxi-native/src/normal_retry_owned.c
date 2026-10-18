#include "normal_retry_owned.h"
#include "game_audio_cleanup.h"
#include "music_playback.h"
static int valid(TaxiNormalRetryOwned *o,unsigned outer){
 if(!o||!o->pause||!o->loading||!o->session||!o->scene||!o->sounds||!o->channels||!o->audio||!o->tracker||!o->output_rate||!o->override_1708||!o->exit_5f1a||*o->override_1708||(o->session->settings[0]&224)==128)return -1;
 TaxiOrdinaryLoadingPending *p=o->loading;
 if(!p->presentation||!p->presentation->palette||!p->presentation->palette->fade||!p->service||(outer&&o->pause->outer_state!=outer))return -1;
 for(unsigned j=0;j<10;j++)if(o->sounds->handles[j]>4)return -1;
 return 0;
}
static void aliases(TaxiNormalRetryOwned *o){
 o->session->outer_1ea8=o->scene->session.outer_state_1ea8=o->pause->outer_state;
 o->session->effect_5f20[0]=o->loading->service->frozen_5f20=o->pause->frozen;
 *o->exit_5f1a=o->pause->exit_flag;
}
static uint32_t stop(void *context,TaxiGameAudioEngineRequest q){
 TaxiNormalRetryOwned *o=context;
 if(q.op==4&&q.handle&&q.handle<=4)taxi_audio_channel_stop(o->channels+q.handle-1,*o->output_rate);
 return 0;
}
static int fade(TaxiNormalRetryOwned *o,const TaxiPauseEvents *event,unsigned outer){
 if(!o||o->executing||o->phase||!event||!event->fade_out||valid(o,outer)||(outer==2&&!o->pause->exit_flag)||o->loading->phase!=TAXI_LOADING_IDLE||o->loading->presentation->palette->fade->fade.active_177b)return -1;
 o->executing=1;aliases(o);const uint32_t args[7]={0,32,0x201,0x201};
 int rc=taxi_city_palette_call(o->loading->presentation->palette,0x0800e5a8,args);
 o->phase=rc==1?1:255;o->executing=0;return rc==1?0:-1;
}
static int transfer(TaxiNormalRetryOwned *o,unsigned outer,int expiry){
 if(!o||o->executing||!o->phase||o->phase==255)return -1;
 if(o->phase==2)return outer==1?2:1;
 if(o->phase!=1||valid(o,outer)||(outer==2&&(expiry?(o->pause->exit_flag||o->pause->frozen):!o->pause->exit_flag)))return -1;
 TaxiOrdinaryLoadingPending *p=o->loading;
 if(p->phase==TAXI_LOADING_ABORTED)return -1;
 if(p->phase!=TAXI_LOADING_IDLE||p->presentation->palette->fade->fade.active_177b)return 0;
 o->executing=1;aliases(o);
 o->parent=(TaxiNormalExit){.outer=o->session->outer_1ea8,.frozen=o->session->effect_5f20[0],.ended=o->session->session_165e[57],.qualified=o->session->session_165e[51],.exit_flag=*o->exit_5f1a};
 int rc=taxi_normal_exit_begin(&o->parent,&o->requests);
 if(rc!=(outer==1?2:1)){o->phase=255;o->executing=0;return -1;}
 for(unsigned i=0;i<o->requests.count;i++)switch(o->requests.calls[i]){
 case 0x08010aac:o->stops=taxi_game_audio_cleanup_dispatch(o->sounds->handles,stop,o);break;
 case 0x08027754:taxi_music_playback(&o->tracker->state.playing,TAXI_MUSIC_RESUME);break;
 case 0x0802771c:taxi_audio_settings_restore_effects(o->audio);break;
 case 0x08027730:taxi_music_playback(&o->tracker->state.playing,TAXI_MUSIC_STOP);break;
 case 0x0800ac70:o->phase=2;break;
 default:o->phase=255;o->executing=0;return -1;
 }
 if(outer==2)o->phase=2;
 o->executing=0;return o->phase==2?(outer==1?2:1):-1;
}

int taxi_normal_retry_owned_fade(TaxiNormalRetryOwned *o,const TaxiPauseEvents *event){return fade(o,event,1);}
int taxi_normal_retry_owned_transfer(TaxiNormalRetryOwned *o){return transfer(o,1,0);}
int taxi_normal_quit_owned_fade(TaxiNormalRetryOwned *o,const TaxiPauseEvents *event){return fade(o,event,2);}
int taxi_normal_quit_owned_transfer(TaxiNormalRetryOwned *o){return transfer(o,2,0);}
/* Only actual source parent writes are mirrored. Credits preserve frozen until
   their real child return; neither bank/counter/RNG nor sound handles reset. */
static void quit_aliases(TaxiNormalRetryOwned *o){
 o->session->outer_1ea8=o->scene->session.outer_state_1ea8=o->pause->outer_state=o->parent.outer;
 o->session->session_165e[57]=o->scene->session.ended_1697=o->parent.ended;
 o->session->effect_5f20[0]=o->loading->service->frozen_5f20=o->pause->frozen=o->parent.frozen;
}
int taxi_normal_quit_owned_prepare(TaxiNormalRetryOwned *o){
 if(!o||o->executing||o->phase<2||o->phase>4||valid(o,0)||!o->parent.exit_flag)return -1;
 if(o->phase==4)return 1;if(o->phase==3)return 0;
 o->executing=1;int rc=taxi_normal_exit_prepare(&o->parent,0,&o->requests);
 if((rc!=1&&rc!=2)||(rc==1&&o->requests.count)||(rc==2&&(o->requests.count!=1||o->requests.calls[0]!=0x080243a4))){o->phase=255;o->executing=0;return -1;}
 quit_aliases(o);o->phase=rc==2?3:4;o->executing=0;return rc;
}
int taxi_normal_quit_owned_resume(TaxiNormalRetryOwned *o,int returned){
 if(!o||o->executing||o->phase<3||o->phase>4||valid(o,0)||!o->parent.exit_flag)return -1;
 if(o->phase==4)return 1;if(!returned)return 0;
 o->executing=1;int rc=taxi_normal_exit_resume(&o->parent,1,&o->requests);
 if(rc!=1||o->requests.count){o->phase=255;o->executing=0;return -1;}
 quit_aliases(o);o->phase=4;o->executing=0;return 1;
}

/* Original BEF6/C06C timer-expiry fade, distinct from pause Yes201 masks. */
int taxi_normal_expiry_owned_fade(TaxiNormalRetryOwned *o){
 if(!o||o->executing||o->phase||valid(o,2)||o->pause->exit_flag||o->pause->frozen||o->scene->session.outer_state_1ea8!=2||o->scene->session.ended_1697||o->loading->phase!=TAXI_LOADING_IDLE||o->loading->presentation->palette->fade->fade.active_177b)return -1;
 o->executing=1;aliases(o);o->session->session_165e[57]=o->scene->session.ended_1697;
 const uint32_t args[7]={0,32,1,1};int rc=taxi_city_palette_call(o->loading->presentation->palette,0x0800e5a8,args);
 o->phase=rc==1?1:255;o->executing=0;return rc==1?0:-1;
}
int taxi_normal_expiry_owned_transfer(TaxiNormalRetryOwned *o){return transfer(o,2,1);}
