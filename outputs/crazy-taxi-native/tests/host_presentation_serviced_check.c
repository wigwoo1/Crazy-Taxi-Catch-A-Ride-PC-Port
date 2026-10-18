#ifdef NDEBUG
#undef NDEBUG
#endif
#include "host_presentation.h"
#include "frame_event.h"
#include "frame_wait.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {
 TaxiFrameEvent event;TaxiFrameWait wait;
 unsigned calls,ticks,audio,callbacks,fail_at;
 uint32_t previous;uint8_t pending;
} Fixture;
static int event_call(void *context,TaxiFrameEvent *event,uint32_t target,uint32_t argument){
 Fixture *f=context;assert(event==&f->event);
 if(target==0x08026888){f->audio++;return 0;}
 if(target==0x0803244c){assert(taxi_frame_event_callback_is_noop(argument));f->callbacks++;return 0;}
 if(target==0x080161f4)return taxi_frame_wait_signal(&f->wait);
 return -1;
}
static void tick(void *context,uint32_t counter){
 Fixture *f=context;assert(!taxi_frame_event_after_counter(&f->event,counter,event_call,f));f->ticks++;
}
static int helper(void *context,TaxiHostTime *host,TaxiPresentationService *s,uint32_t target,uint32_t argument){
 Fixture *f=context;
 const uint32_t with_music[]={0x08016208,0x08022508,0x08015c3c,0x0800ee10,0x08027cd0,0x08028ee0};
 const uint32_t no_music[]={0x08016208,0x08015c3c,0x0800ee10,0x08027cd0,0x08028ee0};
 unsigned index=f->calls++;
 assert(target==(f->pending?with_music[index]:no_music[index]));
 assert(argument==(index==0?1:(target==0x08022508?(f->pending==2):0)));
 assert(s->current_3520==host->counter&&s->previous_3524==f->previous);
 /* Palette/OAM/copy helpers must still see the preceding frame delta. */
 assert(s->delta_3b00==7&&host->frame.delta_3b00==7);
 if(target==0x08016208){f->wait.handshake&=UINT16_C(0xfffe);}
 uint32_t ticks;assert(!taxi_host_time_advance_serviced(host,20000000,tick,f,&ticks));assert(ticks>=1);
 /* Deliberately stale local copy: canonical events own the counter. */
 s->current_3520=0xdeadbeef;
 return f->fail_at==f->calls?-1:0;
}
int main(void){
 unsigned cases=0;
 for(unsigned pending=0;pending<3;pending++)
 for(unsigned frozen=0;frozen<2;frozen++)
 for(unsigned wrap=0;wrap<2;wrap++)
 for(unsigned fail_at=0;fail_at<=(pending?6u:5u);fail_at++){
  uint32_t previous=wrap?UINT32_MAX-2:100;
  TaxiHostTime host={.counter=previous,.frame={.previous_3524=previous,.delta_3b00=7,.elapsed_755c=UINT32_MAX-3,.animation_1e10=UINT32_MAX-1,.page_7540=1}};
  TaxiPresentationService s={.pending_5f24=(uint8_t)pending,.frozen_5f20=(uint8_t)frozen,.previous_3524=previous,.delta_3b00=7,.elapsed_755c=UINT32_MAX-3,.cursor_1e10=UINT32_MAX-1,.parity_7540=1};
  TaxiFrameTime before=host.frame;
  Fixture f={.event={.counter_3520=previous,.settings_1eb0=255},.wait={.event_delivery_enabled=1},.previous=previous,.pending=(uint8_t)pending,.fail_at=fail_at};
  assert(!taxi_frame_event_register(&f.event,0));
  int result=taxi_host_presentation_commit_serviced(&host,&s,helper,&f);
  assert(s.current_3520==host.counter&&host.counter==f.event.counter_3520);
  assert(f.audio==f.ticks&&f.callbacks==f.ticks&&f.event.acknowledge==1&&f.wait.handshake==1);
  assert(f.event.settings_1eb0==239);
  if(fail_at){
   assert(result==-1&&f.calls==fail_at);
   assert(!memcmp(&before,&host.frame,sizeof before));
   assert(s.previous_3524==previous&&s.delta_3b00==7&&s.elapsed_755c==before.elapsed_755c&&s.cursor_1e10==before.animation_1e10&&s.parity_7540==1);
  }else{
   assert(!result&&f.calls==(pending?6u:5u)&&!s.pending_5f24);
   assert(host.frame.previous_3524==host.counter&&host.frame.delta_3b00==(int32_t)f.ticks);
   assert(host.frame.elapsed_755c==before.elapsed_755c+(frozen?0:f.ticks));
   assert(host.frame.animation_1e10==2&&host.frame.page_7540==0);
   assert(s.previous_3524==host.frame.previous_3524&&s.delta_3b00==(uint32_t)host.frame.delta_3b00&&s.elapsed_755c==host.frame.elapsed_755c&&s.cursor_1e10==2&&s.parity_7540==0);
  }
  cases++;
 }
 TaxiHostTime h={0};TaxiPresentationService s={0};Fixture f={0};
 assert(taxi_host_presentation_commit_serviced(NULL,&s,helper,&f)==-1);
 assert(taxi_host_presentation_commit_serviced(&h,NULL,helper,&f)==-1);
 assert(taxi_host_presentation_commit_serviced(&h,&s,NULL,&f)==-1);
 h.frame.delta_3b00=1;assert(taxi_host_presentation_commit_serviced(&h,&s,helper,&f)==-1&&!f.calls);
 printf("PASS live canonical presentation counter: %u success/error cases; ordered helpers, source events, wrap, freeze, single tail and retained error progress\n",cases);
 return 0;
}