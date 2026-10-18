#ifdef NDEBUG
#undef NDEBUG
#endif
#include "host_presentation.h"
#include <stdio.h>
#include <string.h>
typedef struct {TaxiHostTime host;TaxiPresentationService service;TaxiFrameWait wait;TaxiHostPresentationPending pending;unsigned calls,fail_at,bad;} Fixture;
static int work(void *context,TaxiHostTime *host,TaxiPresentationService *service,uint32_t target,uint32_t argument){
 Fixture *f=context;(void)target;(void)argument;f->calls++;
 TaxiHostPresentationPending before=f->pending;TaxiFrameWait flags=f->wait;
 /* A helper may re-enter an owner via a host event pump. It must not start
    another wait, clear flags or discard the executing continuation. */
 if(taxi_host_presentation_pending_begin(&f->pending,host,service,&f->wait,work,f)!=-1){f->bad=1;return -1;}
 if(taxi_host_presentation_pending_poll(&f->pending)!=-1){f->bad=2;return -1;}
 taxi_host_presentation_pending_cancel(&f->pending);
 if(memcmp(&before,&f->pending,sizeof before)||memcmp(&flags,&f->wait,sizeof flags)){f->bad=3;return -1;}
 host->counter++; /* Explicit diagnostic counter delivery, no original-cost claim. */
 return f->calls==f->fail_at?-1:0;
}
int main(void){
 unsigned cases=0;
 for(unsigned request=0;request<3;request++)for(unsigned fail=0;fail<6;fail++){
  Fixture f={0};f.fail_at=fail;f.wait.event_delivery_enabled=1;f.wait.handshake=0xffff;
  f.host.counter=f.host.frame.previous_3524=100;f.host.frame.delta_3b00=7;
  f.service.previous_3524=100;f.service.delta_3b00=7;f.service.pending_5f24=(uint8_t)request;
  TaxiFrameTime held=f.host.frame;
  if(taxi_host_presentation_pending_begin(&f.pending,&f.host,&f.service,&f.wait,work,&f)!=0)return 2;
  f.host.counter++;if(taxi_frame_wait_signal(&f.wait))return 3;
  int result=taxi_host_presentation_pending_poll(&f.pending);
  if(f.bad){printf("FAIL nested pending operation%u accepted/mutated executing owner at helper%u\n",f.bad,f.calls);return 1;}
  unsigned expected=request?5:4;if(fail&&fail<=expected)expected=fail;
  if(f.calls!=expected||f.pending.active||result!=((fail&&fail<=expected)?-1:1))return 4;
  if(result<0&&memcmp(&f.host.frame,&held,sizeof held))return 5;
  if(result>0&&(f.host.frame.animation_1e10!=4||f.host.frame.previous_3524!=f.host.counter))return 6;
  unsigned calls=f.calls;if(taxi_host_presentation_pending_poll(&f.pending)!=-1||f.calls!=calls)return 7;
  TaxiFrameWait before=f.wait;taxi_host_presentation_pending_cancel(&f.pending);
  if(f.pending.host||f.pending.service||memcmp(&before,&f.wait,sizeof before))return 8;
  cases++;
 }
 printf("PASS %u nested begin/poll/cancel success/error phase cases; executing owner and fresh signal retained, terminal state and exactly one tail\n",cases);return 0;
}