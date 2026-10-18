#ifdef NDEBUG
#undef NDEBUG
#endif
#include "host_presentation.h"
#include "frame_event.h"
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int reference_presentation_service(TaxiPresentationService *,TaxiPresentationCall,void *);

typedef struct {
 TaxiHostTime host;TaxiPresentationService service;TaxiFrameWait wait;TaxiFrameEvent event;
 TaxiInput input;TaxiHostPresentationPending pending;
 uint32_t targets[8],arguments[8];unsigned calls,events,audio,callbacks,seed,fail_at;
 uint8_t asynchronous;
} Fixture;
static int event_work(void *ctx,TaxiFrameEvent *e,uint32_t target,uint32_t argument){
 Fixture *f=ctx;assert(e==&f->event&&e->counter_3520==f->host.counter);
 if(target==0x08026888){f->audio++;return 0;}
 if(target==0x0803244c){assert(taxi_frame_event_callback_is_noop(argument));f->callbacks++;return 0;}
 if(target==0x080161f4)return taxi_frame_wait_signal(&f->wait);
 return -1;
}
static void tick(void *ctx,uint32_t counter){
 Fixture *f=ctx;assert(counter==f->host.counter);
 assert(!taxi_frame_event_after_counter(&f->event,counter,event_work,f));f->events++;
}
static void elapsed(Fixture *f,unsigned n){
 /* Explicit test clock injections, not inferred original helper costs. */
 uint32_t count;assert(!taxi_host_time_advance_serviced(&f->host,(uint64_t)n*20000000,tick,f,&count));
 assert(count>=n);
}
static void release_wait(Fixture *f){
 assert(!(f->wait.handshake&1));elapsed(f,1+f->seed%3);
 /* A shared source write while waiting must be read AFTER16208 returns. */
 f->service.pending_5f24=(uint8_t)(f->seed%4);
}
static int body(void *ctx,TaxiHostTime *host,TaxiPresentationService *s,uint32_t target,uint32_t argument){
 Fixture *f=ctx;assert(host==&f->host&&s==&f->service&&s->current_3520==host->counter);
 assert(target!=0x08016208&&s->delta_3b00==7&&host->frame.delta_3b00==7);
 assert(f->calls<8);f->targets[f->calls]=target;f->arguments[f->calls]=argument;f->calls++;
 if(f->asynchronous)assert(taxi_host_presentation_pending_poll(&f->pending)==-1);
 elapsed(f,1+(f->seed+f->calls)%3);
 if(f->calls==f->fail_at)return -1;
 if(target==0x08015c3c){
  assert(s->pending_5f24==0);
  taxi_input_update(&f->input,(TaxiInputSample){.active_low_keys=(uint16_t)(1023^(f->seed&1023)),.elapsed_ticks=(uint16_t)s->delta_3b00});
  s->pending_5f24=(uint8_t)((f->seed/3)%3);s->frozen_5f20=(uint8_t)((f->seed>>1)&1);
  s->mode_7541=(uint8_t)(f->seed%6);
 }
 return 0;
}
static int reference_work(void *ctx,TaxiPresentationService *s,uint32_t target,uint32_t argument){
 Fixture *f=ctx;int result;
 s->current_3520=f->host.counter;
 if(target==0x08016208){assert(argument==1);f->wait.handshake&=UINT16_C(0xfffe);release_wait(f);result=0;}
 else result=body(f,&f->host,s,target,argument);
 s->current_3520=f->host.counter;return result;
}
static void initialize(Fixture *f,unsigned seed,unsigned failure,unsigned wrap){
 memset(f,0,sizeof *f);f->seed=seed;f->fail_at=failure;
 uint32_t previous=wrap?UINT32_MAX-2:123;
 f->host.counter=previous;f->host.frame.previous_3524=previous;f->host.frame.delta_3b00=7;
 f->host.frame.elapsed_755c=UINT32_MAX-2;f->host.frame.animation_1e10=UINT32_MAX-3;f->host.frame.page_7540=255;
 f->service=(TaxiPresentationService){.pending_5f24=(uint8_t)(seed%4),.mode_7541=(uint8_t)(seed%6),.bank_7548=(uint8_t)seed,.frozen_5f20=(uint8_t)(seed&1),.parity_7540=255,.display_control=0xffef,.blend_control=0x1234,.blend_alpha=0x4567,.first_754c=0x08000100,.second_7550=0x08000200,.previous_3524=previous,.delta_3b00=7,.elapsed_755c=UINT32_MAX-2,.cursor_1e10=UINT32_MAX-3};
 f->wait=(TaxiFrameWait){1,0xffab};f->event.counter_3520=previous;f->event.settings_1eb0=255;
 assert(!taxi_frame_event_register(&f->event,0));
}
static void publish_reference(Fixture *f){
 f->host.frame.previous_3524=f->service.previous_3524;f->host.frame.delta_3b00=(int32_t)f->service.delta_3b00;
 f->host.frame.elapsed_755c=f->service.elapsed_755c;f->host.frame.animation_1e10=f->service.cursor_1e10;f->host.frame.page_7540=f->service.parity_7540;
}
static unsigned differential(void){
 unsigned cases=0;
 for(unsigned seed=0;seed<144;seed++)for(unsigned failure=0;failure<6;failure++)for(unsigned wrap=0;wrap<2;wrap++){
  Fixture a,b;initialize(&a,seed,failure,wrap);initialize(&b,seed,failure,wrap);b.asynchronous=1;
  int before=reference_presentation_service(&a.service,reference_work,&a);if(!before)publish_reference(&a);
  assert(taxi_host_presentation_pending_begin(&b.pending,&b.host,&b.service,&b.wait,body,&b)==0);
  TaxiHostTime held_host=b.host;TaxiPresentationService held_service=b.service;
  assert(b.wait.handshake==0xffaa);
  for(unsigned poll=0;poll<17;poll++){
   assert(taxi_host_presentation_pending_poll(&b.pending)==0);
   assert(!b.calls&&!b.events&&!b.audio&&!b.callbacks);
   assert(!memcmp(&held_host,&b.host,sizeof held_host)&&!memcmp(&held_service,&b.service,sizeof held_service));
   assert(b.wait.handshake==0xffaa);
  }
  TaxiHostPresentationPending duplicate=b.pending;
  assert(taxi_host_presentation_pending_begin(&b.pending,&b.host,&b.service,&b.wait,body,&b)==-1);
  assert(!memcmp(&duplicate,&b.pending,sizeof duplicate));
  release_wait(&b);
  int after=taxi_host_presentation_pending_poll(&b.pending);
  assert(after==(!before?1:-1));assert(!b.pending.active);
  unsigned calls=b.calls;TaxiHostTime final_host=b.host;TaxiPresentationService final_service=b.service;
  assert(taxi_host_presentation_pending_poll(&b.pending)==-1&&calls==b.calls);
  assert(!memcmp(&final_host,&b.host,sizeof final_host)&&!memcmp(&final_service,&b.service,sizeof final_service));
  assert(!memcmp(&a.host,&b.host,sizeof a.host)&&!memcmp(&a.service,&b.service,sizeof a.service));
  assert(!memcmp(&a.wait,&b.wait,sizeof a.wait)&&!memcmp(&a.event,&b.event,sizeof a.event));
  assert(!memcmp(&a.input,&b.input,sizeof a.input));
  assert(a.calls==b.calls&&a.events==b.events&&a.audio==b.audio&&a.callbacks==b.callbacks);
  assert(!memcmp(a.targets,b.targets,sizeof a.targets)&&!memcmp(a.arguments,b.arguments,sizeof a.arguments));cases++;
 }
 return cases;
}
static unsigned wait_counts(void){
 unsigned cases=0;
 for(unsigned enabled=0;enabled<4;enabled++)for(unsigned frames=0;frames<512;frames++){
  TaxiFrameWait wait={(uint16_t)enabled,0xa5ff};TaxiFrameWaitPending p={0};
  unsigned count=enabled==1?(uint8_t)frames:0;
  assert(taxi_frame_wait_pending_begin(&p,&wait,frames)==(count?0:1));
  if(!count){assert(wait.handshake==0xa5ff&&taxi_frame_wait_pending_poll(&p)==1);}
  else for(unsigned i=0;i<count;i++){
   assert(!(wait.handshake&1));uint16_t held=wait.handshake;
   for(unsigned j=0;j<3;j++)assert(!taxi_frame_wait_pending_poll(&p)&&wait.handshake==held);
   assert(!taxi_frame_wait_signal(&wait));
   assert(taxi_frame_wait_pending_poll(&p)==(i+1==count?1:0));
  }
  assert(!p.active&&taxi_frame_wait_pending_poll(&p)==-1);cases++;
 }
 return cases;
}
static void cancellation_and_invalid(void){
 Fixture f;initialize(&f,1,0,0);TaxiFrameWait prior=f.wait;
 assert(taxi_host_presentation_pending_begin(NULL,&f.host,&f.service,&f.wait,body,&f)==-1);
 f.service.delta_3b00++;
 assert(taxi_host_presentation_pending_begin(&f.pending,&f.host,&f.service,&f.wait,body,&f)==-1);
 assert(!memcmp(&prior,&f.wait,sizeof prior)&&!f.pending.active);f.service.delta_3b00--;
 assert(!taxi_host_presentation_pending_begin(&f.pending,&f.host,&f.service,&f.wait,body,&f));
 release_wait(&f);TaxiHostTime host=f.host;TaxiPresentationService s=f.service;prior=f.wait;
 taxi_host_presentation_pending_cancel(&f.pending);
 assert(!memcmp(&host,&f.host,sizeof host)&&!memcmp(&s,&f.service,sizeof s)&&!memcmp(&prior,&f.wait,sizeof prior));
 assert(!f.calls&&taxi_host_presentation_pending_poll(&f.pending)==-1);
 /* An already delivered event must not satisfy a NEW fresh wait. */
 assert(!taxi_host_presentation_pending_begin(&f.pending,&f.host,&f.service,&f.wait,body,&f));
 assert(!(f.wait.handshake&1)&&!taxi_host_presentation_pending_poll(&f.pending));taxi_host_presentation_pending_cancel(&f.pending);
 /* Source disabled16208 returns immediately, without consuming the flag. */
 initialize(&f,1,0,0);f.wait.event_delivery_enabled=0;prior=f.wait;
 assert(taxi_host_presentation_pending_begin(&f.pending,&f.host,&f.service,&f.wait,body,&f)==1);
 f.asynchronous=1;assert(taxi_host_presentation_pending_poll(&f.pending)==1&&f.calls==5);
 assert(prior.event_delivery_enabled==f.wait.event_delivery_enabled);
}
int main(void){
 unsigned waits=wait_counts(),frames=differential();cancellation_and_invalid();
 printf("PASS pending fresh wait: %u low-byte/count/delivery cases; %u full26318 differential cases including17empty polls, live source/audio events, input prior-delta, post-wait writes, modes/bank wraps, counter/elapsed wraps, helper failures, cancellation, reentrancy and exactly one timing tail\n",waits,frames);return 0;
}