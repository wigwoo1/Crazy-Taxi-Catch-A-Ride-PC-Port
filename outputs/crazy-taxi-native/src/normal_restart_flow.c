#include "normal_restart_flow.h"
#include <string.h>
typedef struct {TaxiNormalRestartFlow *s;TaxiRestartFlowCall call;void *context;} Bridge;
static int small(void *opaque,uint32_t target,uint32_t a,uint32_t b,uint32_t *value){
 Bridge *p=opaque;const uint32_t args[7]={a,b,0,0,0,0,0};return p->call(p->context,p->s,target,args,value);
}
static int graphics(void *opaque,uint32_t target,const uint32_t args[7]){
 Bridge *p=opaque;uint32_t value=0;return p->call(p->context,p->s,target,args,&value);
}
static int entry(void *opaque,uint32_t target,const uint32_t input[4]){
 Bridge *p=opaque;uint32_t args[7]={0},value=0;memcpy(args,input,4*sizeof *args);
 return p->call(p->context,p->s,target,args,&value);
}
static int result(TaxiNormalRestartFlow *s,int rc){
 if(rc<0)s->phase=TAXI_RESTART_ERROR;
 else if(rc==1)s->phase=TAXI_RESTART_READY;
 return rc;
}
static int setup(TaxiNormalRestartFlow *s,Bridge *b){
 s->phase=TAXI_RESTART_SETUP;
 int status=s->setup.phase?taxi_normal_restart_setup_step(&s->setup,&s->frame,small,b):taxi_normal_restart_setup_begin(&s->setup,&s->frame,s->graphics.retained_sl,small,b);
 if(status<0)return result(s,-1);
 if(!status)return 0;
 s->entry.frame_34d4=s->frame.first_34d4;s->entry.frame_34d8=s->frame.second_34d8;
 s->entry.retained_sl=s->graphics.retained_sl;s->entry.fade_177b=s->graphics.fade_177b;
 s->phase=TAXI_RESTART_ENTRY;
 return result(s,taxi_restart_entry_begin(&s->entry,entry,b));
}
int taxi_normal_restart_flow_begin(TaxiNormalRestartFlow *s,TaxiRestartFlowCall call,void *context){
 if(!s||!call||s->phase||s->graphics.phase||s->entry.phase||s->setup.phase||s->graphics.city>1)return -1;
 Bridge b={s,call,context};s->phase=TAXI_RESTART_PREFIX;
 if(taxi_normal_restart_prefix(&s->prefix,small,&b))return result(s,-1);
 s->phase=TAXI_RESTART_GRAPHICS;int rc=taxi_restart_graphics_begin(&s->graphics,graphics,&b);
 if(rc<0)return result(s,rc);
 return rc?setup(s,&b):0;
}
int taxi_normal_restart_flow_step(TaxiNormalRestartFlow *s,TaxiRestartFlowCall call,void *context){
 if(!s||!call)return -1;
 Bridge b={s,call,context};
 if(s->phase==TAXI_RESTART_GRAPHICS){
  int rc=taxi_restart_graphics_step(&s->graphics,graphics,&b);
  if(rc<0)return result(s,rc);
  return rc?setup(s,&b):0;
 }
 if(s->phase==TAXI_RESTART_SETUP)return setup(s,&b);
 if(s->phase==TAXI_RESTART_ENTRY)return result(s,taxi_restart_entry_step(&s->entry,entry,&b));
 return -1;
}
