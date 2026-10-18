#include "normal_restart_entry.h"
static int invoke(TaxiRestartEntryCall call,void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t c,uint32_t d){
 const uint32_t args[4]={a,b,c,d};return call(context,target,args);
}
/* Native phase3 retains the exact source continuation at26318; the first
   clear and every earlier helper must not be repeated while it is suspended. */
static int after_publication(TaxiRestartEntry *s,TaxiRestartEntryCall call,void *context){
 int status=invoke(call,context,0x08026318,0,0,0,0);
 if(status==1)return 0;
 if(status)return -1;
 if(invoke(call,context,0x0802c9f0,0,0,s->width_7542,s->height_7544)||
    invoke(call,context,0x0800e5a8,1,32,1,1))return -1;
 s->frame_34d4=s->retained_sl;s->frame_34d8=s->retained_sl;
 if(invoke(call,context,0x0801ee28,0,0,0,0)||invoke(call,context,0x0802277c,0,0,0,0))return -1;
 s->scroll_x=0;s->scroll_y=0;
 if(invoke(call,context,0x08026440,0,0,0,0))return -1;
 s->phase=2;return 1;
}
static int after_wait(TaxiRestartEntry *s,TaxiRestartEntryCall call,void *context){
 if(invoke(call,context,0x0802c9f0,0,0,s->width_7542,s->height_7544))return -1;
 s->phase=3;return after_publication(s,call,context);
}
int taxi_restart_entry_begin(TaxiRestartEntry *s,TaxiRestartEntryCall call,void *context){
 if(!s||!call||s->phase)return -1;
 if(invoke(call,context,0x08026440,0,0,0,0)||invoke(call,context,0x0800e5a8,0,32,1,1))return -1;
 s->phase=1;return s->fade_177b?0:after_wait(s,call,context);
}
int taxi_restart_entry_step(TaxiRestartEntry *s,TaxiRestartEntryCall call,void *context){
 if(!s||!call||(s->phase!=1&&s->phase!=3&&s->phase!=4))return -1;
 if(s->phase==3)return after_publication(s,call,context);
 if(s->phase==4||s->fade_177b){int status=invoke(call,context,0x0800d764,0,0,0,0);if(status==1){s->phase=4;return 0;}if(status)return -1;s->phase=1;}
 return s->fade_177b?0:after_wait(s,call,context);
}
