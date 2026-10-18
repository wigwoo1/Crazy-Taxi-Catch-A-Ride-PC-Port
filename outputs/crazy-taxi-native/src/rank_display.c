#include "rank_display.h"
#include <string.h>
static int32_t signed_bits(uint32_t bits) {
    int32_t value; memcpy(&value,&bits,sizeof value); return value;
}
void taxi_rank_display_step(TaxiRankDisplay *s,uint8_t settings,
                           uint8_t rank,uint8_t trigger,int32_t delta) {
    if(settings&0xe0u)return;
    if(trigger==1) {
        int value=rank<128u?(int)rank:(int)rank-256;
        s->target_538e[0]=(uint16_t)((value/10)*8);
        s->target_538e[1]=(uint16_t)((value%10)*8);
        s->active_16d5=1;s->phase_16d6=0;
    }
    if(!s->active_16d5)return;
    uint32_t distance=(uint32_t)delta*640u;
    if(s->phase_16d6==0) {
        s->position_16d8=signed_bits((uint32_t)s->position_16d8-distance);
        if(s->position_16d8<=0xc7ff) {
            s->position_16d8=0xc800;s->phase_16d6=1;
        }
    } else if(s->phase_16d6==1) {
        unsigned matched=0;
        for(unsigned i=0;i<2;i++) {
            if(s->current_5388[i]==s->target_538e[i])matched++;
            else {
                uint16_t next=(uint16_t)(s->current_5388[i]-1u);
                s->current_5388[i]=(next&0x8000u)?79u:next;
            }
        }
        /* Original counts matches BEFORE decrementing: reaching the last
           target on this invocation does not start the hold until next call. */
        if(matched==2){s->phase_16d6=2;s->hold_16dc=180;}
    } else if(s->phase_16d6==2) {
        s->hold_16dc=signed_bits((uint32_t)s->hold_16dc-(uint32_t)delta);
        if(s->hold_16dc<=0)s->phase_16d6=3;
    } else {
        s->position_16d8=signed_bits((uint32_t)s->position_16d8+distance);
        if(s->position_16d8>0xf1ff) {
            s->position_16d8=0xf200;s->phase_16d6=0;s->active_16d5=0;
        }
    }
}
