#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/host_time.h"
#include "../src/frame_event.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define LIMIT 4096
typedef struct {
 TaxiHostTime *host;TaxiFrameEvent event;
 uint32_t counters[LIMIT];unsigned calls,stage,audio,registered,ack;
} Trace;
static int event_call(void *opaque,TaxiFrameEvent *event,uint32_t target,uint32_t argument){
 Trace *t=opaque;assert(t->host->counter==event->counter_3520);
 if(target==0x08026888){assert(t->stage==0);t->stage=1;t->audio++;}
 else if(target==0x0803244c){assert(t->stage==1&&argument==event->callback_352c);t->stage=2;t->registered++;}
 else {assert(target==0x080161f4&&t->stage==2);t->stage=3;t->ack++;}
 return 0;
}
static void service(void *opaque,uint32_t counter){
 Trace *t=opaque;assert(t->calls<LIMIT&&t->host->counter==counter);
 assert(counter==(uint32_t)(t->event.counter_3520+1));t->stage=0;
 assert(!taxi_frame_event(&t->event,160,event_call,t));assert(t->stage==3&&t->event.acknowledge==1);
 assert(t->event.counter_3520==counter);t->counters[t->calls++]=counter;
}
static Trace trace(TaxiHostTime *host){
 Trace t={.host=host,.event={.counter_3520=host->counter,.callback_352c=0x080161dd}};
 return t;
}
static void equal_clock(const TaxiHostTime *a,const TaxiHostTime *b){
 assert(a->clock.phase==b->clock.phase&&a->counter==b->counter);
 assert(!memcmp(&a->frame,&b->frame,sizeof a->frame));
}
int main(void){
 TaxiHostTime whole={.frame={.previous_3524=17,.delta_3b00=7}},plain=whole,split=whole;
 Trace all=trace(&whole),parts=trace(&split);uint32_t ticks=777,old_ticks;
 assert(!taxi_host_time_advance_serviced(&whole,UINT64_C(1000000000),service,&all,&ticks)&&ticks==59);
 assert(!taxi_host_time_advance(&plain,UINT64_C(1000000000),&old_ticks)&&old_ticks==ticks);
 equal_clock(&whole,&plain);assert(all.calls==59&&all.audio==59&&all.registered==59&&all.ack==59);
 assert(all.counters[0]==1&&all.counters[58]==59&&all.event.settings_1eb0==8);
 assert(whole.frame.previous_3524==17&&whole.frame.delta_3b00==7);
 uint32_t sum=0;
 const uint64_t spans[]={1,16742705,1,123456789,250000000,609800504};
 for(unsigned i=0;i<sizeof spans/sizeof *spans;i++){
  assert(!taxi_host_time_advance_serviced(&split,spans[i],service,&parts,&ticks));sum+=ticks;
 }
 equal_clock(&whole,&split);assert(sum==59&&parts.calls==all.calls);
 assert(!memcmp(parts.counters,all.counters,all.calls*sizeof *all.counters));
 TaxiHostTime wrap={.counter=UINT32_MAX-1};Trace wrapped=trace(&wrap);
 assert(!taxi_host_time_advance_serviced(&wrap,UINT64_C(1000000000),service,&wrapped,&ticks));
 assert(ticks==59&&wrap.counter==57&&wrapped.counters[0]==UINT32_MAX&&wrapped.counters[1]==0&&wrapped.counters[58]==57);
 TaxiHostTime minute={0};Trace long_run=trace(&minute);
 assert(!taxi_host_time_advance_serviced(&minute,UINT64_C(60000000000),service,&long_run,&ticks));
 assert(ticks==3583&&long_run.calls==3583&&minute.counter==3583);
 TaxiHostTime zero={0},before=zero;Trace none=trace(&zero);ticks=99;
 assert(!taxi_host_time_advance_serviced(&zero,0,service,&none,&ticks)&&!ticks&&!none.calls);
 equal_clock(&zero,&before);ticks=99;
 assert(taxi_host_time_advance_serviced(&zero,UINT64_MAX,service,&none,&ticks)==-1);
 assert(ticks==99&&!none.calls&&!memcmp(&zero,&before,sizeof zero));
 assert(taxi_host_time_advance_serviced(&zero,100,NULL,&none,&ticks)==-1&&!none.calls);
 assert(taxi_host_time_advance_serviced(NULL,100,service,&none,&ticks)==-1);
 assert(taxi_host_time_advance_serviced(&zero,100,service,&none,NULL)==-1&&!none.calls);
 TaxiHostTime frozen={0};Trace background=trace(&frozen);
 assert(!taxi_host_time_advance_serviced(&frozen,UINT64_C(1000000000),service,&background,&ticks));
 assert(taxi_host_time_present_after_ticks(&frozen,1,60)==0&&background.calls==59);
 assert(taxi_host_time_present_after_ticks(&frozen,1,5)==1);
 assert(frozen.frame.delta_3b00==59&&!frozen.frame.elapsed_755c&&background.calls==59);
 puts("PASS native per-tick counter visibility, original frame event order, batching equivalence, wrap, one-minute delivery, frozen presentation, no-tick and atomic invalid-time rejection");
 return 0;
}
