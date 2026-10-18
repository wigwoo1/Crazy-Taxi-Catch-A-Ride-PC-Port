#include "normal_restart_owned.h"
#include "music_playback.h"
#include "random.h"
static int valid(TaxiNormalRestartOwners *o){
 return !o||!o->tracker||!o->asset||!o->random||!o->loaded||!o->resource?-1:0;
}
static int arguments(uint32_t target,uint32_t a,uint32_t b){
 switch(target){
 case 0x08027730:case 0x0800f5e8:case 0x08007378:case 0x080073c0:return a||b?-1:0;
 case 0x08026ef8:return a!=0x08421fc0||b?-1:0;
 case 0x0801e72c:return a||b!=65535?-1:0;
 case 0x08027110:return b?-1:0;
 case 0x08025c18:return a!=4||b!=1?-1:0;
 default:return -1;
 }
}
int taxi_normal_restart_owned_call(void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 TaxiNormalRestartOwners *o=context;if(valid(o)||!result||arguments(target,a,b))return -1;*result=0;
 switch(target){
 case 0x08027730:return taxi_music_playback(&o->tracker->state.playing,TAXI_MUSIC_STOP);
 case 0x08026ef8:
  if(a!=0x08421fc0)return -1;
  *o->loaded=0;o->tracker->state.state[0]=0;
  return o->enabled?taxi_music_asset_index(o->asset,o->module,o->module_size):0;
 case 0x0801e72c:
  if(a||b!=65535)return -1;
  *result=(uint32_t)taxi_random_range(o->random,0,65535);return 0;
 case 0x08027110:
  if(!o->enabled)return 0;
  if(!taxi_music_tracker_start(o->tracker,o->asset,(uint8_t)a))return -1;
  *o->loaded=1;o->tracker->state.state[0]=1;return 0;
 default:return o->resource(o->context,target,a,b,result);
 }
}
int taxi_normal_restart_owned(TaxiNormalRestart *s,TaxiNormalRestartOwners *o){
 if(!s||valid(o))return -1;
 return taxi_normal_restart_prefix(s,taxi_normal_restart_owned_call,o);
}

int taxi_normal_restart_prefix_owned(void *context,TaxiNormalRestartFlow *flow,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiNormalRestartPrefixOwners *o=context;
 if(!o||!flow||flow->phase!=TAXI_RESTART_PREFIX||!args||!value||!o->qualified_1691||!o->flag_153c||!o->flag_153d||valid(o->services)||arguments(target,args[0],args[1]))return -1;
 for(unsigned i=2;i<7;i++)if(args[i])return -1;
 if(target==0x0800f5e8){
  if(flow->prefix.qualified_1691||flow->prefix.flag_153c||flow->prefix.flag_153d)return -1;
  *o->qualified_1691=flow->prefix.qualified_1691;*o->flag_153c=flow->prefix.flag_153c;*o->flag_153d=flow->prefix.flag_153d;
 }
 return taxi_normal_restart_owned_call(o->services,target,args[0],args[1],value);
}
