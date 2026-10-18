#include <stdio.h>
#include "../src/frame_wait.h"
typedef struct {unsigned delay,polls,events,left;uint16_t clear;} Fixture;
static int service(void *pointer,TaxiFrameWait *state){
    Fixture *f=pointer;f->polls++;
    if(!f->left){f->clear=state->handshake;f->left=f->delay;}
    if(--f->left==0){f->events++;state->handshake=1;}
    return 0;
}
int main(void){
    unsigned frames,enabled,handshake,delay;
    while(scanf("%u %u %u %u",&frames,&enabled,&handshake,&delay)==4){
        TaxiFrameWait s={(uint16_t)enabled,(uint16_t)handshake};Fixture f={.delay=delay};
        if(!delay||taxi_frame_wait(&s,frames,service,&f))return 1;
        printf("%u %u %u %u\n",s.handshake,f.events,f.polls,f.clear);
    }
    /* Service failure propagates after clearing the stale acknowledgement. */
    return 0;
}
