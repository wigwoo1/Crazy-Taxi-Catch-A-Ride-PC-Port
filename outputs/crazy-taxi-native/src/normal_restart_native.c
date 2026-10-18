#include "normal_restart_native.h"
#include <string.h>
static int valid(TaxiNormalRestartNative *o){
 if(!o||!o->prefix||!o->prefix->services||!o->resources||!o->visual||!o->setup||!o->setup->scene||!o->setup->session||!o->setup->passengers||!o->setup->passengers->passengers||!o->visual->loading||!o->visual->loading->presentation)return -1;
 TaxiNormalRestartOwners *p=o->prefix->services;TaxiRestartVisualOwners *v=o->visual;TaxiRestartSetupNative *s=o->setup;
 TaxiOrdinaryGraphicsOwners *g=v->loading->presentation->graphics;
 if(!v->driver_graphics_187c||!v->intro_graphics_1804||!v->frame_34d4||!v->frame_34d8||!g||p->random!=&s->scene->random_state||p->resource!=taxi_restart_resources_call||p->context!=o->resources||o->resources->palette!=g->palette||o->resources->display!=v->display||o->resources->bases!=v->bases||v->session!=s->session||v->pause!=s->pause||v->loading!=s->passengers->loading||!v->loading->retained_palette||!s->use_loading_scratch||o->prefix->qualified_1691!=s->session->session_165e+51)return -1;
 TaxiPassengerInitPending *a=s->passengers->passengers;
 if(!a->override_1708||*a->override_1708||(s->session->settings[0]&224)==128)return -1;
 return 0;
}
int taxi_normal_restart_native(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiNormalRestartNative *o=context;if(!o||o->executing||o->aborted||!s||!args||!value||valid(o)||(o->flow&&o->flow!=s))return -1;
 if(!o->flow&&(s->phase!=TAXI_RESTART_PREFIX||target!=0x08027730||s->graphics.retained_sl!=1||o->visual->loading->phase!=TAXI_LOADING_IDLE||o->visual->graphics_index||o->visual->entry_index||o->setup->phase||o->setup->index))return -1;
 if(s->phase<TAXI_RESTART_PREFIX||s->phase>TAXI_RESTART_ENTRY)return -1;
 o->executing=1;int rc;
 if(s->phase==TAXI_RESTART_PREFIX){
  if(target==0x0800f5e8){
   if(s->prefix.qualified_1691||s->prefix.flag_153c||s->prefix.flag_153d){o->executing=0;return -1;}
   o->setup->scene->session.fare.qualified_1691=s->prefix.qualified_1691;
  }
  rc=taxi_normal_restart_prefix_owned(o->prefix,s,target,args,value);
 }else if(s->phase==TAXI_RESTART_SETUP){
  if(!o->setup->phase){
   *o->visual->frame_34d4=s->frame.first_34d4;*o->visual->frame_34d8=s->frame.second_34d8;
  }
  rc=taxi_restart_setup_native(o->setup,s,target,args,value);
 }else {
  rc=taxi_restart_visual_owned(o->visual,s,target,args,value);
  if(!rc&&s->phase==TAXI_RESTART_ENTRY&&target==0x0801ee28){
   const uint8_t *p=o->setup->session->session_165e+102;uint32_t raw=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
   int32_t offset;memcpy(&offset,&raw,4);o->setup->scene->session.distance_display.offset_16c4=offset;
  }
 }
 if(rc>=0)o->flow=s;else o->aborted=1;o->executing=0;return rc;
}
void taxi_normal_restart_native_cancel(void *context,uint32_t target){
 TaxiNormalRestartNative *o=context;if(!o||o->executing||o->aborted||target!=0x080227ac||!o->setup)return;
 taxi_restart_setup_native_cancel(o->setup);o->aborted=1;
}
