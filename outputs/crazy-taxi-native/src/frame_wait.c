#include "frame_wait.h"
int taxi_frame_wait_signal(TaxiFrameWait *s){
    if(!s)return -1;
    s->handshake=1;
    return 0;
}
int taxi_frame_wait(TaxiFrameWait *s,uint32_t frames,TaxiFrameWaitService service,void *context){
    if(!s)return -1;
    unsigned count=(uint8_t)frames;
    if(s->event_delivery_enabled!=1||!count)return 0;
    if(!service)return -1;
    for(unsigned i=0;i<count;i++){
        s->handshake&=UINT16_C(0xfffe);
        do {if(service(context,s))return -1;} while(!(s->handshake&1));
    }
    return 0;
}

int taxi_frame_wait_pending_begin(TaxiFrameWaitPending *p,TaxiFrameWait *s,uint32_t frames){
 if(!p||!s||p->active)return -1;
 p->shared=s;p->remaining=s->event_delivery_enabled==1?(uint8_t)frames:0;
 p->active=1;
 if(p->remaining)s->handshake&=UINT16_C(0xfffe);
 return p->remaining?0:1;
}
int taxi_frame_wait_pending_poll(TaxiFrameWaitPending *p){
 if(!p||!p->active||!p->shared)return -1;
 if(!p->remaining){p->active=0;return 1;}
 if(!(p->shared->handshake&1))return 0;
 if(!--p->remaining){p->active=0;return 1;}
 p->shared->handshake&=UINT16_C(0xfffe);return 0;
}
void taxi_frame_wait_pending_cancel(TaxiFrameWaitPending *p){
 if(p)*p=(TaxiFrameWaitPending){0};
}
